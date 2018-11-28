#ifndef DOORMAN_DUMMYLOCK_H
#define DOORMAN_DUMMYLOCK_H

#include "Lock.h"

class DummyLock : public Lock {

    virtual void lock();

    virtual void unlock();
};


#endif //DOORMAN_DUMMYLOCK_H
