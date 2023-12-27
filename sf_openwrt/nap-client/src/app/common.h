#ifndef _COMMON_H_
#define _COMMON_H_

using namespace std;

//#define LINUX_PC_APP
//#define SIGFOX_AUTH_DUMMY // sigfox auth server not work.
//#define SIGFOX_GRPC_DUMMY // sigfox grpc test.
//#define RMS_SERVER_DUMMY // rms server not work.
//#define NAP_DUMMY_DATA

// return value
#define RET_OK              0
#define RET_ERROR           1
#define RET_INVALID         2
#define RET_FATAL           3
#define RET_NOT_EXIST       4
#define RET_ERR_CURL        5

#define RET_EXIT            100

typedef unsigned char       u8;
typedef signed char         s8;
typedef unsigned short      u16;
typedef signed short        s16;
typedef unsigned int        u32;
typedef signed int          s32;
typedef unsigned long long  u64;
typedef signed long long    s64;

#endif
