#include "file.h"
#include "print.h"
#include "memory.h"
#include <stddef.h>

// Helper function to copy memory
static void fs_memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while (n--) *d++ = *s++;
}

File* create_file(const char* name, char type) {
    static File files[FS_MAX_FILES];
    static size_t file_count = 0;
    
    if (file_count >= FS_MAX_FILES) {
        return NULL;
    }

    File* file = &files[file_count++];
    
    // Copy name with bounds checking
    size_t i;
    for (i = 0; name[i] && i < FS_MAX_FILENAME - 1; i++) {
        file->name[i] = name[i];
    }
    file->name[i] = '\0';
    
    file->type = type;
    file->parent = NULL;
    file->child_count = 0;
    file->children = NULL;
    file->next_sibling = NULL;
    file->content = NULL;
    file->content_size = 0;
    
    return file;
}

bool file_write_content(File* file, const char* content, size_t size) {
    if (!file || file->type != 'f' || size > FS_MAX_FILE_SIZE) {
        return false;
    }

    // If file already has content, free it
    if (file->content) {
        fs_free(file->content);
        file->content = NULL;
        file->content_size = 0;
    }

    // Allocate new content buffer
    char* new_content = fs_allocate(size);
    if (!new_content) {
        return false;
    }

    // Copy content
    fs_memcpy(new_content, content, size);
    file->content = new_content;
    file->content_size = size;

    return true;
}

const char* file_get_content(const File* file, size_t* size) {
    if (!file || file->type != 'f' || !size) {
        return NULL;
    }

    *size = file->content_size;
    return file->content;
}

void cleanup_filesystem(void) {
    // Currently does nothing since we use static allocation
    // Could be extended to support dynamic allocation in the future
}