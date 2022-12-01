/*
 * Copyright (C) 2019 Javier FILEIV <javier.fileiv@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Eclipse Paho MQTT client test application
 *
 * @author      Javier FILEIV <javier.fileiv@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "paho_mqtt.h"
#include "MQTTClient.h"
#ifdef MODULE_GNRC
#include "msg.h"
#endif
#include "ztimer.h"

#define PAHO_MQTT_PRIO                  (THREAD_PRIORITY_MAIN - 1)

#define PAHO_BUF_SIZE                   512

#define PAHO_COMMAND_TIMEOUT_MS         4000

#define MQTT_VERSION_v311               4       /* MQTT v3.1.1 */

#define DEFAULT_MQTT_CLIENT_ID          ""
#define DEFAULT_MQTT_USER               ""
#define DEFAULT_MQTT_PWD                ""
#define DEFAULT_MQTT_PORT               1883
#define DEFAULT_KEEPALIVE_SEC           0
#define IS_CLEAN_SESSION                1
#define IS_RETAINED_MSG                 0

#define MAX_TOPICS                      4
#define MAX_LEN_TOPIC                   100

#ifdef MODULE_GNRC
#define MAIN_QUEUE_SIZE                 8

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
#endif

static char _shell_buffer[SHELL_DEFAULT_BUFSIZE];

static unsigned char _paho_send_buf[PAHO_BUF_SIZE];
static unsigned char _paho_read_buf[PAHO_BUF_SIZE];

static MQTTClient client;
static Network network;
static int topic_cnt = 0;
static char _topic_to_subscribe[MAX_TOPICS][MAX_LEN_TOPIC];

static int _con(int argc, char **argv);
static int _discon(int argc, char **argv);
static int _pub(int argc, char **argv);
static int _sub(int argc, char **argv);
static int _unsub(int argc, char **argv);

static const shell_command_t _shell_commands[] = {
    { "con", "connect to a MQTT-SN broker", _con },
    { "discon", "disconnect from current broker", _discon },
    { "pub", "publish a number of bytes under a topic", _pub },
    { "sub", "subscribe to a topic", _sub },
    { "unsub", "unsubscribe from a topic", _unsub },
    { NULL, NULL, NULL },
};

static unsigned get_qos(const char *str)
{
    int qos = atoi(str);

    switch (qos) {
    case 1:     return QOS1;
    case 2:     return QOS2;
    default:    return QOS0;
    }
}

static void _on_msg_received(MessageData *data)
{
    printf("message '%.*s' (%d bytes) received on topic '%.*s'\n",
           (int)data->message->payloadlen,
           (char *)data->message->payload,
           (int)data->message->payloadlen,
           (int)data->topicName->lenstring.len,
           data->topicName->lenstring.data);
}

static int _con(int argc, char **argv)
{
    if (argc < 2) {
        printf(
            "usage: %s <brokerip addr> [port] [clientID] [user] [password] "
            "[keepalivetime]\n",
            argv[0]);
        return 1;
    }

    char *remote_ip = argv[1];

    printf("success: connecting to %s\n", argv[1]);

    int ret = -1;

    /* ensure client isn't connected in case of a new connection */
    if (client.isconnected) {
        printf("mqtt_example: client already connected, disconnecting it\n");
        MQTTDisconnect(&client);
        NetworkDisconnect(&network);
    }

    int port = DEFAULT_MQTT_PORT;
    if (argc > 2) {
        port = atoi(argv[2]);
    }

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = MQTT_VERSION_v311;

    data.clientID.cstring = DEFAULT_MQTT_CLIENT_ID;
    if (argc > 3) {
        data.clientID.cstring = argv[3];
    }

    data.username.cstring = DEFAULT_MQTT_USER;
    if (argc > 4) {
        data.username.cstring = argv[4];
    }

    data.password.cstring = DEFAULT_MQTT_PWD;
    if (argc > 5) {
        data.password.cstring = argv[5];
    }

    data.keepAliveInterval = DEFAULT_KEEPALIVE_SEC;
    if (argc > 6) {
        data.keepAliveInterval = atoi(argv[6]);
    }

    data.cleansession = IS_CLEAN_SESSION;
    data.willFlag = 0;

    ret = NetworkConnect(&network, remote_ip, port);
    if (ret < 0) {
        printf("error: unable to connect\n");
        return ret;
    }

    ret = MQTTConnect(&client, &data);
    if (ret < 0) {
        printf("error: unable to connect to broker (%d)\n", ret);
        _discon(0, NULL);
        return ret;
    }
    else {
        printf("success: connected to broker");
    }

    return (ret > 0) ? 0 : 1;
}

static int _discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    topic_cnt = 0;
    int res = MQTTDisconnect(&client);
    if (res < 0) {
        printf("error: unable to disconnect\n");
    }
    else {
        printf("success: disconnected from broker\n");
    }

    NetworkDisconnect(&network);
    return res;
}

static int _pub(int argc, char **argv)
{
    enum QoS qos = QOS0;

    if (argc < 3) {
        printf("usage: %s <topic name> <string msg> [QoS level]\n",
               argv[0]);
        return 1;
    }
    if (argc == 4) {
        qos = get_qos(argv[3]);
    }
    MQTTMessage message;
    message.qos = qos;
    message.retained = IS_RETAINED_MSG;
    message.payload = argv[2];
    message.payloadlen = strlen(message.payload);

    int rc;
    if ((rc = MQTTPublish(&client, argv[1], &message)) < 0) {
        printf("error: unable to publish (%d)\n", rc);
    }
    else {
        printf("success: published message '%s' to topic '%s' with QoS %d\n",
               argv[2], argv[1], (int)qos);
    }

    return rc;
}

static int _sub(int argc, char **argv)
{
    enum QoS qos = QOS0;

    if (argc < 2) {
        printf("usage: %s <topic name> [QoS level]\n", argv[0]);
        return 1;
    }

    if (argc >= 3) {
        qos = get_qos(argv[2]);
    }

    if (topic_cnt > MAX_TOPICS) {
        printf("error: already subscribed to max %d topics\n", topic_cnt);
        return -1;
    }

    if (strlen(argv[1]) + 1 > MAX_LEN_TOPIC) {
        printf("error: not subscribing, topic too long %s\n", argv[1]);
        return -1;
    }
    strncpy(_topic_to_subscribe[topic_cnt], argv[1], MAX_LEN_TOPIC);

    printf("success: subscribing to %s\n", _topic_to_subscribe[topic_cnt]);
    int ret = MQTTSubscribe(&client, _topic_to_subscribe[topic_cnt], qos,
                            _on_msg_received);
    if (ret < 0) {
        printf("error: unable to subscribe to %s (%d)\n",
               _topic_to_subscribe[topic_cnt], ret);
        _discon(0, NULL);
    }
    else {
        printf("success: subscribed to '%s', QoS %d\n", argv[1], (int)qos);
        topic_cnt++;
    }
    return ret;
}

static int _unsub(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <topic name>\n", argv[0]);
        return 1;
    }

    int ret = MQTTUnsubscribe(&client, argv[1]);

    if (ret < 0) {
        printf("error: unable to unsubscribe from '%s'\n", argv[1]);
        _discon(0, NULL);
    }
    else {
        printf("success: unsubscribed from '%s'\n", argv[1]);
        topic_cnt--;
    }
    return ret;
}

int main(void)
{
#ifdef MODULE_LWIP
    /* let LWIP initialize */
    ztimer_sleep(ZTIMER_MSEC, 1000);
#endif
    puts("success: starting test application");

#ifdef MODULE_GNRC
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
#endif

    /* initialize Paho MQTT client */
    NetworkInit(&network);
    MQTTClientInit(&client, &network, PAHO_COMMAND_TIMEOUT_MS, _paho_send_buf,
                   PAHO_BUF_SIZE, _paho_read_buf, PAHO_BUF_SIZE);
    MQTTStartTask(&client);

    /* start shell */
    shell_run(_shell_commands, _shell_buffer, sizeof(_shell_buffer));
    return 0;
}
