#ifndef DOORMAN_SERIALAUTHENTICATOR_H
#define DOORMAN_SERIALAUTHENTICATOR_H

#include "Authenticator.h"
#include "Lock.h"

#define SERIAL_AUTHENTICATOR_BUFFER_LENGTH 50

class SerialAuthenticator : public Authenticator {

public:

    explicit SerialAuthenticator(Lock &lock);

    void init() override;

    Command getCommand() override;

private:

    unsigned int commandBufferPos = 0;
    char commandBuffer[SERIAL_AUTHENTICATOR_BUFFER_LENGTH];

    bool wasLocked = false;
    Lock& lock;

    Command parseLockCommand(char *buffer);

    void printStatus();

};


#endif //DOORMAN_SERIALAUTHENTICATOR_H
