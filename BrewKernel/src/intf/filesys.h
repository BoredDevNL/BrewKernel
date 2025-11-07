#ifndef FILESYS_H
#define FILESYS_H

#include "file.h"
#include <stdbool.h>

// Initialize the filesystem
void fs_init(void);

// List contents of current directory
void fs_list_directory(void);

// Change current directory
// Returns true on success, false on failure
bool fs_change_directory(const char* path);

// Get current working directory path
const char* fs_get_working_directory(void);

// Create a new directory in the current directory
bool fs_create_directory(const char* name);

// List contents of the directory at the given path (absolute or relative)
// Returns true on success, false if the path does not resolve to a directory
bool fs_list_directory_at_path(const char* path);

// Print the current working directory to the console (with a trailing newline)
void fs_print_working_directory(void);

// Create a new file in the current directory
// Returns the created file on success, NULL on failure
File* fs_create_file(const char* name);

// Find a file in the current directory by name
// Returns NULL if not found
File* fs_find_file(const char* name);

// Create a directory at the specified path (absolute or relative)
// Returns true on success, false on failure
bool fs_create_directory_at_path(const char* path);

#endif