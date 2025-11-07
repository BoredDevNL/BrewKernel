#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void* fs_allocate(size_t size);
void fs_free(void* ptr);

#endif