#ifndef DHT11_SENSOR_H
#define DHT11_SENSOR_H

#include "Sensor.h"
#include <DHT.h>

class Dht11Sensor : public Sensor {
private:
    DHT dht;
    unsigned long lastReadTime;
    int currentState;
    int lastTemperature;

public:
    static const int TEMP_NORMAL_EVENT_ID = 30;
    static const int TEMP_WARN_EVENT_ID = 31;
    static const int TEMP_CRIT_EVENT_ID = 32;
    static const Event TEMP_NORMAL_EVENT;
    static const Event TEMP_WARN_EVENT;
    static const Event TEMP_CRIT_EVENT;

    Dht11Sensor(int pin, EventHandler* eventHandler = nullptr);
    
    int getLatestTemperature() { return lastTemperature; }
    
    void begin();
    void update();
};

#endif