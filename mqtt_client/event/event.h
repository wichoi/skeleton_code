#ifndef _EVENT_H_
#define _EVENT_H_

#define Q_INACTIVE          0
#define Q_ACTIVE            1

#define PROC_IDLE           0
#define PROC_CMD            1
#define PROC_EXIT           2

//
#define EV_NONE             0

// adn ae : 0
#define AE_NONE             1000
#define AE_INIT             1001
#define AE_CONNECT          1002
#define AE_SUBSCRIBE        1003
#define AE_PUBLISH          1004
#define AE_RESPONSE         1005
#define AE_DISCON           1006

// ble tas : 1000
#define BL_NONE             2000
#define BL_SET              2001
#define BL_GET              2002

// zigbee tas : 2000
#define ZI_NONE             3000
#define ZI_SET              3001
#define ZI_GET              3002

// zwave tas : 3000
#define ZW_NONE             4000
#define ZW_SET              4001
#define ZW_GET              4002

// test : 4000
#define TE_NONE             5000
#define TE_AE               5001
#define TE_CNT              5002
#define TE_CIN              5003
#define TE_SUB              5004
#define TE_RESP             5005
#define TE_BEAT             5006

#define OP_CODE_NONE        0000
#define OP_CODE_GET         0001
#define OP_CODE_SET         0002
#define OP_CODE_RESP        0003
#define OP_CODE_NOTE        0003

#define Q_DATA_SZ 256

typedef struct _event_queue
{
    u32 event;
    char data[Q_DATA_SZ];
    u32 len;
} event_q;

void init_event(void);
void destroy_event(void);
u8 put_event(u32 ev, /*, u32 op_code */char* data, u32 len);
event_q handle_event(void);

#endif
