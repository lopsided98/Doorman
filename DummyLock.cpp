#include "DummyLock.h"


void DummyLock::lock() {
    Serial.println("Door locked.");
}

void DummyLock::unlock() {
    Serial.println("Door unlocked.");
}