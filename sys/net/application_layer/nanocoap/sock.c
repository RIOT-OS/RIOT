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
#ifdef MODULE_SOCK_ASYNC_EVENT
#include "net/sock/async/event.h"
#endif
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "random.h"
#include "sys/uio.h" /* IWYU pragma: keep (exports struct iovec) */
#include "ztimer.h"

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

int nanocoap_sock_dtls_connect(nanocoap_sock_t *sock, sock_udp_ep_t *local,
                               const sock_udp_ep_t *remote, credman_tag_t tag)
{
#if IS_USED(MODULE_NANOCOAP_DTLS)
    uint8_t buf[CONFIG_NANOCOAP_DTLS_HANDSHAKE_BUF_SIZE];

    sock->type = COAP_SOCKET_TYPE_DTLS;
    return sock_dtls_establish_session(&sock->udp, &sock->dtls, &sock->dtls_session,
                                       tag, local, remote, buf, sizeof(buf));
#else
    (void)sock;
    (void)local;
    (void)remote;
    (void)tag;
    return -ENOTSUP;
#endif
}

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

static inline nanocoap_socket_type_t _get_type(nanocoap_sock_t *sock)
{
#if IS_USED(MODULE_NANOCOAP_DTLS)
    return sock->type;
#else
    (void)sock;
    return COAP_SOCKET_TYPE_UDP;
#endif
}

static int _sock_sendv(nanocoap_sock_t *sock, const iolist_t *snips)
{
    switch (_get_type(sock)) {
    case COAP_SOCKET_TYPE_UDP:
        return sock_udp_sendv(&sock->udp, snips, NULL);
#if IS_USED(MODULE_NANOCOAP_DTLS)
    case COAP_SOCKET_TYPE_DTLS:
        return sock_dtls_sendv(&sock->dtls, &sock->dtls_session, snips,
                               CONFIG_SOCK_DTLS_TIMEOUT_MS);
#endif
    default:
        assert(0);
        return -EINVAL;
    }
}

static int _sock_recv_buf(nanocoap_sock_t *sock, void **data, void **ctx, uint32_t timeout)
{
    switch (_get_type(sock)) {
    case COAP_SOCKET_TYPE_UDP:
        return sock_udp_recv_buf(&sock->udp, data, ctx, timeout, NULL);
#if IS_USED(MODULE_NANOCOAP_DTLS)
    case COAP_SOCKET_TYPE_DTLS:
        return sock_dtls_recv_buf(&sock->dtls, &sock->dtls_session, data, ctx, timeout);
#endif
    default:
        assert(0);
        return -EINVAL;
   }
}

static int _send_ack(nanocoap_sock_t *sock, coap_pkt_t *pkt)
{
    coap_hdr_t ack;

    const iolist_t snip = {
        .iol_base = &ack,
        .iol_len  = sizeof(ack),
    };

    coap_build_hdr(&ack, COAP_TYPE_ACK, NULL, 0,
                   COAP_CODE_EMPTY, ntohs(pkt->hdr->id));

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
        if (pkt->hdr->code != 0) {
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

ssize_t nanocoap_sock_request_cb(nanocoap_sock_t *sock, coap_pkt_t *pkt,
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
        .iol_base = pkt->hdr,
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
            if (coap_parse(pkt, payload, res) < 0) {
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

    memcpy(buf->iov_base, pkt->hdr, pkt_len);

    pkt->hdr = buf->iov_base;
    pkt->payload = (uint8_t*)pkt->hdr + (pkt_len - pkt->payload_len);

    return pkt_len;
}

ssize_t nanocoap_sock_request(nanocoap_sock_t *sock, coap_pkt_t *pkt, size_t len)
{
    struct iovec buf = {
        .iov_base = pkt->hdr,
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
    uint8_t *pktpos = sock->hdr_buf;

    coap_pkt_t pkt = {
        .hdr = (void *)pktpos,
    };

    struct iovec ctx = {
        .iov_base = response,
        .iov_len  = max_len,
    };

    pktpos += coap_build_hdr(pkt.hdr, type, NULL, 0, COAP_METHOD_GET,
                             nanocoap_sock_next_msg_id(sock));
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

    coap_pkt_t pkt = {
        .hdr = (void *)pktpos,
    };

    uint16_t lastonum = 0;

    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET,
                             nanocoap_sock_next_msg_id(&ctx->sock));
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
    uint8_t *pktpos = sock->hdr_buf;

    iolist_t payload = {
        .iol_base = (void *)request,
        .iol_len  = len,
    };

    coap_pkt_t pkt = {
        .hdr = (void *)pktpos,
        .snips = &payload,
    };

    struct iovec ctx = {
        .iov_base = response,
        .iov_len  = max_len,
    };

    uint16_t lastonum = 0;
    pktpos += coap_build_hdr(pkt.hdr, type, NULL, 0, code, nanocoap_sock_next_msg_id(sock));
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
    uint8_t *pktpos = sock->hdr_buf;

    coap_pkt_t pkt = {
        .hdr = (void *)pktpos,
    };

    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_DELETE,
                             nanocoap_sock_next_msg_id(sock));
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

ssize_t nanocoap_request(coap_pkt_t *pkt, const sock_udp_ep_t *local,
                         const sock_udp_ep_t *remote, size_t len)
{
    int res;
    nanocoap_sock_t sock;

    res = nanocoap_sock_connect(&sock, local, remote);
    if (res) {
        return res;
    }

    res = nanocoap_sock_request(&sock, pkt, len);
    nanocoap_sock_close(&sock);

    return res;
}

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
    coap_pkt_t pkt = {
        .hdr = (void *)buf,
    };
    uint16_t lastonum = 0;

    void *token = NULL;
    size_t token_len = 0;

#if CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
    /* HACK: go-coap always expects a token */
    /* see https://github.com/plgd-dev/go-coap/issues/512 */
    token = ctx->token;
    token_len = sizeof(ctx->token);
#endif

    buf += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, token, token_len, COAP_METHOD_GET,
                          nanocoap_sock_next_msg_id(sock));
    buf += coap_opt_put_uri_pathquery(buf, &lastonum, path);
    buf += coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK2, (ctx->blknum << 4) | blksize);

    (void)len;
    assert((uintptr_t)buf - (uintptr_t)pkt.hdr < len);

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

    coap_pkt_t pkt = {
        .hdr = (void *)req->sock->hdr_buf,
        .snips = &snip,
    };

    uint8_t *pktpos = (void *)pkt.hdr;
    uint16_t lastonum = 0;

    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, req->method,
                             nanocoap_sock_next_msg_id(req->sock));
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
    sock_udp_ep_t remote;

    bool is_coaps = false;

    if (IS_USED(MODULE_NANOCOAP_DTLS) && !strncmp(url, "coaps://", 8)) {
        DEBUG("nanocoap: CoAPS URL detected\n");
        is_coaps = true;
    }

    if (!is_coaps && strncmp(url, "coap://", 7)) {
        DEBUG("nanocoap: URL doesn't start with \"coap://\"\n");
        return -EINVAL;
    }

    if (sock_urlsplit(url, hostport, NULL) < 0) {
        DEBUG("nanocoap: invalid URL\n");
        return -EINVAL;
    }

    if (sock_udp_name2ep(&remote, hostport) < 0) {
        DEBUG("nanocoap: invalid URL\n");
        return -EINVAL;
    }

    if (!remote.port) {
        remote.port = is_coaps ? COAPS_PORT : COAP_PORT;
    }

    if (is_coaps) {
#if SOCK_HAS_IPV6
        /* tinydtls wants the interface to match */
        if (!remote.netif && sock_udp_ep_is_v6(&remote) &&
            ipv6_addr_is_link_local((ipv6_addr_t *)remote.addr.ipv6)) {
            netif_t *iface = netif_iter(NULL);
            if (iface == NULL) {
                return -ENODEV;
            }
            remote.netif = netif_get_id(iface);
        }

        sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
        if (!sock_udp_ep_is_v6(&remote)) {
            local.family = AF_INET;
        }
#else
        sock_udp_ep_t local = SOCK_IPV4_EP_ANY;
#endif
        return nanocoap_sock_dtls_connect(sock, &local, &remote, CONFIG_NANOCOAP_SOCK_DTLS_TAG);
    } else {
        return nanocoap_sock_connect(sock, NULL, &remote);
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

ssize_t _udp_recv_aux(sock_udp_t *sock, void *data, size_t max_len,
                      uint32_t timeout, sock_udp_ep_t *remote,
                      sock_udp_aux_rx_t *aux)
{
    void *pkt = NULL, *ctx = NULL;
    uint8_t *ptr = data;
    ssize_t res, ret = 0;
    bool nobufs = false;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    while ((res = sock_udp_recv_buf_aux(sock, &pkt, &ctx, timeout, remote,
                                        aux)) > 0) {
        if (res > (ssize_t)max_len) {
            nobufs = true;
            res = max_len;
        }
        memcpy(ptr, pkt, res);
        ptr += res;
        ret += res;
    }
    return (nobufs) ? -ENOBUFS : ((res < 0) ? res : ret);
}

int nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize)
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
        return -1;
    }

    while (1) {

        sock_udp_aux_rx_t *aux_in_ptr = NULL;
#ifdef MODULE_SOCK_AUX_LOCAL
        sock_udp_aux_rx_t aux_in = {
            .flags = SOCK_AUX_GET_LOCAL,
        };
        aux_in_ptr = &aux_in;
#endif

        res = _udp_recv_aux(&sock, buf, bufsize, SOCK_NO_TIMEOUT,
                            &remote, aux_in_ptr);
        if (res <= 0) {
            DEBUG("nanocoap: error receiving UDP packet %" PRIdSIZE "\n", res);

            if (res == -ENOBUFS) {
                coap_pkt_t pkt;
                coap_parse(&pkt, buf, bufsize);

                res = coap_build_reply(&pkt, COAP_CODE_REQUEST_ENTITY_TOO_LARGE,
                                       buf, bufsize, 0);
                sock_udp_send(&sock, buf, res, &remote);
            }
            continue;
        }

        coap_pkt_t pkt;
        if (coap_parse(&pkt, (uint8_t *)buf, res) < 0) {
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
        if ((res = coap_handle_req(&pkt, buf, bufsize, &ctx)) <= 0) {
            DEBUG("nanocoap: error handling request %" PRIdSIZE "\n", res);
            continue;
        }

        sock_udp_send_aux(&sock, buf, res, &remote, aux_out_ptr);
    }

    return 0;
}

static kernel_pid_t _coap_server_pid;
static void *_nanocoap_server_thread(void *local)
{
    static uint8_t buf[CONFIG_NANOCOAP_SERVER_BUF_SIZE];

    nanocoap_server(local, buf, sizeof(buf));

    return NULL;
}

kernel_pid_t nanocoap_server_start(const sock_udp_ep_t *local)
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

    nanocoap_server_start(&local);
}

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
    memcpy(&ctx->remote, req->remote, sizeof(ctx->remote));
    assert(req->local);
    memcpy(&ctx->local, req->local, sizeof(ctx->local));
    uint32_t no_response = 0;
    coap_opt_get_uint(pkt, COAP_OPT_NO_RESPONSE, &no_response);
    ctx->no_response = no_response;

    return 0;
}

bool nanocoap_server_is_remote_in_response_ctx(const nanocoap_server_response_ctx_t *ctx,
                                               const coap_request_ctx_t *req)
{
    return sock_udp_ep_equal(&ctx->remote, req->remote);
}

ssize_t nanocoap_server_build_separate(const nanocoap_server_response_ctx_t *ctx,
                                       void *buf, size_t buf_len,
                                       unsigned code, unsigned type,
                                       uint16_t msg_id)
{
    assert(type != COAP_TYPE_ACK);
    assert(type != COAP_TYPE_CON); /* TODO: add support */
    if ((sizeof(coap_hdr_t) + COAP_TOKEN_LENGTH_MAX + 1) > buf_len) {
        return -EOVERFLOW;
    }

    const uint8_t no_response_index = (code >> 5) - 1;
    /* If the handler code misbehaved here, we'd face UB otherwise */
    assert(no_response_index < 7);

    const uint8_t mask = 1 << no_response_index;
    if (ctx->no_response & mask) {
        return -ECANCELED;
    }

    return coap_build_hdr(buf, type, ctx->token, ctx->tkl, code, msg_id);
}

int nanocoap_server_sendv_separate(const nanocoap_server_response_ctx_t *ctx,
                                   const iolist_t *reply)
{
    sock_udp_aux_tx_t *aux_out_ptr = NULL;
    /* make sure we reply with the same address that the request was
     * destined for -- except in the multicast case */
    sock_udp_aux_tx_t aux_out = {
        .flags = SOCK_AUX_SET_LOCAL,
        .local = ctx->local,
    };
    if (!sock_udp_ep_is_multicast(&ctx->local)) {
        aux_out_ptr = &aux_out;
    }
    ssize_t retval = sock_udp_sendv_aux(NULL, reply, &ctx->remote, aux_out_ptr);

    if (retval < 0) {
        return retval;
    }

    return 0;
}

int nanocoap_server_send_separate(const nanocoap_server_response_ctx_t *ctx,
                                  unsigned code, unsigned type,
                                  const void *payload, size_t len)
{
    uint8_t rbuf[sizeof(coap_hdr_t) + COAP_TOKEN_LENGTH_MAX + 1];

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
                && sock_udp_ep_equal(&_observer_pool[i].response.remote,
                                     coap_request_ctx_get_remote_udp(req_ctx)))
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
                && sock_udp_ep_equal(&_observer_pool[i].response.remote, coap_request_ctx_get_remote_udp(req_ctx))) {
            DEBUG("nanocoap: observer at index %" PRIuSIZE " unregistered\n", i);
            _observer_pool[i].resource = NULL;
        }
    }
    mutex_unlock(&_observer_pool_lock);
}

void nanocoap_unregister_observer_due_to_reset(const sock_udp_ep_t *ep,
                                               uint16_t msg_id)
{
    mutex_lock(&_observer_pool_lock);
    for (size_t i = 0; i < CONFIG_NANOCOAP_MAX_OBSERVERS; i++) {
        if ((_observer_pool[i].resource != NULL)
                && (_observer_pool[i].msg_id == msg_id)
                && sock_udp_ep_equal(&_observer_pool[i].response.remote, ep)) {
            DEBUG("nanocoap: observer at index %" PRIuSIZE " unregistered due to RST\n", i);
            _observer_pool[i].resource = NULL;
            return;
        }
    }
    mutex_unlock(&_observer_pool_lock);
}

void nanocoap_notify_observers(const coap_resource_t *res, const iolist_t *iol)
{
    mutex_lock(&_observer_pool_lock);
    for (size_t i = 0; i < CONFIG_NANOCOAP_MAX_OBSERVERS; i++) {
        if (_observer_pool[i].resource == res) {
            uint8_t rbuf[sizeof(coap_hdr_t) + COAP_TOKEN_LENGTH_MAX + 1];

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
