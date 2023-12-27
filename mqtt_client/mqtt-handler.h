#ifndef _MQTT_HANDLER_H_
#define _MQTT_HANDLER_H_

#define MQTT_SUCCESS    0   // MQTTASYNC_SUCCESS
#define MQTT_FALSE      -1  // MQTTASYNC_FAILURE

#define MQTT_QOS_0      0   // At most onec (Fire and forget)
#define MQTT_QOS_1      1   // At least once
#define MQTT_QOS_2      2   // Exactly once (Once and one only)

typedef void ConnectedCb(int result);
typedef void ReconnectedCb(char* cause);
typedef void SubscribedCb(int result);
typedef void DisconnectedCb(int result);
typedef void ConnectionLostCb(char* cause);
typedef void MessageDeliveredCb(int token);
typedef void MessageArrivedCb(char* topic, char* payload, int payloadLen);

int mqtt_create(char* host, int port, char* clientID);
int mqtt_setcb(ConnectedCb* cc, ReconnectedCb* re, SubscribedCb* sc, DisconnectedCb* dc,
        ConnectionLostCb* clc, MessageDeliveredCb* mdc, MessageArrivedCb* mac);
int mqtt_connect(int keepalive, char* userName, char* password,
                            int enableServerCertAuth, int cleanSession);
int mqtt_subscribe(char* topic, int qos);
int mqtt_subscribe_array(char**topics, int cnt, int qos);
int mqtt_publish(char* topic, char* payload, int qos);
int mqtt_disconnect(void);
void mqtt_distory(void);
int mqtt_isconnected(void);


#endif
