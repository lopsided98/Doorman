#ifndef DOORMAN_LOCK_H
#define DOORMAN_LOCK_H

class Lock {
public:
    virtual void init();

    virtual void unlock() = 0;

    virtual void lock() = 0;
};

#endif // DOORMAN_LOCK_H
