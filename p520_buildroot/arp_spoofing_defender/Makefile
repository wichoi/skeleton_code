EXTRA_CFLAGS =
CFLAGS = -g -O2 -Wall $(EXTRA_CFLAGS)
LDFLAGS = 
LIBS = -lpthread
#LDFLAGS +=

EXEC = arp_spoofing_defender
#SRCS = asd_main.c asd_dbg.c
OBJS = asd_main.o asd_dbg.o

#EXTRA_CFLAGS += -DDBG

export CC=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc
export CXX=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-g++
export LD=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc
export AR=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-ar
export NM=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-nm 
export STRIP=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-strip
export OBJCOPY=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-objcopy
export RANLIB=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-ranlib
export OBJDUMP=/opt/buildroot-gcc342/bin/mipsel-linux-uclibc-objdump

all: $(EXEC)

$(EXEC): $(OBJS)
	${CC} -g -o $(EXEC) ${OBJS} ${CFLAGS} ${LDFLAGS} ${LIBS}

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.o