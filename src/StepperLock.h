#ifndef DOORMAN_STEPPERLOCK_H
#define DOORMAN_STEPPERLOCK_H

#include <Arduino.h>
#include <AMIS30543.h>
#include "Lock.h"
#include "StepperControl.h"

class StepperLock : public Lock {
public:
    explicit StepperLock(StepperControl &stepper);

    void init() override;

    void lock() override;

    void unlock() override;

private:
    StepperControl &stepper;
};

#endif //DOORMAN_STEPPERLOCK_H
