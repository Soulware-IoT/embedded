#ifndef COCINA360_DEVICE_H
#define COCINA360_DEVICE_H

/**
 * @file Cocina360Device.h
 * @brief Declares the Cocina360Device class.
 *
 * This class is the core orchestrator of the Cocina360 kitchen safety system, built on top of the
 * Modest IoT Nano-framework's Device abstraction. It aggregates the two environmental sensors
 * (Dht11Sensor, Mq2Sensor), the three status LEDs, and the alarm Buzzer into a single
 * event-driven unit implementing hierarchical traffic-light logic. It also manages the ESP32's
 * WiFi connectivity and synchronizes with a cloud backend: polling for updated safety thresholds
 * via HTTP GET and reporting sensor telemetry via HTTP POST.
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

#include "Device.h"
#include "Dht11Sensor.h"
#include "Mq2Sensor.h"
#include "Led.h"
#include "ServoActuator.h"
#include "Buzzer.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

class Cocina360Device : public Device {
private:
    WiFiClient espClient; ///< Underlying TCP client used by the MQTT connection.
    PubSubClient mqttClient; ///< MQTT client used to receive remote commands.

    Dht11Sensor dhtSensor; ///< Ambient temperature sensor.
    Mq2Sensor gasSensor; ///< Flammable gas/smoke sensor.
    Led redLed; ///< Critical-danger status indicator.
    Led yellowLed; ///< Warning status indicator.
    Led greenLed; ///< Safe status indicator.
    Buzzer buzzer; ///< Acoustic alarm actuator.
    ServoActuator servoDisipador; ///< Servo-driven heat dissipator actuator.

    int tempSeverity; ///< Latest severity level reported by the temperature sensor.
    int gasSeverity; ///< Latest severity level reported by the gas sensor.

    int warnTemperatureC; ///< Temperature threshold, in °C, that triggers the warning state.
    int critTemperatureC; ///< Temperature threshold, in °C, that triggers the critical state.
    float warnGasPpm; ///< Gas concentration threshold, in PPM, that triggers the warning state.
    float critGasPPM; ///< Gas concentration threshold, in PPM, that triggers the critical state.

    const char* ssid = "prd29gat"; ///< WiFi network SSID used by connectWiFi().
    const char* password = "alltomorrows2004"; ///< WiFi network password used by connectWiFi().
    const String deviceId = "23f8d970-40a2-49c8-8aa7-d05034199ff7"; ///< Unique identifier for this device on the backend.
    const String deviceApiKey = "c893da9dc96904026fb6aaa5f7f82bfebd7e7802f228b99e6315722244a7c728"; ///< API key sent as the X-API-Key header on backend requests.
    const String edgeServerIp = "http://192.168.247.196:5000"; ///< Base URL of the edge/backend server.
    const String urlConfigGet = edgeServerIp + "/api/v1/config/" + deviceId; ///< Endpoint polled to fetch updated safety thresholds.
    const String urlTelemetryPost = edgeServerIp + "/api/v1/readings"; ///< Endpoint used to publish sensor telemetry.

    //Mosquitto
    const char* mqttServer = "192.168.247.196"; ///< Hostname/IP of the MQTT broker.
    const int mqttPort = 1883; ///< TCP port of the MQTT broker.
    String commandTopic; ///< MQTT topic this device subscribes to for remote commands.
    unsigned long lastFetchTime; ///< Timestamp (ms) of the last threshold synchronization.
    const unsigned long FETCH_INTERVAL_MS = 10000; ///< Interval, in ms, between threshold synchronizations.

    /**
     * @brief Recomputes the overall safety state from the sensor severities and drives the LEDs/buzzer accordingly.
     */
    void evaluateGlobalState();

    /**
     * @brief Connects to the configured WiFi network, blocking briefly with a bounded retry count.
     */
    void connectWiFi();

    /**
     * @brief Connects (or reconnects) to the configured MQTT broker and subscribes to the command topic.
     */
    void reconnectMQTT();

    /**
     * @brief Fetches updated safety thresholds from the backend via HTTP GET.
     *
     * Falls back silently to the current in-memory thresholds if WiFi is disconnected or the
     * request fails, ensuring the device keeps operating with its last known-good configuration.
     */
    void fetchRemoteThresholds();

    /**
     * @brief Publishes a telemetry reading to the backend via HTTP POST.
     * @param temp The current ambient temperature, in °C.
     * @param ppm The current gas concentration, in PPM.
     * @param status Human-readable status string describing the current safety state.
     */
    void sendTelemetry(int temp, float ppm, String status);

    static void mqttCallback(char* topic, byte* payload, unsigned int length);

public:
    static const int PIN_DHT = 19; ///< GPIO pin wired to the DHT11 sensor.
    static const int PIN_MQ2 = 32; ///< Analog GPIO pin wired to the MQ-2 sensor.
    static const int PIN_RED = 25; ///< GPIO pin wired to the red LED.
    static const int PIN_YELLOW = 26; ///< GPIO pin wired to the yellow LED.
    static const int PIN_GREEN = 33; ///< GPIO pin wired to the green LED.
    static const int PIN_BUZZER = 14; ///< GPIO pin wired to the buzzer.
    static const int PIN_SERVO = 27; ///< GPIO pin wired to the servo-driven heat dissipator.

    /**
     * @brief Constructs the device, wiring each sensor/actuator to this instance and setting
     * the default local safety thresholds.
     */
    Cocina360Device();

    /**
     * @brief Handles a severity event raised by one of the owned sensors.
     * @param event The event to handle (temperature or gas severity change).
     */
    void on(Event event) override;

    /**
     * @brief Handles a command directed at the device. Currently a no-op placeholder.
     * @param command The command to handle.
     */
    void handle(Command command) override;

    /**
     * @brief Drives the device's non-blocking main loop: updates sensors/actuators, re-evaluates
     * the global safety state, and periodically sends telemetry and re-syncs thresholds.
     */
    void update();

    /**
     * @brief Performs one-time hardware and network initialization. Must be called once in setup().
     */
    void begin();

    /**
     * @brief Returns the configured warning temperature threshold.
     * @return The threshold, in °C.
     */
    int getWarnTemp() const { return warnTemperatureC; }

    /**
     * @brief Returns the configured critical temperature threshold.
     * @return The threshold, in °C.
     */
    int getCritTemp() const { return critTemperatureC; }

    /**
     * @brief Returns the configured warning gas concentration threshold.
     * @return The threshold, in PPM.
     */
    float getWarnGas() const { return warnGasPpm; }

    /**
     * @brief Returns the configured critical gas concentration threshold.
     * @return The threshold, in PPM.
     */
    float getCritGas() const { return critGasPPM; }
};

#endif