#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "Authenticator.h"
#include "DigitalIO.h"

class ButtonAuthenticator : public Authenticator {
public:
    static const unsigned long DEBOUNCE_MS{50};  // ms

    explicit ButtonAuthenticator(uint8_t pin);

    void init() override;

    Command getCommand() override;

    /// Get the amount of time the button has been held down. If the button is
    /// not pressed, 0 is returned.
    ///
    /// @return button hold time in milliseconds
    unsigned long getHoldTime();

private:
    PinIO pin;
    unsigned long lastBounceTime{0};
    bool pressed{false};
    bool wasPressed{false};
};
