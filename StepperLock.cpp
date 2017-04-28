#include "StepperLock.h"

StepperLock::StepperLock(StepperControl &stepper) : stepper(stepper) {

}

void StepperLock::init() {
}

void StepperLock::lock() {
    Lock::lock();
    stepper.rotate(540, true, true);
}

void StepperLock::unlock() {
    Lock::unlock();
    stepper.rotate(-540, true, true);
}