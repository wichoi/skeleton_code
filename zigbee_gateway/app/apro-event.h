#ifndef _APRO_EVENT_H_
#define _APRO_EVENT_H_

#define Q_INACTIVE              0
#define Q_ACTIVE                1

#define PROC_IDLE               0
#define PROC_CMD                1
#define PROC_EXIT               2

//
#define EV_NONE                 0
#define EV_NOP                  1

// Config Get Event
#define EV_CONF_NONE            1000
#define EV_CONF_VERSION         1001
#define EV_CONF_EUI64           1002
#define EV_CONF_PAN_ID          1003
#define EV_CONF_CHANNEL         1004
#define EV_CONF_NODE            1005
#define EV_CONF_SET             1006

#define EV_CONF_NET_ST          1100
#define EV_CONF_NET_PARAM       1101
#define EV_CONF_NODE_TYPE       1102
#define EV_CONF_NCP             1103
#define EV_CONF_ROUTE_TABLE     1104
#define EV_CONF_ROUTE_SIZE      1105
#define EV_CONF_CHILD           1106
#define EV_CONF_NEIGHBOR        1107
#define EV_CONF_NODE_INFO       1108
#define EV_CONF_GET_EP          1109
#define EV_CONF_GET_CL          1111

#define EV_CONF_BIND_REQ        1200
#define EV_CONF_BIND_CLEAR      1201
#define EV_CONF_BIND_PRINT      1202

// Network Event
#define EV_NET_NONE             2000
#define EV_NET_FORM             2001
#define EV_NET_START            2002
#define EV_NET_STOP             2003
#define EV_NET_OPEN             2004
#define EV_NET_CLOSE            2005
#define EV_NET_LEAVE            2006
#define EV_NET_REMOVE_DEV       2007

// Send/Recv Event
#define EV_SEND_NONE            3000
#define EV_SEND_UNI             3001
#define EV_SEND_BR              3002
#define EV_SEND_MULTI           3003

// IPC Event
#define EV_IPC_NONE             4000
#define EV_IPC_RECV             4001
#define EV_IPC_SEND             4002

// OCF Event
#define EV_OCF_NONE             5000
#define EV_OCF_RECV             5001
#define EV_OCF_SEND             5002

// Command Event
#define EV_EXE_NONE             6000
#define EV_EXE_NODE_MGR         6001

// Test Event
#define EV_TEST_NONE            7000
#define EV_TEST_ADD_NODE        7001
#define EV_TEST_DEL_NODE        7002
#define EV_TEST_DUMMY_NODE      7003

#define EV_TEST_AGING_START     7101
#define EV_TEST_AGING_STOP      7102

// System Event
#define EV_SYS_NONE             8000
#define EV_SYS_REBOOT           8001
#define EV_SYS_EXIT             8002

// Recv Callback Event
#define EV_CB_NONE              9000
#define EV_CB_RST_ATTR          9001
#define EV_CB_PRE_CMD           9002
#define EV_CB_ZDO_MSG           9003
#define EV_CB_RETRIVE_ATTR      9004
#define EV_CB_KEY_EST           9005
#define EV_CB_ATTR_RESP         9006
#define EV_CB_REPORT_ATTR       9007
#define EV_CB_SEND_RESP         9008
#define EV_CB_PRE_MSG_RCV       9009

// Custom Event
#define EV_CUSTOM_NONE          10000

// Operation Code
#define EV_CMD_NONE             0
#define EV_CMD_GET              1 // host -> NCP
#define EV_CMD_SET              2 // host -> NCP
#define EV_CMD_RESP             3 // host <- NCP
#define EV_CMD_NOTI             4 // host -> NCP
#define EV_CMD_CB               5 // host <- NCP

#define EV_CMD_IPC_NONE         10
#define EV_CMD_IPC_GET          11
#define EV_CMD_IPC_SET          12
#define EV_CMD_IPC_RESP         13
#define EV_CMD_IPC_NOTI         14

// Paramater

// zigbee payload
typedef struct _tag_zigbee_frame
{
    u16 dest_id;        // destination network id (unicast only)
    u16 profile_id;     // profile id (HA_PROFILE_ID, EMBER_ZDO_PROFILE_ID)
    u16 cluster_id;     // cluster-id.h (ZCL_ON_OFF_CLUSTER_ID)
    u8 src_ep;          // source End Point (EMBER_ZDO_ENDPOINT)
    u8 dest_ep;         // destination End Point
    u16 option;         // EmberApsOption (EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY|EMBER_APS_OPTION_RETRY)
    u16 group_id;
    u8 sequence;
    u8 radius;
    u8 msg_len;
    u8 msg[128];        // EZSP_MAX_FRAME_LENGTH
} zb_frame_t;

// EV_CB_PRE_CMD
typedef struct _tag_cb_pre_cmd_recv
{
    u16 net_id;
    u16 profile_id;
    u16 cluster_id;
    u8 src_ep;
    u8 dest_ep;
    u16 options;
    u16 group_id;
    u8 sequence;
    u8 radius;
    u8 msg_type;
    u8 cmd_id;
    u8 status;
    u16 buf_len;
    u8 buf[128];
} cb_pre_cmd_t;

// EV_CB_RETRIVE_ATTR
typedef struct _tag_cb_reterive_attr
{
    u8 endpoint;
    u16 cluster_id;
    u16 attr_id;
    u8 mask;
    u16 maunfacturer_code;
    u16 read_len;
} cb_reteriv_t;


// EV_CB_KEY_EST
typedef struct _tag_cb_key_est
{
    u8 eui64[8];
    u8 status;
} cb_key_est_t;

// EV_CB_ATTR_RESP
typedef struct _tag_read_attr_resp
{
    u16 cluster_id;
    u16 buf_len;
    u8 buf[128];
} cb_attr_t;

// EV_CB_PRE_MSG_RCV
typedef struct _tag_cb_rssi
{
    u16 net_id;
    u8 lqi;
    s8 rssi;
} cb_rssi_t;

// ocf payload
typedef struct _tag_ocf_payload
{
    u16 net_id;
    u8 ep;
    u16 cluster;
    u16 attr;
    u8 cmd;
    u8 data[32];
    u8 data_len;
} ocf_send_t;

#define Q_DATA_SZ 512 // EZSP_MAX_FRAME_LENGTH

typedef struct _event_queue
{
    u32 event;
    u32 op_code;
    u32 len;
    char data[Q_DATA_SZ];
} event_q;

void init_event(void);
void destroy_event(void);
u8 put_event(u32 ev, u32 cmd, char* data, u32 len);
event_q handle_event(void);
u8 check_event_q(void);


#endif
