#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "../debug/dbg-log.h"
#include "../event/event.h"
#include "state.h"

static int mqtt_st = MQTT_NONE;

int mqtt_state(int state)
{
    if(state != MQTT_NONE)
        mqtt_st = state;
    log_V("%s : %d\n", __func__, mqtt_st);

    switch(mqtt_st)
    {
    case MQTT_NONE:
        break;

    case MQTT_INIT:
        put_event(AE_CONNECT, NULL, 0);
        mqtt_st = MQTT_CONNECTING;
        break;

    case MQTT_CONNECTING:
        break;

    case MQTT_SUBSCRIBE:
        put_event(AE_SUBSCRIBE, NULL, 0);
        mqtt_st = MQTT_CONNECTED;
        break;

    case MQTT_CONNECTED:
        break;

    case MQTT_RECONNECTED:
        put_event(AE_SUBSCRIBE, NULL, 0);
        mqtt_st = MQTT_CONNECTED;
        break;

    case MQTT_DISCONNECT:
        //put_event(AE_CONNECT, NULL, 0);
        //mqtt_st = MQTT_CONNECTING;
        break;

    default:
        break;
    }

    return mqtt_st;
}

