#pragma once

#include <stdint.h>

// Minimal printf format macros used by Doom codepaths.
#ifndef PRId8
#define PRId8 "d"
#endif
#ifndef PRIu8
#define PRIu8 "u"
#endif
#ifndef PRId16
#define PRId16 "d"
#endif
#ifndef PRIu16
#define PRIu16 "u"
#endif
#ifndef PRId32
#define PRId32 "d"
#endif
#ifndef PRIu32
#define PRIu32 "u"
#endif
#ifndef PRIx32
#define PRIx32 "x"
#endif
#ifndef PRIX32
#define PRIX32 "X"
#endif
#ifndef PRId64
#define PRId64 "lld"
#endif
#ifndef PRIu64
#define PRIu64 "llu"
#endif
#ifndef PRIx64
#define PRIx64 "llx"
#endif
#ifndef PRIX64
#define PRIX64 "llX"
#endif


