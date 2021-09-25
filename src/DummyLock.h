#ifndef DOORMAN_DUMMYLOCK_H
#define DOORMAN_DUMMYLOCK_H

#include "Lock.h"

class DummyLock : public Lock {
    void lock() override;

    void unlock() override;
};

#endif  // DOORMAN_DUMMYLOCK_H
