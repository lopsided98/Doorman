#include "StepperLock.h"

StepperLock::StepperLock(StepperControl &stepper) : stepper(stepper) {

}

void StepperLock::init() {
}

void StepperLock::lock() {
    Lock::lock();
    stepper.rotateUntilStall(true, true, 1500);
}

void StepperLock::unlock() {
    Lock::unlock();
    stepper.rotateUntilStall(false, true, 1500);
}