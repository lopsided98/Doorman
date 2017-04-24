
#ifndef DOORMAN_RFIDAUTHENTICATOR_H
#define DOORMAN_RFIDAUTHENTICATOR_H

#include <SoftwareSerial.h>
#include <SeeedRFIDLib.h>
#include "Authenticator.h"

class RFIDAuthenticator : public Authenticator {
public:

    RFIDAuthenticator(const uint8_t rx, const uint8_t tx);

    virtual bool waitForAttempt();

private:
    SeeedRFIDLib rfid;
};


#endif //DOORMAN_RFIDAUTHENTICATOR_H
