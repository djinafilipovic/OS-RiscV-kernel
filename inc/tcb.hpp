#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
//#include "memoryAllocator.hpp"
#include "../lib/mem.h"

// Thread Control Block
class TCB
{
public:
    ~TCB() { if (stack) delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    bool isBlocked() const { return blocked; }

    void setBlocked(bool value) { blocked = value; }

    bool isMain() const { return this->main; }

    using Body = void (*)(void*);

    static TCB *createThread(Body body, void* arg, uint8* stack);

    static TCB *running;

//    void* operator new(size_t size) {
//        return __mem_alloc(size);
//    }
//    void* operator new[](size_t size) {
//        return __mem_alloc(size);
//    }
//
//    void operator delete(void *ptr) {
//        __mem_free(ptr);
//    }
//    void operator delete[](void *ptr) {
//        __mem_free(ptr);
//    }


private:
    TCB(Body body, void* arg, uint8* stack) :
            body(body),
            stack(body != nullptr ? stack : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            finished(false),
            blocked(false),
            main(body == nullptr),
            arg(arg)
    {}

    struct Context
    {
        uint64 ra;
        uint8* sp;

//        void* operator new(size_t size) {
//            return __mem_alloc(size);
//        }
//        void* operator new[](size_t size) {
//            return __mem_alloc(size);
//        }
//        void operator delete(void *ptr) {
//            __mem_free(ptr);
//        }
//        void operator delete[](void *ptr) {
//            __mem_free(ptr);
//        }
    };

    Body body;
    uint8* stack;
    Context context;
    bool finished;
    bool blocked;
    bool main;
    void* arg;
    List<TCB> joined;

    friend class RiscV;
    friend class Sem;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static void join(TCB* handle);
    void releaseAll();
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP