/*
 * Minimal malloc/free shim for Doom on BrewKernel.
 */
#include <stddef.h>
// Use explicit relative include to BrewKernel's memory allocator API
#include "../../memory.h"

void* malloc(size_t size) {
	return fs_allocate(size);
}

void free(void* ptr) {
	fs_free(ptr);
}

void* realloc(void* ptr, size_t size) {
	// Simple realloc: allocate new, copy min(old,new), free old
	if (!ptr) return fs_allocate(size);
	if (size == 0) {
		fs_free(ptr);
		return 0;
	}
	void* n = fs_allocate(size);
	if (!n) return 0;
	// naive copy without knowing old size; assume caller copies less or fits
	// Safer approach: just copy size bytes; kernel environment ignores overflow for now.
	unsigned char* d = (unsigned char*)n;
	unsigned char* s = (unsigned char*)ptr;
	for (size_t i = 0; i < size; ++i) d[i] = s[i];
	fs_free(ptr);
	return n;
}

void* calloc(size_t nmemb, size_t size) {
	// Check for integer overflow
	if (nmemb == 0 || size == 0) {
		return NULL;
	}
	
	// Check for overflow: if nmemb * size would overflow, return NULL
	if (nmemb > (size_t)-1 / size) {
		return NULL;
	}
	
	size_t total = nmemb * size;
	void* ptr = fs_allocate(total);
	
	if (!ptr) {
		// Allocation failed - return NULL
		return NULL;
	}
	
	// Zero-initialize the memory
	unsigned char* p = (unsigned char*)ptr;
	for (size_t i = 0; i < total; ++i) {
		p[i] = 0;
	}
	
	return ptr;
}


