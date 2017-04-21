#include <Arduino.h>
#include "StepperLock.h"

StepperLock lock(10, 8, 0);

void setup() {
    Serial.begin(9600);

    Serial.println("Startup");

    lock.init();
    Serial.println("Lock init");

}

void loop() {
    lock.lock();
    delay(10000);
    lock.unlock();
    delay(10000);
}