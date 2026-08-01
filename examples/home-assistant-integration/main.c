/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       RIOT node integrated with Home-Assistant
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include "shell.h"
#include "fmt.h"
#include "luid.h"
#include "paho_mqtt.h"
#include "MQTTClient.h"
#include "mjson.h"
#include "saul_reg.h"
#include "saul.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static MQTTClient client;
static Network network;
static unsigned topic_cnt = 0;

#ifndef HASS_DISCOVERY_PREFIX
#define HASS_DISCOVERY_PREFIX           "homeassistant"
#endif

#define BUF_SIZE                        1024
#define MQTT_VERSION_v311               4       /* MQTT v3.1.1 version is 4 */
#define COMMAND_TIMEOUT_MS              4000

#ifndef DEFAULT_MQTT_CLIENT_ID
#define DEFAULT_MQTT_CLIENT_ID          ""
#endif

#ifndef DEFAULT_MQTT_USER
#define DEFAULT_MQTT_USER               ""
#endif

#ifndef DEFAULT_MQTT_PWD
#define DEFAULT_MQTT_PWD                ""
#endif

/**
 * @brief Default MQTT port
 */
#define DEFAULT_MQTT_PORT               1883

/**
 * @brief Keepalive timeout in seconds
 */
#define DEFAULT_KEEPALIVE_SEC           10

#ifndef MAX_LEN_TOPIC
#define MAX_LEN_TOPIC                   100
#endif

#ifndef MAX_TOPICS
#define MAX_TOPICS                      4
#endif

#define IS_CLEAN_SESSION                1
#define IS_RETAINED_MSG                 0

#ifndef HASS_RIOT_DEV_NAME
#ifndef HASS_RIOT_UID_LEN
#define HASS_RIOT_UID_LEN               6
#endif

#ifndef HASS_RIOT_DEV_PREFIX
#define HASS_RIOT_DEV_PREFIX            "RIOT-"
#endif

static char hass_id[sizeof(HASS_RIOT_DEV_PREFIX) + HASS_RIOT_UID_LEN] = HASS_RIOT_DEV_PREFIX;
#endif /* HASS_RIOT_DEV_NAME */

static void _init_hass_dev_name(void)
{
#ifndef HASS_RIOT_DEV_NAME
    size_t buf_len = (HASS_RIOT_UID_LEN + 1) / 2;
    uint8_t luid_buf[buf_len];

    luid_custom(luid_buf, buf_len, 0);

    fmt_bytes_hex(hass_id + sizeof(HASS_RIOT_DEV_PREFIX) - 1, luid_buf, buf_len);
#endif
}

static const char *_get_hass_dev_name(void)
{
#ifndef HASS_RIOT_DEV_NAME
    return hass_id;
#else
    return HASS_RIOT_DEV_NAME;
#endif
}

static const char *_saul_reg_to_hass_type(const saul_reg_t *reg)
{
    uint8_t type = reg->driver->type;
    if ((type & SAUL_CAT_MASK) == SAUL_CAT_SENSE) {
        return "sensor";
    }
    else if (type == SAUL_ACT_SWITCH) {
        return "button";
    }
    else if (type == SAUL_ACT_DIMMER) {
        return "light";
    }

    return NULL;
}

static void _fix_hass_topic(char *topic)
{
    char *pos = strchr(topic, ' ');
    while (pos) {
        *pos = '_';
        pos = strchr(pos, ' ');
    }
}

static int _format_hass_discovery_topic(const saul_reg_t *reg, char *buf, size_t buf_len)
{
    int res = snprintf(buf, buf_len, "%s/%s/%s/%s/config",
            HASS_DISCOVERY_PREFIX,
            _saul_reg_to_hass_type(reg),
            _get_hass_dev_name(),
            reg->name);
    _fix_hass_topic(buf);
    return res;
}

static int _format_hass_topic(const saul_reg_t *reg, char *buf, size_t buf_len)
{
    int res = snprintf(buf, buf_len, "%s/%s", _get_hass_dev_name(), reg->name);
    _fix_hass_topic(buf);
    return res;
}

static int _gen_hass_discovery_payload(const saul_reg_t *reg, char *buf, size_t buf_len)
{
    static char prefix[32];
    _format_hass_topic(reg, prefix, sizeof(prefix));
    int res = mjson_snprintf(buf, buf_len,
            "{\"~\":%Q,\"name\":%Q,\"stat_t\":\"~/state\"", prefix, reg->name);

    if (res < 0) {
        return res;
    }

    if ((reg->driver->type & SAUL_CAT_MASK) == SAUL_CAT_ACT) {
        res += mjson_snprintf(buf + res, buf_len - res, ",\"cmd_t\":\"~/set\"");
    }
    res += mjson_snprintf(buf + res, buf_len - res, "}");
    return res;
}

static int _pub_hass_discovery(const saul_reg_t *reg)
{
    static char topic[64];
    _format_hass_discovery_topic(reg, topic, sizeof(topic));
    static char payload[512];
    _gen_hass_discovery_payload(reg, payload, sizeof(payload));

    MQTTMessage msg;
    msg.qos = 0;
    msg.retained = 0;
    msg.payload = payload;
    msg.payloadlen = strlen(payload);

    int res = MQTTPublish(&client, topic, &msg);
    printf("Result publishing %s: %d\n", topic, res);

    return res;
}

static int _cmd_reg(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("Registering all available SAUL sensors with Home-Assistant");

    const saul_reg_t *reg = saul_reg_find_nth(0);
    while (reg) {
        _pub_hass_discovery(reg);
        reg = reg->next;
    }

    return 0;
}

static int _cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    topic_cnt = 0;
    int res = MQTTDisconnect(&client);
    if (res < 0) {
        printf("mqtt_example: Unable to disconnect\n");
    }
    else {
        printf("mqtt_example: Disconnect successful\n");
    }

    NetworkDisconnect(&network);
    return res;
}

static int _cmd_con(int argc, char **argv)
{
    if (argc < 2) {
        printf(
            "usage: %s <brokerip addr> [port] [clientID] [user] [password] "
            "[keepalivetime]\n",
            argv[0]);
        return 1;
    }

    char *remote_ip = argv[1];

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

    printf("mqtt_example: Connecting to MQTT Broker from %s %d\n",
            remote_ip, port);
    printf("mqtt_example: Trying to connect to %s, port: %d\n",
            remote_ip, port);
    ret = NetworkConnect(&network, remote_ip, port);
    if (ret < 0) {
        printf("mqtt_example: Unable to connect\n");
        return ret;
    }

    printf("user:%s clientId:%s password:%s\n", data.username.cstring,
             data.clientID.cstring, data.password.cstring);
    ret = MQTTConnect(&client, &data);
    if (ret < 0) {
        printf("mqtt_example: Unable to connect client %d\n", ret);
        _cmd_discon(0, NULL);
        return ret;
    }
    else {
        printf("mqtt_example: Connection successfully\n");
    }

    return (ret > 0) ? 0 : 1;
}

static const shell_command_t shell_commands[] =
{
    { "con",    "connect to MQTT broker",             _cmd_con    },
    { "discon", "disconnect from the current broker", _cmd_discon },
    { "register", "Use discovery to register with Home-Assistant", _cmd_reg  },
    { NULL,     NULL,                                 NULL        }
};

static unsigned char buf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];

int main(void)
{
    _init_hass_dev_name();
    if (IS_USED(MODULE_GNRC_ICMPV6_ECHO)) {
        msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    }
#ifdef MODULE_LWIP
    /* let LWIP initialize */
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
#endif

    NetworkInit(&network);

    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE,
                   readbuf,
                   BUF_SIZE);
    puts("Running Home-Assistant integration demo setup");
    printf("MQTT Home-Assistant ID: %s\n", _get_hass_dev_name());

    MQTTStartTask(&client);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
