/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gcoap_forward_proxy    Gcoap Forward Proxy
 * @ingroup     net_gcoap
 * @brief       Forward proxy implementation for Gcoap
 *
 * @see <a href="https://tools.ietf.org/html/rfc7252#section-5.7.2">
 *          RFC 7252
 *      </a>
 *
 * @{
 *
 * @file
 * @brief       Definitions for the Gcoap forward proxy
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */


#ifndef NET_GCOAP_FORWARD_PROXY_H
#define NET_GCOAP_FORWARD_PROXY_H

#include <stdbool.h>
#include <errno.h>

#include "net/nanocoap.h"
#include "net/gcoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Registers a listener for forward proxy operation
 */
void gcoap_forward_proxy_init(void);

/**
 * @brief Handles proxied requests
 *
 * @param[in]  pkt           The incoming request
 * @param[in]  client        Endpoint of the client
 *
 * @return    0              if parsing was successful
 * @return    -ENOTSUP       if the forward proxy is not compiled in
 * @return    -ENOENT        if @p pkt does not contain a Proxy-Uri option
 * @return    -EINVAL        if Proxy-Uri is malformed
 */
int gcoap_forward_proxy_request_process(coap_pkt_t *pkt,
                                        sock_udp_ep_t *client);

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

/**
 * @brief   Sends a buffer containing a CoAP message to the @p remote endpoint
 *
 * @param[in] buf    Buffer that contains the CoAP message to be sent
 * @param[in] len    Length of @p buf
 * @param[in] remote Remote endpoint to send the message to
 *
 * @note see sock_udp_send() for all return valus.
 *
 * @return  length of the packet
 * @return  < 0 on error
 */
ssize_t gcoap_forward_proxy_dispatch(const uint8_t *buf,
                                     size_t len, sock_udp_ep_t *remote);

#ifdef __cplusplus
}
#endif

#endif /* NET_GCOAP_FORWARD_PROXY_H */
/**
 * @}
 */
