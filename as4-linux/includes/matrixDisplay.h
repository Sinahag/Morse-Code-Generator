// Used for the 8x8 LED Matrix module in particular. Will display Led's live status and history in range.
#ifndef _MATRIXDISPLAY_H_
#define _MATRIXDISPLAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <stdbool.h>
#define I2C_DEVICE_ADDRESS 0x70

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"

void Display_init();
void Display_initI2cBus(char *bus, int address);
void Display_writeI2cReg(unsigned char regAddr, unsigned char value);
void Display_displayLedStatus(bool ledStatus);
void Display_clear();

#endif