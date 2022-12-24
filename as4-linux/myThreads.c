#include "includes/myThreads.h"

void *printerThread(void *args){
    bool isLedOn;
    while(!SharedMemLinux_isFinishedMorse()){
        isLedOn = SharedMemLinux_isLedOn();
        if (isLedOn){
            printf("X");
        }
        else{
            printf("_");
        }
        Display_displayLedStatus(isLedOn);
        fflush(stdout);
        msleep(149);
    }

    return NULL;
}