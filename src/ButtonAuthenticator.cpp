#include <EEPROM.h>
#include "ButtonAuthenticator.h"

ButtonAuthenticator::ButtonAuthenticator() = default;

Authenticator::Command ButtonAuthenticator::getCommand() {
    return NONE;
}