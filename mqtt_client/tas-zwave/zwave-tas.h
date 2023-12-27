#ifndef _ZWAVE_TAS_H_
#define _ZWAVE_TAS_H_

typedef void zwaveTasCb(char* data, u32 len);

int zwave_tas_init(zwaveTasCb* cb);
int zwave_tas_set(char *data, u32 len);


#endif
