//
// Created by os on 8/26/24.
//

#include "../h/memoryAllocator.hpp"


MemoryAllocator::Block* MemoryAllocator::freeHead = nullptr;
MemoryAllocator::Block* MemoryAllocator::usedHead = nullptr;

void *MemoryAllocator::mem_alloc(size_t size) {
    size_t newSize = size + sizeof(Block); // u bajtovima
//    if(newSize % MEM_BLOCK_SIZE != 0) {
//        newSize = ((size + sizeof(Block) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;
//    }

    for(Block* curr = MemoryAllocator::freeHead; curr != nullptr; curr=curr->next) {
        if(curr->size < newSize) continue;
        if(curr->size > newSize) {

            Block *newFreeBlock = (Block*) ((char*)curr + newSize + sizeof(Block));

            // azuriranje free liste
            if(curr->prev) curr->prev->next = newFreeBlock;
            else MemoryAllocator::freeHead = newFreeBlock;
            if(curr->next) curr->next->prev = newFreeBlock;
            newFreeBlock->prev = curr->prev;
            newFreeBlock->next = curr->next;

            newFreeBlock->size = curr->size - newSize - sizeof(Block);
            curr->size = newSize;

            //azuriranje USED liste

            Block* iter, *prev = nullptr;
            for (iter = usedHead; iter && iter < curr; prev = iter, iter = iter->next);

            if (prev) prev->next = curr;
            else usedHead = curr;
            curr->prev = prev;
            if (iter) iter->prev = curr;
            curr->next = iter;
        }
        else {
            // iste su velicine
            //Update FREE list
            if (curr->prev) curr->prev->next = curr->next;
            else MemoryAllocator::freeHead = curr->next;

            if(curr->next) curr->next->prev = curr->prev;

            //Update USED list
            Block* iter, *prev = nullptr;
            for (iter = usedHead; iter && iter < curr; prev = iter, iter = iter->next);

            if (prev) prev->next = curr;
            else usedHead = curr;
            curr->prev = prev;
            if (iter) iter->prev = curr;
            curr->next = iter;
        }
        return (char*)curr + sizeof(Block);
    }
    return nullptr; //should not enter here
}

int MemoryAllocator::mem_free(void* ptr) {
    if(usedHead == nullptr) return -1;
    if(ptr == nullptr || ptr < HEAP_START_ADDR || ptr > HEAP_END_ADDR) return -2;

    Block* curr = (Block*)((char*)ptr - sizeof(Block));
    if(curr < usedHead) return -3;

    //Delete from USED list
    if(usedHead == curr) {
        usedHead = curr->next;
        if(usedHead) usedHead->prev = nullptr;
        curr->next = nullptr;
        curr->prev = nullptr;
    } else {
        curr->prev->next = curr->next;
        if(curr->next) curr->next->prev = curr->prev;
        curr->next = nullptr;
        curr->prev = nullptr;
    }

    //Insert into FREE list
    Block* iter;
    Block* prev = nullptr;
    for(iter = freeHead; iter && iter < curr; prev = iter, iter = iter->next);

    if (prev) prev->next = curr;
    else freeHead = curr;
    curr->prev = prev;
    if (iter) iter->prev = curr;
    curr->next = iter;
    tryJoining(curr);
    if (prev) tryJoining(prev);

    return 0;
}

void MemoryAllocator::tryJoining(Block *curr) {
    if(curr->next && (char*)curr + sizeof(Block) + curr->size == (char*)curr->next) {
        curr->size += curr->next->size + sizeof(Block);
        curr->next = curr->next->next;
        if(curr->next) curr->next->prev = curr;
    }
}