/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       emcute MQTT-SN test application
 *
 * @author      Martine Sophie Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "net/emcute.h"
#include "net/mqttsn.h"
#include "net/ipv6/addr.h"
#include "od.h"
#include "shell.h"
#include "thread.h"
#include "net/sock/util.h"

/* get to maximum length for client ID ;-)*/
#define EMCUTE_ID           "emcute test app ......."
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFTOPS           (4U)
#define SHELL_BUFSIZE       (512U)  /* for sub with long topic */

static char _emcute_stack[THREAD_STACKSIZE_DEFAULT];
static char _shell_buffer[SHELL_BUFSIZE];
static uint8_t _pub_buf[CONFIG_EMCUTE_BUFSIZE];

static emcute_topic_t _topics[NUMOFTOPS];
static emcute_sub_t _subscriptions[NUMOFTOPS];
static char _topic_names[NUMOFTOPS][CONFIG_EMCUTE_TOPIC_MAXLEN + 1];
static char _addr_str[IPV6_ADDR_MAX_STR_LEN];

static sock_udp_ep_t _gw = { .family = AF_INET6 };

static int _con(int argc, char **argv);
static int _discon(int argc, char **argv);
static int _reg(int argc, char **argv);
static int _pub(int argc, char **argv);
static int _sub(int argc, char **argv);
static int _unsub(int argc, char **argv);
static int _will(int argc, char **argv);
static int _info(int argc, char **argv);

static const shell_command_t _shell_commands[] = {
    { "con", "connect to a MQTT-SN broker", _con },
    { "discon", "disconnect from current broker", _discon },
    { "reg", "register to a topic", _reg },
    { "pub", "publish a number of bytes under a topic", _pub },
    { "sub", "subscribe to a topic", _sub },
    { "unsub", "unsubscribe from a topic", _unsub },
    { "will", "register a last will", _will },
    { "info", "print client state", _info },
    { NULL, NULL, NULL },
};

static void *_emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

static unsigned _get_qos(const char *str)
{
    int qos = atoi(str);
    switch (qos) {
        case 1:     return EMCUTE_QOS_1;
        case 2:     return EMCUTE_QOS_2;
        default:    return EMCUTE_QOS_0;
    }
}

static void _on_pub(const emcute_topic_t *topic, void *data, size_t len)
{
    (void)data;
    printf("### got publication of %" PRIuSIZE " bytes for topic '%s' [%u] ###\n",
           len, topic->name, topic->id);
}

static int _con(int argc, char **argv)
{
    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;

    if (argc < 2) {
        printf("usage %s <addr> [<will topic> <will msg>]\n",
               argv[0]);
        return 1;
    }

    if (sock_udp_name2ep(&_gw, argv[1]) != 0) {
        puts("error: unable to parse gateway address");
        _gw.port = 0;
        return 1;
    }
    if (_gw.port == 0) {
        _gw.port = CONFIG_EMCUTE_DEFAULT_PORT;
    }
    if (argc >= 4) {
        topic = argv[2];
        message = argv[3];
        len = strlen(message);
    }

    if (emcute_con(&_gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to %s\n", argv[1]);
        _gw.port = 0;
        return 1;
    }
    printf("success: connected to gateway at %s\n", argv[1]);

    return 0;
}

static int _discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res = emcute_discon();
    if (res == EMCUTE_NOGW) {
        puts("error: not connected to any broker");
        return 1;
    }
    else if (res != EMCUTE_OK) {
        puts("error: unable to disconnect");
        return 1;
    }
    puts("success: disconnect successful");
    return 0;
}

static int _topic_name_find(const char *name)
{
    int res = -1;

    for (unsigned i = 0; i < NUMOFTOPS; i++) {
        if ((_topic_names[i][0] == '\0') && (res < 0)) {
            res = i;
        }
        else if (strncmp(name, _topic_names[i], CONFIG_EMCUTE_TOPIC_MAXLEN) == 0) {
            return i;
        }
    }

    return res;
}

static int _reg(int argc, char **argv)
{
    emcute_topic_t *t;
    int idx;
    bool was_set = false;

    if (argc < 2) {
        printf("usage: %s <topic name>\n", argv[0]);
        return 1;
    }

    idx = _topic_name_find(argv[1]);
    if (idx < 0) {
        puts("error: no space left to register");
        return 1;
    }
    if (_topic_names[idx][0] != '\0') {
        was_set = true;
    }
    else {
        strncpy(_topic_names[idx], argv[1], CONFIG_EMCUTE_TOPIC_MAXLEN);
    }
    t = &_topics[idx];
    t->name = _topic_names[idx];
    if (emcute_reg(t) != EMCUTE_OK) {
        puts("error: unable to obtain topic ID");
        if (was_set) {
            _topic_names[idx][0] = '\0';
        }
        return 1;
    }

    printf("success: registered to topic '%s [%d]'\n", t->name, t->id);

    return 0;
}

static int _pub(int argc, char **argv)
{
    unsigned flags = EMCUTE_QOS_0;
    int len;
    emcute_topic_t *t;
    int idx;

    if (argc < 3) {
        printf("usage: %s <topic name> <data_len> [QoS level]\n", argv[0]);
        return 1;
    }

    if (argc >= 4) {
        flags |= _get_qos(argv[3]);
    }

    idx = _topic_name_find(argv[1]);
    if ((idx < 0) || !(_topics[idx].name)) {
        puts("error: topic not registered");
        return 1;
    }
    t = &_topics[idx];
    len = atoi(argv[2]);
    if ((unsigned)len > sizeof(_pub_buf)) {
        printf("error: len %d > %" PRIuSIZE "\n", len, sizeof(_pub_buf));
        return 1;
    }
    memset(_pub_buf, 92, len);
    if (emcute_pub(t, _pub_buf, len, flags) != EMCUTE_OK) {
        printf("error: unable to publish data to topic '%s [%u]'\n",
                t->name, t->id);
        return 1;
    }

    printf("success: published %d bytes to topic '%s [%u]'\n",
           len, t->name, t->id);

    return 0;
}

static int _sub(int argc, char **argv)
{
    unsigned flags = EMCUTE_QOS_0;
    int idx;
    bool was_set = false;

    if (argc < 2) {
        printf("usage: %s <topic name> [QoS level]\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) > CONFIG_EMCUTE_TOPIC_MAXLEN) {
        puts("error: topic name exceeds maximum possible size");
        return 1;
    }
    if (argc >= 3) {
        flags |= _get_qos(argv[2]);
    }

    idx = _topic_name_find(argv[1]);
    if (idx < 0) {
        puts("error: no space to subscribe");
    }

    _subscriptions[idx].cb = _on_pub;
    if (_topic_names[idx][0] != '\0') {
        was_set = true;
    }
    else {
        strncpy(_topic_names[idx], argv[1], CONFIG_EMCUTE_TOPIC_MAXLEN);
    }
    _subscriptions[idx].topic.name = _topic_names[idx];
    if (emcute_sub(&_subscriptions[idx], flags) != EMCUTE_OK) {
        printf("error: unable to subscribe to %s\n", argv[1]);
        if (was_set) {
            _topic_names[idx][0] = '\0';
        }
        memset(&_subscriptions[idx], 0, sizeof(emcute_sub_t));
        return 1;
    }

    printf("success: now subscribed to %s\n", argv[1]);
    return 0;
}

static int _unsub(int argc, char **argv)
{
    int idx;

    if (argc < 2) {
        printf("usage %s <topic name>\n", argv[0]);
        return 1;
    }

    idx = _topic_name_find(argv[1]);
    if ((idx < 0) || !_subscriptions[idx].topic.name) {
        printf("error: no subscription for topic '%s' found\n", argv[1]);
    }
    else if (emcute_unsub(&_subscriptions[idx]) != EMCUTE_OK) {
        printf("error: Unsubscription form '%s' failed\n", argv[1]);
    }
    else {
        memset(&_subscriptions[idx], 0, sizeof(emcute_sub_t));
        printf("success: unsubscribed from '%s'\n", argv[1]);
        return 0;
    }
    return 1;
}

static int _will(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage %s <will topic name> <will message content>\n", argv[0]);
        return 1;
    }

    if (emcute_willupd_topic(argv[1], 0) != EMCUTE_OK) {
        puts("error: unable to update the last will topic");
        return 1;
    }
    if (emcute_willupd_msg(argv[2], strlen(argv[2])) != EMCUTE_OK) {
        puts("error: unable to update the last will message");
        return 1;
    }

    puts("success: updated last will topic and message");
    return 0;
}

static int _info(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (_gw.port > 0) {
        printf("Broker: '[%s]:%u'\n",
               ipv6_addr_to_str(_addr_str, (ipv6_addr_t *)_gw.addr.ipv6,
                                sizeof(_addr_str)), _gw.port);
        puts("- Topics:");
        for (unsigned i = 0; i < NUMOFTOPS; i++) {
            if (_topics[i].name) {
                printf("  %2u: %s\n", _topics[i].id,
                       _topics[i].name);
            }
        }
        puts("- Subscriptions:");
        for (unsigned i = 0; i < NUMOFTOPS; i++) {
            if (_subscriptions[i].topic.name) {
                printf("  %2u: %s\n", _subscriptions[i].topic.id,
                       _subscriptions[i].topic.name);
            }
        }
    }
    return 0;
}

int main(void)
{
    puts("success: starting test application");
    /* start the emcute thread */
    thread_create(_emcute_stack, sizeof(_emcute_stack), EMCUTE_PRIO, 0,
                  _emcute_thread, NULL, "emcute");
    /* start shell */
    shell_run(_shell_commands, _shell_buffer, sizeof(_shell_buffer));
    return 0;
}
