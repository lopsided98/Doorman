#include <TimerOne.h>
#include <wiring_private.h>
#include <avr/sleep.h>
#include "StepperControl.h"

static const AMIS30543::stepMode STEP_MODE = AMIS30543::stepMode::MicroStep32;
static const unsigned int DEFAULT_SPEED = 360;
static const uint16_t STALL_EMF = 125;

StepperControl *StepperControl::instance = nullptr;

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
    {
        Serial.println("# Checking for errors:");
        uint16_t latched = stepper.readLatchedStatusFlagsAndClear();
        uint16_t unlatched = stepper.readNonLatchedStatusFlags();
        if (latched & (
                AMIS30543::OVCXNB |
                AMIS30543::OVCXNT |
                AMIS30543::OVCXPB |
                AMIS30543::OVCXPT |
                AMIS30543::OVCYNB |
                AMIS30543::OVCYNT |
                AMIS30543::OVCYPB |
                AMIS30543::OVCYPT
        )) {
            // Short detected, emergency shut down
            stepper.disableDriver();
            Serial.println("#   ERROR: Coil short");
            // Power down until reset
            noInterrupts();
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
            sleep_bod_disable();
            sleep_cpu();
        }
        if (unlatched & AMIS30543::TW)
            Serial.println("#   ERROR: Thermal shutdown");
        if (unlatched & AMIS30543::CPFAIL)
            Serial.println("#   ERROR: Charge pump failure");
    }
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

void StepperControl::rotate(const int degrees, const bool block,
                            const bool stallDetect) {
    bool direction = degrees > 0;
    noInterrupts();
    stepper.setDirection(direction);
    this->direction = direction;
    steps = (abs(degrees) * stepsPerRevolution) / 360U;
    this->stallDetect = stallDetect;
    start();
    interrupts();

    if (block) {
        while (running) {
//            Serial.print("emfAvg: ");
//            Serial.println(emfAvg);
        }
    }
//    delayMicroseconds(100);
//    Serial.print("final: ");
//    Serial.println(emfAvg);
}

void StepperControl::start() {
    if (!running) {
        // Reset emf average
        emfAvg = 1023;
        stepper.sleepStop();
        running = true;
        Timer1.resume();
    }
}

void StepperControl::stop() {
    running = false;
}

void StepperControl::stepISR() {
    if (instance->steps > 0) {
        instance->nxtPin.highI();
        if (instance->stallDetect &&
            ((instance->stepNum % STEP_MODE) == 0)) {
            // ADC read takes ~20us, giving us enough high time on the
            auto emf = (uint16_t) analogRead(instance->slaPin);
            // Filter emf
            instance->emfAvg = (instance->emfAvg * 2 + emf) / 3;
            if (instance->emfAvg < STALL_EMF) {
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
        Timer1.stop();
        instance->stepper.sleep();
    }
}