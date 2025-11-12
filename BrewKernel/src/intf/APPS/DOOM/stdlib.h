#pragma once

#include <stddef.h>
#include <stdint.h>

void* malloc(size_t size);
void  free(void* ptr);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nmemb, size_t size);

void abort(void);
void exit(int code);

int atoi(const char* s);
long atol(const char* s);
long strtol(const char* nptr, char** endptr, int base);
unsigned long strtoul(const char* nptr, char** endptr, int base);

int abs(int x);
double atof(const char* s);

void srand(unsigned int seed);
int rand(void);

void qsort(void* base, size_t nmemb, size_t size,
           int (*compar)(const void*, const void*));

char* getenv(const char* name);
int putenv(char* string);
int system(const char* command);


