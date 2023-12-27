#ifndef _BLE_TAS_H_
#define _BLE_TAS_H_

typedef void bleTasCb(char* data, u32 len);

int ble_tas_init(bleTasCb* cb);
int ble_tas_set(char *data, u32 len);


#endif
