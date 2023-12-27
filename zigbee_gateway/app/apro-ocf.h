#ifndef _APRO_OCF_H_
#define _APRO_OCF_H_

#ifdef __cplusplus
extern "C" {
#endif

int apro_ocf_init(void);
int apro_ocf_deinit(void);
int apro_ocf_create(void);
int apro_ocf_add_dev(t_node_mgr *mgr);
int apro_ocf_start(void);
int apro_ocf_stop(void);
int apro_ocf_send(char *data, u32 len);

#ifdef __cplusplus
};
#endif

#endif
