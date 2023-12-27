#include "common.h"
#include "log_service.h"

#include "charging_window.h"
#include "ui_charging_window.h"

charging_window::charging_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::charging_window)
{
    ui->setupUi(this);
}

charging_window::~charging_window()
{
    delete ui;
}

int charging_window::ui_start()
{
    int ret_val = RET_OK;
    log_d("charging_window::%s", __func__);

    connect(this, SIGNAL(show_signal()), this, SLOT(show_slot()));
    connect(this, SIGNAL(hide_signal()), this, SLOT(hide_slot()));
    connect(this, SIGNAL(event_signal(event_c)), this, SLOT(event_slot(event_c)));
    //connect(this, SIGNAL(close_signal()), this, SLOT(quit()));

    emit show_signal();
    emit hide_signal();
    return ret_val;
}

int charging_window::init(main_interface *p_main)
{
    int ret_val = RET_OK;
    log_d("charging_window::%s", __func__);

    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_SELF_TEST, this);
    _p_main->event_subscribe(event_c::CMD_CHARGING_TEST, this);

    return ret_val;
}

int charging_window::deinit(void)
{
    int ret_val = RET_OK;
    log_d("charging_window::%s", __func__);
    //emit close_signal();
    _p_main = NULL;
    return ret_val;
}

int charging_window::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    //log_d("charging_window::%s", __func__);
    emit event_signal(ev);
#if 0
    switch(ev._cmd)
    {
    case event_c::CMD_SELF_TEST:
        log_d("CMD_SELF_TEST");
        //selftest();
        break;
    default:
        break;
    }
#endif
    return ret_val;
}

int charging_window::on_timer(u32 id)
{
    int ret_val = RET_OK;
    switch(id)
    {
    case timer_service::TID_CHARGING_TEST:
        log_d("%s TID_CHARGING_TEST", __func__);
        _p_main->kill_timer(timer_service::TID_CHARGING_TEST);
        break;
    default:
        break;

    }
    return ret_val;
}

void charging_window::show_slot()
{
    log_d("charging_window::%s", __func__);
    this->show();
}

void charging_window::hide_slot()
{
    log_d("charging_window::%s", __func__);
    this->hide();
}

void charging_window::event_slot(event_c ev)
{
    //log_d("charging_window::%s", __func__);
    switch(ev._cmd)
    {
    case event_c::CMD_SELF_TEST:
        log_d("CMD_SELF_TEST");
        selftest();
        break;
    case event_c::CMD_CHARGING_TEST:
        log_d("CMD_CHARGING_TEST");
        break;
    default:
        break;
    }
}

void charging_window::closeEvent(QCloseEvent *event)
{
    log_d("charging_window::%s", __func__);
    (void)event;
    //QWidget::closeEvent(event);
    //QCoreApplication::quit();
    _p_main->event_publish(event_c::CMD_EXIT);
}

int charging_window::selftest(void)
{
    int ret_val = RET_OK;
    log_d("charging_window::%s", __func__);
    _p_main->event_publish(event_c::CMD_CHARGING_TEST);
    _p_main->set_timer(timer_service::TID_CHARGING_TEST, 1000, this);
    return ret_val;
}

