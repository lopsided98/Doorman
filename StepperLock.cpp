#include "StepperLock.h"

StepperLock::StepperLock(StepperControl &stepper) : stepper(stepper) {

}

void StepperLock::init() {
}

void StepperLock::lock() {
    Lock::lock();
    stepper.rotateUntilStall(true, true, 3000);
}

void StepperLock::unlock() {
    Lock::unlock();
    stepper.rotate(-180, true);
}