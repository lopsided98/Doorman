#pragma once

#include "stdint.h"
#include "Authenticator.h"
#include "DigitalIO.h"

class ButtonAuthenticator : public Authenticator {
public:

    static const unsigned long DEBOUNCE_TIME = 50; // ms

    explicit ButtonAuthenticator(uint8_t pin);

    void init() override;

    Command getCommand() override;

private:
    PinIO pin;
    unsigned long lastBounceTime = 0;
    bool pressed = false;
    bool wasPressed = false;
};
