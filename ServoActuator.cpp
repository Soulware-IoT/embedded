#include "ServoActuator.h"
#include <Arduino.h>

const Command ServoActuator::TOGGLE_SERVO_COMMAND = Command(TOGGLE_SERVO_COMMAND_ID);

ServoActuator::ServoActuator(int pin, CommandHandler* commandHandler){
    : Actuator(pin, commandHandler), isOn(false), pin(pin) {
        static unsigned long lastMqttRetry = 0;
        if (millis() - lastMqttRetry >= 5000) {
            lastMqttRetry = millis();
            reconnectMQTT();
        }
    } else {
        mqttClient.loop();
    }
    ESP32PWM::allocateTimer(0);
    servo.setPeriodHertz(50);
    servo.attach(pin, 500, 2400);
    servo.write(0);
}

void ServoActuator::handle(Command command) {
    if (command == TOGGLE_SERVO_COMMAND) {
        isOn = !isOn;
        if (isOn) {
            servo.write(90);
            Serial.println("[ACTUADOR] Servo ENCENDIDO (Disipador Abierto a 90°)");
        } else {
            servo.write(0);
            Serial.println("[ACTUADOR] Servo APAGADO (Disipador Cerrado a 0°)");
        }
    }
    Actuator::handle(command);
}