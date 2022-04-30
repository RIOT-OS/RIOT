/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @defgroup    sys_rest_client REST client for RIOT OS
 * @ingroup     sys
 * @brief       REST client for RIOT OS
 *
 * @{
 * @file
 * @brief       REST client API
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include "kernel_defines.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <sys/socket.h>
#include "net/sock/udp.h"

#define REST_CLIENT_DEFAULT_PORT_COAP         5683
#define REST_CLIENT_DEFAULT_PORT_COAP_SECURE  5684
#define REST_CLIENT_DEFAULT_PORT_HTTP         80
#define REST_CLIENT_DEFAULT_PORT_HTTP_SECURE  443
#define REST_CLIENT_DEFAULT_PORT_MQTT         1883
#define REST_CLIENT_DEFAULT_PORT_MQTT_SECURE  8883
#define REST_CLIENT_DEFAULT_PORT_MQTTSN_IPV4  10004
#define REST_CLIENT_DEFAULT_PORT_MQTTSN_IPV6  10006

#define HEADER_AUTHORIZATION_MAX_LENGTH 255

typedef struct {
    int type;
    union {
    #ifdef SOCK_HAS_IPV4
        uint8_t ipv4[4];
    #endif
    #ifdef SOCK_HAS_IPV6
        uint8_t ipv6[16];
    #endif
    };
} _ipvx_addr_t;

typedef enum {
    REST_CLIENT_SCHEME_COAP,
    REST_CLIENT_SCHEME_COAP_SECURE,
    REST_CLIENT_SCHEME_HTTP,
    REST_CLIENT_SCHEME_HTTP_SECURE,
    REST_CLIENT_SCHEME_MQTT,
    REST_CLIENT_SCHEME_MQTT_SECURE, /* Not yet supported */
    REST_CLIENT_SCHEME_MQTTSN,
    REST_CLIENT_SCHEME_MQTTSN_SECURE, /* Not yet supported */
} rest_client_scheme_t;

typedef enum {
    REST_CLIENT_METHOD_GET,
    REST_CLIENT_METHOD_POST,
    REST_CLIENT_METHOD_PUT,
    REST_CLIENT_METHOD_PATCH,
    REST_CLIENT_METHOD_DELETE,
} rest_client_method_t;

typedef enum {
    REST_CLIENT_CONTENT_TYPE_NOT_SET = 0,
    REST_CLIENT_CONTENT_TYPE_TEXT_PLAIN,
    REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON,
    REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR,
    REST_CLIENT_CONTENT_TYPE_APPLICATION_VND_KAFKA_JSON_V2_JSON,
    REST_CLIENT_CONTENT_TYPE_APPLICATION_VND_KAFKA_V2_JSON,
} rest_client_content_type_t;

typedef enum {
    REST_CLIENT_HEADERS_END = 0,
    REST_CLIENT_HEADER_CONTENT_TYPE,
    REST_CLIENT_HEADER_ACCEPT,
    REST_CLIENT_HEADER_TARGET_CONTENT_TYPE,
    REST_CLIENT_HEADER_TARGET_ACCEPT,
    REST_CLIENT_HEADER_AUTHORIZATION,
} rest_client_header_key_t;

typedef struct {
    rest_client_header_key_t key;
    void *value;
} rest_client_header_t;

typedef enum {
    REST_CLIENT_QOS_AT_MOST_ONCE  = 0,  /* MQTT: QoS 0; CoAP: Non-Confirmable */
    REST_CLIENT_QOS_AT_LEAST_ONCE = 1,  /* MQTT: QoS 1; CoAP: Confirmable */
    REST_CLIENT_QOS_EXACTLY_ONCE  = 2,  /* MQTT: QoS 2; CoAP: Confirmable */
} rest_client_qos_t;

typedef enum {
    REST_CLIENT_RESULT_OK,
    REST_CLIENT_ERROR_CONNECTION,
    REST_CLIENT_ERROR_MEMORY,
    REST_CLIENT_ERROR_TIMEOUT,
    REST_CLIENT_ERROR_CLIENT,
    REST_CLIENT_ERROR_SERVER,
} rest_client_result_t;

/* TODO: rest_client_result_t vs rest_client_status_t ? */
typedef enum {
    REST_CLIENT_STATUS_OK,
    REST_CLIENT_STATUS_TIMEOUT,
    REST_CLIENT_STATUS_TRUNCATED,
    REST_CLIENT_STATUS_ERROR,
} rest_client_status_t;

/* TODO: define single timeout value for all transports
 *
 * non-confirmable = CONFIG_GCOAP_NON_TIMEOUT_MSEC
 * confirmable = (uint32_t)CONFIG_COAP_ACK_TIMEOUT * MS_PER_SEC;
 * MQTT = custom
 *
 */

typedef struct rest_client_response_listener_t {
    int status_code;
    rest_client_content_type_t content_type;
    uint8_t *body_buf;
    const size_t body_max_len;
    size_t body_len;
    void (*listener) (struct rest_client_response_listener_t*);
} rest_client_response_listener_t;

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
typedef struct rest_client_coap_proxy_t {
    rest_client_scheme_t scheme;
    char *hostname;
    bool is_numeric_hostname;
    uint16_t port;
    _ipvx_addr_t _addr; /* For internal use only */
    bool proxy_http_status;
} rest_client_coap_proxy_t;

/* Retain request path to re-request if response includes block. User must not
 * start a new request (with a new path) until any blockwise transfer
 * completes or times out. */
#define _LAST_REQ_PATH_MAX (1024)
#define PROXY_URI_LEN 512

typedef struct {
    rest_client_response_listener_t listener;
    bool proxied;
    sock_udp_ep_t proxy_remote;
    char proxy_uri[PROXY_URI_LEN];
    char last_req_path[_LAST_REQ_PATH_MAX];
} rest_client_coap_context_t;
#endif

typedef struct {
    rest_client_scheme_t scheme;
    char *hostname;
    bool is_numeric_hostname;
    uint16_t port;
    _ipvx_addr_t _addr; /* For internal use only */
#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rest_client_coap_context_t coap_context;
    rest_client_coap_proxy_t *coap_proxy;
#endif
} rest_client_t;

/**
 * @brief   Initialize rest_client. The function will try to resolve the hostname set in rest_client
 *
 * @param[inout] rest_client Client configuration
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_init(rest_client_t *rest_client);

/**
 * @brief   Deinitialize rest_client
 *
 * @param[in] rest_client Client configuration
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_deinit(rest_client_t *rest_client);

/**
 * @brief   Establish a connection
 *
 * @param[in] rest_client Client configuration
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_connect(rest_client_t *rest_client);

/**
 * @brief   Disconnect
 *
 * @param[in] rest_client Client configuration
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_disconnect(rest_client_t *rest_client);

/**
 * @brief   Make a GET request
 *
 * @param[in] rest_client   Client configuration
 * @param[in] listener      Callback when response is received, may be NULL
 * @param[in] qos           Level of reliability
 * @param[in] headers       List of header (MUST be terminated with REST_CLIENT_HEADERS_END)
 * @param[in] path          Path
 * @param[in] query_string  Query string
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_get(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string);

/**
 * @brief   Make a PUT request
 *
 * @param[in] rest_client   Client configuration
 * @param[in] listener      Callback when response is received, may be NULL
 * @param[in] qos           Level of reliability
 * @param[in] headers       List of header (MUST be terminated with REST_CLIENT_HEADERS_END)
 * @param[in] path          Path
 * @param[in] query_string  Query string
 * @param[in] body          Request body
 * @param[in] body_len      Request body length
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_put(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len);

/**
 * @brief   Make a POST request
 *
 * @param[in] rest_client   Client configuration
 * @param[in] listener      Callback when response is received, may be NULL
 * @param[in] qos           Level of reliability
 * @param[in] headers       List of header (MUST be terminated with REST_CLIENT_HEADERS_END)
 * @param[in] path          Path
 * @param[in] query_string  Query string
 * @param[in] body          Request body
 * @param[in] body_len      Request body length
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_post(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len);

/**
 * @brief   Make a PATCH request
 *
 * @param[in] rest_client   Client configuration
 * @param[in] listener      Callback when response is received, may be NULL
 * @param[in] qos           Level of reliability
 * @param[in] headers       List of header (MUST be terminated with REST_CLIENT_HEADERS_END)
 * @param[in] path          Path
 * @param[in] query_string  Query string
 * @param[in] body          Request body
 * @param[in] body_len      Request body length
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_patch(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len);

/**
 * @brief   Make a DELETE request
 *
 * @param[in] rest_client   Client configuration
 * @param[in] listener      Callback when response is received, may be NULL
 * @param[in] qos           Level of reliability
 * @param[in] headers       List of header (MUST be terminated with REST_CLIENT_HEADERS_END)
 * @param[in] path          Path
 * @param[in] query_string  Query string
 *
 * @return  result code (see rest_client_result_t)
 */
rest_client_result_t rest_client_delete(
        rest_client_t *rest_client, rest_client_response_listener_t *listener, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string);

#endif /* REST_CLIENT_H */
/** @} */
