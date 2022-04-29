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
 * @brief       Nanocoap sock helpers
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
#include "net/nanocoap_sock.h"
#include "net/sock/util.h"
#include "net/sock/udp.h"
#include "random.h"
#include "sys/uio.h"
#include "timex.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

enum {
    STATE_SEND_REQUEST,
    STATE_AWAIT_RESPONSE,
};

typedef struct {
    coap_blockwise_cb_t callback;
    void *arg;
    bool more;
} _block_ctx_t;

static uint16_t _get_id(void)
{
    __attribute__((section(".noinit")))
    static uint16_t id;
    return atomic_fetch_add_u16(&id, 1);
}

int nanocoap_sock_connect(nanocoap_sock_t *sock, sock_udp_ep_t *local, sock_udp_ep_t *remote)
{
    if (!remote->port) {
        remote->port = COAP_PORT;
    }

    return sock_udp_create(sock, local, remote, 0);
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

static int _send_ack(nanocoap_sock_t *sock, coap_pkt_t *pkt)
{
    coap_hdr_t ack;
    unsigned tkl = coap_get_token_len(pkt);

    coap_build_hdr(&ack, COAP_TYPE_ACK, coap_get_token(pkt), tkl,
                   COAP_CODE_VALID, ntohs(pkt->hdr->id));

    return sock_udp_send(sock, &ack, sizeof(ack), NULL);
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
    const void *pdu = pkt->hdr;
    const size_t pdu_len = coap_get_total_len(pkt);
    const unsigned id = coap_get_id(pkt);
    void *payload, *ctx = NULL;
    const uint8_t *token = coap_get_token(pkt);
    uint8_t token_len = coap_get_token_len(pkt);

    unsigned state = STATE_SEND_REQUEST;

    uint32_t timeout = random_uint32_range(CONFIG_COAP_ACK_TIMEOUT_MS * US_PER_MS,
                                           CONFIG_COAP_ACK_TIMEOUT_MS * CONFIG_COAP_RANDOM_FACTOR_1000);
    uint32_t deadline = _deadline_from_interval(timeout);

    /* add 1 for initial transmit */
    unsigned tries_left = CONFIG_COAP_MAX_RETRANSMIT + 1;

    /* check if we expect a reply */
    const bool confirmable = coap_get_type(pkt) == COAP_TYPE_CON;

    /* try receiving another packet without re-request */
    bool retry_rx = false;

    while (1) {
        switch (state) {
        case STATE_SEND_REQUEST:
            DEBUG("nanocoap: send %u bytes (%u tries left)\n", (unsigned)pdu_len, tries_left);
            if (--tries_left == 0) {
                DEBUG("nanocoap: maximum retries reached\n");
                return -ETIMEDOUT;
            }

            res = sock_udp_send(sock, pdu, pdu_len, NULL);
            if (res <= 0) {
                DEBUG("nanocoap: error sending coap request, %d\n", (int)res);
                return res;
            }

            /* no response needed and no response handler given */
            if (!confirmable && !cb) {
                return 0;
            }

            /* ctx must have been released at this point */
            assert(ctx == NULL);
            state = STATE_AWAIT_RESPONSE;
            /* fall-through */
        case STATE_AWAIT_RESPONSE:
            DEBUG("nanocoap: waiting for response (timeout: %"PRIu32" µs)\n",
                  _deadline_left_us(deadline));
            tmp = sock_udp_recv_buf(sock, &payload, &ctx, _deadline_left_us(deadline), NULL);
            /* sock_udp_recv_buf() is supposed to return multiple packet fragments
             * when called multiple times with the same context.
             * In practise, this is not implemented and it will always return a pointer
             * to the whole packet on the first call and NULL on the second call, which
             * releases the packet.
             * This assertion will trigger should the behavior change in the future.
             */
            if (retry_rx) {
                assert(tmp == 0 && ctx == NULL);
            }
            if (tmp == 0) {
                /* no more data */
                /* sock_udp_recv_buf() needs to be called in a loop until ctx is NULL again
                 * to release the buffer */
                if (retry_rx) {
                    retry_rx = false;
                    continue;
                }
                return res;
            }
            res = tmp;
            if (res == -ETIMEDOUT) {
                DEBUG("nanocoap: timeout\n");
                timeout *= 2;
                deadline = _deadline_from_interval(timeout);
                state = STATE_SEND_REQUEST;
                continue;
            }
            if (res < 0) {
                DEBUG("nanocoap: error receiving coap response, %d\n", (int)res);
                return res;
            }

            /* parse response */
            if (coap_parse(pkt, payload, res) < 0) {
                DEBUG("nanocoap: error parsing packet\n");
                retry_rx = true;
                continue;
            }
            else if (_id_or_token_missmatch(pkt, id, token, token_len)) {
                DEBUG("nanocoap: ID mismatch %u != %u\n", coap_get_id(pkt), id);
                retry_rx = true;
                continue;
            }

            DEBUG("nanocoap: response code=%i\n", coap_get_code(pkt));
            switch (coap_get_type(pkt)) {
            case COAP_TYPE_RST:
                /* TODO: handle different? */
                return -EBADMSG;
            case COAP_TYPE_CON:
                _send_ack(sock, pkt);
                /* fall-through */
            case COAP_TYPE_NON:
            case COAP_TYPE_ACK:
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
    pkt->token  = (uint8_t*)pkt->hdr + sizeof(coap_hdr_t);
    pkt->payload = (uint8_t*)pkt->hdr + (pkt_len - pkt->payload_len);

    return pkt_len;
}

ssize_t nanocoap_sock_request(sock_udp_t *sock, coap_pkt_t *pkt, size_t len)
{
    struct iovec buf = {
        .iov_base = pkt->hdr,
        .iov_len  = len,
    };
    return nanocoap_sock_request_cb(sock, pkt, _request_cb, &buf);
}

static int _get_cb(void *arg, coap_pkt_t *pkt)
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
    uint8_t *pktpos = buf;
    coap_pkt_t pkt = {
        .hdr = buf,
    };

    struct iovec ctx = {
        .iov_base = buf,
        .iov_len  = len,
    };

    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, _get_id());
    pktpos += coap_opt_put_uri_path(pktpos, 0, path);

    pkt.payload = pktpos;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(sock, &pkt, _get_cb, &ctx);
}

ssize_t nanocoap_request(coap_pkt_t *pkt, sock_udp_ep_t *local,
                         sock_udp_ep_t *remote, size_t len)
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

ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path, void *buf, size_t len)
{
    int res;
    nanocoap_sock_t sock;

    res = nanocoap_sock_connect(&sock, NULL, remote);
    if (res) {
        return res;
    }

    res = nanocoap_sock_get(&sock, path, buf, len);
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

    buf += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, _get_id());
    buf += coap_opt_put_uri_pathquery(buf, &lastonum, path);
    buf += coap_opt_put_uint(buf, lastonum, COAP_OPT_BLOCK2, (num << 4) | blksize);

    (void)len;
    assert((uintptr_t)buf - (uintptr_t)pkt.hdr < len);

    pkt.payload = buf;
    pkt.payload_len = 0;

    return nanocoap_sock_request_cb(sock, &pkt, _block_cb, ctx);
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

    unsigned num = 0;
    while (ctx.more) {
        DEBUG("fetching block %u\n", num);

        int res = _fetch_block(sock, buf, sizeof(buf), path, blksize, num, &ctx);
        if (res < 0) {
            DEBUG("error fetching block %u: %d\n", num, res);
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

    if (strncmp(url, "coap://", 7)) {
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

    return nanocoap_sock_connect(sock, NULL, &remote);
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
        return -1;
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
    nanocoap_sock_t sock;
    sock_udp_ep_t remote;

    if (!local->port) {
        local->port = COAP_PORT;
    }

    ssize_t res = sock_udp_create(&sock, local, NULL, 0);
    if (res != 0) {
        return -1;
    }

    while (1) {
        res = sock_udp_recv(&sock, buf, bufsize, -1, &remote);
        if (res < 0) {
            DEBUG("error receiving UDP packet %d\n", (int)res);
        }
        else if (res > 0) {
            coap_pkt_t pkt;
            if (coap_parse(&pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("error parsing packet\n");
                continue;
            }
            if ((res = coap_handle_req(&pkt, buf, bufsize)) > 0) {
                sock_udp_send(&sock, buf, res, &remote);
            }
            else {
                DEBUG("error handling request %d\n", (int)res);
            }
        }
    }

    return 0;
}
