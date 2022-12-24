// Threads needed
#ifndef _MYTHREADS_H_
#define _MYTHREADS_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "timing.h"
#include "morsecode.h"
#include "matrixDisplay.h"
#include "sharedDataStruct.h"
#include "sharedMem-Linux.h"

#define BUFFSIZE 2048

void *printerThread(void *args);

#endif 