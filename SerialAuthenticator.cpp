#include <HardwareSerial.h>
#include "SerialAuthenticator.h"

Authenticator::Command SerialAuthenticator::getCommand() {
    if (Serial.available()) {
        switch (Serial.read()) {
            case 'l':
                return LOCK;
            case 'u':
                return UNLOCK;
            case 't':
                return TOGGLE;
            default:
                break;
        }
    }
    return NONE;
}