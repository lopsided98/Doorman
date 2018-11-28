#ifndef DOORMAN_LOCK_H
#define DOORMAN_LOCK_H

class Lock {
public:
    virtual void init() {};

    virtual void unlock();

    virtual void lock();

    void toggle();

    virtual bool isLocked();

protected:

    bool locked = false;
};

#endif // DOORMAN_LOCK_H
