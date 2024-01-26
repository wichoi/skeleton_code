#ifndef __EVENT_SERVICE_H__
#define __EVENT_SERVICE_H__

//
#define EV_NONE             0000

// event system
#define EV_INIT             1001
#define EV_DEINIT           1002
#define EV_SUBSCRIBE        1003
#define EV_PUBLISH          1004
#define EV_RESPONSE         1005
#define EV_EXIT             1006

// event test
#define EV_HELLO            9001
#define EV_EXAMPLE          9002
#define EV_SELFTEST         9100

// operation code
#define OP_NONE             0000
#define OP_REQUEST          0001
#define OP_RESPONSE         0002
#define OP_NOTIFY           0003
#define OP_CONFIRM          0004

#define EV_DATA_SZ 256

typedef struct _event_data
{
    u32 event;
    u32 op_code;
    char data[EV_DATA_SZ];
    u32 len;
} event_data;

typedef int(*event_fp)(const event_data*);

void event_init(void);
void event_deinit(void);
int event_subscribe(u32 event, event_fp callback);
int event_publish(u32 ev, u32 op_code, char* data, u32 len);
int event_proc(void);

#endif
