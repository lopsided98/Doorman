#ifndef DOORMAN_SERIALAUTHENTICATOR_H
#define DOORMAN_SERIALAUTHENTICATOR_H

#include "Authenticator.h"
#include "Lock.h"

class SerialAuthenticator : public Authenticator {

public:

    explicit SerialAuthenticator(Lock &lock);

    Command getCommand() override;

private:

    unsigned int commandBufferPos{0};
    char commandBuffer[50];

    bool wasLocked{false};
    Lock& lock;

    Command parseLockCommand(char *buffer);

    void printStatus();

};


#endif //DOORMAN_SERIALAUTHENTICATOR_H
