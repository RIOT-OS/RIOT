
#include "mqtts_riot_iface.h"
#include <gnrc_wolfssl_tls.h>

#include <stdio.h>

#include "paho_mqtt.h"
#include "MQTTClient.h"
#include "msg.h"

#ifndef MAX_LEN_TOPIC
#define MAX_LEN_TOPIC 64
#endif

#ifndef MQTTS_PORT
#define MQTTS_PORT 8883
#endif

#ifndef MQTTS_DEFAULT_KEEP_ALIVE_INTERVAL_SEC
#define MQTTS_DEFAULT_KEEP_ALIVE_INTERVAL_SEC 240
#endif

#ifndef MQTTS_DEFAULT_COMMAND_TIMEOUT_MS
#define MQTTS_DEFAULT_COMMAND_TIMEOUT_MS 5000
#endif

#ifndef MQTTS_DEFAULT_IS_CLEAN_SESSION
#define MQTTS_DEFAULT_IS_CLEAN_SESSION true
#endif

#ifndef MQTTS_DEFAULT_IS_RETAINED_MSG
#define MQTTS_DEFAULT_IS_RETAINED_MSG false
#endif

#ifndef MAX_TOPICS
#define MAX_TOPICS 8
#endif

#ifndef MQTT_VERSION_v311
#define MQTT_VERSION_v311 4 /* MQTT v3.1.1 version is 4 */
#endif

static int topic_cnt = 0;
static char _topic_to_subscribe[MAX_TOPICS][MAX_LEN_TOPIC];

struct MQTTSContext {
    TLSContext *tls_ctx;
    MQTTClient *mqtt_client;
    int keep_alive_interval_sec;
    int is_clean_session;
    int is_retained_msg;
    int command_timeout_ms;
};
static MQTTSContext mqttsContext;
static int mqtts_initialized = 0;
static Network networkStack;
static MQTTClient client;

static MqttsMessageArrivalCallback app_layer_callback;
void mqtts_set_message_arrival_callback(MqttsMessageArrivalCallback _callback)
{
    app_layer_callback = _callback;
}

static void _on_msg_received(MessageData *data)
{
    if (app_layer_callback != NULL) {
        app_layer_callback(data->topicName->lenstring.data, (int)data->topicName->lenstring.len,
                           (char *)data->message->payload, (int)data->message->payloadlen);
    }
    else {
        printf("paho_mqtt_example: message received on topic"
               " %.*s: %.*s\n",
               (int)data->topicName->lenstring.len,
               data->topicName->lenstring.data, (int)data->message->payloadlen,
               (char *)data->message->payload);
    }
}

int MQTTS_Read(Network *n, unsigned char *buf, int buf_len, int timeout_ms)
{
    return tls_receive(((MQTTSContext *)n->app_context)->tls_ctx, buf, buf_len, timeout_ms);
}
int MQTTS_Write(Network *n, unsigned char *buf, int buf_len, int timeout_ms)
{
    return tls_send(((MQTTSContext *)n->app_context)->tls_ctx, buf, buf_len, timeout_ms);
}
int mqtts_init(MQTTSContext *mqtts_ctx, unsigned char *writebuf, int writebuf_size,
               unsigned char *readbuf, int readbuf_size)
{
    if (mqtts_initialized) {
        tls_log("[MQTTS]: module already initialized");
        return 0;
    }
    networkStack.mqttread = MQTTS_Read;
    networkStack.mqttwrite = MQTTS_Write;
    networkStack.app_context = mqtts_ctx;

    if (tls_init(mqtts_ctx->tls_ctx) < 0) {
        tls_close(mqtts_ctx->tls_ctx);
        return -1;
    }

    // Perform initialization steps if needed

    tls_log("[MQTTS]: module initialized");

    MQTTClientInit(mqtts_ctx->mqtt_client, &networkStack, mqtts_ctx->command_timeout_ms, writebuf,
                   writebuf_size,
                   readbuf,
                   readbuf_size);

    mqtts_initialized = 1;
    return 0;
}

MQTTSContext *mqtts_create_context(void (*_log_callback)(const char *message))
{
    TLSContext *tls_ctx = tls_create_context(_log_callback); // TLSContext is forward decleared in the header file

    if (tls_ctx == NULL) {
        tls_log("tls context creation failed");
        return NULL;
    }

    if (mqtts_set_tls_context(&mqttsContext, tls_ctx) < 0) {
        return NULL;
    }

    mqttsContext.command_timeout_ms = MQTTS_DEFAULT_COMMAND_TIMEOUT_MS;
    mqttsContext.keep_alive_interval_sec = MQTTS_DEFAULT_KEEP_ALIVE_INTERVAL_SEC;
    mqttsContext.is_clean_session = MQTTS_DEFAULT_IS_CLEAN_SESSION;
    mqttsContext.is_retained_msg = MQTTS_DEFAULT_IS_RETAINED_MSG;
    mqttsContext.mqtt_client = &client;
    return &mqttsContext;
}

int mqtts_set_tls_context(MQTTSContext *mqtts_ctx, TLSContext *tls_ctx)
{
    if (mqtts_ctx == NULL || tls_ctx == NULL) {
        tls_log("Invalid MQTT-S or TLS context");
        return -1;
    }

    mqtts_ctx->tls_ctx = tls_ctx;
    return 0;
}

int mqtts_connect(MQTTSContext *mqtts_ctx, char *remoteAddress, char *clientID,  char *username,
                  char *password)
{
    if (mqtts_ctx == NULL) {
        tls_log("Invalid MQTT-S context");
        return -1;
    }

    /* ensure client isn't connected in case of a new connection */
    if (mqtts_ctx->mqtt_client->isconnected) {
        printf("[MQTTS]: client already connected, try disconnecting it\n");
        return -1;
    }

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    data.MQTTVersion = MQTT_VERSION_v311;

    data.keepAliveInterval = mqtts_ctx->keep_alive_interval_sec;
    data.cleansession = mqtts_ctx->is_clean_session;
    data.willFlag = 0;

    data.username.cstring = username;
    data.clientID.cstring = clientID;
    data.password.cstring = password;

    // Perform MQTT connection setup using Paho MQTT and TLS module
    int ret = -1;

    ret = tls_establish_connection(remoteAddress, MQTTS_PORT, mqtts_ctx->tls_ctx);
    if (ret < 0) {
        printf("[MQTTS]: tls_establish_connection failed with return code %d\n", ret);
        tls_close(mqtts_ctx->tls_ctx);
        return ret;
    }
    // paho mqtt
    MQTTStartTask(mqtts_ctx->mqtt_client);
    ret = MQTTConnect(mqtts_ctx->mqtt_client, &data);
    if (ret < 0) {
        printf("[MQTTS]: MQTTConnect failed %d\n", ret);
        mqtts_disconnect(mqtts_ctx);
    }
    return ret;
}

int mqtts_publish(MQTTSContext *mqtts_ctx, const char *topic, unsigned char *payload, int qos,
                  int retained)
{
    if (mqtts_ctx == NULL) {
        tls_log("Invalid MQTTS context");
        return -1;
    }
    MQTTMessage message;

    message.qos = qos;
    message.retained = mqtts_ctx->is_retained_msg;
    message.payload = payload;
    message.payloadlen = strlen(message.payload);

    int rc;

    if ((rc = MQTTPublish(mqtts_ctx->mqtt_client, topic, &message)) < 0) {
        printf("[MQTTS]: MQTTPublish failed to publish (%d)\n", rc);
    }
    return rc;
}

int mqtts_subscribe(MQTTSContext *mqtts_ctx, const char *topic, int qos)
{
    if (mqtts_ctx == NULL) {
        tls_log("Invalid MQTTS context");
        return -1;
    }

    // Perform MQTT subscribe using Paho MQTT and TLS module
    if (topic_cnt > MAX_TOPICS) {
        printf("[MQTTS]: Already subscribed to max %d topics,"
               "call 'unsub' command\n",
               topic_cnt);
        return -1;
    }

    if (strlen(topic) > MAX_LEN_TOPIC) {
        printf("[MQTTS]: Not subscribing, topic too long %s\n", topic);
        return -1;
    }
    strncpy(_topic_to_subscribe[topic_cnt], topic, strlen(topic));

    int ret = MQTTSubscribe(mqtts_ctx->mqtt_client,
                            _topic_to_subscribe[topic_cnt], qos, _on_msg_received);

    if (ret < 0) {
        printf("[MQTTS]: Unable to subscribe to %s (%d)\n",
               _topic_to_subscribe[topic_cnt], ret);
        mqtts_disconnect(mqtts_ctx);
    }
    else {
        topic_cnt++;
    }
    return ret;
}
int mqtts_unsubscribe(MQTTSContext *mqtts_ctx, const char *topic)
{
    if (mqtts_ctx == NULL) {
        tls_log("Invalid MQTT-S context");
        return -1;
    }

    // Perform MQTT subscribe using Paho MQTT and TLS module
    int ret = MQTTUnsubscribe(mqtts_ctx->mqtt_client, topic);

    if (ret < 0) {
        printf("[MQTTS]: Unable to unsubscribe from topic: %s\n", topic);
        mqtts_disconnect(mqtts_ctx);
    }
    else {
        topic_cnt--;
    }
    return ret;
}
int mqtts_disconnect(MQTTSContext *mqtts_ctx)
{
    if (mqtts_ctx == NULL) {
        return 0;
    }
    // Perform MQTT disconnection using Paho MQTT and TLS module
    topic_cnt = 0;
    int res = 0;

    if (mqtts_ctx->mqtt_client->isconnected == false) {
        goto close_tls_layer;
    }
    res = MQTTDisconnect(mqtts_ctx->mqtt_client);
    if (res < 0) {
        printf("[MQTTS]: Unable to disconnect\n");
    }
    else {
        printf("[MQTTS]: Disconnect successful\n");
    }
close_tls_layer:
    mqtts_cleanup();
    return res;
}

void mqtts_cleanup(void)
{
    if (!mqtts_initialized) {
        tls_log("[MQTTS]: module not initialized");
        return;
    }

    // Perform cleanup steps if needed
    printf("[MQTTS]: MQTTS module cleaned up");
    mqtts_initialized = 0;
}
int mqtts_set_certificate(MQTTSContext *mqtts_ctx, const char *ca_cert_path,
                          const char *cert_chain_path, const char *private_key_path)
{
    if (mqtts_ctx == NULL) {
        tls_log("Invalid MQTT-S context");
        return -1;
    }
    return tls_set_certificate(mqtts_ctx->tls_ctx, ca_cert_path, cert_chain_path, private_key_path);
}
