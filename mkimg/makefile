SHELL = /bin/sh

.SUFFIXES : .c .o

CC = gcc
CFLAGS = -g -c 
LDFLAGS = -lssl -lcrypto -lpthread

OBJECTS = main.o cli.o app.o
TARGET = mkimg

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

clean :
	rm -rf $(OBJECTS) $(TARGET) core

