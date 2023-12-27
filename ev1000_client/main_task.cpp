#include "common.h"
#include "log_service.h"
#include "config_manager.h"
#include "main_task.h"

main_task::main_task(QObject *parent) :
    QObject(parent),
    _config_handle(),
    _event_handle(),
    _timer_handle(),
    _cli_handle(),
    _media_handle(),
    _ui_handle()
{
    _state = 0;
}

main_task::~main_task()
{

}

void main_task::task_start(const QApplication &a)
{
    qRegisterMetaType<event_c>("event_c");

    this->moveToThread(&_main_thread);
    QObject::connect(&_main_thread, SIGNAL(started()), this, SLOT(main_proc()));
    QObject::connect(this, SIGNAL(finished()), &_main_thread, SLOT(quit()));
    QObject::connect(&_main_thread, SIGNAL(finished()), &a, SLOT(quit()));
    _main_thread.start();

    _timer_handle.task_start();
    _cli_handle.task_start();
    _media_handle.task_start();
    _ui_handle.ui_start();
}

int main_task::main_init(void)
{
    int ret_val = RET_OK;
    log_i("%s state[%d]", __func__, _state);
    _config_handle.init();
    _event_handle.init();
    //_event_handle.add_event(event_c::CMD_INIT, this);
    _event_handle.subscribe_event(event_c::CMD_DEINIT, this);

    // test cmd
    _event_handle.subscribe_event(event_c::CMD_HELLOWORLD, this);

    // timer cmd
    _event_handle.subscribe_event(event_c::CMD_TIMER_SET, this);
    _event_handle.subscribe_event(event_c::CMD_TIMER_KILL, this);
    _event_handle.subscribe_event(event_c::CMD_TIMER_PRINT, this);

    _event_handle.subscribe_event(event_c::CMD_EXIT, this);

    _timer_handle.init(config_manager::instance()->get_timer_resolution());
    _cli_handle.init(this, 100);
    _media_handle.init(this, 100);
    _ui_handle.init(this);

    return ret_val;
}

int main_task::main_deinit(void)
{
    int ret_val = RET_OK;

    _ui_handle.deinit();
    _config_handle.deinit();
    _event_handle.deinit();
    _timer_handle.deinit();
    _cli_handle.deinit();
    _media_handle.deinit();

    return ret_val;
}

void main_task::main_proc(void)
{
    while(_state != MAIN_ST_EXIT)
    {
        //log_i("%s state[%d]", __func__, _state);
        switch(_state)
        {
        case MAIN_ST_NONE:
            _state = MAIN_ST_START;
            break;
        case MAIN_ST_START:
            if(1)
            {
                _state = MAIN_ST_INIT;
            }
            break;
        case MAIN_ST_INIT:
            if(main_init() == RET_OK)
            {
                _state = MAIN_ST_IDLE;
            }
            break;
        case MAIN_ST_IDLE:
            if(main_event() != RET_OK)
            {
                _state = MAIN_ST_DEINIT;
            }
            break;
        case MAIN_ST_DEINIT:
            if(main_deinit() == RET_OK)
            {
                _state = MAIN_ST_EXIT;
            }
            break;
        case MAIN_ST_EXIT:
            break;
        default:
            break;
        }
        QThread::msleep(100);
    }

    emit finished();
    //_main_thread.quit();
    _main_thread.wait();
    log_i("%s exit !!!", __func__);
}

int main_task::event_subscribe(u32 cmd, event_listener *p_listener)
{
    int ret_val = RET_OK;
    //log_d("%s", __func__);
    _event_handle.subscribe_event(cmd, p_listener);
    return ret_val;
}

int main_task::event_publish(u32 cmd, u32 op_code, shared_ptr<ev_data> data)
{
    int ret_val = RET_OK;
    //log_d("%s", __func__);
    _event_handle.put_event(cmd, op_code, data);
    return ret_val;
}

int main_task::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    //log_d("main_client::%s cmd[%d]", __func__, ev._cmd);
    switch(ev._cmd)
    {
    case event_c::CMD_DEINIT:
        main_deinit();
        break;
    case event_c::CMD_HELLOWORLD:
        hello_world(ev);
        break;
    case event_c::CMD_TIMER_SET:
    {
        shared_ptr<data_timer> recv = static_pointer_cast<data_timer>(ev._data);
        _timer_handle.set_timer(recv->id, recv->interval, recv->p_timer);
    }
    break;
    case event_c::CMD_TIMER_KILL:
    {
        shared_ptr<data_timer> recv = static_pointer_cast<data_timer>(ev._data);
        _timer_handle.kill_timer(recv->id);
    }
    break;
    case event_c::CMD_TIMER_PRINT:
        print_timer(ev);
        break;
    case event_c::CMD_EXIT:
        ret_val = exit(ev);
        break;
    default:
        //log_d("%s should execute cmd[%d]", __func__, ev._cmd);
        break;
    }

    return ret_val;
}

int main_task::set_timer(u32 id, u32 interval_ms, timer_listener *p_timer)
{
    int ret_val = RET_OK;
    //log_d("%s id[%u] interval[%u]", __func__, id, interval_ms);
    shared_ptr<data_timer> data = make_shared<data_timer>();
    data->id = id;
    data->interval = interval_ms;
    data->p_timer = p_timer;
    event_publish(event_c::CMD_TIMER_SET, event_c::OP_NONE, data);
    return ret_val;
}

int main_task::kill_timer(u32 id)
{
    int ret_val = RET_OK;
    //log_d("%s id[%u]", __func__,  id);
    shared_ptr<data_timer> data = make_shared<data_timer>();
    data->id = id;
    event_publish(event_c::CMD_TIMER_KILL, event_c::OP_NONE, data);
    return ret_val;
}

int main_task::on_timer(u32 id)
{
    int ret_val = RET_OK;
    //log_d("%s id[%u]", __func__, id);
    switch(id)
    {
        case timer_service::TID_HELLO_WORLD:
        {
            kill_timer(timer_service::TID_HELLO_WORLD);
        }
        break;
        case timer_service::TID_REBOOT:
        {
            kill_timer(timer_service::TID_REBOOT);
            event_publish(event_c::CMD_REBOOT);
        }
        break;
        case timer_service::TID_EXIT:
        {
            kill_timer(timer_service::TID_EXIT);
            event_publish(event_c::CMD_EXIT);
        }
        break;
        default:
            break;
    }
    return ret_val;
}

int main_task::print_timer(const event_c &ev)
{
    int ret_val = RET_OK;
    ev;
    _timer_handle.print_timer();
    return ret_val;
}

int main_task::main_event(void)
{
    int ret_val = RET_OK;
    ret_val = _event_handle.event_proc();
    return ret_val;
}

int main_task::hello_world(const event_c &ev)
{
    int ret_val = RET_OK;
    if(ev._data != NULL)
    {
        shared_ptr<data_string> data = static_pointer_cast<data_string>(ev._data);
        log_i("main_task::%s %s", __func__, data->str_val.c_str());
    }
    else
    {
        log_i("main_task::%s", __func__);
    }
    return ret_val;
}

int main_task::exit(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_d("main_task::%s", __func__);
    (void)ev;
    return ret_val;
}

int main_task::main_state(void)
{
    return _state;
}

