
#ifndef DOORMAN_RFIDAUTHENTICATOR_H
#define DOORMAN_RFIDAUTHENTICATOR_H

#include <SeeedRFID.h>
#include <SoftwareSerial.h>

#include "Authenticator.h"
#include "ButtonAuthenticator.h"

class RFIDAuthenticator : public Authenticator {
public:
    RFIDAuthenticator(uint8_t rx, uint8_t tx, ButtonAuthenticator& button);

    Command getCommand() override;

private:
    SeeedRFID rfid;
    ButtonAuthenticator& button;

    bool enrollID(uint32_t id);
    bool checkID(uint32_t id);
};

#endif  // DOORMAN_RFIDAUTHENTICATOR_H
