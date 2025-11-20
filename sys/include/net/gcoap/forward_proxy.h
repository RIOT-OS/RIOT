/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @defgroup    net_gcoap_forward_proxy    GCoAP Forward Proxy
 * @ingroup     net_gcoap
 * @brief       Forward proxy implementation for GCoAP
 * @note Does not support CoAPS yet.
 * @see <a href="https://tools.ietf.org/html/rfc7252#section-5.7.2">
 *          RFC 7252
 *      </a>
 *
 * @{
 *
 * @file
 * @brief       Definitions for the GCoAP forward proxy
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#include <stdbool.h>
#include <errno.h>

#include "net/coap.h"
#include "net/nanocoap.h"
#include "net/gcoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup net_gcoap_conf
 * @{
 */
/**
 * @brief Timeout in milliseconds for the forward proxy to send an empty ACK without response
 */
#ifndef CONFIG_GCOAP_FORWARD_PROXY_EMPTY_ACK_MS
#define CONFIG_GCOAP_FORWARD_PROXY_EMPTY_ACK_MS     ((CONFIG_COAP_ACK_TIMEOUT_MS / 4) * 3)
#endif
/** @} */

/**
 * @brief Registers a listener for forward proxy operation
 */
void gcoap_forward_proxy_init(void);

/**
 * @brief Handles proxied requests
 *
 * @param[in]  pkt           Packet to parse
 * @param[in]  client        Endpoint of the client
 * @param[in]  local         Local endpoint
 *
 * @return    0              if parsing was successful
 * @return    -ENOTSUP       if the forward proxy is not compiled in
 * @return    -ENOENT        if @p pkt does not contain a Proxy-Uri option
 * @return    -EINVAL        if Proxy-Uri is malformed
 */
int gcoap_forward_proxy_request_process(coap_pkt_t *pkt,
                                        const sock_udp_ep_t *client, const sock_udp_ep_t *local);

/**
 * @brief  Finds the memo for an outstanding request within the
 *         _coap_state.open_reqs array. Matches on remote endpoint and
 *         token.
 *
 * @param[out] memo_ptr   Registered request memo, or NULL if not found
 * @param[in]  src_pdu    PDU for token to match
 * @param[in]  remote     Remote endpoint to match
 */
void gcoap_forward_proxy_find_req_memo(gcoap_request_memo_t **memo_ptr,
                                       coap_pkt_t *src_pdu,
                                       const sock_udp_ep_t *remote);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
