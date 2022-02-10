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
#include "net/sock/util.h"
#include "net/sock/udp.h"
#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int nanocoap_sock_connect(nanocoap_sock_t *sock, sock_udp_ep_t *local, sock_udp_ep_t *remote)
{
    if (!remote->port) {
        remote->port = COAP_PORT;
    }

    return sock_udp_create(sock, local, remote, 0);
}

ssize_t nanocoap_sock_request(nanocoap_sock_t *sock, coap_pkt_t *pkt, size_t len)
{
    ssize_t res = -EAGAIN;
    size_t pdu_len = (pkt->payload - (uint8_t *)pkt->hdr) + pkt->payload_len;
    uint8_t *buf = (uint8_t*)pkt->hdr;
    uint32_t id = coap_get_id(pkt);

    /* TODO: timeout random between between ACK_TIMEOUT and (ACK_TIMEOUT *
     * ACK_RANDOM_FACTOR) */
    uint32_t timeout = CONFIG_COAP_ACK_TIMEOUT_MS * US_PER_MS;

    /* add 1 for initial transmit */
    unsigned tries_left = CONFIG_COAP_MAX_RETRANSMIT + 1;

    /* check if we expect a reply */
    bool confirmable = coap_get_type(pkt) == COAP_TYPE_CON;

    while (tries_left) {
        if (res == -EAGAIN) {
            res = sock_udp_send(sock, buf, pdu_len, NULL);
            if (res <= 0) {
                DEBUG("nanocoap: error sending coap request, %d\n", (int)res);
                break;
            }
        }

        /* return if no response is expected */
        if (!confirmable) {
            pkt->payload_len = 0;
            break;
        }

        res = sock_udp_recv(sock, buf, len, timeout, NULL);
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

ssize_t nanocoap_sock_get(nanocoap_sock_t *sock, const char *path, void *buf, size_t len)
{
    ssize_t res;
    coap_pkt_t pkt;
    uint8_t *pktpos = buf;

    pkt.hdr = buf;
    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, 1);
    pktpos += coap_opt_put_uri_path(pktpos, 0, path);
    pkt.payload = pktpos;
    pkt.payload_len = 0;

    res = nanocoap_sock_request(sock, &pkt, len);
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

static int _fetch_block(coap_pkt_t *pkt, uint8_t *buf, nanocoap_sock_t *sock,
                        const char *path, coap_blksize_t blksize, size_t num)
{
    uint8_t *pktpos = buf;
    uint16_t lastonum = 0;

    pkt->hdr = (coap_hdr_t *)buf;

    pktpos += coap_build_hdr(pkt->hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET,
                             num);
    pktpos += coap_opt_put_uri_pathquery(pktpos, &lastonum, path);
    pktpos += coap_opt_put_uint(pktpos, lastonum, COAP_OPT_BLOCK2,
                                (num << 4) | blksize);

    pkt->payload = pktpos;
    pkt->payload_len = 0;

    int res = nanocoap_sock_request(sock, pkt, NANOCOAP_BLOCKWISE_BUF(blksize));
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

int nanocoap_sock_get_blockwise(nanocoap_sock_t *sock, const char *path,
                           coap_blksize_t blksize, void *buf,
                           coap_blockwise_cb_t callback, void *arg)
{
    coap_pkt_t pkt;
    coap_block1_t block2;

    size_t num = 0;
    do {
        DEBUG("fetching block %u\n", (unsigned)num);
        int res = _fetch_block(&pkt, buf, sock, path, blksize, num);
        if (res) {
            DEBUG("error fetching block: %d\n", res);
            return -1;
        }

        /* no block option in response - directly use paylaod */
        if (!coap_get_block2(&pkt, &block2)) {
            block2.more = 0;
            block2.offset = 0;
        }

        if (coap_has_unprocessed_critical_options(&pkt)) {
            return -ENOTSUP;
        }

        res = callback(arg, block2.offset, pkt.payload, pkt.payload_len,
                       block2.more);
        if (res) {
            DEBUG("callback %d != 0, aborting.\n", res);
            return res;
        }

        num += 1;
    } while (block2.more == 1);

    return 0;
}

int nanocoap_get_blockwise_url(const char *url,
                               coap_blksize_t blksize, void *buf,
                               coap_blockwise_cb_t callback, void *arg)
{
    char hostport[CONFIG_SOCK_HOSTPORT_MAXLEN];
    char urlpath[CONFIG_SOCK_URLPATH_MAXLEN];
    sock_udp_ep_t remote;
    nanocoap_sock_t sock;
    int res;

    if (strncmp(url, "coap://", 7)) {
        DEBUG("nanocoap: URL doesn't start with \"coap://\"\n");
        return -EINVAL;
    }

    if (sock_urlsplit(url, hostport, urlpath) < 0) {
        DEBUG("nanocoap: invalid URL\n");
        return -EINVAL;
    }

    if (sock_udp_str2ep(&remote, hostport) < 0) {
        DEBUG("nanocoap: invalid URL\n");
        return -EINVAL;
    }

    res = nanocoap_sock_connect(&sock, NULL, &remote);
    if (res) {
        return res;
    }

    res = nanocoap_sock_get_blockwise(&sock, urlpath, blksize, buf, callback, arg);
    nanocoap_sock_close(&sock);

    return res;
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
