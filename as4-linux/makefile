SOURCES = main.c matrixDisplay.c morsecode.c myThreads.c sharedDataStruct.c timing.c
TARGET = as4-morsecode
PUBDIR = $(HOME)/cmpt433/public/myApps

OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow
# -pg for supporting gprof profiling.
# CFLAGS += -pg

all: $(TARGETS) 
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) -lpthread

clean:
	rm -f $(OUTDIR)/$(TARGET)