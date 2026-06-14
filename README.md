# ToggableLedDevice Example (C++ Edition)

**Version**: 0.1  
**Author**: Angel Velasquez  
**Date**: March 23, 2025
**Last Update** April 28, 2026

## Overview

This project demonstrates the usage of the Modest IoT Nano-framework (C++ Edition) v0.1 by implementing a simple IoT device: the `ToggableLedDevice`. It toggles an LED on an ESP32 via a button press, showcasing the framework’s object-oriented, event-driven, and CQRS-inspired design. This example is not part of the core framework but serves as a practical illustration of how to apply it.

The framework itself is maintained separately in [`Modest-IoT-Nano-framework-Cpp`](https://github.com/avelasquezn/Modest-IoT-Nano-framework-Cpp).

## Prerequisites
- **Hardware**: ESP32 development board, a push button (active-low with pull-up), an LED with a 220Ω resistor.
- **Software**: Arduino IDE with ESP32 support, or Wokwi for simulation.
- **Dependency**: Modest IoT Nano-framework (C++ Edition) v0.1.

## Features
- **Button Control**: Pressing the button toggles the LED state.
- **Event-Driven**: Utilizes event handlers to manage button presses and LED state changes.
- **Modular Design**: Demonstrates the use of sensors, actuators, and devices in a structured manner.   
- **CQRS Pattern**: Separates command handling (button press) from event handling (LED state change).
- **Simulation Support**: Can be run on Wokwi for easy testing and demonstration.
- **Documentation**: Includes user stories and a system diagram for clarity.

## User Stories
See [user-stories.md](user-stories.md) for detailed user stories that guided the development of this example.

## Class Diagram
The following diagram illustrates the relationships between the nano-framework and the main classes in this example:

![Class Diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/upc-pre-202610-1asi0572-sandbox/toggable-led-device-cpp/refs/heads/master/class-diagram.puml?token=GHSAT0AAAAAADVFVJLAI33WPS3XU4IONLMS2PQZDNA)

The class diagram can be found in [class-diagram.puml](class-diagram.puml).


## Installation
1. **Download Framework**: Clone or download [`Modest-IoT-Nano-framework-Cpp`](https://github.com/avelasquezn/Modest-IoT-Nano-framework-Cpp).
2. **Download Example**: Clone or download this repository from [avelasquezn/ToggableLedDevice-Example-Cpp](https://github.com/avelasquezn/ToggableLedDevice-Example-Cpp).
3. **Directory Structure**: Combine files (assuming both repos are siblings):
```planetext
ToggableLedDevice-Example-Cpp/
├── ToggableLedDevice.ino
├── ToggableLedDevice.h
├── ToggableLedDevice.cpp
├── diagram.json
├── wokwi.project.txt
├── user-stories.md
├── class-diagram.puml
├── sketch.ino
├── (copied from Modest-IoT-Nano-framework-Cpp)
│   ├── ModestIoT.h
│   ├── EventHandler.h
│   ├── CommandHandler.h
│   ├── Sensor.h
│   ├── Sensor.cpp
│   ├── Button.h
│   ├── Button.cpp
│   ├── Actuator.h
│   ├── Actuator.cpp
│   ├── Led.h
│   ├── Led.cpp
│   ├── Device.h
│   └── Device.cpp
````
# embedded
# sketch
