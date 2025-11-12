#pragma once

#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
#define assert(cond) do { if (!(cond)) { /* no-op in kernel build */ } } while (0)
#endif


