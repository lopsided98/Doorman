#ifndef DOORMAN_AUTHENTICATOR_H
#define DOORMAN_AUTHENTICATOR_H


#include <avr/io.h>

class Authenticator {

public:

    virtual void init() {};

    virtual bool waitForAttempt() = 0;
};


#endif //DOORMAN_AUTHENTICATOR_H
