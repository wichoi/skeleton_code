#include <string.h>

#include "log.h"
#include "event.h"
#include "timer.h"

event_queue::event_queue() :
    _evt_list(),
    _ev_q(),
    _mtx(),
    _init_flag()
{
    log_d("%s\n", __func__);
}

event_queue::~event_queue()
{
    log_d("%s\n", __func__);
}

int event_queue::init(void)
{
    int ret_val = RET_OK;
    log_d("event_queue::%s\n", __func__);
    _evt_list.clear();
    _init_flag = 1;
    return ret_val;
}

int event_queue::deinit(void)
{
    int ret_val = RET_OK;
    log_d("event_queue::%s\n", __func__);
    _init_flag = 0;
    _evt_list.clear();
    _mtx.lock();
    _ev_q.clear();
    _mtx.unlock();
    return ret_val;
}

u32 event_queue::get_info(void)
{
    return _ev_q.size();
}

int event_queue::subscribe_event(int cmd, event_listener *p_listener)
{
    if(_init_flag == 0)
        return RET_ERROR;

    int ret_val = RET_OK;
    log_v("%s\n", __func__);
    event_list data(cmd, p_listener);
    _evt_list.push_back(data);
    return ret_val;
}

int event_queue::put_event(u32 cmd, u32 op_code, shared_ptr<dat_c> data)
{
    if(_init_flag == 0)
        return RET_ERROR;

    int ret_val = RET_OK;
    log_v("%s : %u, op_code : %u\n", __func__, cmd, op_code);

    _mtx.lock();
    event_c ev;
    ev._cmd = cmd;
    ev._op_code = op_code;
    ev._data = data;
    if(_ev_q.size() < QUE_MAX)
    {
        _ev_q.push_back(ev);
    }
    else
    {
        log_w("event_queue::%s queue_full[%d]\n", __func__, QUE_MAX);
        exit(1);
    }
    _mtx.unlock();
    return ret_val;
}

int event_queue::get_event(event_c &ev)
{
    if(_init_flag == 0)
        return RET_ERROR;

    int ret_val = RET_ERROR;
    log_v("%s\n", __func__);

    _mtx.lock();
    if(_ev_q.size() > 0)
    {
        log_v("%s : %u, op_code : %u\n",
                __func__, _ev_q.front()._cmd, _ev_q.front()._op_code);
        ev = move(_ev_q.front());
        _ev_q.pop_front();
        ret_val = RET_OK;
    }
    _mtx.unlock();

    return ret_val;
}

int event_queue::event_proc(void)
{
    if(_init_flag == 0)
        return RET_ERROR;

    int ret_val = RET_OK;
    //log_v("%s\n", __func__);

    event_c ev;
    if(get_event(ev) == RET_OK)
    {
        list<event_list>::iterator iter;
        for(iter = _evt_list.begin(); iter != _evt_list.end(); ++iter)
        {
            if(ev._cmd == iter->_cmd)
            {
                log_v("%s event_id[%d]\n", __func__, iter->_cmd);
                iter->_p_interface->on_event(ev);
            }

            if(_evt_list.empty())
            {
                break;
            }
        }
    }

    return ret_val;
}

