#ifndef DOORMAN_STEPPERCONTROL_H
#define DOORMAN_STEPPERCONTROL_H

#include <DigitalIO.h>
#include <AMIS30543.h>

class StepperControl {
public:

    StepperControl(AMIS30543 &stepper,
                   uint8_t nxtPin, uint8_t slaPin,
                   unsigned int stepsPerRevolution);

    void init();

    /**
     * Set the speed of the stepper in degrees/sec.
     *
     * @param speed speed in degrees/sec
     */
    void setSpeed(unsigned int speed);

    void rotate(int degrees, bool block = true,
                bool stallDetect = false);

private:

    static StepperControl *instance;

    AMIS30543 &stepper;

    const uint8_t slaPin;
    PinIO nxtPin;

    const uint32_t stepsPerRevolution;

    volatile bool running = false;
    volatile uint32_t steps = 0;
    volatile bool direction = false;
    volatile uint8_t stepNum = 0;
    volatile bool stallDetect = false;
    volatile uint16_t emfAvg = 1023;

    void start();

    void stop();

    static void stepISR();
};

#endif //DOORMAN_STEPPERCONTROL_H
