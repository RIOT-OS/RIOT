/*
 * Copyright (C) 2016-18 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       nanoCoAP sock helpers
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "container.h"
#include "event/thread.h"
#include "net/credman.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"
#include "net/sock/util.h"
#include "random.h"
#include "sys/uio.h" /* IWYU pragma: keep (exports struct iovec) */
#include "time_units.h"
#include "ztimer.h"

#ifdef MODULE_SOCK_ASYNC_EVENT
#  include "net/sock/async/event.h"
#endif

#if MODULE_NANOCOAP_UDP || MODULE_NANOCOAP_TCP
#  include "net/sock/util.h"
#endif

#if MODULE_NANOCOAP_UDP
#  include "net/sock/udp.h"
#endif

#if MODULE_NANOCOAP_TCP
#  include "net/sock/tcp.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Size of the buffer used for the DTLS handshake
 *
 * This size was found suitable for DTLS using a simple PSK in mode AES_128_CCM_8.
 * DTLS places no restriction on its handshake package size therefore this might need change,
 * if mode or key-size change especially if certificates instead of PSK are used.
 */
#ifndef CONFIG_NANOCOAP_DTLS_HANDSHAKE_BUF_SIZE
#  define CONFIG_NANOCOAP_DTLS_HANDSHAKE_BUF_SIZE (160)
#endif

#ifndef CONFIG_NANOCOAP_MAX_OBSERVERS
#  define CONFIG_NANOCOAP_MAX_OBSERVERS 4
#endif

#ifndef CONFIG_NANOCOAP_SERVER_TCP_TIMEOUT_US
#  define CONFIG_NANOCOAP_SERVER_TCP_TIMEOUT_US 100000
#endif

#ifndef CONFIG_NANOCOAP_CLIENT_TCP_TIMEOUT_US
#  define CONFIG_NANOCOAP_CLIENT_TCP_TIMEOUT_US CONFIG_NANOCOAP_SERVER_TCP_TIMEOUT_US
#endif

#ifndef CONFIG_NANOCOAP_TCP_SOCKET_BUF_SIZE
#  define CONFIG_NANOCOAP_TCP_SOCKET_BUF_SIZE 128
#endif

enum {
    STATE_REQUEST_SEND,     /**< request was just sent or will be sent again */
    STATE_STOP_RETRANSMIT,  /**< stop retransmissions due to a matching empty ACK */
    STATE_WAIT_RESPONSE,    /**< waiting for a response */
};

typedef struct {
    coap_blockwise_cb_t callback;
    void *arg;
    uint32_t blknum;
    bool more;
#if CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
    uint8_t token[4];
#endif
} _block_ctx_t;

/**
 * @brief   Structure to track the state of an observation
 */
typedef struct {
    /**
     * @brief   Context needed to build notifications (e.g. Token, endpoint
     *          to send to)
     *
     * @details To safe ROM, we reuse the separate response code to also
     *          send notifications, as the functionality is almost identical.
     */
    nanocoap_server_response_ctx_t response;
    /**
     * @brief   The resource the client has subscribed to
     *
     * @details This is `NULL` when the slot is free
     */
    const coap_resource_t *resource;
    /**
     * @brief   Message ID used in the last notification
     */
    uint16_t msg_id;
} _observer_t;

#if MODULE_NANOCOAP_SERVER_OBSERVE
static _observer_t _observer_pool[CONFIG_NANOCOAP_MAX_OBSERVERS];
static mutex_t _observer_pool_lock;
#endif

ssize_t nanocoap_sock_build_pkt(nanocoap_sock_t *sock, coap_pkt_t *pkt,
                                void *buf, size_t buf_len,
                                uint8_t type,
                                const void *token, size_t token_len,
                                uint8_t code)
{
    assert(sock && buf && (token || (token_len == 0)));

    switch (nanocoap_sock_get_type(sock)) {
    default:
    case COAP_SOCKET_TYPE_UDP:
    case COAP_SOCKET_TYPE_DTLS:
        return coap_build_udp(pkt, buf, buf_len, type, token, token_len, code,
                              nanocoap_sock_next_msg_id(sock));
    case COAP_SOCKET_TYPE_TCP:
        return coap_build_tcp(pkt, buf, buf_len, token, token_len, code);
    }
}

#if MODULE_NANOCOAP_DTLS
int nanocoap_sock_dtls_connect(nanocoap_sock_t *sock, sock_udp_ep_t *local,
                               const sock_udp_ep_t *remote, credman_tag_t tag)
{
    uint8_t buf[CONFIG_NANOCOAP_DTLS_HANDSHAKE_BUF_SIZE];

    nanocoap_sock_set_type(sock, COAP_SOCKET_TYPE_DTLS);
    return sock_dtls_establish_session(&sock->udp, &sock->dtls, &sock->dtls_session,
                                       tag, local, remote, buf, sizeof(buf));
}
#endif

static int _get_error(const coap_pkt_t *pkt)
{
    switch (coap_get_code_class(pkt)) {
    case COAP_CLASS_CLIENT_FAILURE:
        return -ENXIO;
    case COAP_CLASS_SERVER_FAILURE:
        return -ENETRESET;
    default:
        return 0;
    }
}

static int _sock_sendv(nanocoap_sock_t *sock, const iolist_t *snips)
{
    switch (nanocoap_sock_get_type(sock)) {
#if MODULE_NANOCOAP_UDP
    case COAP_SOCKET_TYPE_UDP:
        return sock_udp_sendv(&sock->udp, snips, NULL);
#endif
#if MODULE_NANOCOAP_DTLS
    case COAP_SOCKET_TYPE_DTLS:
        return sock_dtls_sendv(&sock->dtls, &sock->dtls_session, snips,
                               CONFIG_SOCK_DTLS_TIMEOUT_MS);
#endif
#if MODULE_NANOCOAP_TCP
    case COAP_SOCKET_TYPE_TCP:
        {
            int nbytes = 0;
            while (snips) {
                int retval = sock_tcp_write(&sock->tcp, snips->iol_base, snips->iol_len);
                if (retval < 0) {
                    return retval;
                }
                nbytes += retval;
                snips = snips->iol_next;
            }
            return nbytes;
        }
#endif
    default:
        assert(0);
        return -EINVAL;
    }
}

static int _sock_recv_buf(nanocoap_sock_t *sock, void **data, void **ctx, uint32_t timeout)
{
    (void)sock;
    (void)data;
    (void)ctx;
    (void)timeout;
    switch (nanocoap_sock_get_type(sock)) {
#if MODULE_NANOCOAP_UDP
    case COAP_SOCKET_TYPE_UDP:
        return sock_udp_recv_buf(&sock->udp, data, ctx, timeout, NULL);
#endif
#if MODULE_NANOCOAP_DTLS
    case COAP_SOCKET_TYPE_DTLS:
        return sock_dtls_recv_buf(&sock->dtls, &sock->dtls_session, data, ctx, timeout);
#endif
    case COAP_SOCKET_TYPE_TCP:
        /* this API does not work with TCP: There is no sock_tcp_recv_buf()
         * and even if there were: We have no guarantee that CoAP messages
         * align with TCP segments: A single segment could contains multiple
         * CoAP messages or a partial CoAP message. */
    default:
        assert(0);
        return -EINVAL;
   }
}

static int _send_ack(nanocoap_sock_t *sock, coap_pkt_t *pkt)
{
    switch (nanocoap_sock_get_type(sock)) {
    default:
        break;
    case COAP_SOCKET_TYPE_TCP:
        return -ENOTSUP;
    }

    coap_udp_hdr_t ack;

    const iolist_t snip = {
        .iol_base = &ack,
        .iol_len  = sizeof(ack),
    };

    coap_build_empty_ack(pkt, &ack);

    return _sock_sendv(sock, &snip);
}

static bool _id_or_token_missmatch(const coap_pkt_t *pkt, unsigned id,
                                   const void *token, size_t token_len)
{
    switch (coap_get_type(pkt)) {
    case COAP_TYPE_RST:
    case COAP_TYPE_ACK:
        /* message ID only has to match for RST and ACK */
        if (coap_get_id(pkt) != id) {
            return true;
        }
        /* falls through */
    default:
        /* token has to match if message is not empty */
        if (coap_get_code_raw(pkt) != 0) {
            if (coap_get_token_len(pkt) != token_len) {
                return true;
            }
            return memcmp(coap_get_token(pkt), token, token_len);
        }
        else {
            /* but only RST and ACK may be empty */
            return coap_get_type(pkt) != COAP_TYPE_RST &&
                   coap_get_type(pkt) != COAP_TYPE_ACK;
        }
    }
}

static uint32_t _deadline_from_interval(uint32_t interval)
{
    return US_PER_MS * ztimer_now(ZTIMER_MSEC) + interval;
}

static uint32_t _deadline_left_us(uint32_t deadline)
{
    uint32_t now = ztimer_now(ZTIMER_MSEC) * US_PER_MS;
    if (now > deadline) {
        return 0;
    }

    return deadline - now;
}

static void _sock_flush(nanocoap_sock_t *sock)
{
    void *payload, *ctx = NULL;
    while (_sock_recv_buf(sock, &payload, &ctx, 0) > 0 || ctx) {}
}

static ssize_t nanocoap_sock_udp_request_cb(nanocoap_sock_t *sock, coap_pkt_t *pkt,
                                            coap_request_cb_t cb, void *arg)
{
    ssize_t tmp, res = 0;
    const unsigned id = coap_get_id(pkt);
    void *payload, *ctx = NULL;
    const uint8_t *token = coap_get_token(pkt);
    uint8_t token_len = coap_get_token_len(pkt);
    uint8_t state = STATE_REQUEST_SEND;

    /* random timeout, deadline for receive retries */
    uint32_t timeout = random_uint32_range((uint32_t)CONFIG_COAP_ACK_TIMEOUT_MS * US_PER_MS,
                                           (uint32_t)CONFIG_COAP_ACK_TIMEOUT_MS * CONFIG_COAP_RANDOM_FACTOR_1000);
    uint32_t deadline = _deadline_from_interval(timeout);

    /* check if we expect a reply */
    const bool confirmable = coap_get_type(pkt) == COAP_TYPE_CON;

    /* add 1 for initial transmit, retry only when CONfirmable */
    unsigned tries_left = confirmable * CONFIG_COAP_MAX_RETRANSMIT + 1;

    /* Create the first payload snip from the request buffer */
    iolist_t head = {
        .iol_next = pkt->snips,
        .iol_base = pkt->buf,
        .iol_len  = coap_get_total_len(pkt),
    };

    /* clear out stale responses from previous requests */
    _sock_flush(sock);

    while (1) {
        switch (state) {
        case STATE_REQUEST_SEND:
            assert(tries_left > 0);
            --tries_left;

            DEBUG("nanocoap: send %u bytes (%u tries left)\n",
                  (unsigned)iolist_size(&head), tries_left);

            res = _sock_sendv(sock, &head);
            if (res <= 0) {
                DEBUG("nanocoap: error sending coap request, %" PRIdSIZE "\n", res);
                goto release;
            }

            /* no response needed and no response handler given */
            if (!confirmable && !cb) {
                res = 0;
                goto release;
            }

            /* ctx must have been released at this point */
            assert(ctx == NULL);
            state = STATE_WAIT_RESPONSE;
            /* fall-through */
        case STATE_WAIT_RESPONSE:           /* waiting for response, no empty ACK received */
        case STATE_STOP_RETRANSMIT:         /* waiting for response, empty ACK received */
            if (ctx == NULL) {
                DEBUG("nanocoap: waiting for response (timeout: %"PRIu32" µs)\n",
                      _deadline_left_us(deadline));
            }
            const void *old_ctx = ctx;
            tmp = _sock_recv_buf(sock, &payload, &ctx, _deadline_left_us(deadline));
            /* sock_udp_recv_buf() is supposed to return multiple packet fragments
             * when called multiple times with the same context.
             * In practise, this is not implemented and it will always return a pointer
             * to the whole packet on the first call and NULL on the second call, which
             * releases the packet.
             * This assertion will trigger should the behavior change in the future.
             */
            if (old_ctx) {
                assert(tmp == 0 && ctx == NULL);
            }
            if (tmp == 0) {
                /* no more data */
                /* sock_udp_recv_buf() needs to be called in a loop until ctx is NULL again
                 * to release the buffer */
                continue;
            }
            res = tmp;
            if (res == -ETIMEDOUT) {
                if (tries_left == 0) {
                    DEBUG("nanocoap: maximum retries reached\n");
                    goto release;
                }
                state = STATE_REQUEST_SEND;
                DEBUG("nanocoap: timeout waiting for response\n");
                timeout *= 2;
                deadline = _deadline_from_interval(timeout);
                continue;
            }
            if (res < 0) {
                DEBUG("nanocoap: error receiving CoAP response, %" PRIdSIZE "\n", res);
                goto release;
            }

            /* parse response */
            if (coap_parse_udp(pkt, payload, res) < 0) {
                DEBUG("nanocoap: error parsing packet\n");
                continue;
            }
            else if (_id_or_token_missmatch(pkt, id, token, token_len)) {
                DEBUG("nanocoap: ID mismatch, got %u want %u\n", coap_get_id(pkt), id);
                continue;
            }
            DEBUG("nanocoap: response code=%i\n", coap_get_code_decimal(pkt));
            switch (coap_get_type(pkt)) {
            case COAP_TYPE_ACK:
                if (coap_get_code_raw(pkt) == COAP_CODE_EMPTY) {
                    /* empty ACK, wait for separate response */
                    state = STATE_STOP_RETRANSMIT;
                    deadline = _deadline_from_interval(CONFIG_COAP_SEPARATE_RESPONSE_TIMEOUT_MS
                                                     * US_PER_MS);
                    tries_left = 0; /* stop retransmissions */
                    DEBUG("nanocoap: wait for separate response\n");
                    continue;
                }
                /* fall-through */
            case COAP_TYPE_RST:
            case COAP_TYPE_CON:
            case COAP_TYPE_NON:
                if (coap_get_type(pkt) == COAP_TYPE_RST) {
                    /* think about whether cb should be called on RST as well */
                    res = -EBADMSG;
                    goto release;
                }
                if (coap_get_type(pkt) == COAP_TYPE_CON) {
                    _send_ack(sock, pkt);
                }
                if (cb) {
                    res = cb(arg, pkt);
                }
                else {
                    res = _get_error(pkt);
                }
                goto release;
            }
        }
    }

release:
    while (ctx) {
        /* make sure ctx is really deleted in all cases */
        _sock_recv_buf(sock, &payload, &ctx, 0);
    }

    return res;
}

#if MODULE_NANOCOAP_TCP
static int _tcp_send(nanocoap_sock_t *sock, coap_pkt_t *pkt)
{
    coap_finalize_tcp(pkt);
    const uint8_t *ptr = pkt->buf;
    size_t remaining = coap_get_total_len(pkt);
    ssize_t res;
    while (remaining) {
        res = sock_tcp_write(&sock->tcp, ptr, remaining);
        if (res < 0) {
            return res;
        }

        if (res == 0) {
            return -EOF;
        }

        ptr += res;
        remaining -= res;
    }

    return 0;
}

static ssize_t nanocoap_sock_tcp_request_cb(nanocoap_sock_t *sock, coap_pkt_t *pkt,
                                            coap_request_cb_t cb, void *arg)
{
    ssize_t res = 0;
    assume(sock && pkt && sock->tcp_buf);

    int err = _tcp_send(sock, pkt);
    if (err) {
        return err;
    }

    /* We may still have unparsed data in the TCP buf. E.g. we must be prepared
     * to receive CSM messages at any point in time */
    uint8_t *end = sock->tcp_buf + CONFIG_NANOCOAP_TCP_SOCKET_BUF_SIZE;

    bool response_handled = false;
    while (!response_handled) {
        uint8_t *pos = sock->tcp_buf + sock->tcp_buf_fill;
        if (pos >= end) {
            return -ENOBUFS;
        }

        res = coap_parse_tcp(pkt, sock->tcp_buf, sock->tcp_buf_fill);
        if (res == -EAGAIN) {
            /* no full CoAP message in buf yet, receive more */
            res = sock_tcp_read(&sock->tcp, pos, (uintptr_t)end - (uintptr_t)pos,
                                CONFIG_NANOCOAP_CLIENT_TCP_TIMEOUT_US);
            if (res < 0) {
                return res;
            }
            if (res == 0) {
                return -EOF;
            }

            sock->tcp_buf_fill += res;
            continue;
        }

        /* parsing message failed, pass on error */
        if (res < 0) {
            return res;
        }

        if (coap_get_code_class(pkt) != COAP_CLASS_SIGNAL) {
            if (cb != NULL) {
                res = cb(arg, pkt);
            }
            else {
                res = _get_error(pkt);
            }
            response_handled = true;
        }

        /* We cannot have more bytes parsed than there are in the buffer. */
        assume((size_t)res <= sock->tcp_buf_fill);
        sock->tcp_buf_fill = sock->tcp_buf_fill - (size_t)res;
        if (sock->tcp_buf_fill) {
            /* There are still bytes in the receive buffer that have not yet
             * been parsed. Move them to the beginning of the buf */
            memmove(sock->tcp_buf, sock->tcp_buf + res, sock->tcp_buf_fill);
        }
    }

    return res;
}
#endif

ssize_t nanocoap_sock_request_cb(nanocoap_sock_t *sock, coap_pkt_t *pkt,
                                 coap_request_cb_t cb, void *arg)
{
    switch (nanocoap_sock_get_type(sock)) {
    default:
    case COAP_SOCKET_TYPE_UDP:
    case COAP_SOCKET_TYPE_DTLS:
        return nanocoap_sock_udp_request_cb(sock, pkt, cb, arg);
#if MODULE_NANOCOAP_TCP
    case COAP_SOCKET_TYPE_TCP:
        return nanocoap_sock_tcp_request_cb(sock, pkt, cb, arg);
#endif
    }
}

static int _request_cb(void *arg, coap_pkt_t *pkt)
{
    struct iovec *buf = arg;
    size_t pkt_len = coap_get_total_len(pkt);

    int res = _get_error(pkt);
    if (res) {
        return res;
    }

    if (pkt_len > buf->iov_len) {
        return -ENOBUFS;
    }

    memcpy(buf->iov_base, pkt->buf, pkt_len);

    pkt->buf = buf->iov_base;
    pkt->payload = pkt->buf + (pkt_len - pkt->payload_len);

    return pkt_len;
}

ssize_t nanocoap_sock_request(nanocoap_sock_t *sock, coap_pkt_t *pkt, size_t len)
{
    struct iovec buf = {
        .iov_base = pkt->buf,
        .iov_len  = len,
    };
    return nanocoap_sock_request_cb(sock, pkt, _request_cb, &buf);
}

static int _get_put_cb(void *arg, coap_pkt_t *pkt)
{
    struct iovec *buf = arg;

    int res = _get_error(pkt);
    if (res) {
        return res;
    }

    if (pkt->payload_len > buf->iov_len) {
        return -ENOBUFS;
    }

    memcpy(buf->iov_base, pkt->payload, pkt->payload_len);

    return pkt->payload_len;
}

static ssize_t _sock_get(nanocoap_sock_t *sock, const char *path,
                         uint8_t type,
                         void *response, size_t max_len)
{
    coap_pkt_t pkt;

    struct iovec ctx = {
        .iov_base = response,
        .iov_len  = max_len,
    };

    ssize_t hdr_len = nanocoap_sock_build_pkt(sock, &pkt, sock->hdr_buf, sizeof(sock->hdr_buf),
                                              type, NULL, 0, COAP_METHOD_GET);
    assume(hdr_len > 0);
    uint8_t *pktpos = pkt.buf + hdr_len;
    pktpos += coap_opt_put_uri_pathquery(pktpos, NULL, path);
    assert(pktpos < (uint8_t *)sock->hdr_buf + sizeof(sock->hdr_buf));

    pkt.payload = pktpos;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(sock, &pkt, _get_put_cb, &ctx);
}

ssize_t nanocoap_sock_get(nanocoap_sock_t *sock, const char *path,
                          void *response, size_t len_max)
{
    return _sock_get(sock, path, COAP_TYPE_CON, response, len_max);
}

#ifdef MODULE_NANOCOAP_SOCK_OBSERVE
static void _async_udp_handler(sock_udp_t *sock, sock_async_flags_t type, void *arg)
{
    if (!(type & SOCK_ASYNC_MSG_RECV)) {
        return;
    }

    coap_pkt_t pkt;
    void *payload, *ctx = NULL;
    coap_observe_client_t *obs = arg;
    ssize_t res = sock_udp_recv_buf(sock, &payload, &ctx, 0, NULL);
    if (res <= 0) {
        return;
    }

    /* parse response */
    if (coap_parse(&pkt, payload, res) < 0) {
        DEBUG("nanocoap: error parsing packet\n");
        goto out;
    }

    DEBUG("nanocoap: response code=%i\n", coap_get_code_decimal(&pkt));
    switch (coap_get_type(&pkt)) {
    case COAP_TYPE_CON:
        _send_ack(&obs->sock, &pkt);
        /* fall-through */
    case COAP_TYPE_NON:
        obs->cb(obs->arg, &pkt);
        break;
    default:
        DEBUG("nanocoap: ignore observe pkt of invalid type %u\n", coap_get_type(&pkt));
        break;
    }
out:
    /* release data */
    sock_udp_recv_buf(sock, &payload, &ctx, 0, NULL);
}

static int _observe_reg_wrapper(void *arg, coap_pkt_t *pkt)
{
    coap_observe_client_t *obs = arg;
    bool registered = coap_find_option(pkt, COAP_OPT_OBSERVE);
    int res = obs->cb(obs->arg, pkt);

    return registered ? res : -EPROTONOSUPPORT;
}

static ssize_t _get_observe(coap_observe_client_t *ctx, const char *path,
                            bool unregister)
{
    /* buffer for CoAP header */
    uint8_t buffer[CONFIG_NANOCOAP_BLOCK_HEADER_MAX];
    uint8_t *pktpos = buffer;

    coap_pkt_t pkt;

    uint16_t lastonum = 0;

    pktpos += nanocoap_sock_build_pkt(&ctx->sock, &pkt, buffer, sizeof(buffer),
                                      COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET);
    pktpos += coap_opt_put_observe(pktpos, lastonum, unregister);
    lastonum = COAP_OPT_OBSERVE;
    pktpos += coap_opt_put_uri_pathquery(pktpos, &lastonum, path);

    pkt.payload = pktpos;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(&ctx->sock, &pkt, _observe_reg_wrapper, ctx);
}

ssize_t nanocoap_sock_observe_url(const char *url, coap_observe_client_t *ctx,
                                  coap_request_cb_t cb, void *arg)
{
    int res = nanocoap_sock_url_connect(url, &ctx->sock);
    if (res) {
        return res;
    }
    ctx->cb  = cb;
    ctx->arg = arg;

    res = _get_observe(ctx, sock_urlpath(url), false);
    if (res >= 0) {
        sock_udp_event_init(&ctx->sock.udp, CONFIG_NANOCOAP_SOCK_EVENT_PRIO,
                            _async_udp_handler, ctx);
    }
    else {
        nanocoap_sock_close(&ctx->sock);
        ctx->cb = NULL;
    }

    return res;
}

ssize_t nanocoap_sock_unobserve_url(const char *url, coap_observe_client_t *ctx)
{
    if (ctx->cb == NULL) {
        return -ENOTCONN;
    }

    int res = _get_observe(ctx, sock_urlpath(url), true);

    /* we expect no observe option in the response */
    if (res == -EPROTONOSUPPORT) {
        res = 0;
    }

    nanocoap_sock_close(&ctx->sock);
    ctx->cb = NULL;
    return res;
}
#endif /* MODULE_NANOCOAP_SOCK_OBSERVE */

ssize_t _sock_put_post(nanocoap_sock_t *sock, const char *path, unsigned code,
                       uint8_t type, const void *request, size_t len,
                       void *response, size_t max_len)
{
    iolist_t payload = {
        .iol_base = (void *)request,
        .iol_len  = len,
    };

    coap_pkt_t pkt;

    struct iovec ctx = {
        .iov_base = response,
        .iov_len  = max_len,
    };

    uint16_t lastonum = 0;
    ssize_t hdr_len = nanocoap_sock_build_pkt(sock, &pkt, sock->hdr_buf, sizeof(sock->hdr_buf),
                                              type, NULL, 0, code);

    pkt.snips = &payload;
    assume(hdr_len > 0);
    uint8_t *pktpos = pkt.buf + hdr_len;
    pktpos += coap_opt_put_uri_pathquery(pktpos, &lastonum, path);

    if (response == NULL && type == COAP_TYPE_NON) {
        /* all responses (2.xx, 4.xx and 5.xx) are ignored */
        pktpos += coap_opt_put_uint(pktpos, lastonum,
                                    COAP_OPT_NO_RESPONSE, 26);
    }

    if (len) {
        /* set payload marker */
        *pktpos++ = 0xFF;
    }
    assert(pktpos < (uint8_t *)sock->hdr_buf + sizeof(sock->hdr_buf));

    pkt.payload = pktpos;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(sock, &pkt, response ? _get_put_cb : NULL, &ctx);
}

ssize_t nanocoap_sock_put(nanocoap_sock_t *sock, const char *path,
                          const void *request, size_t len,
                          void *response, size_t len_max)
{
    return _sock_put_post(sock, path, COAP_METHOD_PUT, COAP_TYPE_CON, request, len,
                          response, len_max);
}

ssize_t nanocoap_sock_post(nanocoap_sock_t *sock, const char *path,
                           const void *request, size_t len,
                           void *response, size_t len_max)
{
    return _sock_put_post(sock, path, COAP_METHOD_POST, COAP_TYPE_CON, request, len,
                          response, len_max);
}

ssize_t nanocoap_sock_fetch(nanocoap_sock_t *sock, const char *path,
                            const void *request, size_t len,
                            void *response, size_t len_max)
{
    return _sock_put_post(sock, path, COAP_METHOD_FETCH, COAP_TYPE_CON, request, len,
                          response, len_max);
}

ssize_t nanocoap_sock_put_non(nanocoap_sock_t *sock, const char *path,
                              const void *request, size_t len,
                              void *response, size_t len_max)
{
    return _sock_put_post(sock, path, COAP_METHOD_PUT, COAP_TYPE_NON, request, len,
                          response, len_max);
}

ssize_t nanocoap_sock_post_non(nanocoap_sock_t *sock, const char *path,
                               const void *request, size_t len,
                               void *response, size_t len_max)
{
    return _sock_put_post(sock, path, COAP_METHOD_POST, COAP_TYPE_NON, request, len,
                          response, len_max);
}

ssize_t nanocoap_sock_fetch_non(nanocoap_sock_t *sock, const char *path,
                               const void *request, size_t len,
                               void *response, size_t len_max)
{
    return _sock_put_post(sock, path, COAP_METHOD_FETCH, COAP_TYPE_NON, request, len,
                          response, len_max);
}

ssize_t nanocoap_sock_get_non(nanocoap_sock_t *sock, const char *path,
                              void *response, size_t len_max)
{
    return _sock_get(sock, path, COAP_TYPE_NON, response, len_max);
}

static ssize_t _sock_put_post_url(const char *url, unsigned code,
                                  const void *request, size_t len,
                                  void *response, size_t len_max)
{
    nanocoap_sock_t sock;
    int res = nanocoap_sock_url_connect(url, &sock);
    if (res) {
        return res;
    }

    res = _sock_put_post(&sock, sock_urlpath(url), code, COAP_TYPE_CON,
                         request, len, response, len_max);
    nanocoap_sock_close(&sock);

    return res;
}

ssize_t nanocoap_sock_put_url(const char *url,
                              const void *request, size_t len,
                              void *response, size_t len_max)
{
    return _sock_put_post_url(url, COAP_METHOD_PUT, request, len, response, len_max);
}

ssize_t nanocoap_sock_post_url(const char *url,
                               const void *request, size_t len,
                               void *response, size_t len_max)
{
    return _sock_put_post_url(url, COAP_METHOD_POST, request, len, response, len_max);
}

ssize_t nanocoap_sock_fetch_url(const char *url,
                                const void *request, size_t len,
                                void *response, size_t len_max)
{
    return _sock_put_post_url(url, COAP_METHOD_FETCH, request, len, response, len_max);
}

ssize_t nanocoap_sock_delete(nanocoap_sock_t *sock, const char *path)
{
    coap_pkt_t pkt;

    ssize_t hdr_len = nanocoap_sock_build_pkt(sock, &pkt, sock->hdr_buf, sizeof(sock->hdr_buf),
                                              COAP_TYPE_CON, NULL, 0,
                                              COAP_METHOD_DELETE);
    assume(hdr_len > 0);
    uint8_t *pktpos = pkt.buf + hdr_len;
    pktpos += coap_opt_put_uri_pathquery(pktpos, NULL, path);
    assert(pktpos < (uint8_t *)sock->hdr_buf + sizeof(sock->hdr_buf));

    pkt.payload = pktpos;

    return nanocoap_sock_request_cb(sock, &pkt, NULL, NULL);
}

ssize_t nanocoap_sock_delete_url(const char *url)
{
    nanocoap_sock_t sock;
    int res = nanocoap_sock_url_connect(url, &sock);
    if (res) {
        return res;
    }

    res = nanocoap_sock_delete(&sock, sock_urlpath(url));
    nanocoap_sock_close(&sock);

    return res;
}

#if MODULE_NANOCOAP_UDP
ssize_t nanocoap_request_udp(coap_pkt_t *pkt, const sock_udp_ep_t *local,
                             const sock_udp_ep_t *remote, size_t len)
{
    int res;
    nanocoap_sock_t sock;

    res = nanocoap_sock_udp_connect(&sock, local, remote);
    if (res) {
        return res;
    }

    res = nanocoap_sock_request(&sock, pkt, len);
    nanocoap_sock_close(&sock);

    return res;
}
#endif

static int _block_cb(void *arg, coap_pkt_t *pkt)
{
    _block_ctx_t *ctx = arg;
    coap_block1_t block2;

    int res = _get_error(pkt);
    if (res) {
        return res;
    }

    /* response was not block-wise */
    if (!coap_get_block2(pkt, &block2)) {
        block2.offset = 0;
        block2.more = false;
    }

    DEBUG("nanocoap: got block %"PRIu32" (offset %u)\n",
          block2.blknum, (unsigned)block2.offset);

    if (block2.blknum != ctx->blknum) {
        return -EAGAIN;
    }

    ctx->more = block2.more;
    return ctx->callback(ctx->arg, block2.offset, pkt->payload, pkt->payload_len, block2.more);
}

static int _fetch_block(nanocoap_sock_t *sock, uint8_t *buf, size_t len,
                        const char *path, coap_blksize_t blksize,
                        _block_ctx_t *ctx)
{
    coap_pkt_t pkt;
    uint16_t lastonum = 0;

    void *token = NULL;
    size_t token_len = 0;

#if CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
    /* HACK: go-coap always expects a token */
    /* see https://github.com/plgd-dev/go-coap/issues/512 */
    token = ctx->token;
    token_len = sizeof(ctx->token);
#endif

    ssize_t hdr_len = nanocoap_sock_build_pkt(sock, &pkt, buf, len, COAP_TYPE_CON,
                                              token, token_len, COAP_METHOD_GET);
    assume(hdr_len > 0);
    buf += hdr_len;
    buf += coap_opt_put_uri_pathquery(buf, &lastonum, path);
    buf += coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK2, (ctx->blknum << 4) | blksize);

    assume((uintptr_t)buf - (uintptr_t)pkt.buf < len);

    pkt.payload = buf;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(sock, &pkt, _block_cb, ctx);
}

int nanocoap_sock_block_request(coap_block_request_t *req,
                                const void *data, size_t len, bool more,
                                coap_request_cb_t callback, void *arg)
{
    /* clip the payload at the block size */
    if (len > coap_szx2size(req->blksize)) {
        len = coap_szx2size(req->blksize);
        more = true;
    }

    int res;
    iolist_t snip = {
        .iol_base = (void *)data,
        .iol_len  = len,
    };

    coap_pkt_t pkt;
    uint16_t lastonum = 0;

    ssize_t hdr_size = nanocoap_sock_build_pkt(req->sock, &pkt, req->sock->hdr_buf, sizeof(req->sock->hdr_buf),
                                               COAP_TYPE_CON, NULL, 0,
                                               req->method);
    assume(hdr_size > 0);
    uint8_t *pktpos = pkt.buf + hdr_size;
    pktpos += coap_opt_put_uri_pathquery(pktpos, &lastonum, req->path);
    pktpos += coap_opt_put_uint(pktpos, lastonum, COAP_OPT_BLOCK1,
                                (req->blknum << 4) | req->blksize | (more ? 0x8 : 0));
    if (len) {
        /* set payload marker */
        *pktpos++ = 0xFF;
    }
    assert(pktpos < (uint8_t *)req->sock->hdr_buf + sizeof(req->sock->hdr_buf));

    pkt.payload = pktpos;
    pkt.payload_len = 0;
    pkt.snips = &snip;

    res = nanocoap_sock_request_cb(req->sock, &pkt, callback, arg);
    if (res < 0) {
        return res;
    }

    ++req->blknum;
    return len;
}

int nanocoap_sock_get_blockwise(nanocoap_sock_t *sock, const char *path,
                                coap_blksize_t blksize,
                                coap_blockwise_cb_t callback, void *arg)
{
    _block_ctx_t ctx = {
        .callback = callback,
        .arg = arg,
        .more = true,
    };

#if CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
    random_bytes(ctx.token, sizeof(ctx.token));
#endif

    uint8_t retries = CONFIG_COAP_MAX_RETRANSMIT;
    while (ctx.more) {
        DEBUG("nanocoap: fetching block %"PRIu32"\n", ctx.blknum);

        int res = _fetch_block(sock, sock->hdr_buf, sizeof(sock->hdr_buf), path, blksize, &ctx);
        if (res == -EAGAIN) {
            if (--retries) {
                continue;
            }
            res = -EBADMSG;
        }
        if (res < 0) {
            DEBUG("nanocoap: error fetching block %"PRIu32": %d\n", ctx.blknum, res);
            return res;
        }

        ctx.blknum += 1;
        retries = CONFIG_COAP_MAX_RETRANSMIT;
    }

    return 0;
}

typedef struct {
    uint8_t *ptr;
    size_t len;
    size_t offset;
    size_t res;
} _buf_slice_t;

static int _2buf_slice(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    _buf_slice_t *ctx = arg;

    if (offset + len < ctx->offset) {
        return 0;
    }

    if (offset > ctx->offset + ctx->len) {
        return 0;
    }

    if (!ctx->len) {
        return 0;
    }

    offset = ctx->offset - offset;
    len = MIN(len - offset, ctx->len);

    memcpy(ctx->ptr, buf + offset, len);

    ctx->len -= len;
    ctx->ptr += len;
    ctx->offset += len;
    ctx->res += len;

    DEBUG("nanocoap: got %"PRIuSIZE" bytes, %"PRIuSIZE" bytes left (offset: %"PRIuSIZE")\n",
          len, ctx->len, offset);

    if (!more) {
        ctx->len = 0;
    }

    return 0;
}

static unsigned _num_blks(size_t offset, size_t len, coap_blksize_t szx)
{
    uint16_t mask = coap_szx2size(szx) - 1;
    uint8_t shift = szx + 4;
    size_t end = offset + len;

    unsigned num_blks = ((end >> shift) + !!(end & mask))
                      - ((offset >> shift) + !!(offset & mask));
    return num_blks;
}

int nanocoap_sock_get_slice(nanocoap_sock_t *sock, const char *path,
                            coap_blksize_t blksize, size_t offset,
                            void *dst, size_t len)
{
    /* try to find optimal blocksize */
    unsigned num_blocks = _num_blks(offset, len, blksize);
    for (uint8_t szx = 0; szx < blksize; ++szx) {
        if (_num_blks(offset, len, szx) <= num_blocks) {
            blksize = szx;
            break;
        }
    }

    _buf_slice_t dst_ctx = {
        .ptr = dst,
        .len = len,
        .offset = offset,
    };

    _block_ctx_t ctx = {
        .callback = _2buf_slice,
        .arg = &dst_ctx,
        .blknum = offset >> (blksize + 4),
        .more = true,
    };

#if CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
    random_bytes(ctx.token, sizeof(ctx.token));
#endif

    uint8_t retries = CONFIG_COAP_MAX_RETRANSMIT;
    while (dst_ctx.len) {
        DEBUG("nanocoap: fetching block %"PRIu32"\n", ctx.blknum);

        int res = _fetch_block(sock, sock->hdr_buf, sizeof(sock->hdr_buf), path, blksize, &ctx);
        if (res == -EAGAIN) {
            if (--retries) {
                continue;
            }
            res = -EBADMSG;
        }
        if (res < 0) {
            DEBUG("nanocoap: error fetching block %"PRIu32": %d\n", ctx.blknum, res);
            return res;
        }

        ctx.blknum += 1;
        retries = CONFIG_COAP_MAX_RETRANSMIT;
    }

    return dst_ctx.res;
}

int nanocoap_sock_url_connect(const char *url, nanocoap_sock_t *sock)
{
    char hostport[CONFIG_SOCK_HOSTPORT_MAXLEN];
#if MODULE_NANOCOAP_UDP || MODULE_NANOCOAP_DTLS || MODULE_NANOCOAP_TCP
    union {
#  if MODULE_NANOCOAP_UDP || MODULE_NANOCOAP_DTLS
        sock_udp_ep_t udp;
#  endif
#  if MODULE_NANOCOAP_TCP
        sock_tcp_ep_t tcp;
#  endif
    } remote;
#endif

    nanocoap_socket_type_t type = COAP_SOCKET_TYPE_UDP;

    if (IS_USED(MODULE_NANOCOAP_DTLS) && !strncmp(url, "coaps://", 8)) {
        DEBUG_PUTS("nanocoap: CoAP over DTLS URL detected");
        type = COAP_SOCKET_TYPE_DTLS;
    }
    else if (IS_USED(MODULE_NANOCOAP_TCP) && !strncmp(url, "coap+tcp://", 11)) {
        DEBUG_PUTS("nanocoap: CoAP over TCP URL detected");
        type = COAP_SOCKET_TYPE_TCP;
    }
    else if (strncmp(url, "coap://", 7)) {
        DEBUG_PUTS("nanocoap: protocol scheme not recognized/supported");
        return -ENOTSUP;
    }

    if (sock_urlsplit(url, hostport, NULL) < 0) {
        DEBUG("nanocoap: invalid URL\n");
        return -EINVAL;
    }


    switch (type) {
    default:
        return -ENOTSUP;
#if MODULE_NANOCOAP_TCP
    case COAP_SOCKET_TYPE_TCP:
        if (sock_tcp_name2ep(&remote.tcp, hostport) < 0) {
            DEBUG("nanocoap: invalid URL\n");
            return -EINVAL;
        }

        if (!remote.tcp.port) {
            remote.tcp.port = COAP_PORT;
        }
        return nanocoap_sock_tcp_connect(sock, 0, &remote.tcp);
#endif
#if MODULE_NANOCOAP_UDP
    case COAP_SOCKET_TYPE_UDP:
        if (sock_udp_name2ep(&remote.udp, hostport) < 0) {
            DEBUG("nanocoap: invalid URL\n");
            return -EINVAL;
        }

        if (!remote.udp.port) {
            remote.udp.port = COAP_PORT;
        }

        return nanocoap_sock_udp_connect(sock, NULL, &remote.udp);
#endif
#if MODULE_NANOCOAP_DTLS
    case COAP_SOCKET_TYPE_DTLS:
        {
#  if SOCK_HAS_IPV6
            /* tinydtls wants the interface to match */
            if (!remote.udp.netif && sock_udp_ep_is_v6(&remote.udp) &&
                ipv6_addr_is_link_local((ipv6_addr_t *)remote.udp.addr.ipv6)) {
                netif_t *iface = netif_iter(NULL);
                if (iface == NULL) {
                    return -ENODEV;
                }
                remote.udp.netif = netif_get_id(iface);
            }

            sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
            if (!sock_udp_ep_is_v6(&remote.udp)) {
                local.family = AF_INET;
            }
#  else
            sock_udp_ep_t local = SOCK_IPV4_EP_ANY;
#  endif
            if (sock_udp_name2ep(&remote.udp, hostport) < 0) {
                DEBUG("nanocoap: invalid URL\n");
                return -EINVAL;
            }

            if (!remote.udp.port) {
                remote.udp.port = COAPS_PORT;
            }
            return nanocoap_sock_dtls_connect(sock, &local, &remote.udp, CONFIG_NANOCOAP_SOCK_DTLS_TAG);
        }
        break;
#endif
    }
}

int nanocoap_get_blockwise_url(const char *url,
                               coap_blksize_t blksize,
                               coap_blockwise_cb_t callback, void *arg)
{
    nanocoap_sock_t sock;
    int res = nanocoap_sock_url_connect(url, &sock);
    if (res) {
        return res;
    }

    res = nanocoap_sock_get_blockwise(&sock, sock_urlpath(url), blksize, callback, arg);
    nanocoap_sock_close(&sock);

    return res;
}

typedef struct {
    uint8_t *ptr;
    size_t len;
} _buf_t;

static int _2buf(void *arg, size_t offset, uint8_t *buf, size_t len, int more)
{
    _buf_t *dst = arg;

    if (offset + len > dst->len) {
        return -ENOBUFS;
    }

    memcpy(dst->ptr + offset, buf, len);

    if (!more) {
        dst->len = offset + len;
    }

    return 0;
}

ssize_t nanocoap_get_blockwise_url_to_buf(const char *url,
                                          coap_blksize_t blksize,
                                          void *buf, size_t len)
{
    _buf_t _buf = { .ptr = buf, .len = len };
    int res = nanocoap_get_blockwise_url(url, blksize, _2buf, &_buf);

    return (res < 0) ? (ssize_t)res : (ssize_t)_buf.len;
}

ssize_t nanocoap_get_blockwise_to_buf(nanocoap_sock_t *sock, const char *path,
                                      coap_blksize_t blksize,
                                      void *buf, size_t len)
{
    _buf_t _buf = { .ptr = buf, .len = len };

    int res = nanocoap_sock_get_blockwise(sock, path, blksize, _2buf, &_buf);

    return (res < 0) ? (ssize_t)res : (ssize_t)_buf.len;
}


#if MODULE_NANOCOAP_UDP
static kernel_pid_t _coap_server_pid;
static void *_nanocoap_server_thread(void *local)
{
    static uint8_t buf[CONFIG_NANOCOAP_SERVER_BUF_SIZE];

    nanocoap_server(local, buf, sizeof(buf));

    return NULL;
}

kernel_pid_t nanocoap_server_start_udp(const sock_udp_ep_t *local)
{
    static char stack[CONFIG_NANOCOAP_SERVER_STACK_SIZE];

    if (_coap_server_pid) {
        return _coap_server_pid;
    }
    _coap_server_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                                     0, _nanocoap_server_thread,
                                     (void *)local, "nanoCoAP server");
    return _coap_server_pid;
}

void auto_init_nanocoap_server(void)
{
    sock_udp_ep_t local = {
        .port = COAP_PORT,
        .family = AF_INET6,
    };

    nanocoap_server_start_udp(&local);
}
#endif

#if MODULE_NANOCOAP_TCP
int nanocoap_sock_tcp_connect(nanocoap_sock_t *sock,
                              uint16_t local_port, const sock_tcp_ep_t *remote)
{
    memset(sock, 0, sizeof(*sock));
    if (NULL == (sock->tcp_buf = malloc(CONFIG_NANOCOAP_TCP_SOCKET_BUF_SIZE))) {
        return -ENOMEM;
    }

    nanocoap_sock_set_type(sock, COAP_SOCKET_TYPE_TCP);

    int retval = sock_tcp_connect(&sock->tcp, remote, local_port, 0);

    if (retval) {
        free(sock->tcp_buf);
        return retval;
    }

    ssize_t len = nanocoap_send_csm_message(&sock->tcp, sock->tcp_buf,
                                            CONFIG_NANOCOAP_TCP_SOCKET_BUF_SIZE);

    if (len < 0) {
        free(sock->tcp_buf);
        sock_tcp_disconnect(&sock->tcp);
        return len;
    }

    return 0;
}

ssize_t nanocoap_send_csm_message(sock_tcp_t *sock, void *_buf, size_t buf_size)
{
    assert((buf_size >= 16) && (buf_size <= UINT16_MAX));
    uint8_t *buf = _buf;
    ssize_t pos = coap_build_tcp_hdr(buf, buf_size, NULL, 0, COAP_CODE_SIGNAL_CSM);
    if (pos < 0) {
        return pos;
    }

    pos += coap_opt_put_uint(buf + pos, 0, COAP_SIGNAL_CSM_OPT_MAX_MESSAGE_SIZE,
                             buf_size);
    pos += coap_put_option(buf + pos, COAP_SIGNAL_CSM_OPT_MAX_MESSAGE_SIZE,
                           COAP_SIGNAL_CSM_OPT_BLOCK_WISE_TRANSFER, NULL, 0);
    /* Indicate support for extended token length, if selected. Allow up to
     * half of the buffer to be used for the token. */
    if (IS_USED(MODULE_NANOCOAP_TOKEN_EXT)) {
        pos += coap_opt_put_uint(buf + pos,
                                 COAP_SIGNAL_CSM_OPT_BLOCK_WISE_TRANSFER,
                                 COAP_SIGNAL_CSM_OPT_EXTENDED_TOKEN_LENGTH,
                                 buf_size / 2);
    }

    size_t shrunk = coap_finalize_tcp_header_in_buf(buf, pos);
    buf += shrunk;
    pos -= shrunk;
    DEBUG("nanocoap: sending %u B of CSM\n", (unsigned)pos);
    return sock_tcp_write(sock, buf, pos);
}
#endif

#if MODULE_NANOCOAP_SERVER_TCP
int _handle_csm(const coap_pkt_t *pkt)
{
    /* We don't implement any CSM Option. But we need to fail when stumbling
     * upon a critical option */
    if (coap_has_unprocessed_critical_options(pkt)) {
        return -ENOTSUP;
    }

    return 0;
}

static void _server_tcp_data_cb(sock_tcp_t *sock, sock_async_flags_t flags,
                                void *arg)
{
    nanocoap_tcp_server_ctx_t *ctx = arg;
    unsigned idx = index_of(ctx->socks, sock);

    if (flags & SOCK_ASYNC_CONN_FIN) {
        DEBUG("nanocoap_tcp: TCP socket #%u closed. "
              "Event flags = %x\n",
              idx, (unsigned)flags);
        sock_tcp_disconnect(sock);
        return;
    }

    if (!(flags & (SOCK_ASYNC_MSG_RECV | SOCK_ASYNC_MSG_SENT))) {
        DEBUG("nanocoap_tcp: Unhandled event(s) on TCP socket #%u. "
              "Event flags = %x\n",
              idx, (unsigned)flags);
        return;
    }

    uint16_t fill = ctx->rx_fill[idx];
    const size_t bufsize = CONFIG_NANOCOAP_SERVER_BUF_SIZE;
    uint8_t *rx_buf = &ctx->rx_bufs[idx * bufsize];
    uint8_t *tx_buf = ctx->tx_buf;
    ssize_t len = sock_tcp_read(sock, rx_buf + fill, bufsize - fill, 0);

    if (len <= 0) {
        if (len == -EAGAIN) {
            DEBUG_PUTS("nanocoap_tcp: Got data ready event, but no data ready.");
            return;
        }

        DEBUG("nanocoap_tcp: Failed to read data for socket #%u: %" PRIdSIZE "\n",
              idx, len);

        sock_tcp_disconnect(sock);
        return;
    }

    DEBUG("nanocoap_tcp: Got %" PRIdSIZE " B on socket #%u\n", len, idx);

    ctx->rx_fill[idx] = fill += len;

    /* a single segment may contain more than one CoAP message */
    while (fill) {
        coap_pkt_t pkt;
        len = coap_parse_tcp(&pkt, rx_buf, fill);

        if (len == -EAGAIN) {
            DEBUG("nanocoap_tcp: CoAP message on socket #%u not yet complete\n",
                  idx);
            return;
        }

        ssize_t reply_len = 0;
        int res;

        switch (coap_get_code_raw(&pkt)) {
        case COAP_CODE_EMPTY:
            /* > In CoAP over reliable transports, Empty messages
             * > (Code 0.00) can always be sent and MUST be ignored by the
             * > recipient. */
            break;
        case COAP_CODE_SIGNAL_CSM:
            res = _handle_csm(&pkt);
            if (res < 0) {
                DEBUG("nanocoap_tcp: failed to handle CSM of client #%u: %d\n",
                      idx, res);
                sock_tcp_disconnect(sock);
                return;
            }
            break;
        case COAP_CODE_SIGNAL_PING:
            reply_len = coap_reply_simple(&pkt, COAP_CODE_SIGNAL_PONG,
                                          tx_buf, bufsize, 0, NULL, 0);
            break;
        case COAP_CODE_SIGNAL_PONG:
            /* We probably did not send a ping as server, but let's just
             * ignore the pong anyway */
            break;
        default:
            {
                sock_tcp_ep_t remote;
                sock_tcp_get_remote(sock, &remote);
                coap_request_ctx_t req = {
                    .sock_tcp = sock,
                };
                reply_len = coap_handle_req(&pkt, tx_buf, bufsize, &req);
            }
        }

        if (reply_len) {
            if (reply_len < 0) {
                DEBUG("nanocoap_tcp: error handling request of client #%u: %"
                      PRIdSIZE "\n", idx, reply_len);
                sock_tcp_disconnect(sock);
                return;
            }
            size_t shrunk = coap_finalize_tcp_header_in_buf(tx_buf, reply_len);
            tx_buf += shrunk;
            reply_len -= shrunk;
            ssize_t outlen = sock_tcp_write(sock, tx_buf, reply_len);

            if (outlen != reply_len) {
                DEBUG("nanocoap_tcp: failed to respond to client #%u: %" PRIdSIZE "\n",
                      idx, outlen);
                sock_tcp_disconnect(sock);
                return;
            }

            DEBUG("nanocoap_tcp: replied %" PRIdSIZE " B to client #%u\n",
                  outlen, idx);
        }

        fill -= len;

        /* if there is still unparsed data, move it to the front of the buf */
        if (fill) {
            DEBUG("nanocoap_tcp: Still %u B of unparsed data in buf, "
                  "shuffling bytes around\n",
                  (unsigned)fill);
            memmove(rx_buf, rx_buf + len, fill);
        }
    }

    ctx->rx_fill[idx] = fill;
}

static void _server_tcp_listen_cb(sock_tcp_queue_t *queue,
                                  sock_async_flags_t flags, void *arg)
{
    nanocoap_tcp_server_ctx_t *ctx = arg;

    if (!(flags & SOCK_ASYNC_CONN_RECV)) {
        DEBUG("nanocoap_tcp: Unhandled event(s) on listen socket. "
              "Event flags = %x\n",
              (unsigned)flags);
        return;
    }

    sock_tcp_t *client;
    int res = sock_tcp_accept(queue, &client, 0);
    if (res != 0) {
        DEBUG("nanocoap_tcp: sock_tcp_accept failed with %d\n", res);
        return;
    }

    /* On lwIP there is a race: Any data received between `sock_tcp_accept()`
     * and sock_tcp_event_init() is lost. So call this early and hope for the
     * best. */
    sock_tcp_event_init(client, ctx->evq, _server_tcp_data_cb, ctx);

    DEBUG("nanocoap_tcp: accepted connection\n");

    ssize_t len = nanocoap_send_csm_message(client, ctx->tx_buf, sizeof(ctx->tx_buf));
    if (len  < 0) {
        /* Silence linter: len is used with ENABLE_DEBUG == 1 */
        (void)len;
        DEBUG("nanocoap_tcp: error sending CSM message: %" PRIdSIZE
              " --> dropping just accepted connection\n",
              len);
        sock_tcp_disconnect(client);
        return;
    }

    unsigned idx = index_of(ctx->socks, client);
    ctx->rx_fill[idx] = 0;
    DEBUG("nanocoap_tcp: new client %u\n", idx);
}

int nanocoap_server_tcp(nanocoap_tcp_server_ctx_t *ctx,
                        event_queue_t *evq,
                        const sock_tcp_ep_t *local)

{
    assume(ctx && evq);
    memset(ctx, 0, sizeof(*ctx));

    ctx->evq = evq;

    if (local) {
        memcpy(&ctx->local, local, sizeof(ctx->local));
        if (!ctx->local.port) {
            ctx->local.port = COAP_PORT;
        }
    }
    else {
        ctx->local = (sock_tcp_ep_t){ .port = COAP_PORT, .family = AF_INET6 };
    }


    int res = sock_tcp_listen(&ctx->queue, &ctx->local, ctx->socks,
                              ARRAY_SIZE(ctx->socks), 0);
    if (res != 0) {
        return res;
    }

    sock_tcp_queue_event_init(&ctx->queue, evq, _server_tcp_listen_cb, ctx);

    return 0;
}
#endif

#if MODULE_NANOCOAP_UDP
int nanocoap_server_udp(sock_udp_ep_t *local, void *rsp_buf, size_t rsp_buf_len)
{
    sock_udp_t sock;
    sock_udp_ep_t remote;
    coap_request_ctx_t ctx = {
        .remote = &remote,
    };

    if (!local->port) {
        local->port = COAP_PORT;
    }

    ssize_t res = sock_udp_create(&sock, local, NULL, 0);
    if (res != 0) {
        return res;
    }

    void *buf;
    void *buf_ctx = NULL;

    while (1) {

        if (buf_ctx) {
            /* free the buffer */
            res = sock_udp_recv_buf_aux(&sock, &buf, &buf_ctx, 0, NULL, NULL);
            assert(res == 0);
        }

        sock_udp_aux_rx_t *aux_in_ptr = NULL;
#ifdef MODULE_SOCK_AUX_LOCAL
        sock_udp_aux_rx_t aux_in = {
            .flags = SOCK_AUX_GET_LOCAL,
        };
        aux_in_ptr = &aux_in;
#endif

        res = sock_udp_recv_buf_aux(&sock, &buf, &buf_ctx, SOCK_NO_TIMEOUT,
                                    &remote, aux_in_ptr);
        if (res <= 0) {
            DEBUG("nanocoap: error receiving UDP packet %" PRIdSIZE "\n", res);
            continue;
        }
        coap_pkt_t pkt;
        if (coap_parse_udp(&pkt, buf, res) < 0) {
            DEBUG("nanocoap: error parsing packet\n");
            continue;
        }

        sock_udp_aux_tx_t *aux_out_ptr = NULL;
#ifdef MODULE_SOCK_AUX_LOCAL
        /* make sure we reply with the same address that the request was
         * destined for -- except in the multicast case */
        sock_udp_aux_tx_t aux_out = {
            .flags = SOCK_AUX_SET_LOCAL,
            .local = aux_in.local,
        };
        if (!sock_udp_ep_is_multicast(&aux_in.local)) {
            aux_out_ptr = &aux_out;
        }
        ctx.local = &aux_in.local;
#endif
        if ((res = coap_handle_req(&pkt, rsp_buf, rsp_buf_len, &ctx)) <= 0) {
            DEBUG("nanocoap: error handling request %" PRIdSIZE "\n", res);
            continue;
        }

        sock_udp_send_aux(&sock, rsp_buf, res, &remote, aux_out_ptr);
    }

    return 0;
}
#endif

#if MODULE_NANOCOAP_SERVER_SEPARATE
int nanocoap_server_prepare_separate(nanocoap_server_response_ctx_t *ctx,
                                     coap_pkt_t *pkt, const coap_request_ctx_t *req)
{
    size_t tkl = coap_get_token_len(pkt);
    if (tkl > sizeof(ctx->token)) {
        DEBUG_PUTS("nanocoap: token too long for separate response ctx");
        /* Legacy code may not check the return value. To still have somewhat
         * sane behavior, we ask for no response for any response class.
         * Getting no reply is certainly not ideal, but better than one without
         * a matching token. */
        memset(ctx, 0, sizeof(*ctx));
        ctx->no_response = 0xff;
        return -EOVERFLOW;
    }
    ctx->tkl = tkl;
    memcpy(ctx->token, coap_get_token(pkt), tkl);
    uint32_t no_response = 0;
    coap_opt_get_uint(pkt, COAP_OPT_NO_RESPONSE, &no_response);
    ctx->no_response = no_response;

    switch (coap_get_transport(pkt)) {
#if MODULE_NANOCOAP_UDP
    case COAP_TRANSPORT_UDP:
        memcpy(&ctx->remote_udp, req->remote_udp, sizeof(ctx->remote_udp));
        assert(req->local);
        memcpy(&ctx->local_udp, req->local_udp, sizeof(ctx->local_udp));
        break;
#endif
#if MODULE_NANOCOAP_SERVER_TCP
    case COAP_TRANSPORT_TCP:
        ctx->sock_tcp = req->sock_tcp;
        break;
#endif
    default:
        return -ENOTSUP;
    }

#if NANOCOAP_ENABLED_TRANSPORTS > 1
    ctx->transport = coap_get_transport(pkt);
#endif

    return 0;
}

bool nanocoap_server_is_remote_in_response_ctx(const nanocoap_server_response_ctx_t *ctx,
                                               const coap_request_ctx_t *req)
{
    switch (nanocoap_server_response_ctx_transport(ctx)) {
#if MODULE_NANOCOAP_UDP
    case COAP_TRANSPORT_UDP:
        return sock_udp_ep_equal(&ctx->remote_udp, req->remote_udp);
#endif
#if MODULE_NANOCOAP_TCP
    case COAP_TRANSPORT_TCP:
        return ctx->sock_tcp == req->sock_tcp;
#endif
    default:
        (void)ctx;
        (void)req;
        return false;
    }
}

ssize_t nanocoap_server_build_separate(const nanocoap_server_response_ctx_t *ctx,
                                       void *buf, size_t buf_len,
                                       unsigned code, unsigned type,
                                       uint16_t msg_id)
{
    assert(type != COAP_TYPE_ACK);
    assert(type != COAP_TYPE_CON); /* TODO: add support */

    const uint8_t no_response_index = (code >> 5) - 1;
    /* If the handler code misbehaved here, we'd face UB otherwise */
    assert(no_response_index < 7);

    const uint8_t mask = 1 << no_response_index;
    if (ctx->no_response & mask) {
        return -ECANCELED;
    }

    switch (nanocoap_server_response_ctx_transport(ctx)) {
    case COAP_TRANSPORT_UDP:
        return coap_build_udp_hdr(buf, buf_len, type, ctx->token, ctx->tkl,
                              code, msg_id);
    case COAP_TRANSPORT_TCP:
        return coap_build_tcp_hdr(buf, buf_len, ctx->token, ctx->tkl, code);
    default:
        return -ENOTSUP;
    }
}

#  if MODULE_NANOCOAP_SERVER_TCP
/* TODO: Implement sock_tcp_writev() to avoid sending the CoAP header and
 * payload in separate TCP segments even if they would fit in a single one. */
static int _tcp_writev(sock_tcp_t *sock, const iolist_t *iol)
{
    while (iol) {
        const uint8_t *data = iol->iol_base;
        size_t len = iol->iol_len;
        while (len) {
            ssize_t tmp = sock_tcp_write(sock, data, len);
            if (tmp < 0) {
                return tmp;
            }
            data += tmp;
            len -= tmp;
        }

        iol = iol->iol_next;
    }

    return 0;
}
#  endif

#  if MODULE_NANOCOAP_UDP
static int _nanocoap_server_sendv_separate_udp(const nanocoap_server_response_ctx_t *ctx,
                                               const iolist_t *reply)
{
    sock_udp_aux_tx_t *aux_out_ptr = NULL;
    /* make sure we reply with the same address that the request was
     * destined for -- except in the multicast case */
    sock_udp_aux_tx_t aux_out = {
        .flags = SOCK_AUX_SET_LOCAL,
        .local = ctx->local_udp,
    };
    if (!sock_udp_ep_is_multicast(&ctx->local_udp)) {
        aux_out_ptr = &aux_out;
    }
    ssize_t retval = sock_udp_sendv_aux(NULL, reply, &ctx->remote_udp, aux_out_ptr);

    if (retval < 0) {
        return retval;
    }

    return 0;
}
#  endif

int nanocoap_server_sendv_separate(const nanocoap_server_response_ctx_t *ctx,
                                   const iolist_t *reply)
{
    switch (nanocoap_server_response_ctx_transport(ctx)) {
#  if MODULE_NANOCOAP_UDP
    case COAP_TRANSPORT_UDP:
        return _nanocoap_server_sendv_separate_udp(ctx, reply);
#  endif
#  if MODULE_NANOCOAP_SERVER_TCP
        return _tcp_writev(ctx->sock_tcp, reply);
#  endif
    default:
        return -ENOTSUP;
    }
}

int nanocoap_server_send_separate(const nanocoap_server_response_ctx_t *ctx,
                                  unsigned code, unsigned type,
                                  const void *payload, size_t len)
{
    uint8_t rbuf[sizeof(coap_udp_hdr_t) + COAP_TOKEN_LENGTH_MAX + 1];

    ssize_t hdr_len = nanocoap_server_build_separate(ctx, rbuf, sizeof(rbuf),
                                                     code, type, random_uint32());
    if (hdr_len < 0) {
        return hdr_len;
    }

    /* add payload marker if needed */
    if (len) {
        rbuf[hdr_len++] = 0xFF;
    }

    iolist_t data = {
        .iol_base = (void *)payload,
        .iol_len  = len,
    };

    iolist_t head = {
        .iol_next = &data,
        .iol_base = rbuf,
        .iol_len = hdr_len,
    };

    return nanocoap_server_sendv_separate(ctx, &head);
}
#endif

#if MODULE_NANOCOAP_SERVER_OBSERVE
static bool _response_ctx_is_ep_equal(const nanocoap_server_response_ctx_t *resp_ctx,
                                      const coap_request_ctx_t *req_ctx)
{
    switch (nanocoap_server_response_ctx_transport(resp_ctx)) {
#if MODULE_NANOCOAP_UDP
    case COAP_TRANSPORT_UDP:
        return sock_udp_ep_equal(&resp_ctx->remote_udp, req_ctx->remote_udp);
#endif
#if MODULE_NANOCOAP_SERVER_TCP
    case COAP_TRANSPORT_TCP:
        return resp_ctx->sock_tcp == req_ctx->sock_tcp;
#endif
    default:
        return false;
    }
}

int nanocoap_register_observer(const coap_request_ctx_t *req_ctx, coap_pkt_t *req_pkt)
{
    mutex_lock(&_observer_pool_lock);

    _observer_t *free = NULL;
    const coap_resource_t *resource = req_ctx->resource;

    for (size_t i = 0; i < CONFIG_NANOCOAP_MAX_OBSERVERS; i++) {
        if (_observer_pool[i].resource == NULL) {
            free = &_observer_pool[i];
        }
        if ((_observer_pool[i].resource == resource)
                && _response_ctx_is_ep_equal(&_observer_pool[i].response, req_ctx))
        {
            /* Deviation from the standard: Subscribing twice makes no
             * sense with our CoAP implementation, so either this is a
             * reaffirmation of an existing subscription (same token) or the
             * client lost state (different token). We just update the
             * subscription in either case */
            DEBUG("nanocoap: observe slot %" PRIuSIZE " reused\n", i);
            uint8_t tkl = coap_get_token_len(req_pkt);
            _observer_pool[i].response.tkl = tkl;
            memcpy(_observer_pool[i].response.token, coap_get_token(req_pkt), tkl);
            mutex_unlock(&_observer_pool_lock);
            return 0;
        }
    }

    if (!free) {
        DEBUG_PUTS("nanocoap: observe registration failed, no free slot");
        mutex_unlock(&_observer_pool_lock);
        return -ENOMEM;
    }

    int retval = nanocoap_server_prepare_separate(&free->response, req_pkt, req_ctx);
    if (retval) {
        DEBUG("nanocoap: observe registration failed: %d\n", retval);
        mutex_unlock(&_observer_pool_lock);
        return retval;
    }
    free->resource = req_ctx->resource;
    free->msg_id = random_uint32();
    mutex_unlock(&_observer_pool_lock);
    DEBUG("nanocoap: new observe registration at slot %" PRIuSIZE "\n",
          index_of(_observer_pool, free));
    return 0;
}

void nanocoap_unregister_observer(const coap_request_ctx_t *req_ctx,
                                  const coap_pkt_t *req_pkt)
{
    mutex_lock(&_observer_pool_lock);
    for (size_t i = 0; i < CONFIG_NANOCOAP_MAX_OBSERVERS; i++) {
        if ((_observer_pool[i].resource == req_ctx->resource)
                && (_observer_pool[i].response.tkl == coap_get_token_len(req_pkt))
                && !memcmp(_observer_pool[i].response.token, coap_get_token(req_pkt),
                           _observer_pool[i].response.tkl)
                && _response_ctx_is_ep_equal(&_observer_pool[i].response, req_ctx)) {
            DEBUG("nanocoap: observer at index %" PRIuSIZE " unregistred\n", i);
            _observer_pool[i].resource = NULL;
        }
    }
    mutex_unlock(&_observer_pool_lock);
}

#  if MODULE_NANOCOAP_UDP
void nanocoap_unregister_observer_due_to_reset(const sock_udp_ep_t *ep,
                                               uint16_t msg_id)
{
    mutex_lock(&_observer_pool_lock);
    for (size_t i = 0; i < CONFIG_NANOCOAP_MAX_OBSERVERS; i++) {
        if ((_observer_pool[i].resource != NULL)
                && (_observer_pool[i].msg_id == msg_id)
                && sock_udp_ep_equal(&_observer_pool[i].response.remote_udp, ep)) {
            DEBUG("nanocoap: observer at index %" PRIuSIZE " unregistered due to RST\n", i);
            _observer_pool[i].resource = NULL;
            return;
        }
    }
    mutex_unlock(&_observer_pool_lock);
}
#  endif

void nanocoap_notify_observers(const coap_resource_t *res, const iolist_t *iol)
{
    mutex_lock(&_observer_pool_lock);
    for (size_t i = 0; i < CONFIG_NANOCOAP_MAX_OBSERVERS; i++) {
        if (_observer_pool[i].resource == res) {
            uint8_t rbuf[sizeof(coap_udp_hdr_t) + COAP_TOKEN_LENGTH_MAX + 1];

            ssize_t hdr_len = nanocoap_server_build_separate(&_observer_pool[i].response, rbuf, sizeof(rbuf),
                                                             COAP_CODE_CONTENT, COAP_TYPE_NON,
                                                             ++_observer_pool[i].msg_id);
            if (hdr_len < 0) {
                /* no need to keep the observer in the pool, if we cannot
                 * send anyway */
                _observer_pool[i].resource = NULL;
                continue;
            }

            const iolist_t msg = {
                .iol_base = rbuf,
                .iol_len = hdr_len,
                .iol_next = (iolist_t *)iol
            };

            if (nanocoap_server_sendv_separate(&_observer_pool[i].response, &msg)) {
                /* no need to keep the observer in the pool, if we cannot
                 * send anyway */
                _observer_pool[i].resource = NULL;
            }
        }
    }
    mutex_unlock(&_observer_pool_lock);
}
#endif
