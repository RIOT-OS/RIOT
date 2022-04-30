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
 * @brief       Example application for demonstrating RIOT's REST client by calling a Django REST API
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include "kernel_defines.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "time_units.h"
#include "event.h"
#include "event/timeout.h"
#include "cbor.h"
#include "ztimer.h"

#include <time.h>
#include <stdlib.h>

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

#ifndef CONFIG_USERNAME
#define CONFIG_USERNAME "riot-os_node"
#endif

#ifndef CONFIG_EMAIL
#define CONFIG_EMAIL "riot@os.de"
#endif

#ifndef CONFIG_PASSWORD
#define CONFIG_PASSWORD "my-secure-password-123"
#endif

#ifndef CONFIG_EVENT_TIMEOUT_MS
#define CONFIG_EVENT_TIMEOUT_MS 5000
#endif

#ifndef CONFIG_RESPONSE_BUF_SIZE
#define CONFIG_RESPONSE_BUF_SIZE 1024
#endif

#ifndef CONFIG_ENCODER_BUF_SIZE
#define CONFIG_ENCODER_BUF_SIZE 256
#endif

#ifndef CONFIG_AGGREGATE_SENSOR_DATA
#define CONFIG_AGGREGATE_SENSOR_DATA 1
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
    .scheme = REST_CLIENT_SCHEME_HTTP,
    .port = 8000,
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

    /* Paho MQTT-SN runs on different ports for IPv4 and IPv6 */
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

typedef struct {
    char *username;
    char *email;
    char *password1;
    char *password2;
} registration_request_t;

typedef struct {
    char *username;
    char *password;
} login_request_t;

/* request functions */
static rest_client_result_t _login(char *username, char *password);
static rest_client_result_t _register(char *username, char *email, char *password);
static rest_client_result_t _get_user_profile(char *token);
static rest_client_result_t _post_update_sensor_data(char *token);

/* parse function */
static int _parse_access_token(const uint8_t *buf, size_t buf_size,
                               char *access_token_buf, size_t *access_token_buf_size);

/* event handler functions */
static void _event_handler(event_t *event);
static void _timeout_handler(event_t *event);

/* Buffer for responses used within listeners. This buffer can be reused since requests are made sequentially due to the
 * event structure */
static uint8_t _reponse_buf[CONFIG_RESPONSE_BUF_SIZE];

/* cbor encoder buffer */
static uint8_t _encoder_buf[CONFIG_ENCODER_BUF_SIZE];

/* buffer to parse the access token into */
static char _access_token[HEADER_AUTHORIZATION_MAX_LENGTH];
static size_t _access_token_len = sizeof(_access_token);

/* used to move from one event state to the next */
static int _event_id = 0;

/* stop condition for event loop */
static bool _terminate_event_loop = false;

/* event structs */
static event_t _event = { .handler = _event_handler };
static event_t _timeout_event = { .handler = _timeout_handler };
static event_queue_t _event_queue;
static event_timeout_t _timeout;

/* event-loop that chains the requests that partially depend on each other */
static void _event_handler(event_t *event)
{
    (void)event;
    rest_client_result_t rc;

    event_timeout_clear(&_timeout);

    _event_id++;

    event_timeout_set(&_timeout, CONFIG_EVENT_TIMEOUT_MS);

    switch (_event_id) {
    case 1:
        DEBUG("\n\nREGISTER REQUEST\n");
        rc = _register(CONFIG_USERNAME, CONFIG_EMAIL, CONFIG_PASSWORD);
        break;
    case 2:
        DEBUG("\n\nLOGIN REQUEST\n");
        rc = _login(CONFIG_USERNAME, CONFIG_PASSWORD);
        break;
    case 3:
        DEBUG("\n\nGET USER PROFILE REQUEST\n");
        rc = _get_user_profile(_access_token);
        break;
    case 4:
#if !CONFIG_AGGREGATE_SENSOR_DATA
    /* send sensor three times in total */
    case 5:
    case 6:
#endif
        DEBUG("\n\nPOST UPDATE SENSOR DATA REQUEST\n");
        rc = _post_update_sensor_data(_access_token);
        break;
    default:
        DEBUG("\n\nFINISH\n");
        event_timeout_clear(&_timeout);
        _terminate_event_loop = true;
        return;
    }

    if (rc != REST_CLIENT_RESULT_OK) {
        DEBUG("rc != REST_CLIENT_RESULT_OK\n");
        event_timeout_clear(&_timeout);
        _terminate_event_loop = true;
    }
}

static void _timeout_handler(event_t *event)
{
    (void)event;

    DEBUG("TIMEOUT\n");
    _terminate_event_loop = true;
}

static void _listener_register_cb(rest_client_response_listener_t *listener)
{
    (void)listener;

    DEBUG("LISTENER REGISTER REQUEST\n");
    DEBUG("status_code = %d\n", listener->status_code);
    DEBUG("body_len = %d\n", listener->body_len);

    DEBUG("Payload (CBOR)\n");
    for (size_t i = 0; i < listener->body_len; i++) {
        DEBUG("%02x", listener->body_buf[i]);
    }
    DEBUG("\n");

    /* not checking for status code, because user could have been created already in a previous
     * execution */

    event_post(&_event_queue, &_event);
}

static void _listener_login_cb(rest_client_response_listener_t *listener)
{
    DEBUG("LISTENER LOGIN REQUEST\n");
    DEBUG("Status Code = %d\n", listener->status_code);
    DEBUG("Body Length = %d\n", listener->body_len);

    DEBUG("Payload (CBOR)\n");
    for (size_t i = 0; i < listener->body_len; i++) {
        DEBUG("%02x", listener->body_buf[i]);
    }
    DEBUG("\n");

    if (listener->status_code >= 200 && listener->status_code < 300) {
        _parse_access_token((const uint8_t*)listener->body_buf, listener->body_len, _access_token, &_access_token_len);
        DEBUG("Access Token = %s\n", _access_token);

        event_post(&_event_queue, &_event);
    }
    else {
        DEBUG("Response status code not success\n");
    }
}

static void _listener_get_user_profile_cb(rest_client_response_listener_t* listener)
{
    (void)listener;

    DEBUG("LISTENER GET USER PROFILE REQUEST\n");
    DEBUG("Status Code = %d\n", listener->status_code);
    DEBUG("Body Length = %d\n", listener->body_len);

    DEBUG("Payload (CBOR)\n");
    for (size_t i = 0; i < listener->body_len; i++) {
        DEBUG("%02x", listener->body_buf[i]);
    }
    DEBUG("\n");

    event_post(&_event_queue, &_event);
}

static void _listener_post_update_sensor_data_cb(rest_client_response_listener_t *listener)
{
    (void)listener;

    DEBUG("LISTENER POST UPDATE SENSOR DATA REQUEST\n");
    DEBUG("Status Code = %d\n", listener->status_code);
    DEBUG("Body Length = %d\n", listener->body_len);

    DEBUG("Payload (CBOR)\n");
    for (size_t i = 0; i < listener->body_len; i++) {
        DEBUG("%02x", listener->body_buf[i]);
    }
    DEBUG("\n");

    event_post(&_event_queue, &_event);
}

static rest_client_response_listener_t _listener_register = {
    .body_buf = _reponse_buf,
    .body_max_len = sizeof(_reponse_buf),
    .listener = _listener_register_cb,
};

static rest_client_response_listener_t _listener_login = {
    .body_buf = _reponse_buf,
    .body_max_len = sizeof(_reponse_buf),
    .listener = _listener_login_cb,
};

static rest_client_response_listener_t _listener_get_user_profile = {
    .body_buf = _reponse_buf,
    .body_max_len = sizeof(_reponse_buf),
    .listener = _listener_get_user_profile_cb,
};

static rest_client_response_listener_t _listener_post_update_sensor_data = {
    .body_buf = _reponse_buf,
    .body_max_len = sizeof(_reponse_buf),
    .listener = _listener_post_update_sensor_data_cb,
};


static size_t _encode_registration_request_t(registration_request_t *rr,
                                             uint8_t *encoder_buf, size_t encoder_buf_len)
{
    CborEncoder encoder;
    cbor_encoder_init(&encoder, encoder_buf, encoder_buf_len, 0);

    CborEncoder mapEncoder;
    cbor_encoder_create_map(&encoder, &mapEncoder, 4);

    cbor_encode_text_stringz(&mapEncoder, "username");
    cbor_encode_text_stringz(&mapEncoder, rr->username);

    cbor_encode_text_stringz(&mapEncoder, "email");
    cbor_encode_text_stringz(&mapEncoder, rr->email);

    cbor_encode_text_stringz(&mapEncoder, "password1");
    cbor_encode_text_stringz(&mapEncoder, rr->password1);

    cbor_encode_text_stringz(&mapEncoder, "password2");
    cbor_encode_text_stringz(&mapEncoder, rr->password2);

    cbor_encoder_close_container(&encoder, &mapEncoder);

    return cbor_encoder_get_buffer_size(&encoder, _encoder_buf);
}

static size_t _encode_login_request_t(login_request_t *lr,
                                      uint8_t *encoder_buf, size_t encoder_buf_len)
{
    CborEncoder encoder;
    cbor_encoder_init(&encoder, encoder_buf, encoder_buf_len, 0);

    CborEncoder mapEncoder;
    cbor_encoder_create_map(&encoder, &mapEncoder, 2);

    cbor_encode_text_stringz(&mapEncoder, "username");
    cbor_encode_text_stringz(&mapEncoder, lr->username);

    cbor_encode_text_stringz(&mapEncoder, "password");
    cbor_encode_text_stringz(&mapEncoder, lr->password);

    cbor_encoder_close_container(&encoder, &mapEncoder);

    return cbor_encoder_get_buffer_size(&encoder, _encoder_buf);
}

static int _parse_access_token(const uint8_t *buf, size_t buf_size,
                               char *access_token_buf, size_t *access_token_buf_size)
{
    int err;

    CborParser parser;
    CborValue it;
    CborValue map;
    size_t tmp_size;

    err = cbor_parser_init(buf, buf_size, 0, &parser, &it);
    if (err) return 1;

    if (!cbor_value_is_map(&it)) return 1;
    err = cbor_value_enter_container(&it, &map);
    if (err) return 1;

    /* skip key */
    if (!cbor_value_is_text_string(&map)) return 1;
    err = cbor_value_advance(&map);
    if (err) return 1;

    /* get value */
    if (!cbor_value_is_text_string(&map)) return 1;
    tmp_size = *access_token_buf_size;
    err = cbor_value_copy_text_string(&map, access_token_buf, access_token_buf_size, &map);
    if (err || tmp_size == *access_token_buf_size) return 1;

    return 0;
}

static rest_client_result_t _register(char *username, char *email, char *password)
{
    registration_request_t rr = {
        .username = username,
        .email = email,
        .password1 = password,
        .password2 = password,
    };

    size_t payload_len = _encode_registration_request_t(&rr, _encoder_buf, sizeof(_encoder_buf));

    DEBUG("encoded %d bytes\n", payload_len);

    for (size_t i = 0; i < payload_len; i++) {
        DEBUG("%02x", _encoder_buf[i]);
    }
    DEBUG("\n");

    rest_client_header_t headers[] = {
        { REST_CLIENT_HEADER_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_TARGET_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON },
        { REST_CLIENT_HEADER_TARGET_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON },
        { REST_CLIENT_HEADERS_END, NULL }
    };

    rest_client_result_t rc;

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rc = rest_client_post(&_rest_client_coap, &_listener_register, CONFIG_QOS, headers,
                          "/api/auth/registration/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    rc = rest_client_post(&_rest_client_mqtt, &_listener_register, CONFIG_QOS, headers,
                          "/api/auth/registration/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    rc = rest_client_post(&_rest_client_mqttsn, &_listener_register, CONFIG_QOS, headers,
                          "/api/auth/registration/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

    return rc;
}

static rest_client_result_t _login(char *username, char *password)
{
    login_request_t lr = {
        .username = username,
        .password = password,
    };

    size_t payload_len = _encode_login_request_t(&lr, _encoder_buf, sizeof(_encoder_buf));

    DEBUG("Encoded LoginRequest: %d bytes\n", payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        DEBUG("%02x", _encoder_buf[i]);
    }
    DEBUG("\n");

    rest_client_header_t headers[] = {
        { REST_CLIENT_HEADER_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_TARGET_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON },
        { REST_CLIENT_HEADER_TARGET_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON },
        { REST_CLIENT_HEADERS_END, NULL }
    };

    rest_client_result_t rc;

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rc = rest_client_post(&_rest_client_coap, &_listener_login, CONFIG_QOS, headers,
                          "/api/auth/login/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    rc = rest_client_post(&_rest_client_mqtt, &_listener_login, CONFIG_QOS, headers,
                          "/api/auth/login/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    rc = rest_client_post(&_rest_client_mqttsn, &_listener_login, CONFIG_QOS, headers,
                          "/api/auth/login/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

    return rc;
}

static rest_client_result_t _get_user_profile(char *token)
{
    assert(token != NULL);

    char auth_string[HEADER_AUTHORIZATION_MAX_LENGTH];
    snprintf(auth_string, sizeof(auth_string), "JWT %s", token);

    rest_client_header_t headers[] = {
        { REST_CLIENT_HEADER_AUTHORIZATION, auth_string },
        { REST_CLIENT_HEADER_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_TARGET_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON },
        { REST_CLIENT_HEADERS_END, NULL }
    };

    rest_client_result_t rc;

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rc = rest_client_get(&_rest_client_coap, &_listener_get_user_profile, CONFIG_QOS, headers,
                         "/api/users/user-profile/", NULL);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    rc = rest_client_get(&_rest_client_mqtt, &_listener_get_user_profile, CONFIG_QOS, headers,
                         "/api/users/user-profile/", NULL);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    rc = rest_client_get(&_rest_client_mqttsn, &_listener_get_user_profile, CONFIG_QOS, headers,
                         "/api/users/user-profile/", NULL);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

    return rc;
}

static rest_client_result_t _post_update_sensor_data(char *token)
{
    (void)token;
    assert(token != NULL);

    /* TODO: Due to an open issue in the MQTT-SN implementation, the message gets too large and
     *       won't be sent when the auth token is included. It works for CoAP and MQTT, but to keep
     *       comparability disable it for all of them for now) */
//    char auth_string[HEADER_AUTHORIZATION_MAX_LENGTH];
//    snprintf(auth_string, sizeof(auth_string), "JWT %s", token);

    rest_client_header_t headers[] = {
//        { REST_CLIENT_HEADER_AUTHORIZATION, auth_string },
        { REST_CLIENT_HEADER_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR },
        { REST_CLIENT_HEADER_TARGET_CONTENT_TYPE, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON },
        { REST_CLIENT_HEADER_TARGET_ACCEPT, (void*)REST_CLIENT_CONTENT_TYPE_APPLICATION_JSON },
        { REST_CLIENT_HEADERS_END, NULL }
    };

    CborEncoder encoder;
    cbor_encoder_init(&encoder, _encoder_buf, sizeof(_encoder_buf), 0);

#if CONFIG_AGGREGATE_SENSOR_DATA

    CborEncoder mapEncoder;
    cbor_encoder_create_map(&encoder, &mapEncoder, 3);

    cbor_encode_text_stringz(&mapEncoder, "temp1");
    cbor_encode_int(&mapEncoder, (rand() % 36));

    cbor_encode_text_stringz(&mapEncoder, "temp2");
    cbor_encode_int(&mapEncoder, (rand() % 36));

    cbor_encode_text_stringz(&mapEncoder, "temp3");
    cbor_encode_int(&mapEncoder, (rand() % 36));

    cbor_encoder_close_container(&encoder, &mapEncoder);
#else
    CborEncoder mapEncoder;
    cbor_encoder_create_map(&encoder, &mapEncoder, 1);

    cbor_encode_text_stringz(&mapEncoder, "temp1");
    cbor_encode_int(&mapEncoder, (rand() % 36));

    cbor_encoder_close_container(&encoder, &mapEncoder);
#endif

    int payload_len = cbor_encoder_get_buffer_size(&encoder, _encoder_buf);

    DEBUG("Encoded Random Data: %d bytes\n", payload_len);
    for (int i = 0; i < payload_len; i++) {
        DEBUG("%02x", _encoder_buf[i]);
    }
    DEBUG("\n");

    rest_client_result_t rc;

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_COAP)
    rc = rest_client_post(&_rest_client_coap, &_listener_post_update_sensor_data, CONFIG_QOS, headers,
                          "/api/demo/update-sensor-data/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTT)
    rc = rest_client_post(&_rest_client_mqtt, &_listener_post_update_sensor_data, CONFIG_QOS, headers,
                          "/api/demo/update-sensor-data/", NULL, _encoder_buf, payload_len);
    assert(rc == REST_CLIENT_RESULT_OK);
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
    rc = rest_client_post(&_rest_client_mqttsn, &_listener_post_update_sensor_data, CONFIG_QOS, headers,
                          "/api/demo/update-sensor-data/", NULL, _encoder_buf, payload_len);

    assert(rc == REST_CLIENT_RESULT_OK);
#endif

    return rc;
}

static void _wait_for_network_set_up(void)
{
    /* wait for the network to come up */
    /* Todo: It would be nice to have a sophisticated API in RIOT instead of sleeping n seconds */
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

    srand(7);

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

    event_queue_init(&_event_queue);

    event_timeout_ztimer_init(&_timeout, ZTIMER_MSEC, &_event_queue, &_timeout_event);
    event_timeout_set(&_timeout, CONFIG_EVENT_TIMEOUT_MS);

    /* trigger initial call */
    event_post(&_event_queue, &_event);

    event_t *event;
    while (!_terminate_event_loop && (event = event_wait(&_event_queue))) {
        event->handler(event);
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
