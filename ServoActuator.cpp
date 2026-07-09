#include "ServoActuator.h"
#include <Arduino.h>

const Command ServoActuator::TOGGLE_SERVO_COMMAND = Command(TOGGLE_SERVO_COMMAND_ID);

ServoActuator::ServoActuator(int pin, CommandHandler* commandHandler)
    : Actuator(pin, commandHandler), isOn(false), pin(pin), currentAngle(0), step(1), lastUpdate(0), lastToggleTime(0) {
    
    // 1. Asignamos todos los timers disponibles para que la librería 
    // elija uno que NO haga conflicto con el Wi-Fi o MQTT.
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    servo.setPeriodHertz(50);
    
    // 2. Igual que en tu prueba: Lo enganchamos UNA VEZ y jamás lo soltamos
    servo.attach(pin, 500, 2400);
    servo.write(0); // Posición inicial en 0 grados
}

void ServoActuator::handle(Command command) {
    if (command == TOGGLE_SERVO_COMMAND) {
        
        // Escudo Anti-Rebote (Debounce) de 1 segundo
        unsigned long now = millis();
        if (now - lastToggleTime < 1000) {
            return;
        }
        lastToggleTime = now;

        isOn = !isOn; 
        
        if (isOn) {
            Serial.println("[ACTUADOR] Servo ACTIVADO (Barrido continuo)");
        } else {
            Serial.println("[ACTUADOR] Servo DETENIDO (Regresando a 0)");
            currentAngle = 0;
            servo.write(0);
        }
    }
    Actuator::handle(command);
}

void ServoActuator::update() {
    // Si está encendido, hacemos la lógica de tu ciclo for() pero sin usar delay()
    if (isOn) {
        unsigned long now = millis();
        
        // Cada 15 milisegundos (Idéntico a tu delay(15) de prueba)
        if (now - lastUpdate >= 15) {
            lastUpdate = now;
            
            // Avanzamos 1 grado a la vez
            currentAngle += step;
            
            // Si llegamos a 180, invertimos a negativo (Idéntico a tu segundo for)
            if (currentAngle >= 180) {
                currentAngle = 180;
                step = -abs(step); 
            } 
            // Si llegamos a 0, invertimos a positivo (Idéntico a tu primer for)
            else if (currentAngle <= 0) {
                currentAngle = 0;
                step = abs(step);  
            }
            
            servo.write(currentAngle);
        }
    }
}