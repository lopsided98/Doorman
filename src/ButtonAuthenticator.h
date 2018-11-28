#pragma once

#include "Authenticator.h"

class ButtonAuthenticator : public Authenticator {
public:

    ButtonAuthenticator();

    Command getCommand() override;

private:

};
