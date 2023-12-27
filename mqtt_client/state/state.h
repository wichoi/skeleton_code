#ifndef _STATE_H_
#define _STATE_H_

#define MQTT_NONE           0
#define MQTT_INIT           1
#define MQTT_CONNECTING     2
#define MQTT_SUBSCRIBE      3
#define MQTT_CONNECTED      4
#define MQTT_RECONNECTED    5
#define MQTT_DISCONNECT     6

int mqtt_state(int state);

#endif
