#ifndef SERVO_ACTUATOR_H
#define SERVO_ACTUATOR_H

#include "Actuator.h"
#include <ESP32Servo.h>

class ServoActuator : public Actuator {
private:
    Servo servo;
    bool isOn;
    int pin;

public:
    static const int TOGGLE_SERVO_COMMAND_ID = 40;
    static const Command TOGGLE_SERVO_COMMAND;

    ServoActuator(int pin, CommandHandler* commandHandler = nullptr);
    void handle(Command command) override;
    bool getState() const { return isOn; }
};

#endif