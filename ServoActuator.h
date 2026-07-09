#ifndef SERVO_ACTUATOR_H
#define SERVO_ACTUATOR_H

#include "Actuator.h"
#include <ESP32Servo.h>

class ServoActuator : public Actuator {
private:
    Servo servo;
    bool isOn;
    int pin;
    
    // Variables para el barrido continuo
    int currentAngle;
    int step;
    unsigned long lastUpdate;
    
    // Variable para el escudo anti-rebote (Debounce)
    unsigned long lastToggleTime;

public:
    static const int TOGGLE_SERVO_COMMAND_ID = 40;
    static const Command TOGGLE_SERVO_COMMAND;

    ServoActuator(int pin, CommandHandler* commandHandler = nullptr);
    void handle(Command command) override;
    
    // Método para el barrido no bloqueante
    void update(); 
    
    bool getState() const { return isOn; }
};

#endif