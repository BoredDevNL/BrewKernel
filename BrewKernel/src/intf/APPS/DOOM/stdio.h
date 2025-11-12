#pragma once

#include <stdarg.h>
#include <stddef.h>

typedef struct __bk_file FILE;
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int printf(const char* fmt, ...);
int fprintf(FILE* stream, const char* fmt, ...);
int vfprintf(FILE* stream, const char* fmt, va_list ap);
int puts(const char* s);
int putchar(int c);
int sscanf(const char* str, const char* fmt, ...);
int snprintf(char* buf, size_t buf_len, const char* fmt, ...);
int vsnprintf(char* buf, size_t buf_len, const char* fmt, va_list ap);

// File I/O
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* f);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* f);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* f);
long ftell(FILE* f);
int fseek(FILE* f, long offset, int whence);
int fflush(FILE* f);
int remove(const char* path);
int rename(const char* oldpath, const char* newpath);

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

void abort(void);
void exit(int code);


