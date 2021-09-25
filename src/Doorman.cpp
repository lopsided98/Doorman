#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include "ButtonAuthenticator.h"
#include "RFIDAuthenticator.h"
#include "SerialAuthenticator.h"
#include "StepperLock.h"

namespace {

const unsigned int STEPPER_SPEED{700};
const unsigned int STEPPER_CURRENT{1800};
const unsigned int STEPPER_STEPS_PER_REVOLUTION{200};
const uint8_t STEPPER_SS_PIN{10};
const uint8_t STEPPER_NXT_PIN{8};
const uint8_t STEPPER_SLA_PIN{A1};
const uint8_t RFID_RX_PIN{2};
const uint8_t RFID_TX_PIN{3};
const uint8_t BUTTON_PIN{7};

AMIS30543 stepperDriver;
StepperControl stepperControl{stepperDriver, STEPPER_NXT_PIN, STEPPER_SLA_PIN,
                              STEPPER_STEPS_PER_REVOLUTION};
StepperLock lock{stepperControl};

SerialAuthenticator serialAuthenticator{lock};
ButtonAuthenticator buttonAuthenticator{BUTTON_PIN};
RFIDAuthenticator rfidAuthenticator{RFID_RX_PIN, RFID_TX_PIN,
                                    buttonAuthenticator};
Authenticator *authenticators[]{
    &serialAuthenticator,
    &buttonAuthenticator,
    &rfidAuthenticator,
};

}  // namespace

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
    delayMicroseconds(1000);

    // Initialize control algorithm
    stepperControl.init();
    stepperControl.setCurrent(STEPPER_CURRENT);
    stepperControl.setSpeed(STEPPER_SPEED);
    lock.init();

    // Power saving functions
    power_timer2_disable();
    power_twi_disable();
    set_sleep_mode(SLEEP_MODE_IDLE);

    Serial.println("# Initialization complete");
    delay(100);

    // Clear MCU Status Register (reset status)
    MCUSR = 0;
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
    interrupts();
    sleep_cpu();
    sleep_disable();
}
