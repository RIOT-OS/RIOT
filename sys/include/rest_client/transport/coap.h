/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_transport_coap
 * @{
 *
 * @file
 * @brief       REST client transport module for CoAP
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#ifndef REST_CLIENT_TRANSPORT_COAP_H
#define REST_CLIENT_TRANSPORT_COAP_H

#include "net/gcoap.h"

#include "rest_client.h"

/* Retain request path to re-request if response includes block. User must not
 * start a new request (with a new path) until any blockwise transfer
 * completes or times out. */
#define _LAST_REQ_PATH_MAX (1024)
#define PROXY_URI_LEN 512

#define COAP_OPT_COAP_CONTENT_FORMAT 55
#define COAP_OPT_HTTP_CONTENT_FORMAT 59
#define COAP_OPT_PROXY_HTTP_STATUS   67
#define COAP_OPT_AUTHORIZATION       249

int _coap_method_mapper(rest_client_method_t method);

int _coap_format_mapper(rest_client_content_type_t format);

void _coap_resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                               const sock_udp_ep_t *remote);

rest_client_result_t  rest_client_transport_coap_send(
        rest_client_t *rest_client, rest_client_response_listener_t *listener,
        rest_client_method_t method, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len);

rest_client_result_t rest_client_transport_coap_init(rest_client_t *rest_client);

#endif /* REST_CLIENT_TRANSPORT_COAP_H */
