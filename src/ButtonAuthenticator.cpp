#include "ButtonAuthenticator.h"

ButtonAuthenticator::ButtonAuthenticator(const uint8_t pin) : pin(pin) {}

void ButtonAuthenticator::init() { pin.config(INPUT, true); }

Authenticator::Command ButtonAuthenticator::getCommand() {
    bool curPressed = !pin.read();
    unsigned long curTime = millis();

    if (curPressed != wasPressed) {
        lastBounceTime = curTime;
    }
    wasPressed = curPressed;

    if ((curTime - lastBounceTime) > DEBOUNCE_MS) {
        if (curPressed != pressed) {
            pressed = curPressed;
            if (curPressed) {
                return TOGGLE;
            }
        }
    }
    return NONE;
}

unsigned long ButtonAuthenticator::getHoldTime() {
    if (pressed) {
        return millis() - lastBounceTime;
    } else {
        return 0;
    }
}