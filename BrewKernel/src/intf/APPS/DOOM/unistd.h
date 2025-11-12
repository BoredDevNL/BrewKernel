#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

int isatty(int fd);
unsigned int sleep(unsigned int seconds);
int usleep(unsigned int usec);


