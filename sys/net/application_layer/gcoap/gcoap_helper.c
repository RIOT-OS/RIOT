/*
 * Copyright (c) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gcoap_helper
 * @{
 *
 * @file
 * @brief       Helper functions for GCoAP
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "net/gcoap_helper.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define COAP_REQ_CONFIRMABLE    (1 << 0)

struct coap_request_ctx {
    mutex_t lock;
    const void *payload;
    const size_t payload_len;
    void *dst;
    size_t dst_len;
    const size_t dst_len_max;
    const char *path;
    int code;
    uint8_t method;
    uint8_t flags;
};

/*
 * Response callback.
 */
static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                          const sock_udp_ep_t *remote)
{
    (void)remote;       /* not interested in the source currently */

    struct coap_request_ctx *ctx = memo->context;

    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        DEBUG("gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
        ctx->code = -ETIME;
        goto out;
    }
    else if (memo->state == GCOAP_MEMO_ERR) {
        DEBUG("gcoap: error in response\n");
        ctx->code = -EBADMSG;
        goto out;
    }

    coap_block1_t block;
    if (coap_get_block2(pdu, &block) && block.blknum == 0) {
        DEBUG("--- blockwise start ---\n");
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    DEBUG("gcoap: response %s, code %1u.%02u\n", class_str,
                                                 coap_get_code_class(pdu),
                                                 coap_get_code_detail(pdu));
    ctx->code = coap_get_code(pdu);

    if (pdu->payload_len) {
        if (ctx->dst_len + pdu->payload_len > ctx->dst_len_max) {
            DEBUG("gcoap: not enough memory in destination buffer\n");
            ctx->code = -ENOMEM;
            goto out;
        }
        memcpy((uint8_t *)ctx->dst + ctx->dst_len, pdu->payload, pdu->payload_len);
        ctx->dst_len += pdu->payload_len;
    }

    /* ask for next block if present */
    if (coap_get_block2(pdu, &block)) {
        if (block.more) {
            unsigned msg_type = coap_get_type(pdu);

            gcoap_req_init(pdu, (uint8_t *)pdu->hdr, CONFIG_GCOAP_PDU_BUF_SIZE,
                           ctx->method, ctx->path);

            if (msg_type == COAP_TYPE_ACK) {
                coap_hdr_set_type(pdu->hdr, COAP_TYPE_CON);
            }
            block.blknum++;
            coap_opt_add_block2_control(pdu, &block);

            int len = coap_opt_finish(pdu, COAP_OPT_FINISH_NONE);
            gcoap_req_send((uint8_t *)pdu->hdr, len, remote,
                           _resp_handler, memo->context);
        }
        else {
            DEBUG("--- blockwise complete ---\n");
        }
    }

out:
    mutex_unlock(&ctx->lock);
}

static int _fill_remote(const char *host, sock_udp_ep_t *remote)
{
    int res;
    gnrc_netif_t *netif;

    /* parse hostname */
    res = gnrc_netif_parse_hostname(host, (ipv6_addr_t *)&remote->addr, &netif);
    if (res < 0) {
        return res;
    }

    /* fill in CoAP server information */
    remote->family = AF_INET6;
    remote->netif  = netif ? netif->pid : SOCK_ADDR_ANY_NETIF;
    remote->port   = COAP_PORT;

    return 0;
}

static void coap_request(const sock_udp_ep_t *remote, struct coap_request_ctx *ctx)
{
    size_t len;
    coap_pkt_t pdu;
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];

    gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, ctx->method, ctx->path);

    /* set confirmable / non-confirmable */
    if (ctx->flags & COAP_REQ_CONFIRMABLE) {
        coap_hdr_set_type(pdu.hdr, COAP_TYPE_CON);
    } else {
        coap_hdr_set_type(pdu.hdr, COAP_TYPE_NON);
    }

    /* options done */
    if (ctx->payload_len == 0) {
        len = coap_opt_finish(&pdu, COAP_OPT_FINISH_NONE);
    } else {
        len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);

        if (pdu.payload_len < ctx->payload_len) {
            ctx->code = -ENOMEM;
            return;
        }

        memcpy(pdu.payload, ctx->payload, ctx->payload_len);
        pdu.payload_len -= ctx->payload_len;
        len += ctx->payload_len;
    }

    /* send COAP request */
    gcoap_req_send(buf, len, remote, _resp_handler, ctx);

    /* wait for response */
    mutex_lock(&ctx->lock);
}

int coap_get(const char *host, const char *path, bool confirmable,
             void *response, size_t *response_len)
{
    sock_udp_ep_t remote;
    struct coap_request_ctx ctx = {
        .lock    = MUTEX_INIT_LOCKED,
        .dst     = response,
        .dst_len_max = *response_len,
        .path    = path,
        .method  = COAP_METHOD_GET,
        .flags   = confirmable ? COAP_REQ_CONFIRMABLE : 0,
    };

    ctx.code = _fill_remote(host, &remote);
    if (ctx.code) {
        return ctx.code;
    }

    coap_request(&remote, &ctx);

    *response_len = ctx.dst_len;
    return ctx.code;
}

int coap_post(const char *host, const char *path, bool confirmable,
              const void *payload, size_t payload_len,
              void *response, size_t *response_len)
{
    sock_udp_ep_t remote;
    struct coap_request_ctx ctx = {
        .lock    = MUTEX_INIT_LOCKED,
        .payload = payload,
        .payload_len = payload_len,
        .dst     = response,
        .dst_len_max = *response_len,
        .path    = path,
        .method  = COAP_METHOD_POST,
        .flags   = confirmable ? COAP_REQ_CONFIRMABLE : 0,
    };

    ctx.code = _fill_remote(host, &remote);
    if (ctx.code) {
        return ctx.code;
    }

    coap_request(&remote, &ctx);

    *response_len = ctx.dst_len;
    return ctx.code;
}
