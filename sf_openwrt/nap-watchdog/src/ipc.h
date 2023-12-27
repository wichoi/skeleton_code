#ifndef _WATCHDOG_IPC_H_
#define _WATCHDOG_IPC_H_

#define RET_OK                  0
#define RET_ERROR               1
#define RET_WAIT                2

typedef unsigned char       u8;
typedef signed char         s8;
typedef unsigned short      u16;
typedef signed short        s16;
typedef unsigned int        u32;
typedef signed int          s32;
typedef unsigned long       u64;
typedef signed long         s64;

int ipc_init(void);
int ipc_deinit(void);
int ipc_proc(void);
int ipc_send(char* data);

#endif
