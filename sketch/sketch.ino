#include "ModestIoT.h"

Cocina360Device cocina; 

void setup() {
    Serial.begin(115200);
    delay(2000);

    cocina.begin();
    
    Serial.println("=========================================");
    Serial.println("  Cocina360 Iniciado");
    Serial.println("=========================================");
}

void loop() {
    cocina.update();
}