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
#include "timex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @name    Internally used CoAP packet types
 * @{
 */
#define COAP_REQ                (0)
#define COAP_RESP               (2)
#define COAP_RST                (3)
/** @} */

ssize_t coap_handle_req(coap_pkt_t *pkt, uint8_t *resp_buf, unsigned resp_buf_len)
{
    if (coap_get_code_class(pkt) != COAP_REQ) {
        DEBUG("coap_handle_req(): not a request.\n");
        return -EBADMSG;
    }

    if (pkt->hdr->code == 0) {
        return coap_build_reply(pkt, COAP_CODE_EMPTY, resp_buf, resp_buf_len, 0);
    }
    return coap_tree_handler(pkt, resp_buf, resp_buf_len, coap_resources,
                             coap_resources_numof);
}

ssize_t coap_tree_handler(coap_pkt_t *pkt, uint8_t *resp_buf,
                          unsigned resp_buf_len,
                          const coap_resource_t *resources,
                          size_t resources_numof)
{
    coap_method_flags_t method_flag = coap_method2flag(coap_get_code_detail(pkt));

    uint8_t uri[NANOCOAP_URI_MAX];
    if (coap_get_uri_path(pkt, uri) <= 0) {
        return -EBADMSG;
    }
    DEBUG("nanocoap: URI path: \"%s\"\n", uri);

    for (unsigned i = 0; i < resources_numof; i++) {
        const coap_resource_t *resource = &resources[i];
        if (!(resource->methods & method_flag)) {
            continue;
        }

        int res = coap_match_path(resource, uri);
        if (res > 0) {
            continue;
        }
        else if (res < 0) {
            break;
        }
        else {
            return resource->handler(pkt, resp_buf, resp_buf_len, resource->context);
        }
    }

    return coap_build_reply(pkt, COAP_CODE_404, resp_buf, resp_buf_len, 0);
}

ssize_t coap_reply_simple(coap_pkt_t *pkt,
                          unsigned code,
                          uint8_t *buf, size_t len,
                          unsigned ct,
                          const uint8_t *payload, uint8_t payload_len)
{
    uint8_t *payload_start = buf + coap_get_total_hdr_len(pkt);
    uint8_t *bufpos = payload_start;

    if (payload_len) {
        bufpos += coap_put_option_ct(bufpos, 0, ct);
        *bufpos++ = 0xff;
    }

    ssize_t res = coap_build_reply(pkt, code, buf, len,
                                   bufpos - payload_start + payload_len);

    if (payload_len && (res > 0)) {
        assert(payload);
        memcpy(bufpos, payload, payload_len);
    }

    return res;
}

ssize_t coap_build_reply(coap_pkt_t *pkt, unsigned code,
                         uint8_t *rbuf, unsigned rlen, unsigned payload_len)
{
    unsigned tkl = coap_get_token_len(pkt);
    unsigned len = sizeof(coap_hdr_t) + tkl;

    if ((len + payload_len) > rlen) {
        return -ENOSPC;
    }

    /* if code is COAP_CODE_EMPTY (zero), assume Reset (RST) type */
    unsigned type = COAP_TYPE_RST;
    if (code) {
        if (coap_get_type(pkt) == COAP_TYPE_CON) {
            type = COAP_TYPE_ACK;
        }
        else {
            type = COAP_TYPE_NON;
        }
    }

    coap_build_hdr((coap_hdr_t *)rbuf, type, pkt->token, tkl, code,
                   ntohs(pkt->hdr->id));
    coap_hdr_set_type((coap_hdr_t *)rbuf, type);
    coap_hdr_set_code((coap_hdr_t *)rbuf, code);

    len += payload_len;

    return len;
}

ssize_t coap_block2_build_reply(coap_pkt_t *pkt, unsigned code,
                                uint8_t *rbuf, unsigned rlen, unsigned payload_len,
                                coap_block_slicer_t *slicer)
{
    /* Check if the generated data filled the requested block */
    if (slicer->cur < slicer->start) {
        return coap_build_reply(pkt, COAP_CODE_BAD_OPTION, rbuf, rlen, 0);
    }
    coap_block2_finish(slicer);
    return coap_build_reply(pkt, code, rbuf, rlen, payload_len);
}

size_t coap_blockwise_put_char(coap_block_slicer_t *slicer, uint8_t *bufpos, char c)
{
    /* Only copy the char if it is within the window */
    if ((slicer->start <= slicer->cur) && (slicer->cur < slicer->end)) {
        *bufpos = c;
        slicer->cur++;
        return 1;
    }
    slicer->cur++;
    return 0;
}

ssize_t coap_well_known_core_default_handler(coap_pkt_t *pkt, uint8_t *buf, \
                                             size_t len, void *context)
{
    (void)context;
    coap_block_slicer_t slicer;
    coap_block2_init(pkt, &slicer);
    uint8_t *payload = buf + coap_get_total_hdr_len(pkt);
    uint8_t *bufpos = payload;
    bufpos += coap_put_option_ct(bufpos, 0, COAP_FORMAT_LINK);
    bufpos += coap_opt_put_block2(bufpos, COAP_OPT_CONTENT_FORMAT, &slicer, 1);

    *bufpos++ = 0xff;

    for (unsigned i = 0; i < coap_resources_numof; i++) {
        if (i) {
            bufpos += coap_blockwise_put_char(&slicer, bufpos, ',');
        }
        bufpos += coap_blockwise_put_char(&slicer, bufpos, '<');
        unsigned url_len = strlen(coap_resources[i].path);
        bufpos += coap_blockwise_put_bytes(&slicer, bufpos,
                (uint8_t*)coap_resources[i].path, url_len);
        bufpos += coap_blockwise_put_char(&slicer, bufpos, '>');
    }

    unsigned payload_len = bufpos - payload;
    return coap_block2_build_reply(pkt, COAP_CODE_205, buf, len, payload_len,
                                   &slicer);
}

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
    uint32_t timeout = COAP_ACK_TIMEOUT * US_PER_SEC;
    unsigned tries_left = COAP_MAX_RETRANSMIT + 1;  /* add 1 for initial transmit */
    while (tries_left) {

        res = sock_udp_send(&sock, buf, pdu_len, NULL);
        if (res <= 0) {
            DEBUG("nanocoap: error sending coap request, %d\n", (int)res);
            break;
        }

        res = sock_udp_recv(&sock, buf, len, timeout, NULL);
        if (res <= 0) {
            if (res == -ETIMEDOUT) {
                DEBUG("nanocoap: timeout\n");

                timeout *= 2;
                tries_left--;
                if (!tries_left) {
                    DEBUG("nanocoap: maximum retries reached\n");
                }
                continue;
            }
            DEBUG("nanocoap: error receiving coap response, %d\n", (int)res);
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

    sock_udp_close(&sock);

    return res;
}

ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path, uint8_t *buf, size_t len)
{
    ssize_t res;
    coap_pkt_t pkt;
    uint8_t *pktpos = buf;

    pkt.hdr = (coap_hdr_t*)buf;
    pktpos += coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, 1);
    pktpos += coap_opt_put_uri_path(pktpos, 0, path);
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
