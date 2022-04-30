/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @defgroup    sys_rest_client_transport_coap REST client CoAP transport
 * @ingroup     sys_rest_client
 * @brief       REST client communicating over CoAP
 *
 * @{
 * @file
 * @brief       REST client communicating over CoAP
 * @note        Code was derived from examples/gcoap
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include "kernel_defines.h"

#include <assert.h>
#include <string.h>

#include "rest_client.h"
#include "rest_client/util.h"
#include "rest_client/transport/coap.h"

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"

#define COAP_OPT_TARGET_CONTENT_TYPE 55
#define COAP_OPT_TARGET_ACCEPT       59
#define COAP_OPT_PROXY_HTTP_STATUS   67

#if IS_USED(MODULE_GCOAP_DTLS)
#include "net/credman.h"
#include "net/dsm.h"
#include "tinydtls_keys.h"

/* Example credential tag for credman. Tag together with the credential type needs to be unique. */
#define GCOAP_DTLS_CREDENTIAL_TAG 10

static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = GCOAP_DTLS_CREDENTIAL_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif /* IS_USED(MODULE_GCOAP_DTLS) */

void _coap_resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t *pdu,
                               const sock_udp_ep_t *remote)
{
    (void)remote;       /* not interested in the source currently */

    rest_client_coap_context_t *context = (rest_client_coap_context_t *)memo->context;

    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        context->listener.status_code = -REST_CLIENT_STATUS_TIMEOUT;
        context->listener.listener(&context->listener);
        return;
    }
    else if (memo->state != GCOAP_MEMO_RESP) {
        context->listener.status_code = -REST_CLIENT_STATUS_ERROR;
        context->listener.listener(&context->listener);
        return;
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    DEBUG("gcoap: response %s, code %1u.%02u", class_str,
                                                coap_get_code_class(pdu),
                                                coap_get_code_detail(pdu));

    if (pdu->payload_len) {
        unsigned content_type = coap_get_content_type(pdu);
        if (content_type == COAP_FORMAT_TEXT ||
            content_type == COAP_FORMAT_LINK ||
            coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE ||
            coap_get_code_class(pdu) == COAP_CLASS_SERVER_FAILURE)
        {
            /* Expecting diagnostic payload in failure cases */
            DEBUG(", %u bytes\n%.*s\n", pdu->payload_len, pdu->payload_len, (char *)pdu->payload);
        }
        else {
            DEBUG(", %u bytes\n", pdu->payload_len);
        }
    }
    else {
        DEBUG(", empty payload\n");
    }

    if (memo->state == GCOAP_MEMO_RESP_TRUNC) {
        context->listener.status_code = -REST_CLIENT_STATUS_TRUNCATED;
        context->listener.listener(&context->listener);
    }
    else {
        uint32_t http_status;
        coap_opt_get_uint(pdu, COAP_OPT_PROXY_HTTP_STATUS, &http_status);

        if (http_status <= 0) {
            /* use CoAP status code instead */
            http_status = coap_get_code(pdu);
        }

        context->listener.status_code = http_status;
        context->listener.body_buf = pdu->payload;
        context->listener.body_len = pdu->payload_len;
        context->listener.listener(&context->listener);
    }
}

rest_client_result_t rest_client_transport_coap_send(
        rest_client_t *rest_client, rest_client_response_listener_t *listener,
        rest_client_method_t method, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len)
{
    int headers_index_content_type = -1;
    int headers_index_accept = -1;
    int headers_index_target_content_type = -1;
    int headers_index_target_accept = -1;
    int headers_index_authorization = -1;

    int i = 0;
    while (headers[i].key != REST_CLIENT_HEADERS_END) {

        switch (headers[i].key) {
        case REST_CLIENT_HEADER_CONTENT_TYPE:
            headers_index_content_type = i;
            break;
        case REST_CLIENT_HEADER_ACCEPT:
            headers_index_accept = i;
            break;
        case REST_CLIENT_HEADER_TARGET_CONTENT_TYPE:
            headers_index_target_content_type = i;
            break;
        case REST_CLIENT_HEADER_TARGET_ACCEPT:
            headers_index_target_accept = i;
            break;
        case REST_CLIENT_HEADER_AUTHORIZATION:
            headers_index_authorization = i;
            break;
        case REST_CLIENT_HEADERS_END:
            continue;
        /* no default case in order to get an error when we forgot to handle an enum value */
        }

        i++;
    }

    if (rest_client == NULL || path == NULL) {
        return REST_CLIENT_ERROR_CLIENT;
    }

    unsigned coap_method;
    int mapped_method = rest_client_util_translate_to_coap_method(method);
    if (mapped_method < 0) {
        return REST_CLIENT_ERROR_CLIENT;
    }
    else {
        coap_method = (unsigned) mapped_method;
    }

    /* Todo: error handling for mapped types necessary? */

    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;

    rest_client_coap_context_t coap_context = rest_client->coap_context;

    bool _proxied = (rest_client->coap_proxy != NULL);
    coap_context.proxied = _proxied;

    if (rest_client->coap_proxy != NULL) {
        memset(&coap_context.proxy_remote, 0, sizeof(coap_context.proxy_remote));

        switch (rest_client->coap_proxy->_addr.type) {
        #ifdef SOCK_HAS_IPV4
        case AF_INET:
            memcpy(coap_context.proxy_remote.addr.ipv4, rest_client->coap_proxy->_addr.ipv4, 4);
            coap_context.proxy_remote.family = AF_INET;
            break;
        #endif
        #ifdef SOCK_HAS_IPV6
        case AF_INET6:
            memcpy(coap_context.proxy_remote.addr.ipv6, rest_client->coap_proxy->_addr.ipv6, 16);
            coap_context.proxy_remote.family = AF_INET6;
            break;
        #endif
        default:
            assert(0);
            return REST_CLIENT_ERROR_CLIENT;
        }

        /* TODO: add support for netif? */
        coap_context.proxy_remote.netif = SOCK_ADDR_ANY_NETIF;
        coap_context.proxy_remote.port = rest_client->coap_proxy->port;
    }

    char uri[PROXY_URI_LEN];
    int uri_len;

    if (query_string == NULL) {
        uri_len = strlen(path);
    }
    else {
        uri_len = strlen(path) + strlen(query_string);
    }

    if (query_string == NULL) {
        memcpy(uri, path, uri_len + 1);
    }
    else {
        snprintf(uri, uri_len + 2, "%s?%s", path, query_string);
    }

    if (_proxied) {

        if (rest_client->_addr.type == AF_INET6) { /* format IPv6 */
            uri_len = snprintf(coap_context.proxy_uri, PROXY_URI_LEN, "%s://[%s]:%d%s",
                               rest_client_util_scheme_to_string(rest_client->scheme),
                               rest_client->hostname, rest_client->port, uri);
        }
        else { /* format either IPv4 or hostname */
            uri_len = snprintf(coap_context.proxy_uri, PROXY_URI_LEN, "%s://%s:%d%s",
                               rest_client_util_scheme_to_string(rest_client->scheme),
                               rest_client->hostname, rest_client->port, uri);
        }

        memcpy(uri, coap_context.proxy_uri, uri_len + 1);

        gcoap_req_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, coap_method, NULL);
    }
    else{
        gcoap_req_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, coap_method, uri);
    }

    if (qos == REST_CLIENT_QOS_AT_MOST_ONCE) {
        coap_hdr_set_type(pdu.hdr, COAP_TYPE_NON);
    }
    else if (qos == REST_CLIENT_QOS_AT_LEAST_ONCE || qos == REST_CLIENT_QOS_EXACTLY_ONCE) {
        coap_hdr_set_type(pdu.hdr, COAP_TYPE_CON);
    }
    else {
        assert(0);
    }

    memset(coap_context.last_req_path, 0, _LAST_REQ_PATH_MAX);
    if (uri_len < _LAST_REQ_PATH_MAX) {
        memcpy(coap_context.last_req_path, uri, uri_len);
    }

    if (body != NULL) {
        if (body_len) {
            if (headers_index_content_type != -1) {
                int type = (rest_client_content_type_t)headers[headers_index_content_type].value;
                int mapped_type = rest_client_util_translate_to_coap_format(type);
                coap_opt_add_format(&pdu, mapped_type);
            }
            else {
                return REST_CLIENT_ERROR_CLIENT;
            }
        }
    }

    if (headers_index_accept != -1) {
        int type = (rest_client_content_type_t)headers[headers_index_accept].value;
        int mapped_type = rest_client_util_translate_to_coap_format(type);
        coap_opt_add_accept(&pdu, mapped_type);
    }

    if (_proxied) {
        coap_opt_add_proxy_uri(&pdu, uri);
    }

    if (headers_index_target_content_type != -1) {
        int type = (rest_client_content_type_t)headers[headers_index_target_content_type].value;
        int mapped_type = rest_client_util_translate_to_coap_format(type);
        coap_opt_add_uint(&pdu, COAP_OPT_TARGET_CONTENT_TYPE, mapped_type);
    }

    if (headers_index_target_accept != -1) {
        int type = (rest_client_content_type_t)headers[headers_index_target_accept].value;
        int mapped_type = rest_client_util_translate_to_coap_format(type);
        coap_opt_add_uint(&pdu, COAP_OPT_TARGET_ACCEPT, mapped_type);
    }

    if (rest_client->coap_proxy->proxy_http_status) {
        /* input value does not matter, only the received option is interesting */
        coap_opt_add_uint(&pdu, COAP_OPT_PROXY_HTTP_STATUS, 0);
    }

    if (headers_index_authorization != -1) {
        coap_opt_add_string(&pdu, COAP_OPT_AUTHORIZATION, headers[headers_index_authorization].value, '\0');
    }

    if (body_len) {
        len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);
        if (pdu.payload_len >= body_len) {
            memcpy(pdu.payload, body, body_len);
            len += body_len;
        }
        else {
            return REST_CLIENT_ERROR_CLIENT;
        }
    }
    else {
        len = coap_opt_finish(&pdu, COAP_OPT_FINISH_NONE);
    }

    sock_udp_ep_t *remote;
    sock_udp_ep_t new_remote;

    if (_proxied) {
        remote = &coap_context.proxy_remote;
    }
    else {
        switch (rest_client->_addr.type) {
        #ifdef SOCK_HAS_IPV4
        case AF_INET:
            memcpy(new_remote.addr.ipv4, rest_client->_addr.ipv4, 4);
            new_remote.family = AF_INET;
            break;
        #endif
        #ifdef SOCK_HAS_IPV6
        case AF_INET6:
            memcpy(new_remote.addr.ipv6, rest_client->_addr.ipv6, 16);
            new_remote.family = AF_INET6;
            break;
        #endif
        default:
            assert(0);
            return REST_CLIENT_ERROR_CLIENT;
        }

        /* TODO: add support for netif */
        new_remote.netif = SOCK_ADDR_ANY_NETIF;
        new_remote.port = rest_client->port;

        remote = &new_remote;
    }

    size_t bytes_sent = gcoap_req_send(&buf[0], len, remote, _coap_resp_handler, listener);
    if (bytes_sent > 0) {
        return REST_CLIENT_RESULT_OK;
    }
    else {
        return REST_CLIENT_ERROR_CLIENT;
    }
}

rest_client_result_t rest_client_transport_coap_init(rest_client_t *rest_client)
{
    (void)rest_client;

#if IS_USED(MODULE_GCOAP_DTLS)
    int res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        DEBUG("gcoap: cannot add credential to system: %d\n", res);
        return REST_CLIENT_ERROR_CLIENT;
    }
    sock_dtls_t *gcoap_sock_dtls = gcoap_get_sock_dtls();
    res = sock_dtls_add_credential(gcoap_sock_dtls, GCOAP_DTLS_CREDENTIAL_TAG);
    if (res < 0) {
        DEBUG("gcoap: cannot add credential to DTLS sock: %d\n", res);
    }
#endif /* IS_USED(MODULE_GCOAP_DTLS) */

    return REST_CLIENT_RESULT_OK;
}
