#include <HardwareSerial.h>
#include "Lock.h"

void Lock::unlock() {
    Serial.println("Unlocking...");
    locked = false;
}

void Lock::lock() {
    Serial.println("Locking...");
    locked = true;
}

void Lock::toggle() {
    if (isLocked()) {
        unlock();
    } else {
        lock();
    }
}

bool Lock::isLocked() {
    return locked;
}