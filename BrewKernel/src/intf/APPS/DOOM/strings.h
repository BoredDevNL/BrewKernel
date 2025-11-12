#pragma once

#include <stddef.h>

// Legacy BSD strings.h API used by some Doom sources
void bzero(void* s, size_t n);
void bcopy(const void* src, void* dest, size_t n);
int  bcmp(const void* s1, const void* s2, size_t n);

char* index(const char* s, int c);
char* rindex(const char* s, int c);

// Some systems also declare these here; provide prototypes to match usage.
int strcasecmp(const char* s1, const char* s2);
int strncasecmp(const char* s1, const char* s2, size_t n);


