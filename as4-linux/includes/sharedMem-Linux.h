// Module created to interface with the shared memory between the
// linux program and PRU firmware
#ifndef _SHAREDMEMLINUX_H_
#define _SHAREDMEMLINUX_H_

#include <stdint.h>

void SharedMemLinux_init();

void SharedMemLinux_updatePRUMemory(uint32_t *morse, int numBits, bool *ready);

void SharedMemLinux_clearMorse();

bool SharedMemLinux_isLedOn();

bool SharedMemLinux_isButtonPressed();

void SharedMemLinux_setFinished();

void SharedMemLinux_setNumBits();

int SharedMemLinux_getNumBits();

bool SharedMemLinux_isFinishedMorse();

void SharedMemLinux_cleanup();

#endif