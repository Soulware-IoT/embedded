#ifndef COCINA360_DEVICE_H
#define COCINA360_DEVICE_H

#include "Device.h"
#include "Dht11Sensor.h"
#include "Mq2Sensor.h"
#include "Led.h"
#include "Buzzer.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class Cocina360Device : public Device {
private:
    Dht11Sensor dhtSensor;
    Mq2Sensor gasSensor;
    Led redLed;
    Led yellowLed;
    Led greenLed;
    Buzzer buzzer;

    int tempSeverity; 
    int gasSeverity;  

    int warnTemperatureC;
    int critTemperatureC;
    float warnGasPpm;
    float critGasPPM;

    const char* ssid = "prd29gat";
    const char* password = "alltomorrows2004";
    const String deviceId = "ESP32_COCINA_01"; 
    const String urlConfigGet = "https://cocina360-api.free.beeceptor.com/api/config/" + deviceId;
    const String urlTelemetryPost = "https://cocina360-api.free.beeceptor.com/api/telemetria";

    unsigned long lastFetchTime;
    const unsigned long FETCH_INTERVAL_MS = 10000;

    void evaluateGlobalState();
    void connectWiFi();
    void fetchRemoteThresholds();
    void sendTelemetry(int temp, float ppm, String status);

public:
    static const int PIN_DHT = 19;
    static const int PIN_MQ2 = 32;
    static const int PIN_RED = 25;
    static const int PIN_YELLOW = 26;
    static const int PIN_GREEN = 33;
    static const int PIN_BUZZER = 14;

    Cocina360Device();
    
    void on(Event event) override;
    void handle(Command command) override;
    void update();
    void begin();

    int getWarnTemp() const { return warnTemperatureC; }
    int getCritTemp() const { return critTemperatureC; }
    float getWarnGas() const { return warnGasPpm; }
    float getCritGas() const { return critGasPPM; }
};

#endif