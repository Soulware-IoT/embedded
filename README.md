# Cocina360 Device (C++ Edition)

**Version**: 1.0  
**Authors**: IoT Solution Development Team  
**Date**: June 14, 2026  
**Framework Base**: Modest IoT Nano-framework v0.1  

## Overview

**Cocina360** is an automated risk mitigation and safety system engineered for commercial and industrial kitchens. This project implements the core orchestrator `Cocina360Device`, extending the object-oriented ecosystem, event-driven architecture, and Command Query Responsibility Segregation (CQRS) principles provided by the **Modest IoT Nano-framework**.

The device performs asynchronous, non-blocking readings of ambient temperature and combustible gas/smoke concentration. In critical scenarios, it acts autonomously by deploying hardware safety barriers (traffic-light indicators and acoustic buzzer alarms with melodic patterns). Concurrently, it establishes bi-directional communication with cloud services (Beeceptor/Backend API) to report sensor telemetry via HTTP `POST` and dynamically synchronize user-defined safety thresholds via HTTP `GET`.

## Prerequisites

### Hardware
- **Microcontroller**: ESP32 Development Board.
- **Temperature Sensor**: DHT11 environmental sensor.
- **Gas Sensor**: MQ-2 flammable gas and smoke sensor.
- **Visual Actuators**: 3 discrete Common Cathode LEDs (Red, Yellow, Green).
- **Acoustic Actuator**: Passive Buzzer.
- **Protection**: Appropriate current-limiting resistors (220Ω).

### Software & Environment
- **IDE**: Arduino IDE (v2.0+ recommended) with the ESP32 board support package installed.

### Dependencies
- Adafruit `DHT sensor library`.
- `ArduinoJson` library (v6.x or v7.x).

## Features

- **Non-Blocking Concurrent Monitoring**: Evaluation of critical environmental variables through synchronous sampling windows managed with `millis()`, entirely avoiding blocking atomic instructions (`delay`).
- **Hierarchical Traffic-Light Logic**:
  - **SAFE State (Green LED)**: Stable gas and temperature thresholds.
  - **WARNING State (Yellow LED)**: Noticeable gas concentrations or moderate thermal increases. Prompts preventive ventilation.
  - **CRITICAL DANGER State (Red LED + Alarm)**: Maximum safety limits exceeded. Asynchronously triggers the *Star Power Theme* alert melody at 175 BPM.
- **Real Logarithmic Approximation (PPM)**: Built-in mathematical translation algorithm to linearize raw analog ADC data from the MQ-2 sensor into precise Parts Per Million (PPM) concentration curves, protecting the runtime against division-by-zero or numeric overflows.
- **Dynamic Cloud Synchronization**: Integrated HTTP client executing background `GET` polling requests to update operating thresholds according to restaurant needs, guaranteeing system fault tolerance through an automated local fallback mode.
- **Structured Telemetry Dispatching**: Periodic, managed delivery of structured JSON payloads via HTTP `POST` containing the unique device identifier, exact temperature, calculated PPM volume, and literal alert string status.

## Class Diagram

The system architecture decouples logic from monolithic structures into an event-driven design:

```text
               +--------------------+
               |       Device       |
               +--------------------+
                         ^
                         |
               +--------------------+
               |  Cocina360Device   |
               +--------------------+
              /          |           \
             v           v            v
    +---------+     +---------+     +---------+
    |  Sensor |     |Actuator |     |   Led   |
    +---------+     +---------+     +---------+
     /       \           |           (Red, Yellow,
    v         v          v            Green)
+-------+ +-------+ +---------+
| Dht11 | |  Mq2  | |  Buzzer |
+-------+ +-------+ +---------+
```