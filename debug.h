#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG_MODE
#define DEBUG_PRINTF(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(fmt, ...)
#endif

#endif
