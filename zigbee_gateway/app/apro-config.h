#ifndef _APRO_CONFIG_H_
#define _APRO_CONFIG_H_

#define OCF_ST_NONE         0
#define OCF_ST_STANDBY      1

#define WEB_REGI_NONE       0
#define WEB_REGI_OPEN       1

u8 apro_get_net_st(void);
u8 apro_set_net_st(u8 state);

u8 apro_get_ocf_st(void);
u8 apro_set_ocf_st(u8 state);

u8 apro_get_web_regi(void);
u8 apro_set_web_regi(u8 resp);

#endif
