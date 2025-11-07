#include "memory.h"

// Simple memory management for file content

#define MEMORY_SIZE 65536 
static char memory_pool[MEMORY_SIZE];
static size_t memory_used = 0;

void* fs_allocate(size_t size) {
    if (memory_used + size > MEMORY_SIZE) {
        return NULL;  // Out of memory
    }

    void* ptr = &memory_pool[memory_used];
    memory_used += size;
    return ptr;
}

void fs_free(void* ptr) {

}