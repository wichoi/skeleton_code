#ifndef _THYME_PAYLOAD_H_
#define _THYME_PAYLOAD_H_

int payload_ae(char* payload, int index, char* api);
int payload_cnt(char* payload, int index);
int payload_sub_del(char* payload, int index);
int payload_sub_create(char* payload, int index);
int payload_cin(char* payload, char* to, char* value);


#endif
