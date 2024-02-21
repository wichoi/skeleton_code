#ifndef __EVENT_SERVICE_H__
#define __EVENT_SERVICE_H__

//
#define EV_NONE                     0000

// event system
#define EV_INIT                     1001
#define EV_DEINIT                   1002
#define EV_SUBSCRIBE                1003
#define EV_PUBLISH                  1004
#define EV_RESPONSE                 1005
#define EV_EXIT                     1006

// event secc
#define EV_SECC_UDP_RECV            2001
#define EV_SECC_UDP_SEND            2002

#define EV_SECC_TCP_RECV            2011
#define EV_SECC_TCP_SEND            2012

#define EV_SECC_IDLE                2101
#define EV_SECC_PLUG                2102
#define EV_SECC_SESSION_START       2103
#define EV_SECC_SUPPORTED_APP       2104
#define EV_SECC_SESSION_SETUP       2105
#define EV_SECC_SERVICE_DISCOVER    2106
#define EV_SECC_SERVICE_DETAIL      2107
#define EV_SECC_PAYMENT_SEL         2108
#define EV_SECC_PAYMENT_DETAIL      2109
#define EV_SECC_AUTHORIZATION       2110
#define EV_SECC_CHARGING_PARAM      2111
#define EV_SECC_POWER_DELIVERY      2112
#define EV_SECC_CERT_UPDATE         2113
#define EV_SECC_CERT_INSTALL        2114
#define EV_SECC_SESSION_STOP        2115
#define EV_SECC_CHARGING_STATUS     2116
#define EV_SECC_METERING_RECEIPT    2117
#define EV_SECC_CABLE_CHECK         2118
#define EV_SECC_PRE_CHARGE          2119
#define EV_SECC_CURRENT_DEMAND      2120
#define EV_SECC_WELDING_DETECTION   2121
#define EV_SECC_DEINIT              2122
#define EV_SECC_ERROR               2123
#define EV_SECC_DISCONNECT          2124

// event evcc
#define EV_EVCC_UDP_RECV            3001
#define EV_EVCC_UDP_SEND            3002

#define EV_EVCC_TCP_RECV            3011
#define EV_EVCC_TCP_SEND            3012

#define EV_EVCC_IDLE                2101
#define EV_EVCC_PLUG                2102
#define EV_EVCC_SESSION_START       2103
#define EV_EVCC_SUPPORTED_APP       2104
#define EV_EVCC_SESSION_SETUP       2105
#define EV_EVCC_SERVICE_DISCOVER    2106
#define EV_EVCC_SERVICE_DETAIL      2107
#define EV_EVCC_PAYMENT_SEL         2108
#define EV_EVCC_PAYMENT_DETAIL      2109
#define EV_EVCC_AUTHORIZATION       2110
#define EV_EVCC_CHARGING_PARAM      2111
#define EV_EVCC_POWER_DELIVERY      2112
#define EV_EVCC_CERT_UPDATE         2113
#define EV_EVCC_CERT_INSTALL        2114
#define EV_EVCC_SESSION_STOP        2115
#define EV_EVCC_CHARGING_STATUS     2116
#define EV_EVCC_METERING_RECEIPT    2117
#define EV_EVCC_CABLE_CHECK         2118
#define EV_EVCC_PRE_CHARGE          2119
#define EV_EVCC_CURRENT_DEMAND      2120
#define EV_EVCC_WELDING_DETECTION   2121
#define EV_EVCC_DEINIT              2122
#define EV_EVCC_ERROR               2123
#define EV_EVCC_DISCONNECT          2124

// event test
#define EV_HELLO                    9001
#define EV_EXAMPLE                  9002
#define EV_SELFTEST                 9100
#define EV_TCP_SEND                 9101
#define EV_UDP_SEND                 9102

// operation code
#define OP_NONE                     0000
#define OP_REQUEST                  0001
#define OP_RESPONSE                 0002
#define OP_NOTIFY                   0003
#define OP_CONFIRM                  0004

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
