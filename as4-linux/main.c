#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "includes/myThreads.h"

static size_t maxNumChars = 30;

// Prototypes
void runCommand(char *command);
void PRU_init();

static uint32_t *buff;

int main(void)
{
    // Initialize modules
    PRU_init();
    SharedMemLinux_init();
    Display_init();

    pthread_t printer_t;
    buff = (uint32_t *)malloc(sizeof(buff) * morseSize);

    char *msg = (char *)malloc(sizeof(msg) * maxNumChars);

    int numBits = 0, msgSize, location = 0, sizeClipped;
    uint32_t bit_mask = 0x80000000;
    uint32_t MSB;
    bool ready = false;
    printf("Beginning Morse Code!!\n");
    while (1)
    {
        // Clear everything
        SharedMemLinux_clearMorse();
        //memset(msg, '0', strlen(msg));
        for (int s = 0; s < maxNumChars; s++){
            msg[s] = '\0';
        }
        //memset(buff, 0, sizeof(uint32_t));
        for (int s = 0; s < morseSize; s++){
            buff[s] = 0x0;
        }
        numBits = 0;
        location = 0;
        ready = false;
        sizeClipped = 1;
        fflush(stdout);

        bit_mask = 0x80000000;

        printf("> ");

        fflush(stdout);
        msgSize = getline(&msg, &maxNumChars, stdin);
        if (msg[0] == '\n'){
            break;
        }
        msg[msgSize - 1] = '\0';
        int j = 2;
        while(msg[msgSize - j] == ' '){
            msg[msgSize - j] = '\0';
            sizeClipped++;
        }
        msgSize -= sizeClipped;

        MorseCode_getBinaryMorseCode(msg, buff, &numBits); // fill in buff with the msg

        SharedMemLinux_updatePRUMemory(buff, numBits, &ready);

        printf("Flashing out %d characters: '%s'\n", msgSize, msg);
        for (int i = 0; i < numBits; i++)
        {
            MSB = (buff[location]) & bit_mask;
            if (MSB)
            {
                printf("X");
            }
            else
            {
                printf("_");
            }
            bit_mask >>= 1;
            if (bit_mask == 0x00000000)
            {
                bit_mask = 0x80000000;
                location++;
            }
        }
        printf("\n");

        while(!ready);

        pthread_create(&printer_t, NULL, &printerThread, NULL);

        while (!SharedMemLinux_isFinishedMorse());

        pthread_join(printer_t, NULL);
        printf("\n");
        Display_clear();
    }
    printf("Shutting down application.\nDone!\n");
    // Cleanup modules
    Display_clear();
    SharedMemLinux_cleanup();
    free(msg);
    free(buff);
    
    return 0;
}

void PRU_init()
{
    runCommand("config-pin p9_27 pruout");
    runCommand("config-pin p9_28 pruin");
}

void runCommand(char *command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0)
    {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}
