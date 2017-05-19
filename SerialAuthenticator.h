#ifndef DOORMAN_SERIALAUTHENTICATOR_H
#define DOORMAN_SERIALAUTHENTICATOR_H

#include "Authenticator.h"

class SerialAuthenticator : public Authenticator {
    virtual Command getCommand();
};


#endif //DOORMAN_SERIALAUTHENTICATOR_H
