EXEC=nap-watchdog  
SRC=nap-watchdog.c ipc.c

all: $(EXEC)
nap-watchdog: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

