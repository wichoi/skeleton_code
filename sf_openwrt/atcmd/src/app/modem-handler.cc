#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "config-manager.h"
#include "modem-handler.h"

#define RESP_OK             "OK"
#define RESP_ERROR          "ERROR"
#define RESP_CMS            "+CME"
#define RESP_TIMEOUT        "TIMEOUT"

#define ATCMD_GSN           "AT+GSN\r\n"
#define ATCMD_ATI           "ATI\r\n"
#define ATCMD_QAPPVER       "AT+QAPPVER\r\n"
#define ATCMD_CIMI          "AT+CIMI\r\n"
#define ATCMD_CPIN          "AT+CPIN?\r\n"
#define ATCMD_QCCID         "AT+QCCID\r\n"
#define ATCMD_COPS          "AT+COPS?\r\n"
#define ATCMD_QNWINFO       "AT+QNWINFO\r\n"
#define ATCMD_QCSQ          "AT+QCSQ\r\n"
#define ATCMD_QSPN          "AT+QSPN\r\n"
#define ATCMD_CGDCONT       "AT+CGDCONT?\r\n"
#define ATCMD_CCLK          "AT+CCLK?\r\n"
#define ATCMD_QINISTAT      "AT+QINISTAT\r\n"
#define ATCMD_CNUM          "AT+CNUM\r\n"

modem_handler::modem_handler() :
    _p_main(),
    _state(ST_START),
    _rx_buffer(),
    _update_cmd(UPDATE_CMD_NONE)
{

}

modem_handler::~modem_handler()
{
    log_d("%s\n", __func__);
}

int modem_handler::init(main_interface *p_if)
{
    int ret_val = RET_OK;
    _p_main = p_if;
    _p_main->event_subscribe(event_c::CMD_AT_WEB_CMD, this);
    _p_main->event_subscribe(event_c::CMD_AT_MODEM_RX, this);
    _p_main->event_subscribe(event_c::CMD_AT_MODEM_TIMEOUT, this);
    _p_main->event_subscribe(event_c::CMD_AT_UPDATE_CMD, this);

    _state = ST_START;
    _rx_buffer = "";
    _update_cmd = UPDATE_CMD_NONE;

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int modem_handler::deinit(void)
{
    log_d("%s\n", __func__);

    return RET_OK;
}

int modem_handler::modem_proc(void)
{
    int prev_st = _state;
    switch(_state)
    {
    case ST_START:
        _rx_buffer = "";
        _state = ST_INIT;
        break;
    case ST_INIT:
        _rx_buffer = "";
        _state = ST_GSN_TX;
        break;
    case ST_GSN_TX:
        {
            at_tx(ATCMD_GSN);
            _state = ST_GSN_RX;
        }
        break;
    case ST_GSN_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_gsn(_rx_buffer);
            _rx_buffer = "";
            _state = ST_ATI_TX;
        }
        break;
    case ST_ATI_TX:
        {
            at_tx(ATCMD_ATI);
            _state = ST_ATI_RX;
        }
        break;
    case ST_ATI_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_ati(_rx_buffer);
            _rx_buffer = "";
            _state = ST_VER_TX;
        }
        break;
    case ST_VER_TX:
        {
            at_tx(ATCMD_QAPPVER);
            _state = ST_VER_RX;
        }
        break;
    case ST_VER_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_qappver(_rx_buffer);
            _rx_buffer = "";
            _state = ST_CIMI_TX;
        }
        break;
    case ST_CIMI_TX:
        {
            at_tx(ATCMD_CIMI);
            _state = ST_CIMI_RX;
        }
        break;
    case ST_CIMI_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_cimi(_rx_buffer);
            _rx_buffer = "";
            _state = ST_CPIN_TX;
        }
        break;
    case ST_CPIN_TX:
        {
            at_tx(ATCMD_CPIN);
            _state = ST_CPIN_RX;
        }
        break;
    case ST_CPIN_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_cpin(_rx_buffer);
            _rx_buffer = "";
            _state = ST_QCCID_TX;
        }
        break;
    case ST_QCCID_TX:
        {
            at_tx(ATCMD_QCCID);
            _state = ST_QCCID_RX;
        }
        break;
    case ST_QCCID_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_qccid(_rx_buffer);
            _rx_buffer = "";
            _state = ST_COPS_TX;
        }
        break;
    case ST_COPS_TX:
        {
            at_tx(ATCMD_COPS);
            _state = ST_COPS_RX;
        }
        break;
    case ST_COPS_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_cops(_rx_buffer);
            _rx_buffer = "";
            _state = ST_QNWINFO_TX;
        }
        break;
    case ST_QNWINFO_TX:
        {
            at_tx(ATCMD_QNWINFO);
            _state = ST_QNWINFO_RX;
        }
        break;
    case ST_QNWINFO_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_qnwinfo(_rx_buffer);
            _rx_buffer = "";
            _state = ST_QCSQ_TX;
        }
        break;
    case ST_QCSQ_TX:
        {
            at_tx(ATCMD_QCSQ);
            _state = ST_QCSQ_RX;
        }
        break;
    case ST_QCSQ_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_qcsq(_rx_buffer);
            _rx_buffer = "";
            _state = ST_QSPN_TX;
        }
        break;
    case ST_QSPN_TX:
        {
            at_tx(ATCMD_QSPN);
            _state = ST_QSPN_RX;
        }
        break;
    case ST_QSPN_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_qspn(_rx_buffer);
            _rx_buffer = "";
            _state = ST_CGDCONT_TX;
        }
        break;
    case ST_CGDCONT_TX:
        {
            at_tx(ATCMD_CGDCONT);
            _state = ST_CGDCONT_RX;
        }
        break;
    case ST_CGDCONT_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_cgdcont(_rx_buffer);
            _rx_buffer = "";
            _state = ST_CCLK_TX;
        }
        break;
    case ST_CCLK_TX:
        {
            at_tx(ATCMD_CCLK);
            _state = ST_CCLK_RX;
        }
        break;
    case ST_CCLK_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_cclk(_rx_buffer);
            _rx_buffer = "";
            //_state = ST_RESP;
            _state = ST_QINISTAT_TX;
        }
        break;
    case ST_QINISTAT_TX:
        {
            at_tx(ATCMD_QINISTAT);
            _state = ST_QINISTAT_RX;
        }
        break;
    case ST_QINISTAT_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_qinistat(_rx_buffer);
            _rx_buffer = "";
            _state = ST_CNUM_TX;
        }
        break;
    case ST_CNUM_TX:
        {
            at_tx(ATCMD_CNUM);
            _state = ST_CNUM_RX;
        }
        break;
    case ST_CNUM_RX:
        if(_rx_buffer.length() == 0)
        {
            //log_v("%s wait response \n", __func__);
        }
        else
        {
            at_rx_cnum(_rx_buffer);
            _rx_buffer = "";
            _state = ST_RESP;
        }
        break;
    case ST_RESP:
        if(_update_cmd == UPDATE_CMD_RECV)
        {
            _update_cmd = UPDATE_CMD_NONE;
            _p_main->event_publish(event_c::CMD_AT_WEB_RESP);
        }
        else if(_update_cmd == UPDATE_CMD_CONSOLE)
        {
            _update_cmd = UPDATE_CMD_NONE;
            //_p_main->event_publish(event_c::CMD_AT_UPDATE_RESP, event_c::OP_NONE, data);
        }
        _state = ST_END;
        break;
    case ST_END:
        break;
    default:
        _state = ST_START;
        break;
    }

    if(prev_st != _state)
    {
        //log_d("%s state[%d]\n", __func__, _state);
        config_manager::instance()->set_mod_state((u32)_state);
    }

    return RET_OK;
}

int modem_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_v("modem_handler::%s cmd[%d]\n", __func__, ev._cmd);
    switch(ev._cmd)
    {
    case event_c::CMD_AT_WEB_CMD:
        {
            if(_state == ST_END)
            {
                _state = ST_START;
                _update_cmd = UPDATE_CMD_RECV;
            }
        }
        break;
    case event_c::CMD_AT_MODEM_RX:
        {
            shared_ptr<dat_atcmd> data = static_pointer_cast<dat_atcmd>(ev._data);
            _rx_buffer = data->stream_data;
            log_i("\n%s\n", _rx_buffer.c_str());
            if(_rx_buffer.length() > 0)
            {
                _p_main->kill_timer(timer::TID_MODEM_TIMEOUT);
            }
        }
        break;
    case event_c::CMD_AT_MODEM_TIMEOUT:
        {
            _rx_buffer = RESP_TIMEOUT;
            log_i("\n%s\n", _rx_buffer.c_str());
        }
        break;
    case event_c::CMD_AT_UPDATE_CMD:
        {
            if(_state == ST_END)
            {
                _state = ST_START;
                _update_cmd = UPDATE_CMD_CONSOLE;
            }
        }
        break;
    }

    return ret_val;
}

int modem_handler::on_timer(u32 id)
{
    switch(id)
    {
    case timer::TID_MODEM_TIMEOUT:
        {
            _p_main->kill_timer(timer::TID_MODEM_TIMEOUT);
            _p_main->event_publish(event_c::CMD_AT_MODEM_TIMEOUT);
        }
        break;
    default:
        break;
    }

    return RET_OK;
}

int modem_handler::at_tx(string atcmd)
{
    log_i("modem_handler::%s %s \n", __func__, atcmd.c_str());
    u32 rx_tmout = config_manager::instance()->get_mod_timeout();
    shared_ptr<dat_atcmd> data = make_shared<dat_atcmd>();
    data->stream_data = atcmd;
    _p_main->event_publish(event_c::CMD_AT_MODEM_TX, event_c::OP_NONE, data);
    _p_main->set_timer(timer::TID_MODEM_TIMEOUT, rx_tmout, this);
    _rx_buffer = "";
    return RET_OK;
}

/*
AT+GSN
866642050496688

OK
*/
int modem_handler::at_rx_gsn(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.length() > 10)
            {
                break;
            }
        }
    }

    log_i("imei[%s]\n", token.c_str());
    config_manager::instance()->set_imei(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/*
ATI
Quectel
BG95-M3
Revision: BG95M3LAR02A04

OK
*/
int modem_handler::at_rx_ati(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    string find_str = "Revision: ";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.find(find_str) != string::npos)
            {
                token.erase(0, find_str.length());
                break;
            }
        }
    }

    log_i("revision[%s]\n", token.c_str());
    config_manager::instance()->set_revison(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* Application Firmware Version
AT+QAPPVER
+QAPPVER: 01.001.01.001

OK
*/
int modem_handler::at_rx_qappver(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    string find_str = "+QAPPVER: ";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.find(find_str) != string::npos)
            {
                token.erase(0, find_str.length());
                break;
            }
        }
    }

    log_i("version[%s]\n", token.c_str());
    config_manager::instance()->set_version(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* International Mobile Subscriber Identity (IMSI)
AT+CIMI
450061222991409

OK
*/
int modem_handler::at_rx_cimi(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.length() > 10)
            {
                break;
            }
        }
    }

    log_i("imsi[%s]\n", token.c_str());
    config_manager::instance()->set_imsi(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/*
AT+CPIN?
+CPIN: READY

OK
*/
int modem_handler::at_rx_cpin(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    string find_str = "+CPIN: ";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.find(find_str) != string::npos)
            {
                token.erase(0, find_str.length());
                break;
            }
        }
    }

    log_i("cpin[%s]\n", token.c_str());
    config_manager::instance()->set_cpin(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* Query ICCID of the (U)SIM card
AT+QCCID
+QCCID: 8982068086400061718F

OK
*/
int modem_handler::at_rx_qccid(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    string find_str = "+QCCID: ";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.find(find_str) != string::npos)
            {
                token.erase(0, find_str.length());
                break;
            }
        }
    }

    log_i("qccid[%s]\n", token.c_str());
    config_manager::instance()->set_qccid(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* +COPS: <mode>[,<format>[,<oper>][,<Act>]]
AT+COPS?
+COPS: 0,0,"LG U+",8

OK
*/
int modem_handler::at_rx_cops(string &rx_str)
{
    int result = RET_OK;
    string token = "";

    if(rx_str.find(RESP_OK) != string::npos &&
        rx_str.find("+COPS: ") != string::npos)
    {
        char *ptr = NULL;
        char *next_ptr = NULL;

        // mode
        ptr = strtok_r((char*)rx_str.c_str(), ",", &next_ptr);
        if(ptr == NULL)
        {
            result = RET_ERROR;
        }

        // format
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
        }

        // operator
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                token = ptr;
                token.erase(std::remove(token.begin(), token.end(), '"'), token.end());
            }
        }
    }

    log_i("operator[%s]\n", token.c_str());
    config_manager::instance()->set_net_operator(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* +QNWINFO: <Act>,<oper>,<band>,<channel>
AT+QNWINFO
+QNWINFO: "eMTC","45006","LTE BAND 5",2600

OK
*/
int modem_handler::at_rx_qnwinfo(string &rx_str)
{
    int result = RET_OK;
    string acc_tech = "";
    string oper_num = "";
    string band = "";
    string channel = "";
    string find_str = "+QNWINFO: ";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos &&
        (rx_str.find("NOSERVICE") == string::npos) &&
        (rx_str.find("NoService") == string::npos) &&
        (rx_str.find("No Service") == string::npos) &&
        (pos = rx_str.find(find_str) != string::npos))
    {
        char *ptr = NULL;
        char *next_ptr = NULL;

        // find +QNWINFO:
        ptr = strtok_r((char*)rx_str.c_str(), ":", &next_ptr);
        if(ptr == NULL)
        {
            result = RET_ERROR;
        }

        // access technology
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                acc_tech = ptr;
                acc_tech.erase(std::remove(acc_tech.begin(), acc_tech.end(), ' '), acc_tech.end());
                acc_tech.erase(std::remove(acc_tech.begin(), acc_tech.end(), '"'), acc_tech.end());
            }
        }

        // operator in numeric format
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                oper_num = ptr;
                oper_num.erase(std::remove(oper_num.begin(), oper_num.end(), '"'), oper_num.end());
            }
        }

        // band
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                band = ptr;
                band.erase(std::remove(band.begin(), band.end(), '"'), band.end());
            }
        }

        // channel
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                channel = ptr;
            }
        }
    }

    log_i("access_technology[%s]\n", acc_tech.c_str());
    log_i("operator_number[%s]\n", oper_num.c_str());
    log_i("band[%s]\n", band.c_str());
    log_i("channel[%d]\n", atoi(channel.c_str()));
    config_manager::instance()->set_access_technology(acc_tech);
    config_manager::instance()->set_operator_number(oper_num);
    config_manager::instance()->set_band(band);
    config_manager::instance()->set_channel(atoi(channel.c_str()));

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* <SYSMODE>, <RSSI>, <RSRP>, <SINR>, <RSRQ>
AT+QCSQ
+QCSQ: "eMTC",-35,-65,112,-15

OK
*/
int modem_handler::at_rx_qcsq(string &rx_str)
{
    int result = RET_OK;
    string sys_mode = "";
    string rssi = "";
    string rsrp = "";
    string sinr = "";
    string rsrq = "";
    string find_str = "+QCSQ: ";
    size_t pos = 0;
    if(rx_str.find(RESP_OK) != string::npos &&
        (rx_str.find("NOSERVICE") == string::npos) &&
        (rx_str.find("NoService") == string::npos) &&
        (rx_str.find("No Service") == string::npos) &&
        (pos = rx_str.find(find_str) != string::npos))
    {
        char *ptr = NULL;
        char *next_ptr = NULL;

        // find +QNWINFO:
        ptr = strtok_r((char*)rx_str.c_str(), ":", &next_ptr);
        if(ptr == NULL)
        {
            result = RET_ERROR;
        }

        // sysmode
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                sys_mode = ptr;
                sys_mode.erase(std::remove(sys_mode.begin(), sys_mode.end(), ' '), sys_mode.end());
                sys_mode.erase(std::remove(sys_mode.begin(), sys_mode.end(), '"'), sys_mode.end());
            }
        }

        // rssi
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                rssi = ptr;
            }
        }

        // rsrp
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                rsrp = ptr;
            }
        }

        // sinr
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                sinr = ptr;
            }
        }

        // rsrq
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                rsrq = ptr;
            }
        }
    }

    log_i("sys_mode[%s]\n", sys_mode.c_str());
    log_i("rssi[%d]\n", atoi(rssi.c_str()));
    log_i("rsrp[%d]\n", atoi(rsrp.c_str()));
    log_i("sinr[%d]\n", atoi(sinr.c_str()));
    log_i("rsrq[%d]\n", atoi(rsrq.c_str()));
    config_manager::instance()->set_sys_mode(sys_mode);
    config_manager::instance()->set_rssi(atoi(rssi.c_str()));
    config_manager::instance()->set_rsrp(atoi(rsrp.c_str()));
    config_manager::instance()->set_sinr(atoi(sinr.c_str()));
    config_manager::instance()->set_rsrq(atoi(rsrq.c_str()));
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* +QSPN: <FNN>,<SNN>,<SPN>,<alphabet>,<RPLMN>
AT+QSPN
+QSPN: "LG U+","LG U+","LGU+",0,"45006"

OK
*/
int modem_handler::at_rx_qspn(string &rx_str)
{
    int result = RET_OK;
    string fnn = "";
    string snn = "";
    string spn = "";
    string rplwn = "";
    string find_str = "+QSPN: ";
    size_t pos = 0;
    if(rx_str.find(RESP_OK) != string::npos &&
        (pos = rx_str.find(find_str) != string::npos))
    {
        char *ptr = NULL;
        char *next_ptr = NULL;

        // find +QSPN:
        ptr = strtok_r((char*)rx_str.c_str(), ":", &next_ptr);
        if(ptr == NULL)
        {
            result = RET_ERROR;
        }

        // fnn
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                fnn = ptr;
                fnn.erase(std::remove(fnn.begin(), fnn.end(), ' '), fnn.end());
                fnn.erase(std::remove(fnn.begin(), fnn.end(), '"'), fnn.end());
            }
        }

        // snn
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                snn = ptr;
                snn.erase(std::remove(snn.begin(), snn.end(), '"'), snn.end());
            }
        }

        // spn
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                spn = ptr;
                spn.erase(std::remove(spn.begin(), spn.end(), '"'), spn.end());
            }
        }

        // alphabet
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
        }

        // rplwn
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                rplwn = ptr;
                rplwn.erase(std::remove(rplwn.begin(), rplwn.end(), '"'), rplwn.end());
                rplwn = rplwn.substr(0, rplwn.find("\r\n"));
            }
        }
    }

    log_i("fnn[%s]\n", fnn.c_str());
    log_i("snn[%s]\n", snn.c_str());
    log_i("spn[%s]\n", spn.c_str());
    log_i("rplwn[%s]\n", rplwn.c_str());
    config_manager::instance()->set_fnn(fnn);
    config_manager::instance()->set_snn(snn);
    config_manager::instance()->set_spn(spn);
    config_manager::instance()->set_rplwn(rplwn);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/* +CGDCONT: <cid>,<PDP_type>,<APN>,<PDP_addr>,<data_comp>,<head_comp>[¡¦]
AT+CGDCONT?
+CGDCONT: 1,"IPV4V6","","0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0",0,0,0

OK
*/
int modem_handler::at_rx_cgdcont(string &rx_str)
{
    int result = RET_OK;
    string pdp_type = "";
    string apn = "";
    string pdp_addr = "";
    string find_str = "+CGDCONT: ";
    size_t pos = 0;
    if(rx_str.find(RESP_OK) != string::npos &&
        (pos = rx_str.find(find_str) != string::npos))
    {
        char *ptr = NULL;
        char *next_ptr = NULL;

        // find +CGDCONT:
        ptr = strtok_r((char*)rx_str.c_str(), ":", &next_ptr);
        if(ptr == NULL)
        {
            result = RET_ERROR;
        }

        // cid
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
        }

        // pdp_type
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                pdp_type = ptr;
                pdp_type.erase(std::remove(pdp_type.begin(), pdp_type.end(), '"'), pdp_type.end());
            }
        }

        // apn
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                apn = ptr;
                apn.erase(std::remove(apn.begin(), apn.end(), '"'), apn.end());
            }
        }

        // pdp_addr
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                pdp_addr = ptr;
                pdp_addr.erase(std::remove(pdp_addr.begin(), pdp_addr.end(), '"'), pdp_addr.end());
            }
        }
    }

    log_i("pdp_type[%s]\n", pdp_type.c_str());
    log_i("apn[%s]\n", apn.c_str());
    log_i("pdp_addr[%s]\n", pdp_addr.c_str());
    config_manager::instance()->set_pdp_type(pdp_type);
    config_manager::instance()->set_apn(apn);
    config_manager::instance()->set_pdp_addr(pdp_addr);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/*
AT+CCLK?
+CCLK: "22/03/16,08:41:52+36"

OK
*/
int modem_handler::at_rx_cclk(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    string find_str = "+CCLK: ";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.find(find_str) != string::npos)
            {
                token.erase(0, find_str.length());
                token.erase(std::remove(token.begin(), token.end(), '"'), token.end());
                break;
            }
        }
    }

    log_i("rtc[%s]\n", token.c_str());
    config_manager::instance()->set_cclk(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}


/* AT+QINISTAT

+QINISTAT: 3

OK */
int modem_handler::at_rx_qinistat(string &rx_str)
{
    string token = "";
    string delimiter = "\r\n";
    string find_str = "+QINISTAT: ";
    size_t pos = 0;

    if(rx_str.find(RESP_OK) != string::npos)
    {
        while((pos = rx_str.find(delimiter)) != string::npos)
        {
            token = rx_str.substr(0, pos);
            rx_str.erase(0, pos + delimiter.length());
            if(token.find(find_str) != string::npos)
            {
                token.erase(0, find_str.length());
                break;
            }
        }
    }

    log_i("USIM [%s]\n", token.c_str());
    config_manager::instance()->set_usim(token);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

/*
AT+CNUM

+CNUM: ,"+821222991409",145

OK
*/
int modem_handler::at_rx_cnum(string &rx_str)
{
    int result = RET_OK;
    string number = "";
    string find_str = "+CNUM: ";
    size_t pos = 0;
    if(rx_str.find(RESP_OK) != string::npos &&
        (pos = rx_str.find(find_str) != string::npos))
    {
        char *ptr = NULL;
        char *next_ptr = NULL;

        // find +QSPN:
        ptr = strtok_r((char*)rx_str.c_str(), ":", &next_ptr);
        if(ptr == NULL)
        {
            result = RET_ERROR;
        }

        //
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
        }

        // number
        if(result == RET_OK)
        {
            ptr = strtok_r(NULL, ",", &next_ptr);
            if(ptr == NULL)
            {
                result = RET_ERROR;
            }
            else
            {
                number = ptr;
                number.erase(std::remove(number.begin(), number.end(), '"'), number.end());
            }
        }
    }

    log_i("number[%s]\n", number.c_str());
    config_manager::instance()->set_number(number);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return RET_OK;
}

