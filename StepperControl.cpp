#include <TimerOne.h>
#include "StepperControl.h"

static const AMIS30543::stepMode STEP_MODE = AMIS30543::stepMode::MicroStep32;
static const unsigned int DEFAULT_SPEED = 360;

StepperControl *StepperControl::instance = NULL;

StepperControl::StepperControl(AMIS30543 &stepper, const uint8_t nxtPin,
                               const uint8_t slaPin,
                               const unsigned int stepsPerRevolution) :
        stepper(stepper), nxtPin(nxtPin), slaPin(slaPin),
        stepsPerRevolution(stepsPerRevolution * STEP_MODE) {
    if (instance) {
        Timer1.detachInterrupt();
    }
    instance = this;
}

void StepperControl::init() {
    nxtPin.config(OUTPUT, LOW);

    // Initialize settings
    stepper.setStepMode(STEP_MODE);
    stepper.stepOnRisingEdge();
    stepper.setSlaTransparencyOff();

    stepper.enableDriver();
    stepper.sleep();

    Timer1.initialize();
    setSpeed(DEFAULT_SPEED);
    Timer1.stop();
    Timer1.attachInterrupt(StepperControl::stepISR);
}

void StepperControl::setSpeed(const unsigned int speed) {
    noInterrupts();
    unsigned long period =
            (1000000U * 360U) / (abs(speed) * stepsPerRevolution);
    Serial.print(period);
    Serial.println();
    Timer1.setPeriod(period);
    interrupts();
}

void StepperControl::rotate(const int degrees, const bool block) {
    noInterrupts();
    stepper.setDirection(degrees > 0);
    steps = (abs(degrees) * stepsPerRevolution) / 360U;
    start();
    interrupts();

    if (block) while (running);
}

void StepperControl::start() {
    if (!running) {
        stepper.sleepStop();
        running = true;
        Timer1.resume();
    }
}

void StepperControl::stop() {
    if (running) {
        running = false;
        Timer1.stop();
        instance->stepper.sleep();
    }
}

void StepperControl::stepISR() {
    if (instance->steps > 0) {
        instance->nxtPin.highI();
        delayMicroseconds(3);
        instance->nxtPin.lowI();
        --(instance->steps);
    } else {
        instance->stop();
    }


}