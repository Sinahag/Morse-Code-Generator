// Translates a string into a binary string of its morse code representation
#ifndef _MORSECODE_H_
#define _MORSECODE_H_
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#define TIME_BITS_BETWEEN_CHARS 3
#define TIME_BITS_FOR_SPACES 7

void MorseCode_getBinaryMorseCode(char *message, uint32_t* buff, int* numBits);

#endif
