#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H

/**
 * @file Mq2Sensor.h
 * @brief Declares the Mq2Sensor class.
 *
 * This class specializes the Modest IoT Nano-framework's Sensor to read the MQ-2 flammable gas
 * and smoke sensor. It periodically samples the analog output, linearizes it into a Parts Per
 * Million (PPM) concentration using a logarithmic approximation curve, and raises severity events
 * (normal, warning, critical) toward its assigned handler based on the thresholds configured on
 * the owning device.
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

#include "Sensor.h"

class Mq2Sensor : public Sensor {
private:
    unsigned long lastReadTime; ///< Timestamp (ms) of the last completed sampling window.
    int lastADC; ///< Most recent raw analog-to-digital reading.
    float lastPPM; ///< Most recent linearized gas concentration, in PPM.
    int currentState; ///< Last severity level raised (0 = normal, 1 = warning, 2 = critical).

public:
    static const int GAS_NORMAL_EVENT_ID = 20; ///< Identifier for the normal-state event.
    static const int GAS_WARN_EVENT_ID = 21; ///< Identifier for the warning-state event.
    static const int GAS_CRIT_EVENT_ID = 22; ///< Identifier for the critical-state event.
    static const Event GAS_NORMAL_EVENT; ///< Event raised when the gas concentration returns to normal.
    static const Event GAS_WARN_EVENT; ///< Event raised when the gas concentration crosses the warning threshold.
    static const Event GAS_CRIT_EVENT; ///< Event raised when the gas concentration crosses the critical threshold.

    /**
     * @brief Constructs an Mq2Sensor bound to an analog pin.
     * @param pin The analog GPIO pin connected to the MQ-2 sensor's output.
     * @param eventHandler Pointer to an EventHandler to receive severity events (default: nullptr).
     */
    Mq2Sensor(int pin, EventHandler* eventHandler = nullptr);

    /**
     * @brief Returns the most recently computed gas concentration.
     * @return The latest reading, in Parts Per Million (PPM).
     */
    float getLatestPPM() const { return lastPPM; }

    /**
     * @brief Samples the sensor on a non-blocking interval and raises severity events on change.
     *
     * Reads the analog pin, converts it into a PPM value, and compares it against the warning and
     * critical thresholds exposed by the handler (cast to Cocina360Device). Only propagates an
     * event to the handler when the severity level actually changes.
     */
    void update();
};

#endif