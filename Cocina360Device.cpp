#include "Cocina360Device.h"
#include <Arduino.h>

Cocina360Device::Cocina360Device() 
    : dhtSensor(PIN_DHT, this), 
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
    dhtSensor.begin();
    connectWiFi();
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

    if (maxSeverity == 2) {
        redLed.handle(Led::TURN_ON_COMMAND);
        yellowLed.handle(Led::TURN_OFF_COMMAND);
        greenLed.handle(Led::TURN_OFF_COMMAND);
        buzzer.handle(Buzzer::PLAY_ALARM_COMMAND);
        statusStr = "PELIGRO CRÍTICO / EVACUAR";
    } 
    else if (maxSeverity == 1) {
        redLed.handle(Led::TURN_OFF_COMMAND);
        yellowLed.handle(Led::TURN_ON_COMMAND);
        greenLed.handle(Led::TURN_OFF_COMMAND);
        buzzer.handle(Buzzer::STOP_ALARM_COMMAND);
        statusStr = "ADVERTENCIA / VENTILAR";
    } 
    else {
        redLed.handle(Led::TURN_OFF_COMMAND);
        yellowLed.handle(Led::TURN_OFF_COMMAND);
        greenLed.handle(Led::TURN_ON_COMMAND);
        buzzer.handle(Buzzer::STOP_ALARM_COMMAND);
    }

    int currentTemp = dhtSensor.getLatestTemperature();
    float currentPPM = gasSensor.getLatestPPM();

    Serial.println("=================================");
    Serial.printf("Temperatura: %d °C\n", currentTemp, warnTemperatureC, critTemperatureC);
    Serial.printf("Gas: %.2f PPM\n", currentPPM, warnGasPpm, critGasPPM);
    Serial.print("Estado: ");
    Serial.println(statusStr);
}

void Cocina360Device::update() {
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

void Cocina360Device::handle(Command command) {}