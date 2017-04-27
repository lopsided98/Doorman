#include "Lock.h"

void Lock::unlock() {
    locked = false;
}

void Lock::lock() {
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