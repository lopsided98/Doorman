#include "RFIDAuthenticator.h"

RFIDAuthenticator::RFIDAuthenticator(const uint8_t rx, const uint8_t tx) :
        rfid(rx, tx) {

}

bool RFIDAuthenticator::waitForAttempt() {
    while (true) {
        if (rfid.isIdAvailable()) {
            RFIDTag tag = rfid.readId();
            Serial.print("RFID card number: ");
            Serial.println(tag.id);
            break;
        }
    }
    return true;
}
