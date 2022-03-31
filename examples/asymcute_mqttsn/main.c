/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Example application for demonstrating RIOT's MQTT-SN library
 *              Asymcute
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "shell.h"
#include "mutex.h"
#include "thread.h"
#include "net/asymcute.h"
#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"

#ifndef REQ_CTX_NUMOF
#define REQ_CTX_NUMOF       (8U)
#endif

#ifndef SUB_CTX_NUMOF
#define SUB_CTX_NUMOF       (8U)
#endif

#ifndef TOPIC_BUF_NUMOF
#define TOPIC_BUF_NUMOF     (8U + SUB_CTX_NUMOF)
#endif

/* needed for the `ping` shell command */
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static asymcute_con_t _connection;
static asymcute_req_t _reqs[REQ_CTX_NUMOF];
static asymcute_sub_t _subscriptions[SUB_CTX_NUMOF];
static asymcute_topic_t _topics[TOPIC_BUF_NUMOF];

static asymcute_req_t *_get_req_ctx(void)
{
    for (unsigned i = 0; i < REQ_CTX_NUMOF; i++) {
        if (!asymcute_req_in_use(&_reqs[i])) {
            return &_reqs[i];
        }
    }
    puts("error: no request context available\n");
    return NULL;
}

static asymcute_sub_t *_get_sub_ctx(void)
{
    for (unsigned i = 0; i < SUB_CTX_NUMOF; i++) {
        if (!asymcute_sub_active(&_subscriptions[i])) {
            return &_subscriptions[i];
        }
    }
    return NULL;
}

static asymcute_sub_t *_find_sub(const char *name)
{
    for (unsigned i = 0; i < SUB_CTX_NUMOF; i++) {
        if (asymcute_sub_active(&_subscriptions[i]) &&
            strcmp(_subscriptions[i].topic->name, name) == 0) {
            return &_subscriptions[i];
        }
    }
    return NULL;
}

static uint16_t _parse_predef_id(const char *name)
{
    uint16_t id = 0;
    if ((strlen(name) > 4) && (strncmp(name, "pre_", 4) == 0)) {
        id = atoi(&name[4]);
    }
    return id;
}

static asymcute_topic_t *_topic_init(asymcute_topic_t *t, const char *name)
{
    uint16_t id = _parse_predef_id(name);
    if (id != 0) {
        name = NULL;
    }

    if (asymcute_topic_init(t, name, id) != ASYMCUTE_OK) {
        return NULL;
    }
    return t;
}
static asymcute_topic_t *_topic_find(const char *name)
{
    uint16_t id = _parse_predef_id(name);
    for (unsigned i = 0; i < TOPIC_BUF_NUMOF; i++) {
        if ((asymcute_topic_is_predef(&_topics[i]) && (id == _topics[i].id)) ||
            (strncmp(name, _topics[i].name, sizeof(_topics[i].name)) == 0)) {
            return &_topics[i];
        }
    }
    return NULL;
}

static void _topics_clear(void)
{
    memset(_topics, 0, sizeof(_topics));
}

static asymcute_topic_t *_topic_get_free(void)
{
    for (unsigned i = 0; i < TOPIC_BUF_NUMOF; i++) {
        if (!asymcute_topic_is_init(&_topics[i])) {
            return &_topics[i];
        }
    }
    return NULL;
}

static void _topic_print_info(const asymcute_topic_t *topic)
{
    printf(" id: %u, name: %s", (unsigned)topic->id, topic->name);
    if (asymcute_topic_is_short(topic)) {
        printf(" (SHORT)");
    }
    else if (asymcute_topic_is_predef(topic)) {
        printf(" (PREDEF)");
    }
    puts("");
}

static void _topic_print_help(void)
{
    puts("        topic can be\n"
         "        - short topic: 2 byte string\n"
         "        - predefined topic id: pre_XXXXX (e.g. pre_738)\n"
         "        - normal: any string\n");
}

static int _qos_parse(int argc, char **argv, int pos, unsigned *flags)
{
    if (argc <= pos) {
        return 0;
    }
    /* parse QoS level */
    int qos = atoi(argv[pos]);
    switch (qos) {
        case 0: *flags = MQTTSN_QOS_0; break;
        case 1: *flags = MQTTSN_QOS_1; break;
        case 2: *flags = MQTTSN_QOS_2; break;
        default: return -1;
    }
    return qos;
}

static void _on_con_evt(asymcute_req_t *req, unsigned evt_type)
{
    printf("Request %p: ", (void *)req);
    switch (evt_type) {
        case ASYMCUTE_TIMEOUT:
            puts("Timeout");
            break;
        case ASYMCUTE_REJECTED:
            puts("Rejected by gateway");
            break;
        case ASYMCUTE_CONNECTED:
            puts("Connection to gateway established");
            break;
        case ASYMCUTE_DISCONNECTED:
            puts("Connection to gateway closed");
            _topics_clear();
            break;
        case ASYMCUTE_REGISTERED:
            puts("Topic registered");
            break;
        case ASYMCUTE_PUBLISHED:
            puts("Data was published");
            break;
        case ASYMCUTE_SUBSCRIBED:
            puts("Subscribed topic");
            break;
        case ASYMCUTE_UNSUBSCRIBED:
            puts("Unsubscribed topic");
            break;
        case ASYMCUTE_CANCELED:
            puts("Canceled");
            break;
        default:
            puts("unknown event");
            break;
    }
}

static void _on_pub_evt(const asymcute_sub_t *sub, unsigned evt_type,
                        const void *data, size_t len, void *arg)
{
    (void)arg;

    if (evt_type == ASYMCUTE_PUBLISHED) {
        char *in = (char *)data;

        printf("subscription to topic #%i [%s]: NEW DATA\n",
               (int)sub->topic->id, sub->topic->name);
        printf("         data -> ");
        for (size_t i = 0; i < len; i++) {
            printf("%c", in[i]);
        }
        puts("");
        printf("              -> %u bytes\n", (unsigned)len);
    }
    else if (evt_type == ASYMCUTE_CANCELED) {
        printf("subscription -> topic #%i [%s]: CANCELED\n",
               (int)sub->topic->id, sub->topic->name);
    }
}

static int _ok(asymcute_req_t *req)
{
    printf("Request %p: issued\n", (void *)req);
    return 0;
}

static int _cmd_connect(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage %s <cli id> <addr> [<will topic> <will msg>]\n",
               argv[0]);
        return 1;
    }

    /* get sock ep */
    sock_udp_ep_t ep;
    if (sock_udp_name2ep(&ep, argv[2]) != 0) {
        puts("error: unable to parse gateway address");
        return 1;
    }
    if (ep.port == 0) {
        ep.port = CONFIG_ASYMCUTE_DEFAULT_PORT;
    }

    /* get request context */
    asymcute_req_t *req = _get_req_ctx();
    if (req == NULL) {
        return 1;
    }

    int res = asymcute_connect(&_connection, req, &ep, argv[1],
                               true, NULL, _on_con_evt);
    if (res != ASYMCUTE_OK) {
        printf("error: failed to issue CONNECT request (%i)\n", res);
        return 1;
    }
    return _ok(req);
}

static int _cmd_disconnect(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* get request context */
    asymcute_req_t *req = _get_req_ctx();
    if (req == NULL) {
        return 1;
    }

    int res = asymcute_disconnect(&_connection, req);
    if (res != ASYMCUTE_OK) {
        printf("error: failed to issue DISCONNECT request (%i)\n", res);
        return 1;
    }
    return _ok(req);
}

static int _cmd_reg(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <topic name>\n", argv[0]);
        _topic_print_help();
        return 1;
    }

    asymcute_topic_t *t = _topic_find(argv[1]);
    if ((t != NULL) && (asymcute_topic_is_reg(t))) {
        puts("success: topic already registered");
        return 0;
    }

    /* get registration request context */
    asymcute_req_t *req = _get_req_ctx();
    if (req == NULL) {
        puts("error: unable to allocate request context");
        return 1;
    }

    /* find unused slot */
    if (t == NULL) {
        t = _topic_get_free();
        if (t == NULL) {
            puts("error: no empty slot left for storing the topic\n");
            return 1;
        }
        if (_topic_init(t, argv[1]) == NULL) {
            puts("error: unable to initialize topic");
            return 1;
        }
    }

    int res = asymcute_register(&_connection, req, t);
    if (res != ASYMCUTE_OK) {
        printf("error: unable to send REGISTER request (%i)\n", res);
        return 1;
    }
    return _ok(req);
}

static int _cmd_unreg(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <topic name>\n", argv[0]);
        return 1;
    }

    asymcute_topic_t *t = _topic_find(argv[1]);
    if (t) {
        for (unsigned s = 0; s < SUB_CTX_NUMOF; s++) {
            if (_subscriptions[s].topic == t) {
                puts("error: topic used in active subscription");
                return 1;
            }
        }
        asymcute_topic_reset(t);
        puts("success: removed local entry for given topic");
    }
    else {
        puts("error: unable to find topic in list of registered topics");
    }

    return 0;
}

static int _cmd_pub(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <topic> <data> [QoS level]\n", argv[0]);
        _topic_print_help();
        return 1;
    }

    /* parse QoS level */
    unsigned flags = 0;
    int qos = _qos_parse(argc, argv, 3, &flags);
    if (qos < 0) {
        puts("error: unable to parse QoS level");
        return 1;
    }

    /* get request context */
    asymcute_req_t *req = _get_req_ctx();
    if (req == NULL) {
        puts("error: unable to obtain request context");
        return 1;
    }

    /* get topic */
    asymcute_topic_t *t = _topic_find(argv[1]);
    if (t == NULL) {
        puts("error: given topic is not registered");
        return 1;
    }

    /* publish data */
    size_t len = strlen(argv[2]);
    int res = asymcute_publish(&_connection, req, t, argv[2], len, flags);
    if (res != ASYMCUTE_OK) {
        printf("error: unable to send PUBLISH message (%i)\n", res);
        return 1;
    }
    if (qos == 0) {
        printf("Request %p: issued (one way)\n", (void *)req);
        return 0;
    }
    return _ok(req);
}

static int _cmd_sub(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <topic> [QoS level]\n", argv[0]);
        _topic_print_help();
        return 1;
    }

    /* parse QoS level */
    unsigned flags = 0;
    int qos = _qos_parse(argc, argv, 2, &flags);
    if (qos < 0) {
        puts("error: unable to parse QoS level");
        return 1;
    }

    /* get request context */
    asymcute_req_t *req = _get_req_ctx();
    if (req == NULL) {
        puts("error: unable to obtain request context");
        return 1;
    }

    /* get subscription context */
    asymcute_sub_t *sub = _get_sub_ctx();
    if (sub == NULL) {
        puts("error: unable to allocate subscription context");
        return 1;
    }

    /* parse topic, see if it exists, otherwise take empty topic */
    asymcute_topic_t *t = _topic_find(argv[1]);
    if (t == NULL) {
        puts("info: given topic does not exist, creating it now");
        t = _topic_get_free();
        if (t == NULL) {
            puts("error: no free topic memory");
            return 1;
        }
        if (_topic_init(t, argv[1]) == NULL) {
            puts("error: unable to initialize topic");
            return 1;
        }
    }
    else {
        if (!asymcute_topic_is_reg(t)) {
            puts("error: given topic is not registered\n");
            return 1;
        }
    }

    int res = asymcute_subscribe(&_connection, req, sub, t, _on_pub_evt,
                                 NULL, flags);
    if (res != ASYMCUTE_OK) {
        if (!asymcute_topic_is_reg(t)) {
            asymcute_topic_reset(t);
        }
        if (res == ASYMCUTE_SUBERR) {
            puts("error: already subscribed to given topic");
        }
        else {
            printf("error: unable to send SUBSCRIBE request (%i)\n", res);
        }

        return 1;
    }

    return _ok(req);
}

static int _cmd_unsub(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <topic>\n", argv[0]);
        return 1;
    }

    /* find active subscription for given topic name */
    asymcute_sub_t *sub = _find_sub(argv[1]);
    if (sub == NULL) {
        puts("error: no subscription for given topic found");
        return 1;
    }

    /* get request context */
    asymcute_req_t *req = _get_req_ctx();
    if (req == NULL) {
        return 1;
    }

    /* issue unsubscribe request */
    int res = asymcute_unsubscribe(&_connection, req, sub);
    if (res != ASYMCUTE_OK) {
        printf("error: unable to send UNSUBSCRIBE request (%i)\n", res);
        return 1;
    }

    return _ok(req);
}

static int _cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("--- Asymcute MQTT-SN client state ---");
    puts("Topics:");
    for (unsigned i = 0; i < TOPIC_BUF_NUMOF; i++) {
        printf("topic #%2u - ", i);
        if (asymcute_topic_is_reg(&_topics[i])) {
            printf("[registered] ");
            _topic_print_info(&_topics[i]);
        }
        else if (asymcute_topic_is_init(&_topics[i])) {
            printf("[initialized] ");
            _topic_print_info(&_topics[i]);
        }
        else {
            puts("[unused]");
        }
    }

    puts("Subscriptions:");
    for (unsigned i = 0; i < SUB_CTX_NUMOF; i++) {
        printf("sub   #%2u - ", i);
        if (asymcute_sub_active(&_subscriptions[i])) {
            printf("[subscribed]");
            _topic_print_info(_subscriptions[i].topic);
        }
        else {
            puts("[unused]");
        }
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "connect", "connect to MQTT-SN gateway", _cmd_connect },
    { "disconnect", "disconnect from MQTT-SN gateway", _cmd_disconnect },
    { "reg", "register a given topic", _cmd_reg },
    { "unreg", "remove a topic registration [locally]", _cmd_unreg },
    { "pub", "publish data", _cmd_pub },
    { "sub", "subscribe to topic", _cmd_sub },
    { "unsub", "unsubscribe from topic", _cmd_unsub },
    { "info", "print state information", _cmd_info },
    { NULL, NULL, NULL },
};

int main(void)
{
    puts("Asymcute MQTT-SN example application\n");
    puts("Type 'help' to get started and have a look at the README.md for more"
         "information.");

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
