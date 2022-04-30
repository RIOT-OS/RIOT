/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client
 * @{
 *
 * @file
 * @brief       REST client implementation
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include "kernel_defines.h"
#include <assert.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
#include "rest_client/transport/coap.h"
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
#include "rest_client/transport/mqtt.h"
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
#include "rest_client/transport/mqttsn.h"
#endif

#include "rest_client.h"

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"

static rest_client_result_t _dispatcher(
        rest_client_t *rest_client, rest_client_response_listener_t *listener,
        rest_client_method_t method, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len)
{
#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    if (rest_client->coap_proxy != NULL ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP_SECURE)
    {
        return rest_client_transport_coap_send(rest_client, listener, method, qos,
                                               headers, path, query_string, body, body_len);
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTT) {
        return rest_client_transport_mqtt_send(rest_client, listener, method, qos,
                                               headers, path, query_string, body, body_len);
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTTSN) {
        return rest_client_transport_mqttsn_send(rest_client, listener, method, qos,
                                                 headers, path, query_string, body, body_len);
    }
#endif

    return REST_CLIENT_ERROR_CLIENT;
}

static int _resolve(char *hostname, bool is_numeric_hostname, _ipvx_addr_t *addr)
{
    int rc = -1;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    memset(addr, 0, sizeof(_ipvx_addr_t));

    if (is_numeric_hostname) {
        hints.ai_family = AF_UNSPEC;
        hints.ai_flags |= AI_NUMERICHOST;
    }
    else {
        if (IS_USED(MODULE_IPV4_ADDR) && IS_USED(MODULE_IPV6_ADDR)) {
            hints.ai_family = AF_UNSPEC;
        }
        else if (IS_USED(MODULE_IPV4_ADDR)) {
            hints.ai_family = AF_INET;
        }
        else if (IS_USED(MODULE_IPV6_ADDR)) {
            hints.ai_family = AF_INET6;
        }
        else {
            DEBUG("Error: Neither IPv4 nor IPv6 included in build\n");
            assert(0);
        }
    }

    rc = getaddrinfo(hostname, NULL, &hints, &result);
    if (rc == 0) {
        struct addrinfo* result_i = result;

        if (! result) {
            return -1;
        }

        while (result_i) {
            if (hints.ai_family == AF_UNSPEC) {
                /* take first result which is either IPv4 or IPv6 */
                break;
            }
            else if (hints.ai_family == result_i->ai_family) {
                /* take first matching IPv4/IPv6 result */
                break;
            }

            result_i = result_i->ai_next;
        }

        if (result_i) {
            switch (result_i->ai_family) {
        #ifdef SOCK_HAS_IPV4
            case AF_INET:
                addr->type = AF_INET;
                memcpy(addr->ipv4, &((struct sockaddr_in *)(result_i->ai_addr))->sin_addr.s_addr, 4);
                break;
        #endif
        #ifdef SOCK_HAS_IPV6
            case AF_INET6:
                addr->type = AF_INET6;
                memcpy(addr->ipv6, ((struct sockaddr_in6 *)(result_i->ai_addr))->sin6_addr.s6_addr, 16);
                break;
        #endif
            default:
                assert(0);
                rc = -1;
            }
        }
        else {
            rc = -1;
        }

        freeaddrinfo(result);
    }

    if (rc != 0) {
        return -1;
    }

    return 0;
}

rest_client_result_t rest_client_init(rest_client_t *rest_client)
{
#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    if (rest_client->coap_proxy != NULL ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP_SECURE) {

        if (rest_client->coap_proxy != NULL) {
            assert(rest_client->coap_proxy->hostname != NULL);
            assert(rest_client->coap_proxy->port != 0);

            if (_resolve(
                    rest_client->coap_proxy->hostname,
                    rest_client->coap_proxy->is_numeric_hostname,
                    &(rest_client->coap_proxy->_addr)) < 0) {
                DEBUG("_resolve() of %s failed\n", rest_client->coap_proxy->hostname);
                return REST_CLIENT_ERROR_CLIENT;
            }

            /* detect IP version for later use when formatting proxy_uri */
            if (rest_client->is_numeric_hostname) {
                if (_resolve(rest_client->hostname, true, &(rest_client->_addr)) < 0) {
                    DEBUG("_resolve() of %s failed\n", rest_client->hostname);
                    return REST_CLIENT_ERROR_CLIENT;
                }
            }
        }
        else {
            assert(rest_client->scheme == REST_CLIENT_SCHEME_COAP ||
                   rest_client->scheme == REST_CLIENT_SCHEME_COAP_SECURE);
            assert(rest_client->hostname != NULL);
            assert(rest_client->port != 0);

            if (_resolve(
                    rest_client->hostname,
                    rest_client->is_numeric_hostname,
                    &(rest_client->_addr)) < 0) {
                DEBUG("_resolve() of %s failed\n", rest_client->hostname);
                return REST_CLIENT_ERROR_CLIENT;
            }
        }

        return rest_client_transport_coap_init(rest_client);
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTT) {

        assert(rest_client->scheme == REST_CLIENT_SCHEME_MQTT);
        assert(rest_client->hostname != NULL);
        assert(rest_client->port != 0);

        if (_resolve(
                rest_client->hostname,
                rest_client->is_numeric_hostname,
                &(rest_client->_addr)) < 0) {
            DEBUG("_resolve() of %s failed\n", rest_client->hostname);
            return REST_CLIENT_ERROR_CLIENT;
        }

        return rest_client_transport_mqtt_init(rest_client);
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTTSN) {

        assert(rest_client->scheme == REST_CLIENT_SCHEME_MQTTSN);
        assert(rest_client->mqttsn_gateway != NULL);
        assert(rest_client->mqttsn_gateway->hostname != NULL);
        assert(rest_client->mqttsn_gateway->port != 0);

        if (_resolve(
                rest_client->mqttsn_gateway->hostname,
                rest_client->mqttsn_gateway->is_numeric_hostname,
                &(rest_client->mqttsn_gateway->_addr)) < 0) {
            DEBUG("_resolve() of %s failed\n", rest_client->mqttsn_gateway->hostname);
            return REST_CLIENT_ERROR_CLIENT;
        }

        return rest_client_transport_mqttsn_init(rest_client);
    }
#endif

    return REST_CLIENT_ERROR_CLIENT;
}

rest_client_result_t rest_client_deinit(rest_client_t *rest_client)
{
#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    if (rest_client->coap_proxy != NULL ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP_SECURE)
    {
        /* no-op for CoAP based transport */
        return REST_CLIENT_RESULT_OK;
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTT) {
        return rest_client_transport_mqtt_deinit(rest_client);
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTTSN) {
        return rest_client_transport_mqttsn_deinit(rest_client);
    }
#endif

    return REST_CLIENT_ERROR_CLIENT;
}

rest_client_result_t rest_client_connect(rest_client_t *rest_client)
{
#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    if (rest_client->coap_proxy != NULL ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP_SECURE)
    {
        /* no-op for CoAP based transport */
        return REST_CLIENT_RESULT_OK;
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTT) {
        return rest_client_transport_mqtt_connect(rest_client);
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTTSN) {
        return rest_client_transport_mqttsn_connect(rest_client);
    }
#endif

    return REST_CLIENT_ERROR_CLIENT;
}

rest_client_result_t rest_client_disconnect(rest_client_t *rest_client)
{
#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    if (rest_client->coap_proxy != NULL ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP ||
        rest_client->scheme == REST_CLIENT_SCHEME_COAP_SECURE)
    {
        /* no-op for CoAP based transport */
        return REST_CLIENT_RESULT_OK;
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTT) {
        return rest_client_transport_mqtt_disconnect(rest_client);
    }
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    if (rest_client->scheme == REST_CLIENT_SCHEME_MQTTSN) {
        return rest_client_transport_mqttsn_disconnect(rest_client);
    }
#endif

    return REST_CLIENT_ERROR_CLIENT;
}

rest_client_result_t rest_client_get(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string)
{
    return _dispatcher(rest_client, listener, REST_CLIENT_METHOD_GET, qos, headers, path, query_string, NULL, 0);
}

rest_client_result_t rest_client_put(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len)
{
    return _dispatcher(rest_client, listener, REST_CLIENT_METHOD_PUT, qos, headers, path, query_string, body, body_len);
}

rest_client_result_t rest_client_post(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len)
{
    return _dispatcher(rest_client, listener, REST_CLIENT_METHOD_POST, qos, headers, path, query_string, body, body_len);
}

rest_client_result_t rest_client_patch(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len)
{
    return _dispatcher(rest_client, listener, REST_CLIENT_METHOD_PATCH, qos, headers, path, query_string, body, body_len);
}

rest_client_result_t rest_client_delete(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string)
{
    return _dispatcher(rest_client, listener, REST_CLIENT_METHOD_DELETE, qos, headers, path, query_string, NULL, 0);
}
