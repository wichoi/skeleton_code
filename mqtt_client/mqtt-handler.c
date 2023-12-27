#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mqtt-paho/MQTTAsync.h"

#include "debug/dbg-log.h"
#include "mqtt-handler.h"

static MQTTAsync mClient = NULL;
static ConnectedCb* mConnCb;
static ReconnectedCb* mReconnCb;
static SubscribedCb* mSubsCb;
static DisconnectedCb* mDisconCb;
static ConnectionLostCb* mConLostCb;
static MessageDeliveredCb* mMsgSendCb;
static MessageArrivedCb* mMsgRcvdCb;

static void onConnect(void* context, MQTTAsync_successData* response)
{
    if(mConnCb) mConnCb(MQTT_SUCCESS);
}

static void onReConnect(void* context, char* cause)
{
    if(mReconnCb)mReconnCb(cause);
}

static void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    if(mConnCb) mConnCb((response ? response->code : MQTT_FALSE));
}

static void onSubscribe(void* context, MQTTAsync_successData* response)
{
    if(mSubsCb) mSubsCb(MQTTASYNC_SUCCESS);
}

static void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    if(mSubsCb) mSubsCb((response ? response->code : MQTT_FALSE));
}

static void onDisconnect(void* context, MQTTAsync_successData* response)
{
    if(mDisconCb) mDisconCb(MQTT_SUCCESS);
}

static void onConnectionLost(void *context, char *cause)
{
    if(mConLostCb) mConLostCb(cause);
}

static int onMsgArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    if(mMsgRcvdCb) mMsgRcvdCb(topicName, message->payload, message->payloadlen);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

static void onMsgDelivered(void *context, MQTTAsync_token dt)
{
    if(mMsgSendCb) mMsgSendCb((int)dt);
}

int mqtt_create(char* host, int port, char* clientID)
{
    int rc = MQTT_FALSE;
    char serverURI[64] = {0,};

    if(mClient != NULL)
    {
        log_I("mqtt_create() already mClient exist [%x] !!!\n", mClient);
        return rc;
    }

    snprintf(serverURI, sizeof(serverURI), "%s:%d", host, port);
    log_I("mqtt_create() %s, %s\n", serverURI, clientID);
    rc = MQTTAsync_create(&mClient, serverURI, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    return rc;
}

int mqtt_setcb(ConnectedCb* cc, ReconnectedCb* re, SubscribedCb* sc, DisconnectedCb* dc,
        ConnectionLostCb* clc, MessageDeliveredCb* mdc, MessageArrivedCb* mac)
{
    int rc = MQTT_FALSE;

    if(mClient == NULL)
    {
        log_I("mqtt_setcb() mClient is not Exist\n");
        return rc;
    }
    log_I("mqtt_setcb() mClient[%x]\n", mClient);

    mConnCb = cc;
    mReconnCb = re;
    mSubsCb = sc;
    mDisconCb = dc;
    mConLostCb = clc;
    mMsgSendCb = mdc;
    mMsgRcvdCb = mac;

    rc = MQTTAsync_setCallbacks(mClient, NULL, onConnectionLost, onMsgArrived, onMsgDelivered);
    rc = MQTTAsync_setConnected(mClient, NULL, onReConnect);

    return rc;
}

int mqtt_connect(int keepalive, char* userName, char* password,
                            int enableServerCertAuth, int cleanSession)
{
    int rc = MQTT_FALSE;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;

    if(mClient == NULL)
    {
        log_I("mqtt_connect() mClient is not Exist\n");
        return rc;
    }
    log_I("mqtt_connect() mClient[%x]\n", mClient);

    conn_opts.keepAliveInterval = keepalive;
    conn_opts.cleansession = cleanSession;
    conn_opts.automaticReconnect = 0;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = mClient;
    conn_opts.username = userName;
    conn_opts.password = password;

    if(enableServerCertAuth)
        conn_opts.ssl = &ssl_opts;
    ssl_opts.enableServerCertAuth = enableServerCertAuth;

    if((rc = MQTTAsync_connect(mClient, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        mqtt_distory();
        return rc;
    }

    return rc;

}

int mqtt_subscribe(char* topic, int qos)
{
    int rc = MQTT_FALSE;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    if(mClient == NULL)
    {
        log_I("mqtt_subscribe() mClient is not Exist\n");
        return rc;
    }
    log_I("mqtt_subscribe() mClient[%x]\n", mClient);

    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = mClient;
    rc = MQTTAsync_subscribe(mClient, topic, qos, &opts);
    return rc;
}

int mqtt_subscribe_array(char**topics, int cnt, int qos)
{
    int rc = MQTT_FALSE;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int qosArray[cnt];
    int i;

    if(mClient == NULL)
    {
        log_I("mqtt_subscribe() mClient is not Exist\n");
        return rc;
    }
    log_I("mqtt_subscribe_array() mClient[%x]\n", mClient);

    for (i = 0; i < cnt; i++)
    {
        qosArray[i] = qos;
    }

    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = mClient;
    rc = MQTTAsync_subscribeMany(mClient, cnt, topics, qosArray, &opts);
    return rc;
}


int mqtt_publish(char* topic, char* payload, int qos)
{
    int rc = MQTT_FALSE;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    if(mClient == NULL || topic == NULL || payload == NULL)
    {
        log_I("mqtt_publish() mClient[%x] topic[%s], payload[%s]\n", mClient, topic, payload);
        return rc;
    }

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(payload);
    pubmsg.qos = qos;
    pubmsg.retained = 0;

    rc = MQTTAsync_sendMessage(mClient, topic, &pubmsg, &opts);
    return rc;
}

int mqtt_disconnect(void)
{
    int rc = MQTTASYNC_FAILURE;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    log_I("mqtt_disconnect()\n");
    disc_opts.onSuccess = onDisconnect;

    if(mClient != NULL)
    {
        rc = MQTTAsync_disconnect(mClient, &disc_opts);
    }
    return rc;
}

void mqtt_distory(void)
{
    log_I("mqtt_distory()\n");

    if(mClient != NULL)
    {
        if(MQTTAsync_isConnected(mClient))
        {
            mqtt_disconnect();
        }
        MQTTAsync_destroy(&mClient);
        mClient = NULL;
    }
}

int mqtt_isconnected(void)
{
    int rc = MQTT_FALSE;
    if(mClient)
    {
        rc = MQTTAsync_isConnected(mClient);
    }
    return rc;
}

