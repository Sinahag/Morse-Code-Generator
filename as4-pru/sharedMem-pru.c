#include <stdint.h>
#include <stdbool.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "../as4-linux/includes/sharedDataStruct.h"

// Reference for shared RAM:
// https://markayoder.github.io/PRUCookbook/05blocks/blocks.html#_controlling_the_pwm_frequency

// GPIO Configuration
// ----------------------------------------------------------
volatile register uint32_t __R30; // output GPIO register
volatile register uint32_t __R31; // input GPIO register

// Use pru0_rpu_r30_5 as an output (bit 5 = 0b0010 0000)
#define LED_MASK (1 << 5)

// Use pru0_pru_r31_3 as a button (bit 3 = 0b0000 1000)
#define BUTTON_MASK (1 << 3)

// Shared Memory Configuration
// -----------------------------------------------------------
#define THIS_PRU_DRAM 0x00000 // Address of DRAM
#define OFFSET 0x200          // Skip 0x100 for Stack, 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)

// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct_t *pSharedMemStruct = (volatile void *)THIS_PRU_DRAM_USABLE;

#define PROCESSOR_SPEED_MHZ 200000000
#define BUFFER_WINDOW_TIME_S 4
// Delay per sample is:
//  # instructions per second * seconds/window / samples / window
#define DELAY_PER_SAMPLE (PROCESSOR_SPEED_MHZ * BUFFER_WINDOW_TIME_S / NUM_SAMPLES)

#define DELAY_250_MS 50000000
#define DELAY_1000_MS (DELAY_250_MS * 2)
#define DELAY_300_MS ((DELAY_1000_MS * 3) / 10)

void main(void)
{
    // Initialize:
    pSharedMemStruct->isFinished = true;
    pSharedMemStruct->isLedOn = false;
    pSharedMemStruct->isButtonPressed = false;
    volatile uint32_t *buff;
    uint32_t bit_mask = 0x80000000;
    uint32_t MSB;
    uint32_t curBuff;
    int numBits;
    int location = 0;
    bool slow;

    while (1)
    {
        while (pSharedMemStruct->isFinished); // busy wait until there is something to display
        location = 0;
        buff = pSharedMemStruct->morse;
        numBits = pSharedMemStruct->numBits;
        curBuff = buff[location];
        bit_mask = 0x80000000;
        for (int i = 0; i < numBits; i++)
        {
            MSB = (curBuff)&bit_mask;
            if (MSB)
            {
                __R30 |= LED_MASK;
                pSharedMemStruct->isLedOn = true;
            }
            else
            {
                __R30 &= ~LED_MASK;
                pSharedMemStruct->isLedOn = false;
            }

            bit_mask >>= 1;

            if (bit_mask == 0x00000000) // move on to the next block
            {
                bit_mask = 0x80000000;
                location++;
                curBuff = buff[location];
            }

            slow = (__R31 && BUTTON_MASK) != 0;
            if (!slow)
            {
                __delay_cycles(DELAY_300_MS);
            }
            else
            {
                __delay_cycles(DELAY_1000_MS);
            }
            pSharedMemStruct->isButtonPressed = slow;
        }
        __R30 &= ~LED_MASK;
        pSharedMemStruct->isLedOn = false;
        pSharedMemStruct->isFinished = true;
    }
}
