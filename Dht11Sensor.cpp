/**
 * @file Dht11Sensor.cpp
 * @brief Implements the Dht11Sensor class.
 *
 * Samples the DHT11 module on a 2-second non-blocking window and notifies the assigned handler
 * whenever the resulting temperature severity level changes.
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

#include "Dht11Sensor.h"
#include "Cocina360Device.h"

const Event Dht11Sensor::TEMP_NORMAL_EVENT = Event(TEMP_NORMAL_EVENT_ID);
const Event Dht11Sensor::TEMP_WARN_EVENT = Event(TEMP_WARN_EVENT_ID);
const Event Dht11Sensor::TEMP_CRIT_EVENT = Event(TEMP_CRIT_EVENT_ID);

Dht11Sensor::Dht11Sensor(int pin, EventHandler* eventHandler) 
    : Sensor(pin, eventHandler), dht(pin, DHT11), lastReadTime(0), currentState(0), 
      lastTemperature(0) {
}

void Dht11Sensor::begin() {
    dht.begin();
}

void Dht11Sensor::update() {
    unsigned long now = millis();
    if (now - lastReadTime >= 2000) {
        lastReadTime = now;
        
        float raw = dht.readTemperature();
        if (isnan(raw)) return;
        
        lastTemperature = static_cast<int>(raw);
        int newState = 0;
        
        if (handler != nullptr) {
            Cocina360Device* device = static_cast<Cocina360Device*>(handler);
            
            if (lastTemperature >= device->getCritTemp()) newState = 2;
            else if (lastTemperature >= device->getWarnTemp()) newState = 1;
        }

        if (newState != currentState && handler != nullptr) {
            currentState = newState;
            if (currentState == 2) handler->on(TEMP_CRIT_EVENT);
            else if (currentState == 1) handler->on(TEMP_WARN_EVENT);
            else handler->on(TEMP_NORMAL_EVENT);
        }
    }
}