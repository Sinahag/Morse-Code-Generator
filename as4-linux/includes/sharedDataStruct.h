#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>

// WARNING:
// Fields in the struct must be aligned to match ARM's alignment
// bool/char, uint8_t: byte aligned
// int/long, uint32_t: word (4 byte) aligned
// double, uint64_t: dword (8 byte) aligned
// Add padding fields (char _p1) to pad out to alignment.
// My Shared Memory Structure
// ----------------------------------------------------------------

#define morseSize 64

typedef struct
{
    // 1 byte
    bool isLedOn;
    // 1 byte
    bool isFinished; // if 1: linux's turn, if 0: pru's turn
    // 1 byte
    bool isButtonPressed;
    // Padding
    char _p1;

    int numBits; // stores the number of morse bits in the decoded message

    uint32_t morse[morseSize]; // 32*64 = 2048 bits
} sharedMemStruct_t;

volatile void *getPruMmapAddr(void);

void freePruMmapAddr(volatile void *pPruBase);

#endif
