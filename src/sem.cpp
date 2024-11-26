#include "../h/sem.hpp"
#include "../h/syscall_c.h"

int Sem::wait() {
    if(this->closed) {
        return -1;
    }

    if(--this->value < 0) {
        this->blocked.addLast(TCB::running);
        TCB::running->setBlocked(true);
        TCB::dispatch();
    }
    return 0;
}

int Sem::signal() {
    if(this->closed) {
        return -1;
    }
    if(++this->value <= 0) {
        TCB* tcb;
        tcb = this->blocked.removeFirst();
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }
    return 0;
}

int Sem::tryWait() {
    if (this->value - 1 >= 0) {
        this->value--;
        return 0;
    }
    return 1;
}

Sem *Sem::createSemaphore(unsigned int init) {
    return new Sem(init);
}

int Sem::close() {
    if (closed) {
        return -1;
    }
    closed = true;

    while (this->blocked.peekFirst()) {
        TCB* tcb = this->blocked.removeFirst();
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }
    return 0;
}
