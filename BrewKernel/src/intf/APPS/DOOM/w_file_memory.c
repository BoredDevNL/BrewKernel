//
// w_file_memory.c
// In-memory WAD file handler for embedded WAD data
//
// Provides a WAD file interface for reading WAD data that is
// already loaded in memory (e.g., embedded in the binary).
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "doomtype.h"
#include "w_file.h"

// Forward declaration of embedded WAD data
// These are linker symbols that mark the start and end of the embedded binary
extern unsigned char _binary_DOOM_WAD_start[];
extern unsigned char _binary_DOOM_WAD_end[];

typedef struct {
    wad_file_t base;
    unsigned char *data;
    size_t size;
} memory_wad_file_t;

static wad_file_t *W_OpenFileMemory(char *path)
{
    memory_wad_file_t *result;

    // Only open if the path matches our embedded WAD name
    if (path == NULL || 
        (strcmp(path, "DOOM.WAD") != 0 && 
         strcmp(path, "DOOM") != 0 &&
         strcmp(path, "doom.wad") != 0 &&
         strcmp(path, "doom") != 0))
    {
        printf("[w_file_memory] Not a WAD request: %s\n", path ? path : "NULL");
        return NULL;
    }

    printf("[w_file_memory] Opening embedded WAD: %s\n", path);
    
    // The linker symbols themselves are memory addresses
    unsigned char *start = _binary_DOOM_WAD_start;
    unsigned char *end = _binary_DOOM_WAD_end;
    size_t wad_size = (size_t)(end - start);
    
    printf("[w_file_memory] WAD data at %p - %p, size=%zu bytes\n", 
           (void*)start, (void*)end, wad_size);
           
    if (wad_size == 0)
    {
        printf("[w_file_memory] ERROR: Embedded WAD is empty (size=0)!\n");
        return NULL;
    }

    result = (memory_wad_file_t *)malloc(sizeof(memory_wad_file_t));
    
    if (result == NULL)
    {
        printf("[w_file_memory] ERROR: Could not allocate memory_wad_file_t\n");
        return NULL;
    }

    result->data = start;
    result->size = wad_size;

    printf("[w_file_memory] SUCCESS: Embedded WAD loaded, size=%zu bytes\n", result->size);

    result->base.file_class = NULL;  // Will be set by caller
    result->base.mapped = result->data;
    result->base.length = (unsigned int)result->size;

    return (wad_file_t *)result;
}

static void W_CloseFileMemory(wad_file_t *wad)
{
    // Don't free the data - it's part of the binary
    // Just free the wrapper struct
    free(wad);
}

static size_t W_ReadMemory(wad_file_t *wad, unsigned int offset,
                           void *buffer, size_t buffer_len)
{
    memory_wad_file_t *mem_wad;
    size_t to_read;

    if (wad == NULL || buffer == NULL)
    {
        return 0;
    }

    mem_wad = (memory_wad_file_t *)wad;

    // Clamp read to available data
    if (offset >= mem_wad->size)
    {
        printf("[w_file_memory] Read: offset %u >= size %zu\n", offset, mem_wad->size);
        return 0;
    }

    to_read = mem_wad->size - offset;
    
    if (to_read > buffer_len)
    {
        to_read = buffer_len;
    }

    // Debug: Check what we're reading
    if (offset == 0 && to_read >= 4)
    {
        printf("[w_file_memory] Read header: %.4s\n", (char*)mem_wad->data);
    }

    memcpy(buffer, mem_wad->data + offset, to_read);

    return to_read;
}

wad_file_class_t memory_wad_file = {
    W_OpenFileMemory,
    W_CloseFileMemory,
    W_ReadMemory,
};
