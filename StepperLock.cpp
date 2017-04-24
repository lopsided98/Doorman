#include "StepperLock.h"

StepperLock::StepperLock(StepperControl &stepper) : stepper(stepper) {

}

void StepperLock::init() {
}

void StepperLock::lock() {
    stepper.rotate(180, true);
}

void StepperLock::unlock() {
    stepper.rotate(-180, true);
}