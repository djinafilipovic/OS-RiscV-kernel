#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
public:

    struct Block {
        size_t size;
        Block* next;
        Block* prev;
    };

    static Block* freeHead;
    static Block* usedHead;

    static void* mem_alloc(size_t size);

    static int mem_free (void* ptr);

    static void tryJoining(Block* curr);

    static void initFreeBlock() {

        MemoryAllocator::freeHead = (Block*)((char*)HEAP_START_ADDR);
        usedHead = nullptr;

        freeHead->next = nullptr;
        freeHead->prev  = nullptr;
        freeHead->size = ((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - sizeof(Block));

    }
};

#endif //MEMORYALLOCATOR_HPP
