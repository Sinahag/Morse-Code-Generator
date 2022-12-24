#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include "includes/sharedDataStruct.h"

// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------

#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000     // Length of PRU memory
#define PRU0_DRAM 0x00000   // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000  // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap
// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ((base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ((base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ((base) + PRU_SHAREDMEM)

static volatile void *pPruBase_;
static volatile sharedMemStruct_t *pSharedPru0_;

// Return the address of the PRU's base memory
volatile void *getPruMmapAddr(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }
    // Points to start of PRU memory.
    volatile void *pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED)
    {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return pPruBase;
}

void freePruMmapAddr(volatile void *pPruBase)
{
    if (munmap((void *)pPruBase, PRU_LEN))
    {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}

void SharedMemLinux_init()
{
    pPruBase_ = getPruMmapAddr();
    pSharedPru0_ = PRU0_MEM_FROM_BASE(pPruBase_);
}

void SharedMemLinux_updatePRUMemory(uint32_t *morse, int numBits, bool *ready)
{
    // Overwrite all of the memory with the morse code input
    for (int i = 0; i < morseSize; i++)
    {
        pSharedPru0_->morse[i] = morse[i];
    }
    *ready = true;
    pSharedPru0_->numBits = numBits;
    pSharedPru0_->isFinished = false;
}

void SharedMemLinux_clearMorse()
{
    for (int i = 0; i < morseSize; i++)
    {
        pSharedPru0_->morse[i] = 0x0;
    }
}

bool SharedMemLinux_isLedOn()
{
    return pSharedPru0_->isLedOn;
}

bool SharedMemLinux_isButtonPressed()
{
    return pSharedPru0_->isButtonPressed;
}

int SharedMemLinux_getNumBits()
{
    return pSharedPru0_->numBits;
}

void SharedMemLinux_setFinished()
{
    pSharedPru0_->isFinished = false;
}

bool SharedMemLinux_isFinishedMorse()
{
    return pSharedPru0_->isFinished;
}

void SharedMemLinux_cleanup()
{
    freePruMmapAddr(pPruBase_);
}