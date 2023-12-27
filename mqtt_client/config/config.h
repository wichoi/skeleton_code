#ifndef _CONFIG_H_
#define _CONFIG_H_

#define PAYLOAD_SZ  512

int init_config(void);

char* get_broker_ip(void);
u16 get_broker_port(void);
int get_keepalive(void);
int get_cert_auth(void);

char* get_ae_name(void);
char* get_ae_id(void);
char* get_cse_id(void);
char* get_cb_name(void);

char* get_req_topic(void);
char* get_resp_topic(void);
char* get_noti_topic(void);
char* get_noti_resp_topic(void);
char* get_heartbeat_topic(void);

#endif
