#include "StepperLock.h"

static unsigned const int STEPS_PER_REVOLUTION = 200;
static unsigned const int CURRENT = 300;
static AMIS30543::stepMode STEP_MODE = AMIS30543::stepMode::MicroStep32;
static const unsigned int STEP_DELAY = 100;

StepperLock::StepperLock(const uint8_t ssPin,
                         const uint8_t nxtPin,
                         const uint8_t slaPin) : ssPin(ssPin), slaPin(slaPin),
                                                 nxtPin(nxtPin) {

}

void StepperLock::init() {
    SPI.begin();
    stepper.init(ssPin);

    pinMode(nxtPin, OUTPUT);
    digitalWrite(nxtPin, LOW);

    // Wait for driver to start
    delay(1);

    // Initialize settings
    stepper.resetSettings();
    stepper.setCurrentMilliamps(CURRENT);
    stepper.setStepMode(STEP_MODE);
    stepper.stepOnRisingEdge();

    stepper.enableDriver();
    stepper.sleep();
}

void StepperLock::lock() {
    rotate(180);
}

void StepperLock::unlock() {
    rotate(-180);
}

void StepperLock::rotate(int degrees) {
    uint32_t steps =
            (abs(degrees) * STEP_MODE *
             (uint32_t) STEPS_PER_REVOLUTION) / 360;

    stepper.sleepStop();
    stepper.setDirection(degrees < 0);

    unsigned long lastTime = micros();
    while (steps > 0) {
        unsigned long currTime = micros();
        if (currTime - lastTime > STEP_DELAY) {
            lastTime = currTime;
            --steps;
            digitalWrite(nxtPin, HIGH);
            delayMicroseconds(3);
            digitalWrite(nxtPin, LOW);
        }
    }
    stepper.sleep();
}