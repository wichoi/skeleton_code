#ifndef _THYME_RESOURCE_H_
#define _THYME_RESOURCE_H_

int res_ae_read(int ix, char* ty, char* to, char* rn);
int res_cnt_read(int ix, char* ty, char* to, char* rn);
int res_sub_read(int ix, char* ty, char* to, char* rn);

void res_init(void);

#endif
