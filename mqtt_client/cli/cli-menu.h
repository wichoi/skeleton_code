#ifndef _CLI_MENU_H_
#define _CLI_MENU_H_

int cli_proc(void);

int cli_test_publish_ae(char *data, u32 len);
int cli_test_publish_cnt(char *data, u32 len);
int cli_test_publish_cin(char *data, u32 len);
int cli_test_publish_sub(char *data, u32 len);
int cli_test_publish_resp(char *data, u32 len);
int cli_test_publish_heartbeat(char *data, u32 len);


#endif
