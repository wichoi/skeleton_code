CC=gcc
EXEC = nCube-Thyme
INC = -I./mqtt-paho \
		-I./cJSON \
		-I./debug \
		-I./config/*.o \
		-I./timer/*.o \
		-I./event/*.o \
		-I./state/*.o \
		-I./tas-ble/*.o \
		-I./tas-zigbee/*.o \
		-I./tas-zwave/*.o \
		-I./cli/*.o

SRCS = $(wildcard *.c) \
		$(wildcard ./mqtt-paho/*.c) \
		$(wildcard ./cJSON/*.c) \
		$(wildcard ./debug/*.c) \
		$(wildcard ./config/*.c) \
		$(wildcard ./timer/*.c) \
		$(wildcard ./event/*.c) \
		$(wildcard ./state/*.c) \
		$(wildcard ./tas-ble/*.c) \
		$(wildcard ./tas-zigbee/*.c) \
		$(wildcard ./tas-zwave/*.c) \
		$(wildcard ./cli/*.c)
OBJS = $(SRCS:.c=.o)

EXTRA_CFLAGS += -DDEBUG_ENABLE
CFLAGS = -g -O2 -Wall $(EXTRA_CFLAGS)
LDFLAGS = -Xlinker -Map=${EXEC}.map -rdynamic
LIBS = -lpthread -lrt

all: $(EXEC)

$(EXEC): $(OBJS)
	${CC} -g -o $(EXEC) ${OBJS} ${CFLAGS} ${LDFLAGS} ${LIBS} ${INC}

romfs:
	$(STRIP) /bin/$(EXEC)
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) \
			$(EXEC).map\
			./mqtt-paho/*.o \
			./cJSON/*.o \
			./debug/*.o \
			./config/*.o \
			./timer/*.o \
			./event/*.o \
			./state/*.o \
			./tas-ble/*.o \
			./tas-zigbee/*.o \
			./tas-zwave/*.o \
			./cli/*.o \
			./*.o
