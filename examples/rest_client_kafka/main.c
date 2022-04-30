/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating RIOT's REST client by calling Apache Kafka
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include "kernel_defines.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "cbor.h"
#include "ztimer.h"

#include "rest_client.h"

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"

#ifndef CONFIG_HOST
    #ifdef SOCK_HAS_IPV6
        #define CONFIG_HOST "fe80::d33f:536a:9c15:2af4"
    #else
        #define CONFIG_HOST "192.168.2.135"
    #endif
#endif /* CONFIG_HOST */

#ifndef CONFIG_HOST_IS_NUMERIC
#define CONFIG_HOST_IS_NUMERIC 1
#endif

#ifndef CONFIG_QOS
#define CONFIG_QOS 2
#endif

#ifndef CONFIG_RESPONSE_BUF_SIZE
#define CONFIG_RESPONSE_BUF_SIZE 1024
#endif

#ifndef CONFIG_ENCODER_BUF_SIZE
#define CONFIG_ENCODER_BUF_SIZE 256
#endif

#ifndef CONFIG_NODE_NAME
#define CONFIG_NODE_NAME "riot-node-17"
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)    \
    + IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)  \
    + IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN) > 1
#error "Application is not designed to support more than one transport at the same time"
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
static rest_client_coap_proxy_t _coap_proxy = {
#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAPS)
    .scheme = REST_CLIENT_SCHEME_COAP_SECURE,
    .port = REST_CLIENT_DEFAULT_PORT_COAP_SECURE,
#else
    .scheme = REST_CLIENT_SCHEME_COAP,
    .port = REST_CLIENT_DEFAULT_PORT_COAP,
#endif
    .hostname = CONFIG_HOST,
    .is_numeric_hostname = CONFIG_HOST_IS_NUMERIC,
    .proxy_http_status = true,
};

static rest_client_t _rest_client_coap = {
    .scheme = REST_CLIENT_SCHEME_HTTP_SECURE,
    .port = 8084,
#ifdef SOCK_HAS_IPV6
    .hostname = "::1",
#else
    .hostname = "127.0.0.1",
#endif
    .is_numeric_hostname = true,
    .coap_proxy = &_coap_proxy,
};
#endif /* IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP) */

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
static rest_client_t _rest_client_mqtt = {
    .scheme = REST_CLIENT_SCHEME_MQTT,
    .hostname = CONFIG_HOST,
    .is_numeric_hostname = CONFIG_HOST_IS_NUMERIC,
    .port = REST_CLIENT_DEFAULT_PORT_MQTT,
};
#endif /* IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT) */

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
static rest_client_mqttsn_gateway_t mqttsn_gateway = {
    .hostname = CONFIG_HOST,
    .is_numeric_hostname = CONFIG_HOST_IS_NUMERIC,
    /* Todo: does not work in dual stack */
#ifdef SOCK_HAS_IPV6
    .port = REST_CLIENT_DEFAULT_PORT_MQTTSN_IPV6,
#else
    .port = REST_CLIENT_DEFAULT_PORT_MQTTSN_IPV4,
#endif
};

/* hostname and port are not used when using mqttsn because the upstream mqtt broker is configured
 * by the gateway's config */
static rest_client_t _rest_client_mqttsn = {
    .scheme = REST_CLIENT_SCHEME_MQTTSN,
    .mqttsn_gateway = &mqttsn_gateway,
};
#endif /* IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN) */

/* response buffer for listeners */
static uint8_t _reponse_buf[CONFIG_RESPONSE_BUF_SIZE];

/* cbor encoder buffer */
static uint8_t _encoder_buf[CONFIG_ENCODER_BUF_SIZE];

static void _listener_cb(rest_client_response_listener_t *listener)
{
    DEBUG("Status Code = %d\n", listener->status_code);
    DEBUG("Body Length = %d\n", listener->body_len);

    DEBUG("Payload (CBOR)\n");
    for (size_t i = 0; i < listener->body_len; i++) {
        DEBUG("%02x", listener->body_buf[i]);
    }
    DEBUG("\n");
}

static rest_client_response_listener_t _listener = {
    .body_buf = _reponse_buf,
    .body_max_len = CONFIG_RESPONSE_BUF_SIZE,
    .listener = _listener_cb,
};

static int _get_array_length(void **ptr)
{
    int count = 0;
    void *p = ptr[0];
    while (p != NULL) {
        p = ptr[++count];
    }

    return count;
}

typedef struct {
    char *key;      /**< optional */
    char *value;    /**< mandatory */
} kafka_produce_record_t;

typedef struct {
    kafka_produce_record_t **records;   /**< NOTE: last item must be NULL */
} kafka_produce_request_t;

static void _encode_kafka_produce_record_t(kafka_produce_record_t *kpr, CborEncoder *encoder)
{
    int count = 2;

    /* key is optional */
    if (kpr->key == NULL) count--;

    CborEncoder mapEncoder;
    cbor_encoder_create_map(encoder, &mapEncoder, count);

    if (kpr->key != NULL) {
        cbor_encode_text_stringz(&mapEncoder, "key");
        cbor_encode_text_stringz(&mapEncoder, kpr->key);
    }

    assert(kpr->value != NULL);
    cbor_encode_text_stringz(&mapEncoder, "value");
    cbor_encode_text_stringz(&mapEncoder, kpr->value);

    cbor_encoder_close_container(encoder, &mapEncoder);
}

static size_t _encode_kafka_produce_request_t(kafka_produce_request_t *kpr,
                                              uint8_t *encoder_buf, size_t encoder_buf_len)
{
    CborEncoder encoder;
    cbor_encoder_init(&encoder, encoder_buf, encoder_buf_len, 0);

    int count = _get_array_length((void**)kpr->records);

    CborEncoder mapEncoder;
    cbor_encoder_create_map(&encoder, &mapEncoder, 1);

    cbor_encode_text_stringz(&mapEncoder, "records");

    CborEncoder arrayEncoder;
    cbor_encoder_create_array(&mapEncoder, &arrayEncoder, count);

    for (int i = 0; i < count; i++) {
        _encode_kafka_produce_record_t(kpr->records[i], &arrayEncoder);
    }

    cbor_encoder_close_container(&mapEncoder, &arrayEncoder);
    cbor_encoder_close_container(&encoder, &mapEncoder);

    return cbor_encoder_get_buffer_size(&encoder, _encoder_buf);
}

static rest_client_result_t _pub_kafka(char *key, char *value)
{
    assert(value != NULL);

    rest_client_result_t rc;

    kafka_produce_record_t record = { .key = key, .value = value };
    kafka_produce_record_t* records[] = { &record, NULL };

    kafka_produce_request_t kpr = {
        .records = records,
    };

    size_t payload_len = _encode_kafka_produce_request_t(&kpr, _encoder_buf, sizeof(_encoder_buf));

    DEBUG("encoded %d bytes\n", payload_len);

    for (size_t i = 0; i < payload_len; i++) {
        DEBUG("%02x", _encoder_buf[i]);
    }
    DEBUG("\n");

    rest_client_header_t headers[] = {
        { REST_CLIENT_HEADER_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_TARGET_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_VND_KAFKA_JSON_V2_JSON },
        { REST_CLIENT_HEADER_TARGET_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_VND_KAFKA_V2_JSON },
        { REST_CLIENT_HEADERS_END, NULL }
    };

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rc = rest_client_post(&_rest_client_coap, &_listener, CONFIG_QOS, headers,
                          "/topics/"CONFIG_NODE_NAME"-temperature", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    rc = rest_client_post(&_rest_client_mqtt, &_listener, CONFIG_QOS, headers,
                          "/topics/"CONFIG_NODE_NAME"-temperature", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    rc = rest_client_post(&_rest_client_mqttsn, &_listener, CONFIG_QOS, headers,
                          "/topics/"CONFIG_NODE_NAME"-temperature", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

    return rc;
}

static void _wait_for_network_set_up(void)
{
    /* wait for the network to come up */
    /* Todo: would be nice to have a sophisticated API in RIOT instead of sleeping n seconds */
#if IS_USED(MODULE_ESP_WIFI)
    ztimer_sleep(ZTIMER_MSEC, 10000);
#elif IS_USED(MODULE_NETDEV_TAP)
    ztimer_sleep(ZTIMER_MSEC, 3000);
#elif IS_USED(MODULE_NETDEV_ETH)
    ztimer_sleep(ZTIMER_MSEC, 4000);
#endif
}

int main(void)
{
    rest_client_result_t rc;
    (void)rc;

    _wait_for_network_set_up();

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rc = rest_client_init(&_rest_client_coap);
    assert(rc == REST_CLIENT_RESULT_OK);
    rc = rest_client_connect(&_rest_client_coap);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    rc = rest_client_init(&_rest_client_mqtt);
    assert(rc == REST_CLIENT_RESULT_OK);
    rc = rest_client_connect(&_rest_client_mqtt);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    rc = rest_client_init(&_rest_client_mqttsn);
    assert(rc == REST_CLIENT_RESULT_OK);
    rc = rest_client_connect(&_rest_client_mqttsn);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

    rc = _pub_kafka(NULL, "25.7");

    if (rc != REST_CLIENT_RESULT_OK) {
        DEBUG("rc != REST_CLIENT_RESULT_OK\n");
    }

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rc = rest_client_disconnect(&_rest_client_coap);
    assert(rc == REST_CLIENT_RESULT_OK);
    rc = rest_client_deinit(&_rest_client_coap);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    rc = rest_client_disconnect(&_rest_client_mqtt);
    assert(rc == REST_CLIENT_RESULT_OK);
    rc = rest_client_deinit(&_rest_client_mqtt);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    rc = rest_client_disconnect(&_rest_client_mqttsn);
    assert(rc == REST_CLIENT_RESULT_OK);
    rc = rest_client_deinit(&_rest_client_mqttsn);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

    return 0;
}
