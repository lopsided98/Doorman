#include "SerialAuthenticator.h"

#include <Arduino.h>

SerialAuthenticator::SerialAuthenticator(Lock &lock) : lock(lock) {}

Authenticator::Command SerialAuthenticator::getCommand() {
    bool commandComplete = false;
    while (Serial.available()) {
        int c = Serial.read();
        if (commandBufferPos < sizeof(commandBuffer)) {
            commandBuffer[commandBufferPos++] = static_cast<char>(c);
        }
        if (c == '\r' || c == '\n') {
            if (commandBufferPos <= sizeof(commandBuffer)) {
                commandComplete = true;
            }
            commandBufferPos = 0;
        }
    }

    if (commandComplete) {
        // Check command character
        switch (*commandBuffer) {
            case 'l':
                return parseLockCommand(commandBuffer + 1);
            case 's':
                printStatus();
            default:
                return NONE;
        }
    }

    if (lock.isLocked() != wasLocked) {
        wasLocked = lock.isLocked();
        printStatus();
    }
    return NONE;
}

Authenticator::Command SerialAuthenticator::parseLockCommand(char *buffer) {
    switch (*buffer) {
        case 'l':
            return LOCK;
        case 'u':
            return UNLOCK;
        case 't':
            return TOGGLE;
        default:
            return NONE;
    }
}

void SerialAuthenticator::printStatus() {
    Serial.print('s');
    Serial.print(lock.isLocked() ? 'l' : 'u');
    Serial.print('\n');
}