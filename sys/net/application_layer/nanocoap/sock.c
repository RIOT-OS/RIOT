/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
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

#include "net/nanocoap.h"
#include "net/sock/udp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

ssize_t nanocoap_request(coap_pkt_t *pkt, sock_udp_ep_t *local, sock_udp_ep_t *remote, size_t len)
{
    ssize_t res;
    size_t pdu_len = (pkt->payload - (uint8_t *)pkt->hdr) + pkt->payload_len;
    uint8_t *buf = (uint8_t*)pkt->hdr;
    sock_udp_t sock;

    if (!remote->port) {
        remote->port = COAP_PORT;
    }

    res = sock_udp_create(&sock, local, remote, 0);
    if (res < 0) {
        return res;
    }

    /* TODO: timeout random between between ACK_TIMEOUT and (ACK_TIMEOUT *
     * ACK_RANDOM_FACTOR) */
    uint32_t timeout = COAP_ACK_TIMEOUT * (1000000U);
    int tries = 0;
    while (tries++ < COAP_MAX_RETRANSMIT) {
        if (!tries) {
            DEBUG("nanocoap: maximum retries reached.\n");
            res = -ETIMEDOUT;
            goto out;
        }

        res = sock_udp_send(&sock, buf, pdu_len, NULL);
        if (res <= 0) {
            DEBUG("nanocoap: error sending coap request\n");
            goto out;
        }

        res = sock_udp_recv(&sock, buf, len, timeout, NULL);
        if (res <= 0) {
            if (res == -ETIMEDOUT) {
                DEBUG("nanocoap: timeout\n");

                timeout *= 2;
                continue;
            }
            DEBUG("nanocoap: error receiving coap request\n");
            break;
        }
        else {
            if (coap_parse(pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("nanocoap: error parsing packet\n");
                res = -EBADMSG;
            }
            break;
        }
    }

out:
    sock_udp_close(&sock);

    return res;
}

ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path, uint8_t *buf, size_t len)
{
    ssize_t res;
    coap_pkt_t pkt;
    uint8_t *pktpos = buf;

    pkt.hdr = (coap_hdr_t*)buf;
    pktpos += coap_build_hdr(pkt.hdr, COAP_REQ, NULL, 0, COAP_METHOD_GET, 1);
    pktpos += coap_put_option_uri(pktpos, 0, path, COAP_OPT_URI_PATH);
    pkt.payload = pktpos;
    pkt.payload_len = 0;

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

int nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize)
{
    sock_udp_t sock;
    sock_udp_ep_t remote;

    if (!local->port) {
        local->port = COAP_PORT;
    }

    ssize_t res = sock_udp_create(&sock, local, NULL, 0);
    if (res == -1) {
        return -1;
    }

    while (1) {
        res = sock_udp_recv(&sock, buf, bufsize, -1, &remote);
        if (res == -1) {
            DEBUG("error receiving UDP packet\n");
            return -1;
        }
        else {
            coap_pkt_t pkt;
            if (coap_parse(&pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("error parsing packet\n");
                continue;
            }
            if ((res = coap_handle_req(&pkt, buf, bufsize)) > 0) {
                res = sock_udp_send(&sock, buf, res, &remote);
            }
        }
    }

    return 0;
}
