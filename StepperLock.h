#ifndef DOORMAN_STEPPERLOCK_H
#define DOORMAN_STEPPERLOCK_H

#include <Arduino.h>
#include <AMIS30543.h>
#include "Lock.h"
#include "StepperControl.h"

class StepperLock : public Lock {
public:
    StepperLock(StepperControl &stepper);

    virtual void init();

    virtual void lock();

    virtual void unlock();

private:
    StepperControl &stepper;
};

#endif //DOORMAN_STEPPERLOCK_H
