
#ifndef DOORMAN_RFIDAUTHENTICATOR_H
#define DOORMAN_RFIDAUTHENTICATOR_H

#include <SoftwareSerial.h>
#include <SeeedRFID.h>
#include "Authenticator.h"

class RFIDAuthenticator : public Authenticator {
public:

    RFIDAuthenticator(uint8_t rx, uint8_t tx);

    virtual Command getCommand();

private:
    SeeedRFID rfid;

    bool enrollID(uint32_t id);
    bool checkID(uint32_t id);

};


#endif //DOORMAN_RFIDAUTHENTICATOR_H
