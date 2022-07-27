/*
 * Copyright (C) 2020 HAW Hamburg
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gcoap_forward_proxy    Gcoap Forward Proxy
 * @ingroup     net_gcoap
 * @brief       Forward proxy implementation for Gcoap
 * @note Does not support CoAPS yet.
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
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
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
 * @brief Maximum length of the URI to an upstream proxy for the forward proxy
 *
 * @note requires module `gcoap_forward_proxy_upstream`
 */
#ifndef CONFIG_GCOAP_FORWARD_PROXY_UPSTREAM_URI_MAX
#define CONFIG_GCOAP_FORWARD_PROXY_UPSTREAM_URI_MAX (64U)
#endif

/**
 * @brief Registers a listener for forward proxy operation
 */
void gcoap_forward_proxy_init(void);

/**
 * @brief Handles proxied requests
 *
 * @param[in]  pkt           Packet to parse
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

/**
 * @brief   Sets an upstream proxy for this forward proxy
 *
 * @param[in] proxy_uri The URI for the upstream proxy for this forward proxy.
 *                      May be NULL or empty to unset the upstream proxy.
 *
 * @return  >0 on success (the length of @p proxy_uri)
 * @return  0 on success (when @p proxy_uri was 0)
 * @return  -ENOTSUP, when module `gcoap_forward_proxy_upstream` is not used
 * @return  -EINVAL, when @p proxy_uri is longer than CONFIG_GCOAP_FORWARD_PROXY_UPSTREAM_URI_MAX.
 */
int gcoap_forward_proxy_upstream_set(const char *proxy_uri);

/**
 * @brief   Check if an upstream proxy for this forward proxy is set
 *
 * @retval  true    An upstream proxy for this forward proxy is set
 * @retval  false   No upstream proxy is set or module `gcoap_forward_proxy_upstream` is not used.
 */
bool gcoap_forward_proxy_upstream_is_set(void);

/**
 * @brief   Get the current upstream proxy for this forward proxy
 *
 * @param[out] proxy_uri        The URI for the upstream proxy for this forward proxy.
 * @param[in]  proxy_uri_len    The maximum length for @p proxy_uri
 *
 * @return  >0 on success (the length of @p proxy_uri on return).
 * @return  0 on success (if no current upstream proxy is set).
 * @return  -ENOBUFS if the length of @p proxy_uri on return would be greater than @p proxy_uri_len.
 */
ssize_t gcoap_forward_proxy_upstream_get(char *proxy_uri, size_t proxy_uri_len);

#ifdef __cplusplus
}
#endif

#endif /* NET_GCOAP_FORWARD_PROXY_H */
/**
 * @}
 */
