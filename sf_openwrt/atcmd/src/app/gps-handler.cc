#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "utils.h"
#include "event-data.h"
#include "event.h"
#include "config-manager.h"
#include "modem-handler.h"
#include "gps-handler.h"

/*
AT+QGPS?
+QGPS: 1

OK
AT+QGPS=1
OK
AT+QGPSCFG="gnssconfig",5
OK
AT+QGPSLOC=2
+QGPSLOC: 095341.000,37.38569,126.93606,2.2,53.2,2,0.00,0.0,0.0,100322,03

OK
AT+QGPSEND
OK
*/

#define GPS_RESP_OK         "OK"
#define GPS_RESP_ERROR      "ERROR"
#define GPS_RESP_CMS        "+CME"
#define GPS_RESP_TIMEOUT    "TIMEOUT"

gps_handler::gps_handler() :
    _p_main(),
    _state(ST_START),
    _ret_cnt(0),
    _gnss_read_st(GNSS_READ),
    _gnss_read_flag(0),
    _wait_killed(0),
    _update(UPDATE_NONE),
    _rx_buffer(),
    _onetime_init(0)
{

}

gps_handler::~gps_handler()
{
    log_d("%s\n", __func__);
}

int gps_handler::init(main_interface *p_if)
{
    int ret_val = RET_OK;
    _p_main = p_if;
    _p_main->event_subscribe(event_c::CMD_AT_GPS_RX, this);
    _p_main->event_subscribe(event_c::CMD_AT_GPS_TIMEOUT, this);

    _state = ST_START;
    _ret_cnt = 0;
    _gnss_read_st = GNSS_READ;
    _gnss_read_flag= 0;
    _wait_killed = 0;
    _update = UPDATE_NONE;
    _rx_buffer = "";
    _onetime_init = 0;

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int gps_handler::deinit(void)
{
    log_d("%s\n", __func__);

    return RET_OK;
}

int gps_handler::gps_proc(void)
{
    int prev_st = _state;
    switch(_state)
    {
    case ST_START:
        if(config_manager::instance()->get_gnss_enable())
        {
            utils::gps_updating_create();
            _rx_buffer = "";
            _ret_cnt = 0;
            _gnss_read_st = GNSS_READ;
            _gnss_read_flag = 0;
            _wait_killed = 0;
            _update = UPDATE_NONE;
            _state = ST_INIT;
        }
        else
        {
            // gnss read disable
            _update = UPDATE_WAIT;
            _state = ST_END;
        }
        break;
    case ST_INIT:
        if(config_manager::instance()->get_mod_state() == modem_handler::ST_END)
        {
            system("killall -9 pppd");
            _rx_buffer = "";
            _ret_cnt++;
            _gnss_read_st = GNSS_READ;
            _gnss_read_flag = 0;
            _wait_killed = 0;
            _update = UPDATE_NONE;
            _state = ST_WAIT_KILLED;
            _p_main->set_timer(timer::TID_GPS_WAIT_KILLED, 2 * 1000, this); // 2000msec
        }
        else
        {
            // wait for modem init
        }
        break;
    case ST_WAIT_KILLED:
        if(_wait_killed == 1)
        {
            _state = ST_ON_TX;
        }
        else
        {
            // wait pppd killed
        }
        break;
    case ST_ON_TX:
        {
            atcmd_tx("AT+QGPS=1\r\n");
            _state = ST_ON_RX;
            log_v("%s move to ST_ON_RX \n", __func__);
        }
        break;
    case ST_ON_RX:
        if(_rx_buffer.length() == 0)
        {
            log_v("%s wait response \n", __func__);
        }
        else if(_rx_buffer.find(GPS_RESP_OK) != string::npos)
        {
            _rx_buffer = "";
            _state = ST_CONF_TX;
            log_v("%s move to ST_CONF_TX \n", __func__);
        }
        else
        {
            _rx_buffer = "";
            _state = ST_ERR_TX;
            log_v("%s move to ST_ERR_TX \n", __func__);
        }
        break;
    case ST_CONF_TX:
        {
            u32 gnss_conf = config_manager::instance()->get_gnss_config();
            // AT+QGPSCFG="gnssconfig",1
            string cmd_str = "AT+QGPSCFG=\"gnssconfig\",";
            cmd_str.append(to_string(gnss_conf));
            cmd_str.append("\r\n");
            atcmd_tx(cmd_str);
            _state = ST_CONF_RX;
            log_v("%s move to ST_CONF_RX \n", __func__);
        }
        break;
    case ST_CONF_RX:
        if(_rx_buffer.length() == 0)
        {
            log_v("%s wait response \n", __func__);
        }
        else if(_rx_buffer.find(GPS_RESP_OK) != string::npos)
        {
            u32 fix_tout = config_manager::instance()->get_gnss_fix_timeout();
            if(_onetime_init == 0)
            {
                if(config_manager::instance()->get_latitude() == 0 &&
                   config_manager::instance()->get_longitude() == 0)
                {
                    fix_tout = 1000 * 60 * 10; // 10 min
                }
            }
            _onetime_init = 1;

            _p_main->set_timer(timer::TID_GPS_FIX_TIMEOUT, fix_tout, this);
            u32 fix_int = config_manager::instance()->get_gnss_fix_interval();
            _p_main->set_timer(timer::TID_GPS_FIX_INTERVAL, fix_int, this);
            _rx_buffer = "";
            _state = ST_LOC_TX;
            log_v("%s move to ST_LOC_TX \n", __func__);
        }
        else
        {
            _rx_buffer = "";
            _state = ST_ERR_TX;
            log_v("%s move to ST_ERR_TX \n", __func__);
        }
        break;
    case ST_LOC_TX:
        if(_gnss_read_flag == 1)
        {
            _gnss_read_flag = 0;
            atcmd_tx("AT+QGPSLOC=2\r\n");
            _state = ST_LOC_RX;
            log_v("%s move to ST_LOC_RX \n", __func__);
        }
        else
        {
            // wait for read interval;
        }
        break;
    case ST_LOC_RX:
        if(_rx_buffer.length() == 0)
        {
            log_v("%s wait response \n", __func__);
        }
        else if(_rx_buffer.find(GPS_RESP_OK) != string::npos)
        {
            // +QGPSLOC: <UTC>,<latitude>,<longitude>,<HDOP>,<altitude>,<fix>,<COG>,<spkm>,<spkn>,<date>,<nsat>
            // +QGPSLOC: 095341.000,37.38569,126.93606,2.2,53.2,2,0.00,0.0,0.0,100322,03
            char *ptr = NULL;
            char *next_ptr = NULL;
            double read_value = 0.0;
            int result = RET_OK;
            log_d("%s\n%s\n", __func__, _rx_buffer.c_str());
            ptr = strtok_r((char*)_rx_buffer.c_str(), ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
                log_d("UTC fail !!!\n");
            }

            if(result == RET_OK)
            {
                ptr = strtok_r(NULL, ",", &next_ptr);
                if(ptr == NULL)
                {
                    result = RET_ERROR;
                    log_d("latitude fail !!!\n");
                }
                else
                {
                    read_value = atof(ptr);
                    config_manager::instance()->set_latitude(read_value);
                    log_i("latitude [%lf]\n", read_value);
                }
            }

            if(result == RET_OK)
            {
                ptr = strtok_r(NULL, ",", &next_ptr);
                if(ptr == NULL)
                {
                    result = RET_ERROR;
                    log_d("longitude fail !!!\n");
                }
                else
                {
                    read_value = atof(ptr);
                    config_manager::instance()->set_longitude(read_value);
                    log_i("longitude [%lf]\n", read_value);
                }
            }

            if(result == RET_OK)
            {
                _rx_buffer = "";
                _state = ST_OFF_TX;
                _p_main->kill_timer(timer::TID_GPS_FIX_TIMEOUT);
                log_v("%s move to ST_OFF_TX \n", __func__);
                _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
            }
            else
            {
                if(_gnss_read_st == GNSS_TIMEOUT)
                {
                    _rx_buffer = "";
                    _state = ST_ERR_TX;
                    log_v("%s move to ST_ERR_TX \n", __func__);
                }
                else
                {
                    u32 fix_int = config_manager::instance()->get_gnss_fix_interval();
                    _p_main->set_timer(timer::TID_GPS_FIX_INTERVAL, fix_int, this);
                    _rx_buffer = "";
                    _state = ST_LOC_TX;
                    log_v("%s move to ST_LOC_TX \n", __func__);
                }

            }
        }
        else
        {
            if(_gnss_read_st == GNSS_TIMEOUT)
            {
                _rx_buffer = "";
                _state = ST_ERR_TX;
                log_v("%s move to ST_ERR_TX \n", __func__);
            }
            else
            {
                u32 fix_int = config_manager::instance()->get_gnss_fix_interval();
                _p_main->set_timer(timer::TID_GPS_FIX_INTERVAL, fix_int, this);
                _rx_buffer = "";
                _state = ST_LOC_TX;
                log_v("%s move to ST_LOC_TX \n", __func__);
            }
        }
        break;
    case ST_OFF_TX:
        {
            atcmd_tx("AT+QGPSEND\r\n");
            _state = ST_OFF_RX;
            log_v("%s move to ST_OFF_RX \n", __func__);
        }
        break;
    case ST_OFF_RX:
        if(_rx_buffer.length() == 0)
        {
            log_v("%s wait response \n", __func__);
        }
        else if(_rx_buffer.find(GPS_RESP_OK) != string::npos)
        {
            _rx_buffer = "";
            _state = ST_CLEAR;
            log_v("%s move to ST_CLEAR \n", __func__);
        }
        else
        {
            _rx_buffer = "";
            _state = ST_ERR_TX;
            log_v("%s move to ST_ERR_TX \n", __func__);
        }
        break;
    case ST_ERR_TX:
        {
            atcmd_tx("AT+QGPSEND\r\n");
            _state = ST_ERR_RX;
            log_v("%s move to ST_OFF_RX \n", __func__);
        }
        break;
    case ST_ERR_RX:
        if(_rx_buffer.length() == 0)
        {
            log_v("%s wait response \n", __func__);
        }
        else
        {
            log_i("%s gps read fail cnt[%d] \n", __func__, _ret_cnt);
            if(_ret_cnt < config_manager::instance()->get_gnss_retry_cnt())
            {
                _state = ST_INIT;
                log_v("%s move to ST_INIT \n", __func__);
            }
            else
            {
                _state = ST_CLEAR;
                log_v("%s move to ST_CLEAR \n", __func__);
            }
        }
        break;
    case ST_CLEAR:
        {
            //system("pppd call quectel-ppp &"); // todo
            //system("pppd call quectel-pppv6 &"); // todo
            _state = ST_END;
        }
        break;
    case ST_END:
        if(_update == UPDATE_NONE)
        {
            utils::gps_updating_remove();
            _update = UPDATE_WAIT;
            u32 loc_interval = config_manager::instance()->get_gnss_interval();
            _p_main->set_timer(timer::TID_GPS_UPDATE, loc_interval, this);
            log_i("%s TID_GPS_UPDATE [%d] \n", __func__, loc_interval);
        }
        else if(_update == UPDATE_EXECUTE)
        {
            _state = ST_START;
        }
        else
        {
            // wait for gps update time
        }
        break;
    default:
        _state = ST_START;
        break;
    }

    if(prev_st != _state)
    {
        log_d("%s state[%d]\n", __func__, _state);
        config_manager::instance()->set_gnss_state((u32)_state);
    }

    return RET_OK;
}

int gps_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_v("gps_handler::%s cmd[%d]\n", __func__, ev._cmd);
    switch(ev._cmd)
    {
    case event_c::CMD_AT_GPS_RX:
        {
            shared_ptr<dat_atcmd> data = static_pointer_cast<dat_atcmd>(ev._data);
            _rx_buffer = data->stream_data;
            log_i("\n%s\n", _rx_buffer.c_str());
            if(_rx_buffer.length() > 0)
            {
                _p_main->kill_timer(timer::TID_GPS_TIMEOUT);
            }
        }
        break;
    case event_c::CMD_AT_GPS_TIMEOUT:
        {
            _rx_buffer = GPS_RESP_TIMEOUT;
            log_i("\n%s\n", _rx_buffer.c_str());
        }
        break;
    }

    return ret_val;
}

int gps_handler::on_timer(u32 id)
{
    switch(id)
    {
    case timer::TID_GPS_FIX_TIMEOUT:
        {
            _p_main->kill_timer(timer::TID_GPS_FIX_TIMEOUT);
            _gnss_read_st = GNSS_TIMEOUT;
        }
        break;
    case timer::TID_GPS_FIX_INTERVAL:
        {
            _p_main->kill_timer(timer::TID_GPS_FIX_INTERVAL);
            _gnss_read_flag = 1;
        }
        break;
    case timer::TID_GPS_UPDATE:
        {
            _p_main->kill_timer(timer::TID_GPS_UPDATE);
            _update = UPDATE_EXECUTE;
        }
        break;
    case timer::TID_GPS_TIMEOUT:
        {
            _p_main->kill_timer(timer::TID_GPS_TIMEOUT);
            _p_main->event_publish(event_c::CMD_AT_GPS_TIMEOUT);
        }
        break;
    case timer::TID_GPS_WAIT_KILLED:
        {
            _wait_killed = 1;
            _p_main->kill_timer(timer::TID_GPS_WAIT_KILLED);
        }
        break;
    default:
        break;
    }

    return RET_OK;
}

int gps_handler::atcmd_tx(string atcmd)
{
    log_i("gps_handler::%s %s \n", __func__, atcmd.c_str());
    u32 rx_tmout = config_manager::instance()->get_gnss_rx_timeout();
    shared_ptr<dat_atcmd> data = make_shared<dat_atcmd>();
    data->stream_data = atcmd;
    _p_main->event_publish(event_c::CMD_AT_GPS_TX, event_c::OP_NONE, data);
    _p_main->set_timer(timer::TID_GPS_TIMEOUT, rx_tmout, this);
    _rx_buffer = "";
    return RET_OK;
}

