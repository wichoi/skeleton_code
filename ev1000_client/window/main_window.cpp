#include "common.h"
#include "log_service.h"

#include "main_window.h"
#include "ui_main_window.h"

#include "idle_window.h"
#include "charging_window.h"

main_window::main_window()
{

}

main_window::~main_window()
{
}

int main_window::ui_start()
{
    int ret_val = RET_OK;
    log_d("main_window::%s", __func__);

    _win_list.clear();
    _win_list.push_back(win_list_c(win_list_c::WIN_IDLE, make_shared<idle_window>()));
    _win_list.push_back(win_list_c(win_list_c::WIN_CHARGING, make_shared<charging_window>()));

    list<win_list_c>::iterator iter;
    for(iter = _win_list.begin(); iter != _win_list.end(); ++iter)
    {
        iter->p_handle->ui_start();
    }

    return ret_val;
}

int main_window::init(main_interface *p_main)
{
    int ret_val = RET_OK;
    log_d("main_window::%s", __func__);

    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_SELF_TEST, this);

    list<win_list_c>::iterator iter;
    for(iter = _win_list.begin(); iter != _win_list.end(); ++iter)
    {
        iter->p_handle->init(p_main);
    }

    return ret_val;
}

int main_window::deinit(void)
{
    int ret_val = RET_OK;
    log_d("main_window::%s", __func__);

    list<win_list_c>::iterator iter;
    for(iter = _win_list.begin(); iter != _win_list.end(); ++iter)
    {
        iter->p_handle->deinit();
    }

    _p_main = NULL;
    return ret_val;
}

int main_window::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    switch(ev._cmd)
    {
        case event_c::CMD_SELF_TEST:
            log_d("CMD_SELF_TEST");
            selftest();
            break;
        default:
            break;
    }
    return ret_val;
}

int main_window::on_timer(u32 id)
{
    int ret_val = RET_OK;
    switch(id)
    {
        case timer_service::TID_SELF_TEST:
        {
            log_d("%s TID_SELF_TEST", __func__);
            _p_main->kill_timer(timer_service::TID_SELF_TEST);
        }
        break;
        default:
            break;
    }
    return ret_val;
}

int main_window::selftest(void)
{
    int ret_val = RET_OK;
    log_d("main_window::%s", __func__);
    _p_main->set_timer(timer_service::TID_SELF_TEST, 1000, this);
    return ret_val;
}

