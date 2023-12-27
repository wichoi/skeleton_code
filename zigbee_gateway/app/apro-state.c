#include <stdio.h>
#include <string.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-zcl-basic.h"
#include "apro-node.h"
#include "apro-config.h"
#include "apro-ocf.h"
#include "apro-state.h"

static int apro_st = ST_NONE;
static u8 state_cnt = 0;

int apro_state(u8 cmd)
{
    //log_v("%s : %d\n", __func__, apro_st);

    switch(apro_st)
    {
    case ST_NONE:
        apro_st = ST_INIT_ZB_NET;
        state_cnt = 0;
        log_i("%s move to ST_INIT_ZB_NET \n", __func__);
        break;

    case ST_INIT_ZB_NET:
        {
            u8 state = apro_get_net_st();
            if(state == 2) // EMBER_JOINED_NETWORK
            {
                apro_st = ST_INIT_ANNOUNCE;
                state_cnt = 0;
                log_i("%s move to ST_INIT_ANNOUNCE \n", __func__);
            }
            else
            {
                if(state_cnt++ < 3)
                {
                    put_event(EV_CONF_NET_ST, EV_CMD_NONE, NULL, 0);
                }
                else
                {
                    apro_st = ST_OCF_CREATE;
                    state_cnt = 0;
                    log_i("%s move to ST_OCF_CREATE \n", __func__);
                }
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_ANNOUNCE: // send br (update node info)
        {
            zb_frame_t data = {0,};
            apro_zcl_basic_attribute(0xFFFF, 1, &data);
            put_event(EV_SEND_BR, EV_CMD_NONE, (char*)&data, sizeof(data));
            apro_st = ST_INIT_ANNOUNCE_WAIT;
            state_cnt = 0;
            log_i("%s move to ST_INIT_ANNOUNCE_WAIT \n", __func__);

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_ANNOUNCE_WAIT:
        {
            state_cnt++;
            if(state_cnt >= 5)
            {
                apro_st = ST_INIT_GET_CHILD;
                state_cnt = 0;
                log_i("%s move to ST_INIT_GET_CHILD \n", __func__);
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_GET_CHILD: // read child
        {
            put_event(EV_CONF_CHILD, EV_CMD_NONE, NULL, 0);
            apro_st = ST_INIT_GET_CHILD_WAIT;
            state_cnt = 0;
            log_i("%s move to ST_INIT_GET_CHILD_WAIT \n", __func__);

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_GET_CHILD_WAIT:
        {
            state_cnt++;
            if(state_cnt >= 3)
            {
                apro_st = ST_INIT_GET_NEI;
                state_cnt = 0;
                log_i("%s move to ST_INIT_GET_NEI \n", __func__);
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_GET_NEI: // read nei
        {
            put_event(EV_CONF_NEIGHBOR, EV_CMD_NONE, NULL, 0);
            apro_st = ST_INIT_GET_NEI_WAIT;
            state_cnt = 0;
            log_i("%s move to ST_INIT_GET_NEI_WAIT \n", __func__);

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_GET_NEI_WAIT:
        {
            state_cnt++;
            if(state_cnt >= 3)
            {
                apro_st = ST_INIT_GET_ROUTE;
                state_cnt = 0;
                log_i("%s move to ST_INIT_GET_ROUTE \n", __func__);
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_GET_ROUTE: // read route table
        {
            put_event(EV_CONF_ROUTE_TABLE, EV_CMD_NONE, NULL, 0);
            apro_st = ST_INIT_GET_ROUTE_WAIT;
            state_cnt = 0;
            log_i("%s move to ST_INIT_GET_ROUTE_WAIT \n", __func__);

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_GET_ROUTE_WAIT:
        {
            state_cnt++;
            if(state_cnt >= 3)
            {
                apro_st = ST_INIT_UPDATE_EP;
                state_cnt = 0;
                log_i("%s move to ST_INIT_UPDATE_EP \n", __func__);
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_UPDATE_EP: // update end point
        {
            if(check_event_q() == 1) // empty
            {
                t_node_mgr *mgr = apro_node_get();
                while(state_cnt < mgr->node_cnt)
                {
                    if(mgr->node[state_cnt].ep_cnt == 0)
                    {
                        put_event(EV_CONF_GET_EP, EV_CMD_NONE, (char*)&(mgr->node[state_cnt].net_id), 2);
                        state_cnt++;
                        break;
                    }
                    else
                    {
                        state_cnt++;
                    }
                }

                if(state_cnt >= mgr->node_cnt)
                {
                    apro_st = ST_INIT_UPDATE_EP_WAIT;
                    state_cnt = 0;
                    log_i("%s move to ST_INIT_UPDATE_EP_WAIT \n", __func__);
                }
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
        }
        break;

    case ST_INIT_UPDATE_EP_WAIT:
        {
            state_cnt++;
            if(state_cnt >= 5)
            {
                apro_st = ST_INIT_UPDATE_CL;
                state_cnt = 0;
                log_i("%s move to ST_INIT_UPDATE_CL \n", __func__);
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
            else if(cmd == ST_CMD_NEW_NODE)
            {
                apro_st = ST_INIT_UPDATE_EP;
                state_cnt = 0;
            }
        }
        break;

    case ST_INIT_UPDATE_CL:
        {
            if(check_event_q() == 1) // empty
            {
                t_node_mgr *mgr = apro_node_get();
                while(state_cnt < mgr->node_cnt)
                {
                    if(mgr->node[state_cnt].ep_cnt > 0)
                    {
                        u8 update_cl = 0;
                        int i;
                        for(i = 0; i < APRO_MAX_EP; i++)
                        {
                            if(mgr->node[state_cnt].ep[i].ep_id != 0 &&
                                mgr->node[state_cnt].ep[i].cl_cnt == 0)
                            {
                                char buf[3] = {0,};
                                memcpy(buf, (char*)&mgr->node[state_cnt].net_id, 2);
                                buf[2] = mgr->node[state_cnt].ep[i].ep_id;
                                put_event(EV_CONF_GET_CL, EV_CMD_NONE, buf, 3);
                                update_cl = 1;
                            }
                        }
                        state_cnt++;
                        if(update_cl == 1)
                        {
                            break;
                        }
                    }
                    else
                    {
                        state_cnt++;
                    }
                }

                if(state_cnt >= mgr->node_cnt)
                {
                    apro_st = ST_INIT_UPDATE_CL_WAIT;
                    state_cnt = 0;
                    log_i("%s move to ST_INIT_UPDATE_CL_WAIT \n", __func__);
                }
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
            else if(cmd == ST_CMD_NEW_NODE)
            {
                apro_st = ST_INIT_UPDATE_EP;
                state_cnt = 0;
            }
        }
        break;

    case ST_INIT_UPDATE_CL_WAIT:
        {
            state_cnt++;
            if(state_cnt >= 5)
            {
                apro_st = ST_OCF_CREATE;
                state_cnt = 0;
                apro_node_file_wirte();
                if(apro_get_web_regi() == WEB_REGI_OPEN)
                {
                    char buf[32] = {0,};
                    u8 buf_len = 0;
                    apro_ipc_payload_regi_done(buf, &buf_len, 0);
                    put_event(EV_IPC_SEND, EV_CMD_IPC_NOTI, buf, buf_len);
                    apro_set_web_regi(WEB_REGI_NONE);
                }
                log_i("%s move to ST_OCF_CREATE \n", __func__);
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
            else if(cmd == ST_CMD_NEW_NODE)
            {
                apro_st = ST_INIT_UPDATE_EP;
                state_cnt = 0;
            }
            else if(cmd == ST_CMD_UPDATE_CL)
            {
                apro_st = ST_INIT_UPDATE_CL;
                state_cnt = 0;
            }
        }
        break;

    case ST_OCF_CREATE:
        {
#if 1 // wichoi 20201120 RS Demo
            t_node_mgr *mgr = apro_node_get();
            if(mgr->node_cnt == 0)
            {
                log_i("%s Zigbee Node Not Exist !!! \n", __func__);
                apro_st = ST_IDLE;
                state_cnt = 0;
                log_i("%s move to ST_IDLE \n", __func__);
            }
            else
#endif
            if(apro_get_ocf_st() == OCF_ST_NONE)
            {
                if(apro_ocf_create() == RET_SUCCESS)
                {
                    if(apro_ocf_add_dev(apro_node_get()) == RET_SUCCESS)
                    {
                        if(apro_ocf_start() == RET_SUCCESS)
                        {
                            apro_set_ocf_st(OCF_ST_STANDBY);
                            apro_st = ST_IDLE;
                            state_cnt = 0;
                            log_i("%s move to ST_IDLE \n", __func__);
                        }
                    }
                }
            }
            else
            {
                apro_st = ST_IDLE;
                state_cnt = 0;
                log_i("%s move to ST_IDLE \n", __func__);
            }

            if(state_cnt++ > 5)
            {
                log_i("%s ocf start failed \n", __func__);
                apro_st = ST_IDLE;
                state_cnt = 0;
                log_i("%s move to ST_IDLE \n", __func__);
            }

            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
            else if(cmd == ST_CMD_NEW_NODE)
            {
                apro_st = ST_INIT_UPDATE_EP;
                state_cnt = 0;
            }
            else if(cmd == ST_CMD_UPDATE_CL)
            {
                apro_st = ST_INIT_UPDATE_CL;
                state_cnt = 0;
            }
        }
        break;

    case ST_IDLE:
        {
            if(cmd == ST_CMD_ERR)
            {
                apro_st = ST_NONE;
            }
            else if(cmd == ST_CMD_NEW_NODE)
            {
                apro_st = ST_INIT_UPDATE_EP;
                state_cnt = 0;
            }
            else if(cmd == ST_CMD_UPDATE_CL)
            {
                apro_st = ST_INIT_UPDATE_CL;
                state_cnt = 0;
            }
        }
        break;

    case ST_EXIT:
        break;

    default:
        break;
    }

    return apro_st;
}

