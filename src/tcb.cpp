//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../test/printing.hpp"
#include "../h/syscall_c.h"

TCB *TCB::running = nullptr;

TCB *TCB::createThread(Body body, void* arg, uint8* stack) {
    TCB* newTCB = new TCB(body, arg, stack);
    if (body) Scheduler::put(newTCB);
    return newTCB;
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    RiscV::popSppSpie();

    running->body(running->arg);
    thread_exit();
}

void TCB::join(TCB* handle) {
    if(!handle->isFinished()) {
        running->setBlocked(true);
        handle->joined.addLast(running);
    }
}

void TCB::releaseAll() {
    while (this->joined.peekFirst()) {
        TCB* tcb = this->joined.removeFirst();
        tcb->setBlocked(false);
        Scheduler::put(tcb);
    }
}