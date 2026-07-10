/**
 * @file Cocina360Device.cpp
 * @brief Implements the Cocina360Device class.
 *
 * Wires together the sensors, LEDs, and buzzer into the hierarchical traffic-light safety logic,
 * and manages WiFi connectivity, threshold synchronization, and telemetry reporting against the
 * backend edge server.
 *
 * @author IoT Solution Development Team
 * @date June 14, 2026
 * @version 1.0
 */

/*
 * This file is part of the Cocina360 Device (C++ Edition) project.
 * Copyright (c) 2026 IoT Solution Development Team
 *
 * Licensed under the MIT License. See LICENSE.md for details.
 */

#include "Cocina360Device.h"
#include <Arduino.h>

Cocina360Device* globalDeviceInstance = nullptr;

Cocina360Device::Cocina360Device() 
    : mqttClient(espClient),
      dhtSensor(PIN_DHT, this), 
      gasSensor(PIN_MQ2, this), 
      redLed(PIN_RED, false, this), 
      yellowLed(PIN_YELLOW, false, this), 
      greenLed(PIN_GREEN, true, this), 
      buzzer(PIN_BUZZER, this),
      tempSeverity(0), gasSeverity(0),
      warnTemperatureC(35), critTemperatureC(50), 
      warnGasPpm(1000.0), critGasPPM(3000.0),
      lastFetchTime(0) {
}

void Cocina360Device::begin() {
    commandTopic = "cocina360/" + deviceId + "/command"; 
    redLed.begin();
    yellowLed.begin();
    greenLed.begin();
    buzzer.begin();
    dhtSensor.begin();
    connectWiFi();
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(mqttCallback);
    reconnectMQTT();
    fetchRemoteThresholds();
}

void Cocina360Device::connectWiFi() {
    Serial.print("[WIFI] Conectando a ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    int intentos = 0;
    const int MAX_INTENTOS = 20;

    while (WiFi.status() != WL_CONNECTED && intentos < MAX_INTENTOS) {
        delay(500);
        Serial.print(".");
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WIFI] ¡Conectado con éxito!");
    } else {
        Serial.println("\n[WIFI] No se pudo conectar. El sistema operará en MODO LOCAL.");
    }
}

void Cocina360Device::reconnectMQTT() {
    if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
        Serial.print("[MQTT] Intentando conectar al Broker Mosquitto...");
        if (mqttClient.connect(deviceId.c_str())) {
            Serial.println("¡Conectado con éxito!");
            
            mqttClient.subscribe(commandTopic.c_str()); 
            
        } else {
            Serial.printf("Falló, código de estado=%d. Se reintentará en el próximo ciclo.\n", mqttClient.state());
        }
    }
}

void Cocina360Device::mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.println("\n--- [DEBUG MQTT INCOMING] ---");
    Serial.printf("Tópico de origen : [%s]\n", topic);
    Serial.printf("Longitud del msg : %u bytes\n", length);
    Serial.printf("Texto del mensaje: \"%s\"\n", message.c_str());
    
    message.trim(); 
    
    if (message == "TOGGLE") {
        Serial.println("[DEBUG] -> ¡ÉXITO! El mensaje calza perfectamente con 'TOGGLE'.");
    } else {
        Serial.printf("[DEBUG] -> ALERTA: El mensaje recibido es '%s', pero se esperaba 'TOGGLE'.\n", message.c_str());
    }
    Serial.println("-----------------------------\n");

    if (message == "TOGGLE" && globalDeviceInstance != nullptr) {
        Serial.println("[MQTT] Comando TOGGLE recibido. Disparando melodía de éxito...");
        pinMode(2, OUTPUT);
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        globalDeviceInstance->handle(Buzzer::PLAY_SUCCESS_COMMAND);
    }
}

void Cocina360Device::fetchRemoteThresholds() {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(urlConfigGet);
    http.addHeader("X-API-Key", deviceApiKey); 
    
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
        String payload = http.getString();
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            warnTemperatureC = doc["warnTemperatureC"] | warnTemperatureC;
            critTemperatureC = doc["critTemperatureC"] | critTemperatureC;
            warnGasPpm = doc["warnGasPpm"] | warnGasPpm;
            critGasPPM = doc["critGasPpm"] | critGasPPM;
            Serial.println("[HTTP] Umbrales sincronizados con éxito desde el Edge v1.");
        }
    } else {
        Serial.printf("[HTTP] Error de sincronización v1. Código: %d\n", httpResponseCode);
    }
    http.end();
}

void Cocina360Device::sendTelemetry(int temp, float ppm, String status) {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(urlTelemetryPost);
    http.addHeader("Content-Type", "application/json");
    
    http.addHeader("X-API-Key", deviceApiKey); 

    StaticJsonDocument<128> doc;
    doc["temperature_c"] = temp;
    doc["gas_ppm"] = ppm;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode == 202) {
        Serial.println("[HTTP] Telemetría aceptada por el Edge (202 ACCEPTED).");
    } else {
        Serial.printf("[HTTP] Error en envío de telemetría. Código: %d\n", httpResponseCode);
    }
    
    http.end();
}

void Cocina360Device::evaluateGlobalState() {
    int maxSeverity = max(tempSeverity, gasSeverity);
    String statusStr = "SEGURO";

    static int lastSeverity = -1;
    bool stateChanged = (maxSeverity != lastSeverity);

    if (maxSeverity == 2) {
        if (stateChanged) {
            redLed.handle(Led::TURN_ON_COMMAND);
            yellowLed.handle(Led::TURN_OFF_COMMAND);
            greenLed.handle(Led::TURN_OFF_COMMAND);
            buzzer.handle(Buzzer::PLAY_ALARM_COMMAND);
        }
        statusStr = "PELIGRO CRÍTICO / EVACUAR";
    } 
    else if (maxSeverity == 1) {
        if (stateChanged) {
            redLed.handle(Led::TURN_OFF_COMMAND);
            yellowLed.handle(Led::TURN_ON_COMMAND);
            greenLed.handle(Led::TURN_OFF_COMMAND);
            buzzer.handle(Buzzer::STOP_ALARM_COMMAND);
        }
        statusStr = "ADVERTENCIA / VENTILAR";
    } 
    else {
        if (stateChanged) {
            redLed.handle(Led::TURN_OFF_COMMAND);
            yellowLed.handle(Led::TURN_OFF_COMMAND);
            greenLed.handle(Led::TURN_ON_COMMAND);
            buzzer.handle(Buzzer::STOP_ALARM_COMMAND);
        }
    }

    lastSeverity = maxSeverity;

    static unsigned long lastLogTime = 0;
    if (millis() - lastLogTime >= 2000) {
        lastLogTime = millis();
        int currentTemp = dhtSensor.getLatestTemperature();
        float currentPPM = gasSensor.getLatestPPM();
        
        Serial.println("=================================");
        Serial.printf("Temperatura: %d °C\n", currentTemp);
        Serial.printf("Gas: %.2f PPM\n", currentPPM);
        Serial.print("Estado: ");
        Serial.println(statusStr);
    }
}

void Cocina360Device::update() {
    
    if (!mqttClient.connected()) {
        static unsigned long lastMqttRetry = 0;
        if (millis() - lastMqttRetry >= 5000) {
            lastMqttRetry = millis();
            reconnectMQTT();
        }
    } else {
        mqttClient.loop();
    }

    dhtSensor.update();
    gasSensor.update();
    buzzer.update();
    
    evaluateGlobalState();

    unsigned long now = millis();
    
    static unsigned long lastPostTime = 0;
    if (now - lastPostTime >= 5000) { 
        lastPostTime = now;
        int currentTemp = dhtSensor.getLatestTemperature();
        float currentPPM = gasSensor.getLatestPPM();
        String statusStr = "SEGURO";
        int maxSeverity = max(tempSeverity, gasSeverity);
        if (maxSeverity == 2) statusStr = "PELIGRO CRÍTICO / EVACUAR";
        else if (maxSeverity == 1) statusStr = "ADVERTENCIA / VENTILAR";
        
        sendTelemetry(currentTemp, currentPPM, statusStr);
    }

    if (now - lastFetchTime >= FETCH_INTERVAL_MS) {
        lastFetchTime = now;
        fetchRemoteThresholds();
    }
}

void Cocina360Device::on(Event event) {
    if (event == Dht11Sensor::TEMP_CRIT_EVENT) tempSeverity = 2;
    else if (event == Dht11Sensor::TEMP_WARN_EVENT) tempSeverity = 1;
    else if (event == Dht11Sensor::TEMP_NORMAL_EVENT) tempSeverity = 0;
    
    else if (event == Mq2Sensor::GAS_CRIT_EVENT) gasSeverity = 2;
    else if (event == Mq2Sensor::GAS_WARN_EVENT) gasSeverity = 1;
    else if (event == Mq2Sensor::GAS_NORMAL_EVENT) gasSeverity = 0;
}

void Cocina360Device::handle(Command command) {
    if (command == Buzzer::PLAY_SUCCESS_COMMAND) {
        buzzer.handle(command);
    }
}