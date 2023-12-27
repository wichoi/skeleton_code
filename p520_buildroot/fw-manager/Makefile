#EXTRA_CFLAGS =
CFLAGS = -shared -g -O2 -Wall $(EXTRA_CFLAGS) \
	-I$(ROOTDIR)/lib/openssl-1.0.2u/include \
	-I$./ -I$./upgrade -I$(ROOTDIR)/lib/zlib
LDFLAGS = -L$(ROOTDIR)/lib/openssl-1.0.2u -lssl -lcrypto

EXEC = libfwmgr.so
OBJS = fw-main.o fw-api.o fw-crc32.o fw-crypto.o fw-dbg.o fw-merger.o fw-splitter.o \
	upgrade/md5.o upgrade/crc32.o upgrade/modem-upgrade.o upgrade/router-upgrade.o

#EXTRA_CFLAGS += -DDBG

all: $(EXEC)

$(EXEC): $(OBJS)
	${CC} -g -o $(EXEC) ${OBJS} ${CFLAGS} ${LDFLAGS}

shared:
	echo "ROOTDIR=$(ROOTDIR)."
	cp -f $(EXEC) $(ROOTDIR)/lib/lib/
	cp -rf fw-api.h $(ROOTDIR)/lib/include

clean:
	-rm -f $(EXEC) *.o
