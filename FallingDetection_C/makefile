SHELL = /bin/sh

.SUFFIXES : .c .o

CC = gcc
CFLAGS = -g -c

OBJECTS = main.o FallingDetectionReadData.o FallingDetectionFSM.o AprotechSpeedyAlgorithm.o
TARGET = FallingDetection

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

main.o : main.h main.c
FallingDetectionReadData.o : FallingDetectionReadData.c
FallingDetectionFSM.o : FallingDetectionFSM.c
AprotechSpeedyAlgorithm.o : AprotechSpeedyAlgorithm.c

clean :
	rm -rf $(OBJECTS) $(TARGET) core 

