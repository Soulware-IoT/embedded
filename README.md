# Cocina360 Device (C++ Edition)

**Version**: 1.0  
**Authors**: IoT Solution Development Team  
**Date**: July 9, 2026  
**Framework Base**: Modest IoT Nano-framework v0.1  

## Overview

**Cocina360** is an automated risk mitigation and safety system engineered for commercial and industrial kitchens. This project implements the core orchestrator `Cocina360Device`, extending the object-oriented ecosystem, event-driven architecture, and Command Query Responsibility Segregation (CQRS) principles provided by the **Modest IoT Nano-framework**.

The device performs asynchronous, non-blocking readings of ambient temperature and combustible gas/smoke concentration. In critical scenarios, it acts autonomously by deploying hardware safety barriers (traffic-light indicators, acoustic buzzer alarms with melodic patterns, and a servo-driven heat dissipator). Concurrently, it establishes bi-directional communication with cloud services: reporting sensor telemetry via HTTP `POST` and synchronizing user-defined safety thresholds via HTTP `GET` against the backend API, and receiving remote actuator commands in real time over MQTT.

## Purpose

Beyond its safety function, `Cocina360Device` is built as a practical demonstration of the Modest IoT Nano-framework's event-driven, CQRS-inspired design applied to real embedded hardware. It illustrates:
- **Encapsulation**: Sensor sampling state and actuator hardware details are bundled within their own classes (e.g., `Mq2Sensor`, `Buzzer`), hidden behind a small public interface.
- **Inheritance**: Concrete sensors and actuators (`Dht11Sensor`, `Mq2Sensor`, `Led`, `Buzzer`, `ServoActuator`) extend the framework's abstract `Sensor` and `Actuator` base classes.
- **Polymorphism**: Overridden `on(Event)` and `handle(Command)` methods let `Cocina360Device` react uniformly to events and commands regardless of which concrete sensor or actuator raised them.
- **Abstraction**: The `Device`, `EventHandler`, and `CommandHandler` interfaces decouple the orchestrator from the specifics of any single sensor or actuator implementation, whether the command originates locally or over MQTT.

## Prerequisites

### Hardware
- **Microcontroller**: ESP32 Development Board.
- **Temperature Sensor**: DHT11 environmental sensor.
- **Gas Sensor**: MQ-2 flammable gas and smoke sensor.
- **Visual Actuators**: 3 discrete Common Cathode LEDs (Red, Yellow, Green).
- **Acoustic Actuator**: Passive Buzzer.
- **Heat Dissipator**: SG90 (or compatible) servo motor.
- **Protection**: Appropriate current-limiting resistors (220Ω).

### Software & Environment
- **IDE**: Arduino IDE (v2.0+ recommended) with the ESP32 board support package installed.
- **MQTT Broker**: A reachable Mosquitto (or other MQTT 3.1.1-compatible) broker, used to deliver remote actuator commands to the device.

### Dependencies
- Adafruit `DHT sensor library`.
- `ArduinoJson` library (v6.x or v7.x).
- `ESP32Servo` library.
- `PubSubClient` library.

## Features

- **Non-Blocking Concurrent Monitoring**: Evaluation of critical environmental variables through synchronous sampling windows managed with `millis()`, entirely avoiding blocking atomic instructions (`delay`).
- **Hierarchical Traffic-Light Logic**:
  - **SAFE State (Green LED)**: Stable gas and temperature thresholds.
  - **WARNING State (Yellow LED)**: Noticeable gas concentrations or moderate thermal increases. Prompts preventive ventilation.
  - **CRITICAL DANGER State (Red LED + Alarm)**: Maximum safety limits exceeded. Asynchronously triggers the *Star Power Theme* alert melody at 175 BPM.
- **Real Logarithmic Approximation (PPM)**: Built-in mathematical translation algorithm to linearize raw analog ADC data from the MQ-2 sensor into precise Parts Per Million (PPM) concentration curves, protecting the runtime against division-by-zero or numeric overflows.
- **Dynamic Cloud Synchronization**: Integrated HTTP client executing background `GET` polling requests to update operating thresholds according to restaurant needs, guaranteeing system fault tolerance through an automated local fallback mode.
- **Structured Telemetry Dispatching**: Periodic, managed delivery of structured JSON payloads via HTTP `POST` containing the unique device identifier, exact temperature, calculated PPM volume, and literal alert string status.
- **MQTT Remote Command Channel**: The device subscribes to a per-device topic (`cocina360/<deviceId>/command`) on a Mosquitto broker and reacts to incoming commands in its non-blocking main loop, auto-reconnecting to WiFi/broker if the connection drops.
- **Servo-Driven Heat Dissipator**: A `ServoActuator` toggled remotely over MQTT (`TOGGLE` command) performs a continuous, non-blocking 0°–180° sweep to actively dissipate heat, with a 1-second debounce guard and automatic return to the 0° rest position when switched off.

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
              /          |            \
             v           v             v
    +---------+     +---------+     +---------+
    |  Sensor |     |Actuator |     |   Led   |
    +---------+     +---------+     +---------+
     /       \       /   |    \      (Red, Yellow,
    v         v     v    v     v      Green)
+-------+ +-------+ +-------+ +-------+ +---------------+
| Dht11 | |  Mq2  | |Buzzer | |  Led  | | ServoActuator |
+-------+ +-------+ +-------+ +-------+ +---------------+
```

For the complete UML diagram, including attributes, methods, and relationships for every class in the framework and the project, see [docs/class-diagram.md](docs/class-diagram.md).

For the end-user personas and Given-When-Then acceptance criteria behind this design, see [docs/user-stories.md](docs/user-stories.md).

## Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/<your-username>/embedded.git
   cd embedded
   ```
2. **Install the Arduino IDE**: Version 2.0+ recommended, with the ESP32 board support package installed via `File > Preferences > Additional Board Manager URLs` and `Tools > Board > Boards Manager`.
3. **Install Library Dependencies** via `Tools > Manage Libraries...`:
   ```text
   Adafruit DHT sensor library
   ArduinoJson (v6.x or v7.x)
   ESP32Servo
   PubSubClient
   ```
4. **Open the Sketch**: Open `sketch.ino` in the Arduino IDE; the accompanying `.h`/`.cpp` files load automatically as part of the same sketch.
5. **Configure Credentials**: Update the `ssid`, `password`, `deviceId`, `deviceApiKey`, and `edgeServerIp` fields in `Cocina360Device.h` to match your WiFi network and backend deployment.
6. **Configure the MQTT Broker**: Update `mqttServer` and `mqttPort` in `Cocina360Device.h` to point at your Mosquitto (or other MQTT) broker. The device auto-subscribes to `cocina360/<deviceId>/command` on connect.
7. **Select Board and Port**: Choose your ESP32 board under `Tools > Board` and the correct serial port under `Tools > Port`.
8. **Upload**:
   ```text
   Sketch > Upload
   ```

## Usage

Wire the hardware as described in [Prerequisites](#prerequisites), upload the sketch, then open the Serial Monitor (115200 baud) to observe the device connecting to WiFi and the MQTT broker, synchronizing thresholds, and reporting its safety state on every sampling cycle.

To remotely toggle the servo-driven heat dissipator, publish the payload `TOGGLE` to the device's command topic, `cocina360/<deviceId>/command`, e.g.:
```bash
mosquitto_pub -h <broker-host> -t "cocina360/23f8d970-40a2-49c8-8aa7-d05034199ff7/command" -m "TOGGLE"
```

### Example Output
```plaintext
[WIFI] Conectando a prd29gat
....
[WIFI] ¡Conectado con éxito!
[MQTT] Intentando conectar al Broker Mosquitto...¡Conectado con éxito!
[HTTP] Umbrales sincronizados con éxito desde el Edge v1.
=================================
Temperatura: 28 °C
Gas: 412.35 PPM
Estado: SEGURO
[HTTP] Telemetría aceptada por el Edge (202 ACCEPTED).
=================================
Temperatura: 36 °C
Gas: 1120.80 PPM
Estado: ADVERTENCIA / VENTILAR
[HTTP] Telemetría aceptada por el Edge (202 ACCEPTED).
[ACTUADOR] Servo ACTIVADO (Barrido continuo)
```