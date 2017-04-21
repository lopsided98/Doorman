#ifndef DOORMAN_STEPPERLOCK_H
#define DOORMAN_STEPPERLOCK_H

#include <Arduino.h>
#include <AMIS30543.h>
#include "Lock.h"

class StepperLock : public Lock {
public:
    StepperLock(const uint8_t ssPin,
                const uint8_t nxtPin,
                const uint8_t slaPin);

    virtual void init();

    virtual void lock();

    virtual void unlock();

private:
    AMIS30543 stepper;
    const uint8_t nxtPin;
    const uint8_t ssPin;
    const uint8_t slaPin;

    void rotate(int degrees);
};

#endif //DOORMAN_STEPPERLOCK_H
