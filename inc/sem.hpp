#ifndef SEM_HPP
#define SEM_HPP

#include "list.hpp"
#include "tcb.hpp"
#include "memoryAllocator.hpp"

class Sem {
public:
    explicit Sem(unsigned init = 1) {
        this->value = init;
        this->closed = false;
    }

    int wait();
    int signal();
    int tryWait();
    static Sem* createSemaphore(unsigned init = 1);
    unsigned getValue() const { return value; }
    int close();

//    void* operator new(size_t size) {
//        return MemoryAllocator::mem_alloc(size);
//    }
//    void* operator new[](size_t size) {
//        return MemoryAllocator::mem_alloc(size);
//    }
//    void operator delete(void *ptr) {
//        MemoryAllocator::mem_free(ptr);
//    }
//    void operator delete[](void *ptr) {
//        MemoryAllocator::mem_free(ptr);
//    }
//    void* operator new(size_t size) {
//        return __mem_alloc(size);
//    }
//    void* operator new[](size_t size) {
//        return __mem_alloc(size);
//    }
//    void operator delete(void *ptr) {
//        __mem_free(ptr);
//    }
//    void operator delete[](void *ptr) {
//        __mem_free(ptr);
//    }

private:
    bool closed;
    int value;
    List<TCB> blocked;
};

#endif //SEM_HPP