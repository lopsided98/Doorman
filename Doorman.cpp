#include <Arduino.h>
#include <avr/sleep.h>
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

void sleep();

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

    set_sleep_mode(SLEEP_MODE_IDLE);

    Serial.println("Initialization complete.");
    delay(100);
}

void loop() {
    sleep();
    Serial.println("test");
    if (authenticator.waitForAuthentication()) {
        lock.toggle();
    }
    delayMicroseconds(100);
}

void sleep() {
    noInterrupts();
    sleep_enable();
    interrupts();
    sleep_cpu();
    sleep_disable();
}