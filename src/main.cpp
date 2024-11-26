#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../lib/console.h"
#include "../test/Threads_C_API_test.hpp"

void userMain();
void userMainWrapper(void* arg) {
    userMain();
}

int main()
{
    //stvec: adresa prekidne rutine, poravnata na 4 bajta;
    RiscV::w_stvec((uint64)&RiscV::supervisorTrap);
    MemoryAllocator::initFreeBlock();

    TCB* threads[2];
    //uvek u scheduleru postoji main nit koja se ne moze izbaciti iz njega
    thread_create(&threads[0], nullptr, nullptr);
    TCB::running = threads[0];

    thread_create(&threads[1], userMainWrapper, nullptr);

    printString("UserMain started\n");

    while (!(threads[1]->isFinished())) {
        thread_dispatch();
    }

//    for (auto &coroutine: threads) {
//        delete coroutine;
//    }
    //printString("Finished\n");

    return 0;
}