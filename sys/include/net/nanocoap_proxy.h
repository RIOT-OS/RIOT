/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef NET_NANOCOAP_PROXY_H
#define NET_NANOCOAP_PROXY_H

/**
 * @defgroup    net_nanocoap_proxy  nanocoap proxy implementation
 * @ingroup     net
 * @brief       A trivial reverse proxy on top of nanocoap
 *
 * @{
 *
 * @file
 * @brief       nanocoap reverse proxy
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include <stdint.h>
#include <unistd.h>

#include "bitfield.h"
#include "event.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS
/**
 * @brief   Number of requests to handle in parallel
 *
 * @note    The forwards will currently be forwarded one at a time, but a higher
 *          value will allow accepting more requests that come in roughly at the
 *          same time.
 */
#  define CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS 1
#endif

/**
 * @brief   Canonical name of the NanoCoAP reverse proxy context
 */
typedef struct nanocoap_rproxy_ctx nanocoap_rproxy_ctx_t;

/**
 * @brief   Context for a forwarded request/reply
 */
typedef struct {
    event_t ev; /**< Event used to execute the forwarding */
    nanocoap_rproxy_ctx_t *proxy; /**< The proxy this belongs to */
    /**
     * @brief   The name of the endpoint to forward to
     *
     * @note    This is needed to translate e.g. Location-Path options in the
     *          reply into the corresponding proxy-path
     *
     * @details `54 == strlen("[0000:0000:0000:0000:0000:ffff:192.168.100.228]:65535") + 1`
     */
    char ep[54];
    nanocoap_sock_t client; /**< Client socket to use to send this request */
    nanocoap_server_response_ctx_t response_ctx; /**< response ctx to use to reply */
    coap_pkt_t req; /**< Request for forward */
    uint8_t buf[256]; /**< Buffer used to store the request to forward */
} nanocoap_rproxy_forward_ctx_t;

/**
 * @brief   Contents of @ref nanocoap_rproxy_ctx_t
 */
struct nanocoap_rproxy_ctx {
    event_queue_t *evq; /**< Event queue that handles the forwards */
    /**
     * @brief   Request forwarding contexts
     */
    nanocoap_rproxy_forward_ctx_t forwards[CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS];
    /**
     * @brief   Bookkeeping for @ref nanocoap_rproxy_ctx_t::forwards
     */
    BITFIELD(forwards_used, CONFIG_NANOCOAP_RPROXY_PARALLEL_FORWARDS);
    /**
     * @brief   URI-Scheme to use when forwarding
     */
    char scheme[16];
    /**
     * @brief   Target endpoint (in the format that @ref nanocoap_sock_url_connect() can parse)
     *
     * @note    Set this to `NULL` to parse the endpoint from request URI instead
     */
    const char *target_ep;
};

/**
 * @brief   nanocoap resource handler implementing the proxy
 *
 * Usage:
 *
 * ```C
 * static nanocoap_rproxy_ctx_t _udp_proxy = {
 *     .evq = EVENT_PRIO_MEDIUM,
 *     .scheme = "coap://"
 * };
 *
 * NANOCOAP_RESOURCE(udp_proxy) {
 *     .path = "/udp",
 *     .methods = COAP_GET | COAP_PUT | COAP_POST | COAP_DELETE | COAP_MATCH_SUBTREE,
 *     .handler= nanocoap_rproxy_handler,
 *     .context = &_udp_proxy,
 * };
 * ```
 *
 * @param[in]   pkt     Received packet to forward
 * @param[out]  buf     Buffer to write the response to
 * @param[in]   len     Length of @p buf in bytes
 * @param[in]   ctx     Request ctx
 */
ssize_t nanocoap_rproxy_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                coap_request_ctx_t *ctx);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NET_NANOCOAP_PROXY_H */
