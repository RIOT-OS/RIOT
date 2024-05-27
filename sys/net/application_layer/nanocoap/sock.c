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

#include "atomic_utils.h"
#include "net/credman.h"
#include "net/nanocoap_sock.h"
#include "net/sock/util.h"
#include "net/sock/udp.h"
#include "net/iana/portrange.h"
#include "random.h"
#include "sys/uio.h"
#include "timex.h"
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
#define CONFIG_NANOCOAP_DTLS_HANDSHAKE_BUF_SIZE (160)
#endif

enum {
    STATE_REQUEST_SEND,     /**< request was just sent or will be sent again */
    STATE_STOP_RETRANSMIT,  /**< stop retransmissions due to a matching empty ACK */
    STATE_RESPONSE_OK,      /**< valid response was received                 */
};

typedef struct {
    coap_blockwise_cb_t callback;
    void *arg;
    bool more;
#if CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
    uint8_t token[4];
#endif
} _block_ctx_t;

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
        return coap_get_id(pkt) != id;
    default:
        if (coap_get_token_len(pkt) != token_len) {
            return true;
        }
        return memcmp(coap_get_token(pkt), token, token_len);
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
                return res;
            }

            /* no response needed and no response handler given */
            if (!confirmable && !cb) {
                return 0;
            }

            /* ctx must have been released at this point */
            assert(ctx == NULL);
            /* fall-through */
        case STATE_STOP_RETRANSMIT:
        case STATE_RESPONSE_OK:
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
                if (state == STATE_STOP_RETRANSMIT) {
                    continue;
                }
                return res;
            }
            res = tmp;
            if (res == -ETIMEDOUT) {
                if (tries_left == 0) {
                    DEBUG("nanocoap: maximum retries reached\n");
                    return -ETIMEDOUT;
                }
                DEBUG("nanocoap: timeout waiting for response\n");
                timeout *= 2;
                deadline = _deadline_from_interval(timeout);
                continue;
            }
            if (res < 0) {
                DEBUG("nanocoap: error receiving CoAP response, %" PRIdSIZE "\n", res);
                return res;
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
            case COAP_TYPE_RST:
                /* TODO: handle different? */
                res = -EBADMSG;
                break;
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
            case COAP_TYPE_CON:
            case COAP_TYPE_NON:
                state = STATE_RESPONSE_OK;
                if (coap_get_type(pkt) == COAP_TYPE_CON) {
                    /* send ACK */
                    _send_ack(sock, pkt);
                }
                /* call user callback */
                if (cb) {
                    res = cb(arg, pkt);
                } else {
                    res = _get_error(pkt);
                }
                break;
            }
        }
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

ssize_t nanocoap_sock_get(nanocoap_sock_t *sock, const char *path, void *buf, size_t len)
{
    /* buffer for CoAP header */
    uint8_t buffer[CONFIG_NANOCOAP_BLOCK_HEADER_MAX];
    uint8_t *pktpos = buffer;

    coap_pkt_t pkt = {
        .hdr = (void *)pktpos,
    };

    struct iovec ctx = {
        .iov_base = buf,
        .iov_len  = len,
    };

    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET,
                             nanocoap_sock_next_msg_id(sock));
    pktpos += coap_opt_put_uri_pathquery(pktpos, NULL, path);

    pkt.payload = pktpos;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(sock, &pkt, _get_put_cb, &ctx);
}

ssize_t _sock_put_post(nanocoap_sock_t *sock, const char *path, unsigned code,
                       uint8_t type, const void *request, size_t len,
                       void *response, size_t max_len)
{
    /* buffer for CoAP header */
    uint8_t buffer[CONFIG_NANOCOAP_BLOCK_HEADER_MAX];
    uint8_t *pktpos = buffer;

    iolist_t payload = {
        .iol_base = (void *)request,
        .iol_len  = len,
    };

    coap_pkt_t pkt = {
        .hdr = (void *)buffer,
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
    /* buffer for CoAP header */
    uint8_t buffer[CONFIG_NANOCOAP_BLOCK_HEADER_MAX];
    uint8_t *pktpos = buffer;

    coap_pkt_t pkt = {
        .hdr = (void *)pktpos,
    };

    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_DELETE,
                             nanocoap_sock_next_msg_id(sock));
    pktpos += coap_opt_put_uri_pathquery(pktpos, NULL, path);

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

    ctx->more = block2.more;
    return ctx->callback(ctx->arg, block2.offset, pkt->payload, pkt->payload_len, block2.more);
}

static int _fetch_block(nanocoap_sock_t *sock, uint8_t *buf, size_t len,
                        const char *path, coap_blksize_t blksize, unsigned num,
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
    buf += coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK2, (num << 4) | blksize);

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
    uint8_t buf[CONFIG_NANOCOAP_BLOCK_HEADER_MAX];
    iolist_t snip = {
        .iol_base = (void *)data,
        .iol_len  = len,
    };

    coap_pkt_t pkt = {
        .hdr = (void *)buf,
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
    uint8_t buf[CONFIG_NANOCOAP_BLOCK_HEADER_MAX];

    _block_ctx_t ctx = {
        .callback = callback,
        .arg = arg,
        .more = true,
    };

#if CONFIG_NANOCOAP_SOCK_BLOCK_TOKEN
    random_bytes(ctx.token, sizeof(ctx.token));
#endif

    unsigned num = 0;
    while (ctx.more) {
        DEBUG("nanocoap: fetching block %u\n", num);

        int res = _fetch_block(sock, buf, sizeof(buf), path, blksize, num, &ctx);
        if (res < 0) {
            DEBUG("nanocoap: error fetching block %u: %d\n", num, res);
            return res;
        }

        num += 1;
    }

    return 0;
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

        res = sock_udp_recv_aux(&sock, buf, bufsize, SOCK_NO_TIMEOUT,
                                &remote, aux_in_ptr);
        if (res <= 0) {
            DEBUG("nanocoap: error receiving UDP packet %" PRIdSIZE "\n", res);
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
                                     THREAD_CREATE_STACKTEST, _nanocoap_server_thread,
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

void nanocoap_server_prepare_separate(nanocoap_server_response_ctx_t *ctx,
                                    coap_pkt_t *pkt, const coap_request_ctx_t *req)
{
    ctx->tkl = coap_get_token_len(pkt);
    memcpy(ctx->token, coap_get_token(pkt), ctx->tkl);
    memcpy(&ctx->remote, req->remote, sizeof(ctx->remote));
#ifdef MODULE_SOCK_AUX_LOCAL
    assert(req->local);
    memcpy(&ctx->local, req->local, sizeof(ctx->local));
#endif
    uint32_t no_response = 0;
    coap_opt_get_uint(pkt, COAP_OPT_NO_RESPONSE, &no_response);
    ctx->no_response = no_response;
}

int nanocoap_server_send_separate(const nanocoap_server_response_ctx_t *ctx,
                                unsigned code, unsigned type,
                                const void *payload, size_t len)
{
    uint8_t rbuf[sizeof(coap_hdr_t) + COAP_TOKEN_LENGTH_MAX + 1];
    assert(type != COAP_TYPE_ACK);
    assert(type != COAP_TYPE_CON); /* TODO: add support */

    const uint8_t no_response_index = (code >> 5) - 1;
    /* If the handler code misbehaved here, we'd face UB otherwise */
    assert(no_response_index < 7);

    const uint8_t mask = 1 << no_response_index;
    if (ctx->no_response & mask) {
        return 0;
    }

    iolist_t data = {
        .iol_base = (void *)payload,
        .iol_len  = len,
    };

    iolist_t head = {
        .iol_next = &data,
        .iol_base = rbuf,
    };
    head.iol_len = coap_build_hdr((coap_hdr_t *)rbuf, type,
                                  ctx->token, ctx->tkl,
                                  code, random_uint32());
    if (len) {
        rbuf[head.iol_len++] = 0xFF;
    }

    sock_udp_aux_tx_t *aux_out_ptr = NULL;
#ifdef MODULE_SOCK_AUX_LOCAL
    /* make sure we reply with the same address that the request was
     * destined for -- except in the multicast case */
    sock_udp_aux_tx_t aux_out = {
        .flags = SOCK_AUX_SET_LOCAL,
        .local = ctx->local,
    };
    if (!sock_udp_ep_is_multicast(&ctx->local)) {
        aux_out_ptr = &aux_out;
    }
#endif
    return sock_udp_sendv_aux(NULL, &head, &ctx->remote, aux_out_ptr);
}
