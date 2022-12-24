#include "includes/matrixDisplay.h"
#include "includes/timing.h"

// GLOBAL VARIABLES

static int i2cFD;

static unsigned char DisplayRows[] = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E};

static unsigned char currentStatus;

void Display_init()
{
    Display_initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    Display_writeI2cReg(0x21, 0x00); // Turn on LED matrix
    Display_writeI2cReg(0x81, 0x00); // Set display to be on
    currentStatus = 0x00;
    // Clear all rows
    for (int i = 0; i < 8; i++)
    {
        Display_writeI2cReg(DisplayRows[i], 0x00);
    }
}

void Display_initI2cBus(char *bus, int address)
{
    i2cFD = open(bus, O_RDWR);
    int result = ioctl(i2cFD, I2C_SLAVE, address);
    if (result < 0)
    {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
}

void Display_writeI2cReg(unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFD, buff, 2);
    if (res != 2)
    {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

// 0b abcd efgh
// Display:
// bcde fgha
void Display_displayLedStatus(bool ledStatus)
{
    // Always rotate left
    unsigned char droppedMSB = (currentStatus & 0x80) == 0x80 ? 0x1 : 0x0;
    currentStatus = (currentStatus << 1) | droppedMSB;
    // Drop the left most bit
    currentStatus &= 0x7F;

    if (ledStatus)
    { // write a 4x1 on the very right
        currentStatus |= 0x80;
    }
    for (int i = 0; i < 4; i++)
    {
        Display_writeI2cReg(DisplayRows[i], currentStatus);
    }
}

void Display_clear(){
    while (currentStatus != 0x00){
        msleep(150);
        Display_displayLedStatus(false);
    }
}