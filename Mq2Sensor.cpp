/**
 * @file Mq2Sensor.cpp
 * @brief Implements the Mq2Sensor class.
 *
 * Samples the MQ-2 analog output on a 2-second non-blocking window, converts the reading into a
 * gas concentration in PPM through a sensor-load-resistor logarithmic approximation, and notifies
 * the assigned handler whenever the resulting severity level changes.
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

#include "Mq2Sensor.h"
#include "Cocina360Device.h"
#include <Arduino.h>
#include <cmath>

const Event Mq2Sensor::GAS_NORMAL_EVENT = Event(GAS_NORMAL_EVENT_ID);
const Event Mq2Sensor::GAS_WARN_EVENT = Event(GAS_WARN_EVENT_ID);
const Event Mq2Sensor::GAS_CRIT_EVENT = Event(GAS_CRIT_EVENT_ID);

const float VCC = 5.0; ///< Sensor supply voltage, in volts.
const float RL = 1.0; ///< Load resistor value used in the Rs/RL ratio, in kΩ.

Mq2Sensor::Mq2Sensor(int pin, EventHandler* eventHandler) 
    : Sensor(pin, eventHandler), lastReadTime(0), lastADC(-1), lastPPM(0.0), currentState(0) {
    pinMode(pin, INPUT);
}

void Mq2Sensor::update() {
    unsigned long now = millis();
    if (now - lastReadTime >= 2000) {
        lastReadTime = now;
        
        int adc = analogRead(pin);
        
        if (adc < 10) adc = 10; 

        if (adc != lastADC) {
            float vout = adc * (VCC / 4095.0);

            if (vout >= (VCC - 0.05)) {
                vout = VCC - 0.05; 
            }
            if (vout <= 0.05) vout = 0.05;

            float Rs = RL * (VCC - vout) / vout;
            
            lastPPM = 2.79 * pow(Rs, -2.41);
            lastADC = adc;
        }

        int newState = 0;
        
        if (handler != nullptr) {
            Cocina360Device* device = static_cast<Cocina360Device*>(handler);
            
            if (lastPPM >= device->getCritGas()) newState = 2;
            else if (lastPPM >= device->getWarnGas()) newState = 1;
        }

        if (newState != currentState && handler != nullptr) {
            currentState = newState;
            if (currentState == 2) handler->on(GAS_CRIT_EVENT);
            else if (currentState == 1) handler->on(GAS_WARN_EVENT);
            else handler->on(GAS_NORMAL_EVENT);
        }
    }
}