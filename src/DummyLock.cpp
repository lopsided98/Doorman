#include "DummyLock.h"

#include <Arduino.h>

void DummyLock::lock() {
    Lock::lock();
    Serial.println("Door locked.");
}

void DummyLock::unlock() {
    Lock::unlock();
    Serial.println("Door unlocked.");
}