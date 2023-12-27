#include <stdio.h>
#include <string.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"

static event_q ev_q[32];
static u8 event_head = 0;
static u8 event_tail = 0;
static u8 is_active = Q_INACTIVE;

void init_event(void)
{
    memset(ev_q, 0x0, sizeof(ev_q));
    event_head = 0;
    event_tail = 0;
    is_active = Q_ACTIVE;
}

void destroy_event(void)
{
    // todo free all event
    is_active = Q_INACTIVE;
}

u8 put_event(u32 ev, u32 op_code, char* data, u32 len)
{
    u8 rc = RET_SUCCESS;
    if(is_active == Q_ACTIVE)
    {
        if(((event_head+1)&0x1F)  == event_tail)
        {
            rc = RET_ERROR;
            log_e("ERROR Event Queue Full\n");
            return rc;
        }

        event_head++;
        event_head&=0x1F;
        ev_q[event_head].event = ev;
        ev_q[event_head].op_code = op_code;
        ev_q[event_head].len = 0;
        if(len > 0 && len < Q_DATA_SZ)
        {
            ev_q[event_head].len = len;
            memcpy(ev_q[event_head].data, data, len);
        }
        log_v("put_event ev[%u] op_code[%u]\n", ev, op_code);
    }
    else
    {
        rc = RET_ERROR;
        log_e("Event Queue InActive\n");
    }

    return rc;
}

static event_q get_event(void)
{
    event_tail++;
    event_tail&=0x1F;
    event_q ev = ev_q[event_tail];
    log_v("get_event [%u]\n", ev.event);
    return ev;
}

event_q handle_event(void)
{
    event_q ev = {0,};
    if(event_head == event_tail)
    {
        //ev.event = 0;
        //memset(ev.data, 0x0, sizeof(ev.data));
        //ev.len = 0;
    }
    else
    {
        ev = get_event();
    }

    return ev;
}

u8 check_event_q(void)
{
    u8 ret = 0;
    if(event_head == event_tail)
    {
        ret = 1; // empty
    }

    log_v("%s [%u]\n", __func__, ret);
    return ret;
}

