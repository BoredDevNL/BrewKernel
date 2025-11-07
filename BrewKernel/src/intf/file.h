#ifndef FILE_H
#define FILE_H

#include <stddef.h>
#include <stdbool.h>

#define FS_MAX_FILENAME 256
#define FS_MAX_FILES 100
#define FS_MAX_FILE_SIZE 4096

typedef struct File {
    char name[FS_MAX_FILENAME];
    char type;              // 'd' for directory, 'f' for file
    struct File* parent;    // Parent directory
    size_t child_count;     // Number of children (for directories)
    struct File* children;  // First child in linked list
    struct File* next_sibling; // Next sibling in parent's children list
    char* content;          // File content (NULL for directories)
    size_t content_size;    // Size of content (0 for directories)
} File;

// File operations
File* create_file(const char* name, char type);
void cleanup_filesystem(void);
bool file_write_content(File* file, const char* content, size_t size);
const char* file_get_content(const File* file, size_t* size);

#endif