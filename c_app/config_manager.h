#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

int config_init(void);
int config_deinit(void);

int config_read(void);
int config_write(void);
int config_update(void);

const char* config_version_get(void);
int config_version_set(char *value);

const char* config_model_name_get(void);
int config_model_name_set(char *value);

int config_main_state_get(void);
int config_main_state_set(int value);

int config_secc_state_get(void);
int config_secc_state_set(int value);

#endif

