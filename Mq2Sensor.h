#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H

#include "Sensor.h"

class Mq2Sensor : public Sensor {
private:
    unsigned long lastReadTime;
    int lastADC;
    float lastPPM;
    int currentState;

public:
    static const int GAS_NORMAL_EVENT_ID = 20;
    static const int GAS_WARN_EVENT_ID = 21;
    static const int GAS_CRIT_EVENT_ID = 22;
    static const Event GAS_NORMAL_EVENT;
    static const Event GAS_WARN_EVENT;
    static const Event GAS_CRIT_EVENT;

    Mq2Sensor(int pin, EventHandler* eventHandler = nullptr);
    float getLatestPPM() const { return lastPPM; }
    void update();
};

#endif