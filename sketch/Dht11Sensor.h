#ifndef DHT11_SENSOR_H
#define DHT11_SENSOR_H

/**
 * @file Dht11Sensor.h
 * @brief Declares the Dht11Sensor class.
 *
 * This class specializes the Modest IoT Nano-framework's Sensor to read the DHT11 temperature
 * and humidity module. It periodically samples the ambient temperature and raises severity events
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
#include <DHT.h>

class Dht11Sensor : public Sensor {
private:
    DHT dht; ///< Underlying Adafruit DHT driver instance.
    unsigned long lastReadTime; ///< Timestamp (ms) of the last completed sampling window.
    int currentState; ///< Last severity level raised (0 = normal, 1 = warning, 2 = critical).
    int lastTemperature; ///< Most recent ambient temperature reading, in degrees Celsius.

public:
    static const int TEMP_NORMAL_EVENT_ID = 30; ///< Identifier for the normal-state event.
    static const int TEMP_WARN_EVENT_ID = 31; ///< Identifier for the warning-state event.
    static const int TEMP_CRIT_EVENT_ID = 32; ///< Identifier for the critical-state event.
    static const Event TEMP_NORMAL_EVENT; ///< Event raised when the temperature returns to normal.
    static const Event TEMP_WARN_EVENT; ///< Event raised when the temperature crosses the warning threshold.
    static const Event TEMP_CRIT_EVENT; ///< Event raised when the temperature crosses the critical threshold.

    /**
     * @brief Constructs a Dht11Sensor bound to a digital pin.
     * @param pin The GPIO pin connected to the DHT11 module's data line.
     * @param eventHandler Pointer to an EventHandler to receive severity events (default: nullptr).
     */
    Dht11Sensor(int pin, EventHandler* eventHandler = nullptr);

    /**
     * @brief Returns the most recently sampled ambient temperature.
     * @return The latest reading, in degrees Celsius.
     */
    int getLatestTemperature() { return lastTemperature; }

    /**
     * @brief Initializes the underlying DHT driver. Must be called once before update().
     */
    void begin();

    /**
     * @brief Samples the sensor on a non-blocking interval and raises severity events on change.
     *
     * Reads the DHT11 module, discards invalid (NaN) samples, and compares the temperature
     * against the warning and critical thresholds exposed by the handler (cast to
     * Cocina360Device). Only propagates an event to the handler when the severity level changes.
     */
    void update();
};

#endif