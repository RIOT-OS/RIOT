/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_asymcute
 * @{
 *
 * @file
 * @brief       Asynchronous MQTT-SN implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <limits.h>

#include "random.h"
#include "byteorder.h"
#include "timex.h"

#include "net/sock/async/event.h"
#include "net/asymcute.h"

#define ENABLE_DEBUG            0
#include "debug.h"

#define PROTOCOL_VERSION        (0x01)

#define RETRY_TO                (CONFIG_ASYMCUTE_T_RETRY * US_PER_SEC)
#define KEEPALIVE_TO            (CONFIG_ASYMCUTE_KEEPALIVE_PING * US_PER_SEC)

#define VALID_PUBLISH_FLAGS     (MQTTSN_QOS_1 | MQTTSN_DUP | MQTTSN_RETAIN)
#define VALID_SUBSCRIBE_FLAGS   (MQTTSN_QOS_1 | MQTTSN_DUP)

#define MINLEN_CONNACK          (3U)
#define MINLEN_DISCONNECT       (2U)
#define MINLEN_REGACK           (7U)
#define MINLEN_PUBACK           (7U)
#define MINLEN_SUBACK           (8U)
#define MINLEN_UNSUBACK         (4U)

#define IDPOS_REGACK            (4U)
#define IDPOS_PUBACK            (4U)
#define IDPOS_SUBACK            (5U)
#define IDPOS_UNSUBACK          (2U)

#define LEN_PINGRESP            (2U)

#define MIN_PKT_LEN             (2)

/* Internally used connection states */
enum {
    NOTCON = 0,             /**< not connected to any gateway */
    CONNECTING,             /**< connection is being setup */
    CONNECTED,              /**< connection is established */
    TEARDOWN,               /**< connection is being torn down */
};

/* the main handler thread needs a stack and a message queue */
static event_queue_t _queue;
static char _stack[ASYMCUTE_HANDLER_STACKSIZE];

/* necessary forward function declarations */
static void _on_req_timeout(void *arg);

static size_t _len_set(uint8_t *buf, size_t len)
{
    if (len < (0xff - 7)) {
        buf[0] = len + 1;
        return 1;
    }
    else {
        buf[0] = 0x01;
        byteorder_htobebufs(&buf[1], (uint16_t)(len + 3));
        return 3;
    }
}

static ssize_t _len_get(uint8_t *buf, size_t pkt_len, size_t *len)
{
    if (buf[0] != 0x01) {
        *len = (uint16_t)buf[0];
        return 1;
    }
    else {
        if (pkt_len < 3) {
            return -1;
        }
        *len = byteorder_bebuftohs(&buf[1]);
        return 3;
    }
}

/* @pre con is locked */
static uint16_t _msg_id_next(asymcute_con_t *con)
{
    if (++con->last_id == 0) {
        return ++con->last_id;
    }
    return con->last_id;
}

static uint8_t _req_type(asymcute_req_t *req)
{
    size_t len;
    ssize_t pos = _len_get(req->data, req->data_len, &len);
    /* requests are created by us and should thus always be valid */
    assert(pos != -1 && (size_t)pos < req->data_len);
    return req->data[(size_t)pos];
}

/* @pre con is locked */
static asymcute_req_t *_req_preprocess(asymcute_con_t *con,
                                       size_t msg_len, size_t min_len,
                                       const uint8_t *buf, unsigned id_pos,
                                       uint8_t rtype)
{
    /* verify message length */
    if (msg_len < min_len) {
        return NULL;
    }

    uint16_t msg_id = (buf == NULL) ? 0 : byteorder_bebuftohs(&buf[id_pos]);

    asymcute_req_t *res = NULL;
    asymcute_req_t *iter = con->pending;
    if (iter == NULL) {
        return NULL;
    }
    if (iter->msg_id == msg_id) {
        res = iter;
        con->pending = iter->next;
    }
    while (iter && !res) {
        asymcute_req_t *r = iter->next;
        if (r && (r->msg_id == msg_id) && (_req_type(r) == rtype)) {
            res = r;
            iter->next = iter->next->next;
        }
        iter = iter->next;
    }

    if (res) {
        res->con = NULL;
        event_timeout_clear(&res->to_timer);
    }
    return res;
}

/* @pre con is locked */
static void _req_remove(asymcute_con_t *con, asymcute_req_t *req)
{
    if (con->pending == req) {
        con->pending = con->pending->next;
    }
    for (asymcute_req_t *cur = con->pending; cur; cur = cur->next) {
        if (cur->next == req) {
            cur->next = cur->next->next;
        }
    }
    req->con = NULL;
}

/* @pre con is locked */
static void _compile_sub_unsub(asymcute_req_t *req, asymcute_con_t *con,
                               asymcute_sub_t *sub, uint8_t type)
{
    size_t topic_len = strlen(sub->topic->name);
    size_t pos = _len_set(req->data, (topic_len + 4));

    req->msg_id = _msg_id_next(con);
    req->data[pos] = type;
    req->data[pos + 1] = sub->topic->flags;
    byteorder_htobebufs(&req->data[pos + 2], req->msg_id);
    if (sub->topic->flags & MQTTSN_TIT_MASK) {
        memcpy(&req->data[pos + 4], &sub->topic->id, 2);
    }
    else {
        memcpy(&req->data[pos + 4], sub->topic->name, topic_len);
    }
    req->data_len = (pos + 4 + topic_len);
    req->arg = sub;
}

static ssize_t _req_resend(asymcute_req_t *req, asymcute_con_t *con, int initial)
{
    ssize_t n = sock_udp_send(&con->sock, req->data, req->data_len, NULL);
    /* if sending the initial packet fails we do not set the retry timer, as we
     * handle the return value directly */
    if (!((initial == 1) && (n < MIN_PKT_LEN))) {
        event_timeout_set(&req->to_timer, RETRY_TO);
    }
    return n;
}

/* @pre con is locked */
static int _req_send(asymcute_req_t *req, asymcute_con_t *con,
                      asymcute_to_cb_t cb)
{
    /* initialize request */
    req->con = con;
    req->cb = cb;
    req->retry_cnt = CONFIG_ASYMCUTE_N_RETRY;
    event_callback_init(&req->to_evt, _on_req_timeout, req);
    event_timeout_init(&req->to_timer, &_queue, &req->to_evt.super);
    /* add request to the pending queue (if non-con request) */
    req->next = con->pending;
    con->pending = req;
    /* send request */
    ssize_t n = _req_resend(req, con, 1);
    if (n < MIN_PKT_LEN) {
        req->con = NULL;
        mutex_unlock(&req->lock);
        return ASYMCUTE_SENDERR;
    }
    return ASYMCUTE_OK;
}

static int _req_send_once(asymcute_req_t *req, asymcute_con_t *con)
{
    ssize_t n = sock_udp_send(&con->sock, req->data, req->data_len, NULL);
    mutex_unlock(&req->lock);
    return (n >= MIN_PKT_LEN) ? ASYMCUTE_OK : ASYMCUTE_SENDERR;
}

static void _req_cancel(asymcute_req_t *req)
{
    asymcute_con_t *con = req->con;
    event_timeout_clear(&req->to_timer);
    req->con = NULL;
    mutex_unlock(&req->lock);
    con->user_cb(req, ASYMCUTE_CANCELED);
}

static void _sub_cancel(asymcute_sub_t *sub)
{
    sub->cb(sub, ASYMCUTE_CANCELED, NULL, 0, sub->arg);
    sub->topic = NULL;
}

/* @pre con is locked */
static void _disconnect(asymcute_con_t *con, uint8_t state)
{
    if (con->state == CONNECTED) {
        /* cancel all pending requests */
        event_timeout_clear(&con->keepalive_timer);
        for (asymcute_req_t *req = con->pending; req; req = req->next) {
            _req_cancel(req);
        }
        con->pending = NULL;
        for (asymcute_sub_t *sub = con->subscriptions; sub; sub = sub->next) {
            _sub_cancel(sub);
        }
        con->subscriptions = NULL;
    }
    if (state == NOTCON) {
        sock_udp_close(&con->sock);
    }
    con->state = state;
}

static void _on_req_timeout(void *arg)
{
    asymcute_req_t *req = arg;

    /* only process the timeout, if the request is still active */
    if (req->con == NULL) {
        return;
    }

    if (req->retry_cnt--) {
        /* resend the packet */
        _req_resend(req, req->con, 0);
        return;
    }
    else {
        asymcute_con_t *con = req->con;
        mutex_lock(&con->lock);
        _req_remove(con, req);
        /* communicate timeout to outer world */
        unsigned ret = ASYMCUTE_TIMEOUT;
        if (req->cb) {
            ret = req->cb(con, req);
        }
        mutex_unlock(&req->lock);
        mutex_unlock(&con->lock);
        con->user_cb(req, ret);
    }
}

static unsigned _on_con_timeout(asymcute_con_t *con, asymcute_req_t *req)
{
    (void)req;

    con->state = NOTCON;
    sock_udp_close(&con->sock);
    return ASYMCUTE_TIMEOUT;
}

static unsigned _on_discon_timeout(asymcute_con_t *con, asymcute_req_t *req)
{
    (void)req;

    _disconnect(con, NOTCON);
    return ASYMCUTE_DISCONNECTED;
}

static unsigned _on_suback_timeout(asymcute_con_t *con, asymcute_req_t *req)
{
    (void)con;

    /* reset the subscription context */
    asymcute_sub_t *sub = req->arg;
    if (sub == NULL) {
        return ASYMCUTE_REJECTED;
    }
    sub->topic = NULL;
    return ASYMCUTE_TIMEOUT;
}

static void _on_keepalive_evt(void *arg)
{
    asymcute_con_t *con = arg;

    mutex_lock(&con->lock);

    if (con->state != CONNECTED) {
        mutex_unlock(&con->lock);
        return;
    }

    if (con->keepalive_retry_cnt) {
        /* (re)send keep alive ping and set dedicated retransmit timer */
        uint8_t ping[2] = { 2, MQTTSN_PINGREQ };
        sock_udp_send(&con->sock, ping, sizeof(ping), NULL);
        con->keepalive_retry_cnt--;
        event_timeout_set(&con->keepalive_timer, RETRY_TO);
        mutex_unlock(&con->lock);
    }
    else {
        _disconnect(con, NOTCON);
        mutex_unlock(&con->lock);
        con->user_cb(NULL, ASYMCUTE_DISCONNECTED);
    }
}

static void _on_connack(asymcute_con_t *con, const uint8_t *data, size_t len)
{
    mutex_lock(&con->lock);
    asymcute_req_t *req = _req_preprocess(con, len, MINLEN_CONNACK, NULL, 0, MQTTSN_CONNECT);
    if (req == NULL) {
        mutex_unlock(&con->lock);
        return;
    }

    /* check return code and mark connection as established */
    unsigned ret = ASYMCUTE_REJECTED;
    if (data[2] == MQTTSN_ACCEPTED) {
        con->state = CONNECTED;
        /* start keep alive timer */
        con->keepalive_retry_cnt = CONFIG_ASYMCUTE_N_RETRY;
        event_timeout_set(&con->keepalive_timer, KEEPALIVE_TO);
        ret = ASYMCUTE_CONNECTED;
    }

    mutex_unlock(&req->lock);
    mutex_unlock(&con->lock);
    con->user_cb(req, ret);
}

static void _on_disconnect(asymcute_con_t *con, size_t len)
{
    mutex_lock(&con->lock);

    /* we might have triggered the DISCONNECT process ourselves, so make sure
     * the pending request is being handled */
    asymcute_req_t *req = _req_preprocess(con, len, MINLEN_DISCONNECT, NULL, 0, MQTTSN_DISCONNECT);

    /* put the connection back to NOTCON in any case and let the user know */
    _disconnect(con, NOTCON);
    if (req) {
        mutex_unlock(&req->lock);
    }
    mutex_unlock(&con->lock);
    con->user_cb(req, ASYMCUTE_DISCONNECTED);
}

static void _on_pingreq(asymcute_con_t *con)
{
    /* simply reply with a PINGRESP message */
    mutex_lock(&con->lock);
    uint8_t resp[2] = { LEN_PINGRESP, MQTTSN_PINGRESP };
    sock_udp_send(&con->sock, resp, sizeof(resp), NULL);
    mutex_unlock(&con->lock);
}

static void _on_pingresp(asymcute_con_t *con)
{
    mutex_lock(&con->lock);
    /* only handle ping resp message if we are actually waiting for a reply */
    if (con->keepalive_retry_cnt < CONFIG_ASYMCUTE_N_RETRY) {
        event_timeout_clear(&con->keepalive_timer);
        con->keepalive_retry_cnt = CONFIG_ASYMCUTE_N_RETRY;
        event_timeout_set(&con->keepalive_timer, KEEPALIVE_TO);
    }
    mutex_unlock(&con->lock);
}

static void _on_regack(asymcute_con_t *con, const uint8_t *data, size_t len)
{
    mutex_lock(&con->lock);
    asymcute_req_t *req = _req_preprocess(con, len, MINLEN_REGACK,
                                          data, IDPOS_REGACK,
                                          MQTTSN_REGISTER);
    if (req == NULL) {
        mutex_unlock(&con->lock);
        return;
    }

    /* check return code */
    unsigned ret = ASYMCUTE_REJECTED;
    if (data[6] == MQTTSN_ACCEPTED) {
        /* finish the registration by applying the topic id */
        asymcute_topic_t *topic = req->arg;
        if (topic == NULL) {
            mutex_unlock(&con->lock);
            return;
        }

        topic->id = byteorder_bebuftohs(&data[2]);
        topic->con = con;
        ret = ASYMCUTE_REGISTERED;
    }

    /* finally notify the user and free the request */
    mutex_unlock(&req->lock);
    mutex_unlock(&con->lock);
    con->user_cb(req, ret);
}

static void _on_publish(asymcute_con_t *con, uint8_t *data,
                        size_t pos, size_t len)
{
    /* verify message length */
    if (len < (pos + 6)) {
        return;
    }

    uint16_t topic_id = byteorder_bebuftohs(&data[pos + 2]);

    /* find any subscription for that topic */
    mutex_lock(&con->lock);
    asymcute_sub_t *sub = NULL;
    for (asymcute_sub_t *cur = con->subscriptions; cur; cur = cur->next) {
        if (cur->topic->id == topic_id) {
            sub = cur;
            break;
        }
    }

    /* send PUBACK if needed (QoS > 0 or on invalid topic ID) */
    if ((sub == NULL) || (data[pos + 1] & MQTTSN_QOS_1)) {
        uint8_t ret = (sub) ? MQTTSN_ACCEPTED : MQTTSN_REJ_INV_TOPIC_ID;
        uint8_t pkt[7] = { 7, MQTTSN_PUBACK, 0, 0, 0, 0, ret };
        /* copy topic and message id */
        memcpy(&pkt[2], &data[pos + 2], 4);
        sock_udp_send(&con->sock, pkt, 7, NULL);
    }

    /* release the context and notify the user (on success) */
    mutex_unlock(&con->lock);
    if (sub) {
        sub->cb(sub, ASYMCUTE_PUBLISHED,
                &data[pos + 6], (len - (pos + 6)), sub->arg);
    }
}

static void _on_puback(asymcute_con_t *con, const uint8_t *data, size_t len)
{
    mutex_lock(&con->lock);
    asymcute_req_t *req = _req_preprocess(con, len, MINLEN_PUBACK,
                                          data, IDPOS_PUBACK,
                                          MQTTSN_PUBLISH);
    if (req == NULL) {
        mutex_unlock(&con->lock);
        return;
    }

    unsigned ret = (data[6] == MQTTSN_ACCEPTED) ?
                    ASYMCUTE_PUBLISHED : ASYMCUTE_REJECTED;
    mutex_unlock(&req->lock);
    mutex_unlock(&con->lock);
    con->user_cb(req, ret);
}

static void _on_suback(asymcute_con_t *con, const uint8_t *data, size_t len)
{
    mutex_lock(&con->lock);
    asymcute_req_t *req = _req_preprocess(con, len, MINLEN_SUBACK,
                                          data, IDPOS_SUBACK,
                                          MQTTSN_SUBSCRIBE);
    if (req == NULL) {
        mutex_unlock(&con->lock);
        return;
    }

    unsigned ret = ASYMCUTE_REJECTED;
    /* parse and apply assigned topic id */
    asymcute_sub_t *sub = req->arg;
    if (sub == NULL) {
        mutex_unlock(&con->lock);
        return;
    }

    if (data[7] == MQTTSN_ACCEPTED) {
        /* do not assign a topic ID for short and predefined topics */
        if (!(sub->topic->flags & MQTTSN_TIT_MASK)) {
            sub->topic->id = byteorder_bebuftohs(&data[3]);
        }
        sub->topic->con = con;
        /* insert subscription to connection context */
        sub->next = con->subscriptions;
        con->subscriptions = sub;
        ret = ASYMCUTE_SUBSCRIBED;
    }
    else {
        sub->topic = NULL;
    }

    /* notify the user */
    mutex_unlock(&req->lock);
    mutex_unlock(&con->lock);
    con->user_cb(req, ret);
}

static void _on_unsuback(asymcute_con_t *con, const uint8_t *data, size_t len)
{
    mutex_lock(&con->lock);
    asymcute_req_t *req = _req_preprocess(con, len, MINLEN_UNSUBACK,
                                          data, IDPOS_UNSUBACK,
                                          MQTTSN_UNSUBSCRIBE);
    if (req == NULL) {
        mutex_unlock(&con->lock);
        return;
    }

    /* remove subscription from list */
    asymcute_sub_t *sub = req->arg;
    if (sub == NULL) {
        mutex_unlock(&con->lock);
        return;
    } else if (con->subscriptions == sub) {
        con->subscriptions = sub->next;
    }
    else {
        for (asymcute_sub_t *e = con->subscriptions; e && e->next; e = e->next) {
            if (e->next == sub) {
                e->next = e->next->next;
                break;
            }
        }
    }

    /* reset subscription context */
    sub->topic = NULL;

    /* notify user */
    mutex_unlock(&req->lock);
    mutex_unlock(&con->lock);
    con->user_cb(req, ASYMCUTE_UNSUBSCRIBED);
}

void *_eventloop(void *arg)
{
    (void)arg;
    event_queue_init(&_queue);
    event_loop(&_queue);
    /* should never be reached */
    return NULL;
}

void _on_pkt(sock_udp_t *sock, sock_async_flags_t type, void *arg)
{
    asymcute_con_t *con = (asymcute_con_t *)arg;

    if (type & SOCK_ASYNC_MSG_RECV) {
        ssize_t pkt_len = sock_udp_recv(sock, con->rxbuf,
                                        CONFIG_ASYMCUTE_BUFSIZE, 0, NULL);
        if (pkt_len >= MIN_PKT_LEN) {
            size_t len;
            ssize_t lret = _len_get(con->rxbuf, pkt_len, &len);
            if (lret == -1) {
                /* first octet was 0x01 but pkt does not have more than 3 octets */
                return;
            }
            size_t pos = (size_t)lret;

            /* validate incoming data: verify message length */
            if (((size_t)pkt_len <= pos) || ((size_t)pkt_len < len)) {
                /* length field of MQTT-SN packet seems to be invalid -> drop the pkt */
                return;
            }

            /* figure out required action based on message type */
            uint8_t type = con->rxbuf[pos];
            switch (type) {
                case MQTTSN_CONNACK:
                    _on_connack(con, con->rxbuf, len);
                    break;
                case MQTTSN_DISCONNECT:
                    _on_disconnect(con, len);
                    break;
                case MQTTSN_PINGREQ:
                    _on_pingreq(con);
                    break;
                case MQTTSN_PINGRESP:
                    _on_pingresp(con);
                    break;
                case MQTTSN_REGACK:
                    _on_regack(con, con->rxbuf, len);
                    break;
                case MQTTSN_PUBLISH:
                    _on_publish(con, con->rxbuf, pos, len);
                    break;
                case MQTTSN_PUBACK:
                    _on_puback(con, con->rxbuf, len);
                    break;
                case MQTTSN_SUBACK:
                    _on_suback(con, con->rxbuf, len);
                    break;
                case MQTTSN_UNSUBACK:
                    _on_unsuback(con, con->rxbuf, len);
                    break;
                default:
                    break;
            }
        }
    }
}

void asymcute_handler_run(void)
{
    thread_create(_stack, sizeof(_stack), ASYMCUTE_HANDLER_PRIO,
                  0, _eventloop, NULL, "asymcute_main");
}

int asymcute_topic_init(asymcute_topic_t *topic, const char *topic_name,
                        uint16_t topic_id)
{
    assert(topic);

    size_t len = 0;

    if (asymcute_topic_is_reg(topic)) {
        return ASYMCUTE_REGERR;
    }
    if (topic_name == NULL) {
        if ((topic_id == 0) || (topic_id == UINT16_MAX)) {
            return ASYMCUTE_OVERFLOW;
        }
    }
    else {
        len = strlen(topic_name);
        if ((len == 0) || (len > CONFIG_ASYMCUTE_TOPIC_MAXLEN)) {
            return ASYMCUTE_OVERFLOW;
        }
    }

    /* reset given topic */
    asymcute_topic_reset(topic);
    /* pre-defined topic ID? */
    if (topic_name == NULL) {
        topic->id = topic_id;
        topic->flags = MQTTSN_TIT_PREDEF;
        memcpy(topic->name, "..\0", 3);
    }
    else {
        strncpy(topic->name, topic_name, sizeof(topic->name));
        if (len == 2) {
            memcpy(&topic->id, topic_name, 2);
            topic->flags = MQTTSN_TIT_SHORT;
        }
    }

    return ASYMCUTE_OK;
}

bool asymcute_is_connected(const asymcute_con_t *con)
{
    return (con->state == CONNECTED);
}

int asymcute_connect(asymcute_con_t *con, asymcute_req_t *req,
                     sock_udp_ep_t *server, const char *cli_id, bool clean,
                     asymcute_will_t *will, asymcute_evt_cb_t callback)
{
    assert(con);
    assert(req);
    assert(server);
    assert(cli_id);

    int ret = ASYMCUTE_OK;
    size_t id_len = strlen(cli_id);

    /* the will feature is not yet supported */
    if (will) {
        return ASYMCUTE_NOTSUP;
    }
    /* make sure the client ID will fit into the dedicated buffer */
    if ((id_len < MQTTSN_CLI_ID_MINLEN) || (id_len > MQTTSN_CLI_ID_MAXLEN)) {
        return ASYMCUTE_OVERFLOW;
    }
    /* check if the context is not already connected to any gateway */
    mutex_lock(&con->lock);
    if (con->state != NOTCON) {
        ret = ASYMCUTE_BUSY;
        goto end;
    }
    /* get mutual access to the request context */
    if (mutex_trylock(&req->lock) != 1) {
        ret = ASYMCUTE_BUSY;
        goto end;
    }

    /* initialize the connection context */
    memset(con, 0, sizeof(asymcute_con_t));
    random_bytes((uint8_t *)&con->last_id, 2);
    con->keepalive_retry_cnt = CONFIG_ASYMCUTE_N_RETRY;
    event_callback_init(&con->keepalive_evt, _on_keepalive_evt, con);
    event_timeout_init(&con->keepalive_timer, &_queue, &con->keepalive_evt.super);
    con->user_cb = callback;
    con->state = CONNECTING;
    strncpy(con->cli_id, cli_id, sizeof(con->cli_id));

    /* create a socket for this listener, using an ephemeral port */
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = 0;
    local.netif = server->netif;
    if (sock_udp_create(&con->sock, &local, server, 0) != 0) {
        con->state = NOTCON;
        ret = ASYMCUTE_GWERR;
        goto end;
    }
    sock_udp_event_init(&con->sock, &_queue, _on_pkt, con);

    /* compile and send connect message */
    req->msg_id = 0;
    req->data[0] = (uint8_t)(id_len + 6);
    req->data[1] = MQTTSN_CONNECT;
    req->data[2] = ((clean) ? MQTTSN_CS : 0);
    req->data[3] = PROTOCOL_VERSION;
    byteorder_htobebufs(&req->data[4], CONFIG_ASYMCUTE_KEEPALIVE);
    memcpy(&req->data[6], cli_id, id_len);
    req->data_len = (size_t)req->data[0];
    ret =  _req_send(req, con, _on_con_timeout);
    if (ret != ASYMCUTE_OK) {
        _disconnect(con, NOTCON);
    }

end:
    mutex_unlock(&con->lock);
    return ret;
}

int asymcute_disconnect(asymcute_con_t *con, asymcute_req_t *req)
{
    assert(con);
    assert(req);

    int ret = ASYMCUTE_OK;

    /* check if we are actually connected */
    mutex_lock(&con->lock);
    if (!asymcute_is_connected(con)) {
        ret = ASYMCUTE_GWERR;
        goto end;
    }
    /* get mutual access to the request context */
    if (mutex_trylock(&req->lock) != 1) {
        ret = ASYMCUTE_BUSY;
        goto end;
    }

    /* put connection into TEARDOWN state */
    _disconnect(con, TEARDOWN);

    /* prepare and send disconnect message */
    req->msg_id = 0;
    req->data[0] = 2;
    req->data[1] = MQTTSN_DISCONNECT;
    req->data_len = 2;
    ret = _req_send(req, con, _on_discon_timeout);

end:
    mutex_unlock(&con->lock);
    return ret;
}

int asymcute_register(asymcute_con_t *con, asymcute_req_t *req,
                      asymcute_topic_t *topic)
{
    assert(con);
    assert(req);
    assert(topic);

    int ret = ASYMCUTE_OK;

    /* test if topic is already registered */
    if (asymcute_topic_is_reg(topic)) {
        return ASYMCUTE_REGERR;
    }
    /* make sure we are connected */
    mutex_lock(&con->lock);
    if (!asymcute_is_connected(con)) {
        ret = ASYMCUTE_GWERR;
        goto end;
    }
    /* get mutual access to the request context */
    if (mutex_trylock(&req->lock) != 1) {
        ret = ASYMCUTE_BUSY;
        goto end;
    }

    /* if we have a short or predefined topic, there is no need to send a
     * registration message. We assign the connection right away */
    if (topic->flags & MQTTSN_TIT_MASK) {
        topic->con = con;
        mutex_unlock(&req->lock);
        goto end;
    }

    /* prepare topic */
    req->arg = topic;
    size_t topic_len = strlen(topic->name);

    /* prepare registration request */
    req->msg_id = _msg_id_next(con);
    size_t pos = _len_set(req->data, (topic_len + 5));
    req->data[pos] = MQTTSN_REGISTER;
    byteorder_htobebufs(&req->data[pos + 1], 0);
    byteorder_htobebufs(&req->data[pos + 3], req->msg_id);
    memcpy(&req->data[pos + 5], topic->name, topic_len);
    req->data_len = (pos + 5 + topic_len);

    /* send the request */
    ret = _req_send(req, con, NULL);

end:
    mutex_unlock(&con->lock);
    return ret;
}

int asymcute_publish(asymcute_con_t *con, asymcute_req_t *req,
                     const asymcute_topic_t *topic,
                     const void *data, size_t data_len, uint8_t flags)
{
    assert(con);
    assert(req);
    assert(topic);
    assert((data_len == 0) || data);

    int ret = ASYMCUTE_OK;

    /* check for valid flags */
    if ((flags & VALID_PUBLISH_FLAGS) != flags) {
        return ASYMCUTE_NOTSUP;
    }
    /* check for message size */
    if ((data_len + 9) > CONFIG_ASYMCUTE_BUFSIZE) {
        return ASYMCUTE_OVERFLOW;
    }
    /* make sure topic is registered */
    if (!asymcute_topic_is_reg(topic) || (topic->con != con)) {
        return ASYMCUTE_REGERR;
    }
    /* check if we are connected to a gateway */
    mutex_lock(&con->lock);
    if (!asymcute_is_connected(con)) {
        ret = ASYMCUTE_GWERR;
        goto end;
    }
    /* make sure request context is clear to be used */
    if (mutex_trylock(&req->lock) != 1) {
        ret = ASYMCUTE_BUSY;
        goto end;
    }

    /* set MsgId only for QoS 1 and 2, else it must be set to 0 */
    if (((flags & MQTTSN_QOS_MASK) == MQTTSN_QOS_1) ||
        ((flags & MQTTSN_QOS_MASK) == MQTTSN_QOS_2)) {
        req->msg_id = _msg_id_next(con);
    }
    else {
        req->msg_id = 0;
    }

    /* assemble message */
    size_t pos = _len_set(req->data, data_len + 6);
    req->data[pos] = MQTTSN_PUBLISH;
    req->data[pos + 1] = (flags | topic->flags);
    byteorder_htobebufs(&req->data[pos + 2], topic->id);
    byteorder_htobebufs(&req->data[pos + 4], req->msg_id);
    memcpy(&req->data[pos + 6], data, data_len);
    req->data_len = (pos + 6 + data_len);

    /* publish selected data */
    if (flags & MQTTSN_QOS_1) {
        ret = _req_send(req, con, NULL);
    }
    else {
        ret = _req_send_once(req, con);
    }

end:
    mutex_unlock(&con->lock);
    return ret;
}

int asymcute_subscribe(asymcute_con_t *con, asymcute_req_t *req,
                       asymcute_sub_t *sub, asymcute_topic_t *topic,
                       asymcute_sub_cb_t callback, void *arg, uint8_t flags)
{
    assert(con);
    assert(req);
    assert(sub);
    assert(topic);
    assert(callback);

    int ret = ASYMCUTE_OK;

    /* check flags for validity */
    if ((flags & VALID_SUBSCRIBE_FLAGS) != flags) {
        return ASYMCUTE_NOTSUP;
    }
    /* is topic initialized? (though it does not need to be registered) */
    if (!asymcute_topic_is_init(topic)) {
        return ASYMCUTE_REGERR;
    }
    /* check if we are connected to a gateway */
    mutex_lock(&con->lock);
    if (!asymcute_is_connected(con)) {
        ret = ASYMCUTE_GWERR;
        goto end;
    }
    /* check if we are already subscribed to the given topic, but only if the
     * topic was already registered */
    if (asymcute_topic_is_reg(topic)) {
        for (asymcute_sub_t *sub = con->subscriptions; sub; sub = sub->next) {
            if (asymcute_topic_equal(topic, sub->topic)) {
                ret = ASYMCUTE_SUBERR;
                goto end;
            }
        }
    }
    /* make sure request context is clear to be used */
    if (mutex_trylock(&req->lock) != 1) {
        ret = ASYMCUTE_BUSY;
        goto end;
    }

    /* prepare subscription context */
    sub->cb = callback;
    sub->arg = arg;
    sub->topic = topic;
    topic->flags |= flags;

    /* send SUBSCRIBE message */
    _compile_sub_unsub(req, con, sub, MQTTSN_SUBSCRIBE);
    ret = _req_send(req, con, _on_suback_timeout);

end:
    mutex_unlock(&con->lock);
    return ret;
}

int asymcute_unsubscribe(asymcute_con_t *con, asymcute_req_t *req,
                         asymcute_sub_t *sub)
{
    assert(con);
    assert(req);
    assert(sub);

    int ret = ASYMCUTE_OK;

    /* make sure the subscription is actually active */
    if (!asymcute_sub_active(sub)) {
        return ASYMCUTE_SUBERR;
    }
    /* check if we are connected to a gateway */
    mutex_lock(&con->lock);
    if (!asymcute_is_connected(con)) {
        ret = ASYMCUTE_GWERR;
        goto end;
    }
    /* make sure request context is clear to be used */
    if (mutex_trylock(&req->lock) != 1) {
        ret = ASYMCUTE_BUSY;
        goto end;
    }

    /* prepare and send UNSUBSCRIBE message */
    _compile_sub_unsub(req, con, sub, MQTTSN_UNSUBSCRIBE);
    ret = _req_send(req, con, NULL);

end:
    mutex_unlock(&con->lock);
    return ret;
}
