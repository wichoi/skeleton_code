#ifndef __EVENT_SERVICE_H__
#define __EVENT_SERVICE_H__

//
#define EV_NONE                 0000

// event system
#define EV_INIT                 1001
#define EV_DEINIT               1002
#define EV_SUBSCRIBE            1003
#define EV_PUBLISH              1004
#define EV_RESPONSE             1005
#define EV_EXIT                 1006

// event secc
#define EV_SECC_UDP_RECV        2001
#define EV_SECC_UDP_SEND        2002

#define EV_SECC_TCP_RECV        2011
#define EV_SECC_TCP_SEND        2012

#define EV_SECC_IDLE            2101
#define EV_SECC_PLUG            2102
#define EV_SECC_SESSION_START   2103
#define EV_SECC_SESSION_SETUP   2104
#define EV_SECC_SERVICE         2105
#define EV_SECC_AUTHORIZATION   2106
#define EV_SECC_CHARGING_PARAM  2107
#define EV_SECC_POWER_DELIVERY  2108
#define EV_SECC_SESSION_STOP    2109
#define EV_SECC_DEINIT          2110
#define EV_SECC_ERROR           2111
#define EV_SECC_DISCONNECT      2112

// event evcc
#define EV_EVCC_UDP_RECV        3001
#define EV_EVCC_UDP_SEND        3002

#define EV_EVCC_TCP_RECV        3011
#define EV_EVCC_TCP_SEND        3012

#define EV_EVCC_IDLE            3101
#define EV_EVCC_PLUG            3102
#define EV_EVCC_SESSION_START   3103
#define EV_EVCC_SESSION_SETUP   3104
#define EV_EVCC_SERVICE         3105
#define EV_EVCC_AUTHORIZATION   3106
#define EV_EVCC_CHARGING_PARAM  3107
#define EV_EVCC_POWER_DELIVERY  3108
#define EV_EVCC_SESSION_STOP    3109
#define EV_EVCC_DEINIT          3110
#define EV_EVCC_ERROR           3111
#define EV_EVCC_DISCONNECT      3112

// event test
#define EV_HELLO                9001
#define EV_EXAMPLE              9002
#define EV_SELFTEST             9100
#define EV_TCP_SEND             9101
#define EV_UDP_SEND             9102

// operation code
#define OP_NONE                 0000
#define OP_REQUEST              0001
#define OP_RESPONSE             0002
#define OP_NOTIFY               0003
#define OP_CONFIRM              0004

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
