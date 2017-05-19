#ifndef DOORMAN_AUTHENTICATOR_H
#define DOORMAN_AUTHENTICATOR_H


#include <avr/io.h>

class Authenticator {

public:

    enum Command {
        NONE = 0,
        UNLOCK,
        LOCK,
        TOGGLE,
    };

    virtual void init() {};

    virtual Command getCommand() = 0;
};


#endif //DOORMAN_AUTHENTICATOR_H
