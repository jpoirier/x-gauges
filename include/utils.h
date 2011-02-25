// Copyright (c) 2010 Joseph D Poirier
// Distributable under the terms of The New BSD License
// that can be found in the LICENSE file.

// pack754 and unpack754 are from "Beej's Guide to Network Programming"
// by Brian "Beej Jorgensen" Hall, beej@beej.us, and is under public domain.
// http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#serialization

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define pack754_32(f)   (pack754((f), 32, 8))
#define pack754_64(f)   (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

extern uint64_t pack754(long double f, unsigned int bits, unsigned int expbits);
extern long double unpack754(uint64_t i, unsigned int bits, unsigned int expbits);

#ifdef __cplusplus
}
#endif

#endif
