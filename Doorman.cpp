#include <Arduino.h>
#include <avr/power.h>
#include "StepperLock.h"
#include "RFIDAuthenticator.h"

static const unsigned int STEPPER_CURRENT = 500;
static const unsigned int STEPPER_STEPS_PER_REVOLUTION = 200;
static const uint8_t STEPPER_SS_PIN = 10;
static const uint8_t STEPPER_NXT_PIN = 8;
static const uint8_t STEPPER_SLA_PIN = A1;

RFIDAuthenticator authenticator(3, 2);

AMIS30543 stepperDriver;
StepperControl stepperControl(stepperDriver, STEPPER_NXT_PIN, STEPPER_SLA_PIN,
                              STEPPER_STEPS_PER_REVOLUTION);
StepperLock lock(stepperControl);

void setup() {
    Serial.begin(9600);

    Serial.println("Starting...");

    authenticator.init();

    SPI.begin();
    stepperDriver.init(STEPPER_SS_PIN);
    // Wait for driver to start
    delay(1);

    // Initialize driver
    stepperDriver.resetSettings();
    stepperDriver.setCurrentMilliamps(STEPPER_CURRENT);

    // Initialize control algorithm
    stepperControl.init();
    stepperControl.setSpeed(1000);
    lock.init();

    // Power saving functions
    power_timer2_disable();
    power_twi_disable();

    Serial.println("Initialization complete.");
}

void loop() {
    if (authenticator.waitForAttempt()) {
        lock.lock();
        delay(500);
//        lock.unlock();
    }
}