#ifndef _APRO_STATE_H_
#define _APRO_STATE_H_

#define ST_NONE                 0
#define ST_INIT_ZB_NET          1
#define ST_INIT_ANNOUNCE        2
#define ST_INIT_ANNOUNCE_WAIT   3
#define ST_INIT_GET_CHILD       4
#define ST_INIT_GET_CHILD_WAIT  5
#define ST_INIT_GET_NEI         6
#define ST_INIT_GET_NEI_WAIT    7
#define ST_INIT_GET_ROUTE       8
#define ST_INIT_GET_ROUTE_WAIT  9
#define ST_INIT_UPDATE_EP       10
#define ST_INIT_UPDATE_EP_WAIT  11
#define ST_INIT_UPDATE_CL       12
#define ST_INIT_UPDATE_CL_WAIT  13
#define ST_OCF_CREATE           14
#define ST_IDLE                 15
#define ST_EXIT                 16


#define ST_CMD_NONE             0
#define ST_CMD_ERR              1
#define ST_CMD_ZB_ERR           2
#define ST_CMD_OCF_ERR          3

#define ST_CMD_NEW_NODE         10
#define ST_CMD_UPDATE_EP        11
#define ST_CMD_UPDATE_CL        12

#define ST_CMD_EXIT             100

int apro_state(u8 cmd);

#endif
