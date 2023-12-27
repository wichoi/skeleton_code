#include "apro-common.h"
#include "apro-log.h"
#include "apro-config.h"


static s8 _net_st = 0; // EmberNetworkStatus
static u8 _ocf_st = OCF_ST_NONE;
static u8 _web_regi_st = WEB_REGI_NONE;

u8 apro_get_net_st(void)
{
    log_d("%s: 0x%02X\n", __func__, _net_st);
    return _net_st;
}

u8 apro_set_net_st(u8 state)
{
    _net_st = state;
    log_d("%s: 0x%02X\n", __func__, _net_st);
    return _net_st;
}

u8 apro_get_ocf_st(void)
{
    log_d("%s: 0x%02X\n", __func__, _ocf_st);
    return _ocf_st;
}

u8 apro_set_ocf_st(u8 state)
{
    _ocf_st = state;
    log_d("%s: 0x%02X\n", __func__, _ocf_st);
    return _ocf_st;
}

u8 apro_get_web_regi(void)
{
    log_d("%s: 0x%02X\n", __func__, _web_regi_st);
    return _web_regi_st;
}

u8 apro_set_web_regi(u8 state)
{
    _web_regi_st = state;
    log_d("%s: 0x%02X\n", __func__, _web_regi_st);
    return _web_regi_st;
}

