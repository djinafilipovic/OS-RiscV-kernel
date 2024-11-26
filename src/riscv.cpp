//
// Created by marko on 20.4.22..
//

#include "../lib/mem.h"
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/sem.hpp"
#include "../test/printing.hpp"

void RiscV::popSppSpie() {
    mc_sstatus(SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

using Body = void (*)(void*);

void RiscV::handleSupervisorTrap() {
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 volatile codeOperation;
        asm volatile("ld %0, 80(fp)" : "=r"(codeOperation));
        int returnValue;
        Sem* semHandlePtr;
        char c;
        switch (codeOperation) {
            case 0x01:
                //__mem_alloc(size)

                size_t size;
                void* ptr;
                asm volatile("ld %0, 88(fp)" : "=r"(size));
                ptr = MemoryAllocator::mem_alloc(size*MEM_BLOCK_SIZE);

                asm volatile("sd %0, 80(fp)" :: "r"(ptr));
                break;

            case 0x02:
                //mem_free(ptr)

                void* memptr;
                asm volatile("ld %0, 88(fp)" : "=r"(memptr));

                returnValue = MemoryAllocator::mem_free(memptr);

                asm volatile("sd %0, 80(fp)" :: "r"(returnValue));
                break;

                //THREAD DEO
            case 0x11:
                //thread_create
                TCB** tcb;
                Body body;
                void* arg;
                uint8* stack;
                asm volatile("ld %0, 88(fp)" : "=r"(tcb));
                asm volatile("ld %0, 96(fp)" : "=r"(body));
                asm volatile("ld %0, 104(fp)" : "=r"(arg));
                asm volatile("ld %0, 112(fp)" : "=r"(stack));
                *tcb = TCB::createThread(body, arg, stack);
                if(*tcb != nullptr) {
                    asm volatile("sd %0, 80(fp)" :: "r"(0));
                }
                else {
                    asm volatile("sd %0, 80(fp)" :: "r"(-1));
                }
                break;

            case 0x12:
                //thread_exit()
                TCB::running->setFinished(true);
                TCB::running->releaseAll();
                TCB::dispatch();
                asm volatile("sd %0, 80(fp)" :: "r"(0));
                break;

            case 0x13:
                //thread_dispatch()
                TCB::dispatch();
                break;

//            case 0x14:
//                //thread_join(handle)
//                thread_t handle;
//                asm volatile("ld %0, 88(fp)" : "=r"(handle));
//                TCB::join(handle);
//                TCB::dispatch();
//                break;

            case 0x21:
                //sem_open
                unsigned init;
                Sem** semHandle;

                asm volatile("ld %0, 88(fp)" : "=r"(semHandle));
                asm volatile("ld %0, 96(fp)" : "=r"(init));
                *semHandle = Sem::createSemaphore(init);

                if(*semHandle != nullptr) {
                    asm volatile("sd %0, 80(fp)" :: "r"(0));
                }
                else {
                    asm volatile("sd %0, 80(fp)" :: "r"(-1));
                }
                break;

            case 0x22:
                //sem_close

                asm volatile("ld %0, 88(fp)" : "=r"(semHandlePtr));
                if(semHandlePtr!= nullptr) {
                    returnValue = semHandlePtr->close();
                }
                else returnValue = -2;

                asm volatile("sd %0, 80(fp)" :: "r"(returnValue));
                break;

            case 0x23:
                //sem_wait
                asm volatile("ld %0, 88(fp)" : "=r"(semHandlePtr));
                if(semHandlePtr!= nullptr) {
                    returnValue = semHandlePtr->wait();
                }
                else returnValue = -2;

                asm volatile("sd %0, 80(fp)" :: "r"(returnValue));
                break;
            case 0x24:
                //sem_signal
                asm volatile("ld %0, 88(fp)" : "=r"(semHandlePtr));
                if(semHandlePtr!= nullptr)
                    returnValue = semHandlePtr->signal();
                else
                    returnValue = -2;

                asm volatile("sd %0, 80(fp)" :: "r"(returnValue));
                break;

            case 0x26:
                //sem_trywait
                asm volatile("ld %0, 88(fp)" : "=r"(semHandlePtr));
                if(semHandlePtr!= nullptr) {
                    returnValue = semHandlePtr->tryWait();
                }
                else returnValue = -2;

                asm volatile("sd %0, 80(fp)" :: "r"(returnValue));
                break;
            case 0x41:
                //getc
                c = __getc();
                asm volatile("sd %0, 80(fp)" :: "r"(c));
                break;
            case 0x42:
                asm volatile("ld %0, 88(fp)" : "=r"(c));
                __putc(c);
                break;
            default:
                break;
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    } else if (scause == 0x8000000000000001UL) {
        //interrupt: yes, cause code: supervisor software interrupt (timer)
        mc_sip(RiscV::SIP_SSIP);
    } else if (scause == 0x8000000000000009UL) {
        //interrupt: yes, cause code: supervisor external interrupt (console)
        console_handler();
    } else {
        // unexpected trap cause
        uint64 sepc = r_sepc();
        printString("ERROR! SCAUSE:");
        printInt(scause);
        printString(", SEPC:");
        printInt(sepc, 16);
        printString("\n");
        while(1) {};
    }
}
