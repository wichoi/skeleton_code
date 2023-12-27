#ifndef _ZIGBEE_TAS_H_
#define _ZIGBEE_TAS_H_

typedef void zigbeeTasCb(char* data, u32 len);

int zigbee_tas_init(zigbeeTasCb* cb);
int zigbee_tas_set(char *data, u32 len);

#endif
