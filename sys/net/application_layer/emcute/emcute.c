/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_emcute
 * @{
 *
 * @file
 * @brief       MQTT-SN implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "log.h"
#include "mutex.h"
#include "sched.h"
#include "xtimer.h"
#include "byteorder.h"
#include "thread_flags.h"

#include "net/emcute.h"
#include "net/mqttsn.h"
#include "emcute_internal.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define PROTOCOL_VERSION    (0x01)

#define PUB_FLAGS           (EMCUTE_QOS_MASK | EMCUTE_RETAIN)
#define SUB_FLAGS           (EMCUTE_DUP | EMCUTE_QOS_MASK | EMCUTE_TIT_MASK)

#define TFLAGS_RESP         (0x0001)
#define TFLAGS_TIMEOUT      (0x0002)
#define TFLAGS_ANY          (TFLAGS_RESP | TFLAGS_TIMEOUT)

static const char *cli_id;
static sock_udp_t sock;
static sock_udp_ep_t gateway;

static uint8_t rbuf[CONFIG_EMCUTE_BUFSIZE];
static uint8_t tbuf[CONFIG_EMCUTE_BUFSIZE];

static emcute_sub_t *subs = NULL;

static mutex_t txlock;

static xtimer_t timer;
static uint16_t id_next = 0x1234;
static volatile uint8_t waiton = 0xff;
static volatile uint16_t waitonid = 0;
static volatile int result;

static size_t set_len(uint8_t *buf, size_t len)
{
    /* - `len` field minimum length == 1
     * - `((len + 1) <= 0xff) == len < 0xff` */
    if (len < 0xff) {
        buf[0] = len + 1;
        return 1;
    }
    else {
        buf[0] = 0x01;
        byteorder_htobebufs(&tbuf[1], (uint16_t)(len + 3));
        return 3;
    }
}

static size_t get_len(uint8_t *buf, uint16_t *len)
{
    if (buf[0] != 0x01) {
        *len = (uint16_t)buf[0];
        return 1;
    }
    else {
        *len = byteorder_bebuftohs(&buf[1]);
        return 3;
    }
}

static void time_evt(void *arg)
{
    thread_flags_set(arg, TFLAGS_TIMEOUT);
}

static int syncsend(uint8_t resp, size_t len, bool unlock)
{
    int res = EMCUTE_TIMEOUT;
    waiton = resp;
    timer.arg = thread_get_active();
    /* clear flags, in case the timer was triggered last time right before the
     * remove was called */
    thread_flags_clear(TFLAGS_ANY);

    for (unsigned retries = 0; retries <= CONFIG_EMCUTE_N_RETRY; retries++) {
        DEBUG("[emcute] syncsend: sending round %i\n", retries);
        sock_udp_send(&sock, tbuf, len, &gateway);

        xtimer_set(&timer, (CONFIG_EMCUTE_T_RETRY * US_PER_SEC));
        thread_flags_t flags = thread_flags_wait_any(TFLAGS_ANY);
        if (flags & TFLAGS_RESP) {
            DEBUG("[emcute] syncsend: got response [%i]\n", result);
            xtimer_remove(&timer);
            res = result;
            break;
        }
    }

    /* cleanup sync state */
    waiton = 0xff;
    if (unlock) {
        mutex_unlock(&txlock);
    }
    return res;
}

static void on_disconnect(void)
{
    if (waiton == DISCONNECT) {
        gateway.port = 0;
        result = EMCUTE_OK;
        thread_flags_set(timer.arg, TFLAGS_RESP);
    }
}

static void on_ack(uint8_t type, int id_pos, int ret_pos, int res_pos)
{
    if ((waiton == type) &&
        (!id_pos || (waitonid == byteorder_bebuftohs(&rbuf[id_pos])))) {
        if (!ret_pos || (rbuf[ret_pos] == ACCEPT)) {
            if (res_pos == 0) {
                result = EMCUTE_OK;
            } else {
                result = (int)byteorder_bebuftohs(&rbuf[res_pos]);
            }
        } else {
            result = EMCUTE_REJECT;
        }
        thread_flags_set(timer.arg, TFLAGS_RESP);
    }
}

static void on_publish(size_t len, size_t pos)
{
    /* make sure packet length is valid - if not, drop packet silently */
    if (len < (pos + 6)) {
        return;
    }

    emcute_sub_t *sub;
    uint16_t tid = byteorder_bebuftohs(&rbuf[pos + 2]);

    /* allocate a response packet */
    uint8_t buf[7] = { 7, PUBACK, 0, 0, 0, 0, ACCEPT };
    /* and populate message ID and topic ID fields */
    memcpy(&buf[2], &rbuf[pos + 2], 4);

    /* return error code in case we don't support/understand active flags. So
     * far we only understand QoS 1... */
    if (rbuf[pos + 1] & ~(EMCUTE_QOS_1 | EMCUTE_TIT_SHORT | EMCUTE_RETAIN)) {
        buf[6] = REJ_NOTSUP;
        sock_udp_send(&sock, &buf, 7, &gateway);
        return;
    }

    /* find the registered topic */
    for (sub = subs; sub && (sub->topic.id != tid); sub = sub->next) {}
    if (sub == NULL) {
        buf[6] = REJ_INVTID;
        sock_udp_send(&sock, &buf, 7, &gateway);
        DEBUG("[emcute] on pub: no subscription found\n");
    }
    else {
        if (rbuf[pos + 1] & EMCUTE_QOS_1) {
            sock_udp_send(&sock, &buf, 7, &gateway);
        }
        DEBUG("[emcute] on pub: got %" PRIiSIZE " bytes of data\n", len - pos - 6);
        size_t dat_len = (len - pos - 6);
        void *dat = (dat_len > 0) ? &rbuf[pos + 6] : NULL;
        sub->cb(&sub->topic, dat, dat_len);
    }
}

static void on_pingreq(sock_udp_ep_t *remote)
{
    /* @todo    respond with a PINGRESP only if the PINGREQ came from the
     *          connected gateway -> see spec v1.2, section 6.11 */
    uint8_t buf[2] = { 2, PINGRESP };
    sock_udp_send(&sock, &buf, 2, remote);
}

static void on_pingresp(void)
{
    /** @todo: trigger update something like a 'last seen' value */
}

static void send_ping(void)
{
    if (gateway.port != 0) {
        uint8_t buf[2] = { 2, PINGREQ };
        sock_udp_send(&sock, &buf, 2, &gateway);
    }
}

int emcute_con(sock_udp_ep_t *remote, bool clean, const char *will_topic,
               const void *will_msg, size_t will_msg_len, unsigned will_flags)
{
    int res;
    size_t len;

    assert(!will_topic || (will_topic && will_msg && !(will_flags & ~PUB_FLAGS)));

    mutex_lock(&txlock);

    /* check for existing connections and copy given UDP endpoint */
    if (gateway.port != 0) {
        return EMCUTE_NOGW;
    }
    memcpy(&gateway, remote, sizeof(sock_udp_ep_t));

    /* figure out which flags to set */
    uint8_t flags = (clean) ? EMCUTE_CS : 0;
    if (will_topic) {
        flags |= EMCUTE_WILL;
    }

    /* compute packet size */
    len = (strlen(cli_id) + 6);
    tbuf[0] = (uint8_t)len;
    tbuf[1] = CONNECT;
    tbuf[2] = flags;
    tbuf[3] = PROTOCOL_VERSION;
    byteorder_htobebufs(&tbuf[4], CONFIG_EMCUTE_KEEPALIVE);
    memcpy(&tbuf[6], cli_id, strlen(cli_id));

    /* configure 'state machine' and send the connection request */
    if (will_topic) {
        size_t topic_len = strlen(will_topic);
        if ((topic_len > CONFIG_EMCUTE_TOPIC_MAXLEN) ||
            ((will_msg_len + 4) > CONFIG_EMCUTE_BUFSIZE)) {
            gateway.port = 0;
            return EMCUTE_OVERFLOW;
        }

        res = syncsend(WILLTOPICREQ, len, false);
        if (res != EMCUTE_OK) {
            gateway.port = 0;
            return res;
        }

        /* now send WILLTOPIC */
        size_t pos = set_len(tbuf, (topic_len + 2));
        len = (pos + topic_len + 2);
        tbuf[pos++] = WILLTOPIC;
        tbuf[pos++] = will_flags;
        memcpy(&tbuf[pos], will_topic, topic_len);

        res = syncsend(WILLMSGREQ, len, false);
        if (res != EMCUTE_OK) {
            gateway.port = 0;
            return res;
        }

        /* and WILLMSG afterwards */
        pos = set_len(tbuf, (will_msg_len + 1));
        len = (pos + will_msg_len + 1);
        tbuf[pos++] = WILLMSG;
        memcpy(&tbuf[pos], will_msg, will_msg_len);
    }

    res = syncsend(CONNACK, len, true);
    if (res != EMCUTE_OK) {
        gateway.port = 0;
    }
    return res;
}

int emcute_discon(void)
{
    if (gateway.port == 0) {
        return EMCUTE_NOGW;
    }

    mutex_lock(&txlock);

    tbuf[0] = 2;
    tbuf[1] = DISCONNECT;

    return syncsend(DISCONNECT, 2, true);
}

int emcute_reg(emcute_topic_t *topic)
{
    assert(topic && topic->name);

    if (gateway.port == 0) {
        return EMCUTE_NOGW;
    }
    if (strlen(topic->name) > CONFIG_EMCUTE_TOPIC_MAXLEN) {
        return EMCUTE_OVERFLOW;
    }

    mutex_lock(&txlock);

    tbuf[0] = (strlen(topic->name) + 6);
    tbuf[1] = REGISTER;
    byteorder_htobebufs(&tbuf[2], 0);
    byteorder_htobebufs(&tbuf[4], id_next);
    waitonid = id_next++;
    memcpy(&tbuf[6], topic->name, strlen(topic->name));

    int res = syncsend(REGACK, (size_t)tbuf[0], true);
    if (res > 0) {
        topic->id = (uint16_t)res;
        res = EMCUTE_OK;
    }
    return res;
}

int emcute_pub(emcute_topic_t *topic, const void *data, size_t len,
               unsigned flags)
{
    int res = EMCUTE_OK;

    assert((topic->id != 0) && data && (len > 0) && !(flags & ~PUB_FLAGS));

    if (gateway.port == 0) {
        return EMCUTE_NOGW;
    }
    if (len >= (CONFIG_EMCUTE_BUFSIZE - 9)) {
        return EMCUTE_OVERFLOW;
    }
    if (flags & EMCUTE_QOS_2) {
        return EMCUTE_NOTSUP;
    }

    mutex_lock(&txlock);

    size_t pos = set_len(tbuf, (len + 6));
    tbuf[pos++] = PUBLISH;
    tbuf[pos++] = flags;
    byteorder_htobebufs(&tbuf[pos], topic->id);
    pos += 2;
    /* set generated MessageId for QOS 1 and 2, else set it to 0 */
    if (((flags & MQTTSN_QOS_MASK) == MQTTSN_QOS_1) ||
        ((flags & MQTTSN_QOS_MASK) == MQTTSN_QOS_2)) {
        byteorder_htobebufs(&tbuf[pos], id_next);
        waitonid = id_next++;
    }
    else {
        memset(&tbuf[pos], 0, 2);
    }
    pos += 2;
    memcpy(&tbuf[pos], data, len);

    if (flags & EMCUTE_QOS_1) {
        res = syncsend(PUBACK, len + pos, true);
    }
    else {
        sock_udp_send(&sock, tbuf, len + pos, &gateway);
        mutex_unlock(&txlock);
    }

    return res;
}

int emcute_sub(emcute_sub_t *sub, unsigned flags)
{
    assert(sub && (sub->cb) && (sub->topic.name) && !(flags & ~SUB_FLAGS));
    size_t topic_len;

    if (gateway.port == 0) {
        return EMCUTE_NOGW;
    }
    if ((flags & EMCUTE_TIT_MASK) == EMCUTE_TIT_NORMAL) {
        topic_len=strlen(sub->topic.name);
    }
    else if ((flags & EMCUTE_TIT_MASK) == EMCUTE_TIT_PREDEF) {
        topic_len=2;
    }
    else {
        return EMCUTE_NOTSUP;
    }
    if (topic_len > CONFIG_EMCUTE_TOPIC_MAXLEN) {
        return EMCUTE_OVERFLOW;
    }

    mutex_lock(&txlock);

    tbuf[0] = (uint8_t)(5 + topic_len);
    tbuf[1] = SUBSCRIBE;
    tbuf[2] = flags;
    byteorder_htobebufs(&tbuf[3], id_next);
    waitonid = id_next++;
    if ((flags & EMCUTE_TIT_MASK) == EMCUTE_TIT_NORMAL) {
        memcpy(&tbuf[5], sub->topic.name, topic_len);
    }
    if ((flags & EMCUTE_TIT_MASK) == EMCUTE_TIT_PREDEF) {
        byteorder_htobebufs(&tbuf[5], sub->topic.id);
    }

    int res = syncsend(SUBACK, (size_t)tbuf[0], false);
    if (res > 0) {
        DEBUG("[emcute] sub: success, topic id is %i\n", res);
        sub->topic.id = res;

        /* check if subscription is already in the list, only insert if not*/
        emcute_sub_t *s;
        for (s = subs; s && (s != sub); s = s->next) {}
        if (!s) {
            sub->next = subs;
            subs = sub;
            res = EMCUTE_OK;
        }
    }

    mutex_unlock(&txlock);
    return res;
}

int emcute_unsub(emcute_sub_t *sub)
{
    assert(sub && sub->topic.name);

    if (gateway.port == 0) {
        return EMCUTE_NOGW;
    }

    mutex_lock(&txlock);

    tbuf[0] = (strlen(sub->topic.name) + 5);
    tbuf[1] = UNSUBSCRIBE;
    tbuf[2] = 0;
    byteorder_htobebufs(&tbuf[3], id_next);
    waitonid = id_next++;
    memcpy(&tbuf[5], sub->topic.name, strlen(sub->topic.name));

    int res = syncsend(UNSUBACK, (size_t)tbuf[0], false);
    if (res == EMCUTE_OK) {
        if (subs == sub) {
            subs = sub->next;
        }
        else {
            emcute_sub_t *s;
            for (s = subs; s; s = s->next) {
                if (s->next == sub) {
                    s->next = sub->next;
                    break;
                }
            }
        }
    }

    mutex_unlock(&txlock);
    return res;
}

int emcute_willupd_topic(const char *topic, unsigned flags)
{
    assert(!(flags & ~PUB_FLAGS));

    if (gateway.port == 0) {
        return EMCUTE_NOGW;
    }
    if (topic && (strlen(topic) > CONFIG_EMCUTE_TOPIC_MAXLEN)) {
        return EMCUTE_OVERFLOW;
    }

    mutex_lock(&txlock);

    tbuf[1] = WILLTOPICUPD;
    if (!topic) {
        tbuf[0] = 2;
    }
    else {
        tbuf[0] = (strlen(topic) + 3);
        tbuf[2] = flags;
        memcpy(&tbuf[3], topic, strlen(topic));
    }

    return syncsend(WILLTOPICRESP, (size_t)tbuf[0], true);
}

int emcute_willupd_msg(const void *data, size_t len)
{
    assert(data && (len > 0));

    if (gateway.port == 0) {
        return EMCUTE_NOGW;
    }
    if (len > (CONFIG_EMCUTE_BUFSIZE - 4)) {
        return EMCUTE_OVERFLOW;
    }

    mutex_lock(&txlock);

    size_t pos = set_len(tbuf, (len + 1));
    tbuf[pos++] = WILLMSGUPD;
    memcpy(&tbuf[pos], data, len);

    return syncsend(WILLMSGRESP, (pos + len), true);
}

void emcute_run(uint16_t port, const char *id)
{
    assert(strlen(id) >= MQTTSN_CLI_ID_MINLEN &&
           strlen(id) <= MQTTSN_CLI_ID_MAXLEN);

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote;
    local.port = port;
    cli_id = id;
    timer.callback = time_evt;
    timer.arg = NULL;
    mutex_init(&txlock);

    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        LOG_ERROR("[emcute] unable to open UDP socket on port %i\n", (int)port);
        return;
    }

    uint32_t start = xtimer_now_usec();
    uint32_t t_out = (CONFIG_EMCUTE_KEEPALIVE * US_PER_SEC);

    while (1) {
        ssize_t len = sock_udp_recv(&sock, rbuf, sizeof(rbuf), t_out, &remote);

        if ((len < 0) && (len != -ETIMEDOUT)) {
            LOG_ERROR("[emcute] error while receiving UDP packet\n");
            continue;
        }

        if (len >= 2) {
            /* handle the packet */
            uint16_t pkt_len;
            /* catch invalid length field */
            if ((len == 2) && (rbuf[0] == 0x01)) {
                continue;
            }
            /* parse length field */
            size_t pos = get_len(rbuf, &pkt_len);
            /* verify length to prevent overflows */
            if (((size_t)pkt_len > (size_t)len) || (pos >= (size_t)len)) {
                continue;
            }
            /* get packet type */
            uint8_t type = rbuf[pos];

            switch (type) {
                case CONNACK:       on_ack(type, 0, 2, 0);              break;
                case WILLTOPICREQ:  on_ack(type, 0, 0, 0);              break;
                case WILLMSGREQ:    on_ack(type, 0, 0, 0);              break;
                case REGACK:        on_ack(type, 4, 6, 2);              break;
                case PUBLISH:       on_publish((size_t)pkt_len, pos);   break;
                case PUBACK:        on_ack(type, 4, 6, 0);              break;
                case SUBACK:        on_ack(type, 5, 7, 3);              break;
                case UNSUBACK:      on_ack(type, 2, 0, 0);              break;
                case PINGREQ:       on_pingreq(&remote);                break;
                case PINGRESP:      on_pingresp();                      break;
                case DISCONNECT:    on_disconnect();                    break;
                case WILLTOPICRESP: on_ack(type, 0, 0, 0);              break;
                case WILLMSGRESP:   on_ack(type, 0, 0, 0);              break;
                default:
                    LOG_DEBUG("[emcute] received unexpected type [%s]\n",
                              emcute_type_str(type));
            }
        }

        uint32_t now = xtimer_now_usec();
        if ((now - start) >= (CONFIG_EMCUTE_KEEPALIVE * US_PER_SEC)) {
            send_ping();
            start = now;
            t_out = (CONFIG_EMCUTE_KEEPALIVE * US_PER_SEC);
        }
        else {
            t_out = (CONFIG_EMCUTE_KEEPALIVE * US_PER_SEC) - (now - start);
        }
    }
}
