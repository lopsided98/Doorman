#include <TimerOne.h>
#include <wiring_private.h>
#include <avr/sleep.h>
#include "StepperControl.h"

static const unsigned int NXT_TIME = 3; // us
static const AMIS30543::stepMode STEP_MODE = AMIS30543::stepMode::MicroStep32;
static const uint16_t DEFAULT_CURRENT = 1800;
static const unsigned int DEFAULT_SPEED = 700;
static const uint16_t STALL_EMF = 125;

static const unsigned int SELF_TEST_CURRENT = 700;
static const unsigned int OVERCURRENT_FLAGS =
        AMIS30543::OVCXNB |
        AMIS30543::OVCXNT |
        AMIS30543::OVCXPB |
        AMIS30543::OVCXPT |
        AMIS30543::OVCYNB |
        AMIS30543::OVCYNT |
        AMIS30543::OVCYPB |
        AMIS30543::OVCYPT;

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
    stepper.resetSettings();
    stepper.stepOnRisingEdge();

    // Turn SLA transparency off to avoid spikes in the signal. I was going to
    // implement the SLA algorithm as described here:
    // http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.364.4376&rep=rep1&type=pdf
    // but for some reason I can't sample at the correct step consistently. The
    // current algorithm should work fine as long as the chip does not get too
    // hot and cause the signal to sag.
    stepper.setSlaTransparencyOff();

    stepper.enableDriver();

    // Run self test (will block if there is an error that could damage
    // hardware)
    selfTest();

    // Set after self test because test uses a different mode
    stepper.setStepMode(STEP_MODE);
    stepper.sleep();

    Timer1.initialize();
    setCurrent(DEFAULT_CURRENT);
    setSpeed(DEFAULT_SPEED);
    Timer1.stop();
    Timer1.attachInterrupt(StepperControl::stepISR);
}

void StepperControl::setCurrent(const uint16_t current) {
    stepper.setCurrentMilliamps(current);
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
            delayMicroseconds(NXT_TIME);
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

void StepperControl::selfTest() {
    // Overly in-depth self test routine

    uint16_t unlatched_errors = 0;
    uint16_t latched_errors = 0;
    int8_t position = -1;

    // Only run full self test on power on
    if (!bitRead(MCUSR, PORF)) {
        Serial.println("# Checking for motor errors...");
        unlatched_errors = stepper.readNonLatchedStatusFlags();
        latched_errors = stepper.readLatchedStatusFlagsAndClear();
    } else {
        Serial.println("# Running motor self test...");

        stepper.setCurrentMilliamps(SELF_TEST_CURRENT);

        stepper.setStepMode(AMIS30543::stepMode::CompensatedHalf);

        // Only runs on power on, so position will always be reset to 0
        position = 0;

        // Rotate through all electrical positions
        stepper.setDirection(true);
        for (uint8_t i = 0; i < 7; ++i) {
            nxtPin.high();
            delayMicroseconds(NXT_TIME);
            nxtPin.low();
            delayMicroseconds(50000);
            ++position;

            unlatched_errors |= stepper.readNonLatchedStatusFlags();
            latched_errors = stepper.readLatchedStatusFlagsAndClear();
            if (latched_errors) goto error;
        }

        // Rotate back to start
        stepper.setDirection(false);
        for (uint8_t i = 0; i < 7; --i) {
            nxtPin.high();
            delayMicroseconds(NXT_TIME);
            nxtPin.low();
            delayMicroseconds(50000);
            --position;

            unlatched_errors |= stepper.readNonLatchedStatusFlags();
            latched_errors = stepper.readLatchedStatusFlagsAndClear();
            if (latched_errors) goto error;
        }
    }
    error:

    if (unlatched_errors & AMIS30543::TW)
        Serial.println("#   WARNING: Thermal warning limit exceeded");
    if (unlatched_errors & AMIS30543::CPFAIL)
        Serial.println("#   ERROR: Charge pump failure");
    if (unlatched_errors & AMIS30543::OPENX)
        Serial.println("#   ERROR: Coil X open");
    if (unlatched_errors & AMIS30543::OPENY)
        Serial.println("#   ERROR: Coil Y open");

    if (latched_errors & AMIS30543::latchedStatusFlag::TSD)
        Serial.println("#   ERROR: Thermal shutdown");

    if (latched_errors & OVERCURRENT_FLAGS) {
        // Short detected, emergency shut down
        stepper.disableDriver();
        Serial.print("#   ERROR: Coil short, position: ");
        if (position == -1) {
            Serial.print("unknown");
        } else {
            Serial.print(position);
        }
        Serial.print(", flags: 0x");
        Serial.println(latched_errors, 16);
        Serial.println("#   Shutting down");
        Serial.flush();
        // Power down until reset
        noInterrupts();
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_bod_disable();
        sleep_cpu();
    }
}