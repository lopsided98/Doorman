#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "StepperLock.h"
#include "RFIDAuthenticator.h"
#include "SerialAuthenticator.h"
#include "ButtonAuthenticator.h"

static const unsigned int STEPPER_SPEED = 700;
static const unsigned int STEPPER_CURRENT = 1800;
static const unsigned int STEPPER_STEPS_PER_REVOLUTION = 200;
static const uint8_t STEPPER_SS_PIN = 10;
static const uint8_t STEPPER_NXT_PIN = 8;
static const uint8_t STEPPER_SLA_PIN = A1;

AMIS30543 stepperDriver;
StepperControl stepperControl(stepperDriver, STEPPER_NXT_PIN, STEPPER_SLA_PIN,
                              STEPPER_STEPS_PER_REVOLUTION);
StepperLock lock(stepperControl);

RFIDAuthenticator rfidAuthenticator(2, 3);
SerialAuthenticator serialAuthenticator(lock);
ButtonAuthenticator buttonAuthenticator;
Authenticator *authenticators[] = {&rfidAuthenticator,
                                   &serialAuthenticator,
                                   &buttonAuthenticator};

void sleep();

void setup() {
    Serial.begin(9600);

    Serial.println("# Starting...");

    for (auto &authenticator : authenticators) {
        authenticator->init();
    }

    SPI.begin();
    stepperDriver.init(STEPPER_SS_PIN);
    // Wait for driver to start
    delay(1);

    // Initialize driver
    stepperDriver.resetSettings();
    stepperDriver.setCurrentMilliamps(STEPPER_CURRENT);

    // Initialize control algorithm
    stepperControl.init();
    stepperControl.setSpeed(STEPPER_SPEED);
    lock.init();

    // Power saving functions
    power_timer2_disable();
    power_twi_disable();
    set_sleep_mode(SLEEP_MODE_IDLE);

    Serial.println("# Initialization complete.");
    delay(100);
}

void loop() {
    sleep();
    for (auto &authenticator : authenticators) {
        switch (authenticator->getCommand()) {
            case Authenticator::Command::TOGGLE:
                Serial.println("# Received command: toggle");
                lock.toggle();
                break;
            case Authenticator::Command::LOCK:
                Serial.println("# Received command: lock");
                lock.lock();
                break;
            case Authenticator::Command::UNLOCK:
                Serial.println("# Received command: unlock");
                lock.unlock();
                break;
            default:
                break;
        }
    }
}

void sleep() {
    noInterrupts();
    sleep_enable();
    sleep_bod_disable();
    interrupts();
    sleep_cpu();
    sleep_disable();
}
