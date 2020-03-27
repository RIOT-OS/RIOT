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
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "net/nanocoap_sock.h"
#include "net/sock/udp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static ssize_t _nanocoap_req_raw(coap_pkt_t *pkt, sock_udp_t *sock, size_t len);

static inline uint32_t deadline_from_interval(int32_t interval)
{
    assert(interval >= 0);
    return xtimer_now_usec() + (uint32_t)interval;
}

static inline uint32_t deadline_left(uint32_t deadline)
{
    int32_t left = (int32_t)(deadline - xtimer_now_usec());

    if (left < 0) {
        left = 0;
    }
    return left;
}


ssize_t nanocoap_request(coap_pkt_t *pkt, sock_udp_ep_t *local,
                         sock_udp_ep_t *remote, size_t len)
{
    sock_udp_t sock;

    if (!remote->port) {
        remote->port = COAP_PORT;
    }

    ssize_t res = sock_udp_create(&sock, local, remote, 0);
    if (res < 0) {
        return res;
    }
    res = _nanocoap_req_raw(pkt, &sock, len);
    sock_udp_close(&sock);
    return res;
}

static ssize_t _nanocoap_req_raw(coap_pkt_t *pkt, sock_udp_t *sock, size_t len)
{
    size_t pdu_len = (pkt->payload - (uint8_t *)pkt->hdr) + pkt->payload_len;
    uint8_t *buf = (uint8_t*)pkt->hdr;
    uint32_t id = coap_get_id(pkt);
    ssize_t res = -EAGAIN;

    /* TODO: timeout random between between ACK_TIMEOUT and (ACK_TIMEOUT *
     * ACK_RANDOM_FACTOR) */
    uint32_t timeout = COAP_ACK_TIMEOUT * US_PER_SEC;
    uint32_t deadline = deadline_from_interval(timeout);
    /* add 1 for initial transmit */
    unsigned tries_left = COAP_MAX_RETRANSMIT + 1;
    while (tries_left) {
        if (res == -EAGAIN) {
            res = sock_udp_send(sock, buf, pdu_len, NULL);
            if (res <= 0) {
                DEBUG("nanocoap: error sending coap request, %d\n", (int)res);
                break;
            }
        }

        res = sock_udp_recv(sock, buf, len, deadline_left(deadline), NULL);
        if (res <= 0) {
            if (res == -ETIMEDOUT) {
                DEBUG("nanocoap: timeout\n");

                tries_left--;
                if (!tries_left) {
                    DEBUG("nanocoap: maximum retries reached\n");
                    break;
                }
                else {
                    timeout *= 2;
                    deadline = deadline_from_interval(timeout);
                    res = -EAGAIN;
                    continue;
                }
            }
            DEBUG("nanocoap: error receiving coap response, %d\n", (int)res);
            break;
        }
        else {
            if (coap_parse(pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("nanocoap: error parsing packet\n");
                res = -EBADMSG;
            }
            else if (coap_get_id(pkt) != id) {
                res = -EBADMSG;
                continue;
            }

            break;
        }
    }


    return res;
}

uint8_t *_fmt_hdr(uint8_t *buf, coap_pkt_t *pkt, uint16_t id, const char *path)
{
    uint8_t *pktpos = buf;
    pkt->hdr = (coap_hdr_t*)buf;
    pktpos += coap_build_hdr(pkt->hdr, COAP_TYPE_CON, NULL, 0,
                             COAP_METHOD_GET, id);
    pktpos += coap_opt_put_uri_path(pktpos, 0, path);
    pkt->payload = pktpos;
    pkt->payload_len = 0;
    return pktpos;
}

static int _fetch_block(coap_pkt_t *pkt, sock_udp_t *sock,
                        coap_block1_t *block, size_t len)
{
    uint8_t *pktpos = pkt->payload;
    pktpos += coap_opt_put_block2_control(pktpos, COAP_OPT_URI_PATH, block);
    pkt->payload = pktpos;
    pkt->payload_len = 0;

    int res = _nanocoap_req_raw(pkt, sock, len);
    if (res < 0) {
        return res;
    }

    res = coap_get_code(pkt);
    DEBUG("code=%i\n", res);
    if (res != 205) {
        return -res;
    }

    return 0;
}

ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path,
                     uint8_t *buf, size_t len)
{
    ssize_t res;
    coap_pkt_t pkt;

    _fmt_hdr(buf, &pkt, 0x01, path);

    res = nanocoap_request(&pkt, NULL, remote, len);
    if (res < 0) {
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        if (res != 205) {
            res = -res;
        }
        else {
            if (pkt.payload_len) {
                memmove(buf, pkt.payload, pkt.payload_len);
            }
            res = pkt.payload_len;
        }
    }
    return res;
}

int nanocoap_get_blockwise(sock_udp_ep_t *remote, const char *path,
                           uint8_t *buf, size_t len, coap_blocksize_t size,
                           coap_blockwise_cb_t callback, void *arg)
{
    sock_udp_t sock;

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    ssize_t res = sock_udp_create(&sock, &local, remote, 0);
    coap_pkt_t pkt;
    coap_block1_t block = {
        .szx = size,
    };

    if (!remote->port) {
        remote->port = COAP_PORT;
    }

    int more = 1;
    res = -1;
    uint16_t id = 1;
    while (more == 1) {
        DEBUG("fetching block %u\n", (unsigned)block.blknum);
        _fmt_hdr(buf, &pkt, id++, path);
        res = _fetch_block(&pkt, &sock, &block, len);
        DEBUG("res=%i\n", res);

        if (!res) {
            coap_block1_t block2;
            coap_get_block2(&pkt, &block2);
            more = block2.more;

            if (callback(&pkt, arg)) {
                DEBUG("callback res != 0, aborting.\n");
                res = -1;
                goto out;
            }
        }
        else {
            DEBUG("error fetching block\n");
            res = -1;
            goto out;
        }

        block.blknum++;
    }

out:
    sock_udp_close(&sock);
    return res;
}

int nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize)
{
    sock_udp_t sock;
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
