#include <TimerOne.h>
#include <wiring_private.h>
#include "StepperControl.h"

static const AMIS30543::stepMode STEP_MODE = AMIS30543::stepMode::MicroStep32;
static const unsigned int DEFAULT_SPEED = 360;
static const uint16_t STALL_EMF = 100;

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

    // Increase ADC frequency to 1 MHz
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);

    // Initialize settings
    stepper.setStepMode(STEP_MODE);
    stepper.stepOnRisingEdge();

    // Turn SLA transparency off to avoid spikes in the signal. I was going to
    // implement the SLA algorithm as described here:
    // http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.364.4376&rep=rep1&type=pdf
    // but for some reason I can't sample at the correct step consistently. The
    // current algorithm should work fine as long as the chip does not get too
    // hot and cause the signal to sag.
    stepper.setSlaTransparencyOff();

    stepper.enableDriver();
    stepper.sleep();

    Timer1.initialize();
    setSpeed(DEFAULT_SPEED);
    Timer1.stop();
    Timer1.attachInterrupt(StepperControl::stepISR);
}

void StepperControl::setSpeed(const unsigned int speed) {
    unsigned long period =
            (1000000U * 360U) / (abs(speed) * stepsPerRevolution);
    noInterrupts();
    Timer1.setPeriod(period);
    interrupts();
}

void StepperControl::rotateUntilStall(const bool direction, const bool block) {
    noInterrupts();
    stepper.setDirection(direction);
    this->direction = direction;
    steps = UINT32_MAX;
    stallDetect = true;
    start();
    interrupts();

    if (block) while (running);
}

void StepperControl::rotate(const int degrees, const bool block) {
    bool direction = degrees > 0;
    noInterrupts();
    stepper.setDirection(direction);
    this->direction = direction;
    steps = (abs(degrees) * stepsPerRevolution) / 360U;
    stallDetect = false;
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
    running = false;
}

void StepperControl::stepISR() {
    static DigitalPin<3> pin3;

    if (instance->steps > 0) {
        instance->nxtPin.highI();
        if (instance->stallDetect &&
            ((instance->stepNum % STEP_MODE) == 0)) {
            // ADC read takes ~20us, giving us enough high time on the
            // NXT pin
            pin3.high();
            uint16_t emf = (uint16_t) analogRead(instance->slaPin);
            pin3.low();
            // Filter emf
            instance->emfAvg = (instance->emfAvg * 3 + emf) / 4;
            if (instance->emfAvg <= STALL_EMF) {
                instance->running = false;
            }
        } else {
            // If we don't read the ADC, we have add a busy wait to ensure the
            // pin stays high long enough
            delayMicroseconds(3);
        }
        instance->nxtPin.lowI();
        instance->stepNum += (instance->direction) ? 1 : -1;
        --(instance->steps);
    } else {
        instance->running = false;
    }

    if (!instance->running) {
        // Reset emf average
        instance->emfAvg = 1023;
        Timer1.stop();
        instance->stepper.sleep();
    }
}