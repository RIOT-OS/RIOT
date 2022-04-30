/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @defgroup    sys_rest_client_internal_mqtt REST client MQTT internal
 * @ingroup     sys_rest_client
 * @brief       REST client communicating over MQTT
 *
 * @{
 * @file
 * @brief       Internals for MQTT-based transport
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include <assert.h>

#include <stdio.h>

#include "cbor.h"

#include "rest_client/internal/mqtt.h"
#include "rest_client/util.h"

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"

#define KEY_REQUEST_ID          0
#define KEY_QUERY_STRING        1
#define KEY_CONTENT_TYPE        2
#define KEY_ACCEPT              3
#define KEY_TARGET_CONTENT_TYPE 4
#define KEY_TARGET_ACCEPT       5
#define KEY_AUTHORIZATION       6
#define KEY_BODY                7
#define KEY_HTTP_STATUS         8

void _rest_client_internal_mqtt_build_mqtt_request_topic(
        char *buf, size_t buf_len, rest_client_method_t method, const char *path)
{
    assert(path[0] == '/');

    const char *method_str = rest_client_util_method_to_string(method);
    snprintf(buf, buf_len, "%s%s", method_str, path);
    buf[buf_len - 1] = '\0';
}

void _rest_client_internal_mqtt_build_mqtt_response_topic(
        char *buf, size_t buf_len, rest_client_method_t method, const char *path, const char *uuid)
{
    assert(path[0] == '/');

    const char *method_str = rest_client_util_method_to_string(method);
    snprintf(buf, buf_len, "%s%s^%s", method_str, path, uuid);
    buf[buf_len - 1] = '\0';
}

size_t _rest_client_internal_mqtt_encode_mqtt_payload(
        _mqtt_payload_t *mqtt_payload, uint8_t *encoder_buf, size_t encoder_buf_len)
{
    CborEncoder encoder;
    cbor_encoder_init(&encoder, encoder_buf, encoder_buf_len, 0);

    int map_size = 8;
    if (mqtt_payload->request_id == NULL) map_size -= 1;
    if (mqtt_payload->query_string == NULL) map_size -= 1;
    if (mqtt_payload->authorization == NULL) map_size -= 1;
    if (mqtt_payload->content_type == REST_CLIENT_CONTENT_TYPE_NOT_SET) map_size -= 1;
    if (mqtt_payload->accept == REST_CLIENT_CONTENT_TYPE_NOT_SET) map_size -= 1;
    if (mqtt_payload->target_content_type == REST_CLIENT_CONTENT_TYPE_NOT_SET) map_size -= 1;
    if (mqtt_payload->target_accept == REST_CLIENT_CONTENT_TYPE_NOT_SET) map_size -= 1;
    if (mqtt_payload->body == NULL) map_size -= 1;

    CborEncoder mapEncoder;
    cbor_encoder_create_map(&encoder, &mapEncoder, map_size);

    if (mqtt_payload->request_id != NULL) {
        cbor_encode_int(&mapEncoder, KEY_REQUEST_ID);
        cbor_encode_byte_string(&mapEncoder, (uint8_t *)mqtt_payload->request_id, 16);
    }

    if (mqtt_payload->query_string != NULL) {
        cbor_encode_int(&mapEncoder, KEY_QUERY_STRING);
        cbor_encode_text_stringz(&mapEncoder, mqtt_payload->query_string);
    }

    if (mqtt_payload->authorization != NULL) {
        cbor_encode_int(&mapEncoder, KEY_AUTHORIZATION);
        cbor_encode_text_stringz(&mapEncoder, mqtt_payload->authorization);
    }

    if (mqtt_payload->content_type != REST_CLIENT_CONTENT_TYPE_NOT_SET) {
        cbor_encode_int(&mapEncoder, KEY_CONTENT_TYPE);
        cbor_encode_int(&mapEncoder, mqtt_payload->content_type);
    }

    if (mqtt_payload->accept != REST_CLIENT_CONTENT_TYPE_NOT_SET) {
        cbor_encode_int(&mapEncoder, KEY_ACCEPT);
        cbor_encode_int(&mapEncoder, mqtt_payload->accept);
    }

    if (mqtt_payload->target_content_type != REST_CLIENT_CONTENT_TYPE_NOT_SET) {
        cbor_encode_int(&mapEncoder, KEY_TARGET_CONTENT_TYPE);
        cbor_encode_int(&mapEncoder, mqtt_payload->target_content_type);
    }

    if (mqtt_payload->target_accept != REST_CLIENT_CONTENT_TYPE_NOT_SET) {
        cbor_encode_int(&mapEncoder, KEY_TARGET_ACCEPT);
        cbor_encode_int(&mapEncoder, mqtt_payload->target_accept);
    }

    if (mqtt_payload->body != NULL) {
        cbor_encode_int(&mapEncoder, KEY_BODY);
        cbor_encode_byte_string(&mapEncoder, mqtt_payload->body, mqtt_payload->body_len);
    }

    cbor_encoder_close_container(&encoder, &mapEncoder);

    return cbor_encoder_get_buffer_size(&encoder, encoder_buf);
}

int _rest_client_internal_mqtt_parse_mqtt_payload(
        const uint8_t *buf, size_t buf_size,
        int *http_status, rest_client_content_type_t *content_type,
        uint8_t *cbor_payload_buf, size_t *cbor_payload_buf_size)
{
    int err;

    CborParser parser;
    CborValue it;
    CborValue map;
    size_t map_size;

    err = cbor_parser_init(buf, buf_size, 0, &parser, &it);
    if (err) return 1;

    if (!cbor_value_is_map(&it)) return 1;
    if (!cbor_value_is_length_known(&it)) return 1;
    err = cbor_value_get_map_length(&it, &map_size);
    if (err) {
        DEBUG("cbor_value_get_map_length failed\n");
        return 1;
    }

    err = cbor_value_enter_container(&it, &map);
    if (err) {
        DEBUG("cbor_value_enter_container failed\n");
        return 1;
    }

    for (size_t i = 0; i < map_size; i++) {

        int key;

        size_t tmp_size;
        int tmp_int;

        /* keys in cbor data have to be integers */
        if (!cbor_value_is_integer(&map)) return 1;
        err = cbor_value_get_int(&map, &key);
        if (err) {
            DEBUG("cbor_value_get_int failed\n");
            return 1;
        }

        /* advance to the value */
        err = cbor_value_advance(&map);
        if (err) {
            DEBUG("cbor_value_advance failed\n");
            return 1;
        }

        switch (key) {
        case KEY_HTTP_STATUS:
            if (!cbor_value_is_integer(&map)) return 1;
            err = cbor_value_get_int(&map, http_status);
            if (err) {
                DEBUG("cbor_value_get_int failed\n");
                return 1;
            }
            break;
        case KEY_CONTENT_TYPE:
            if (!cbor_value_is_integer(&map)) return 1;
            err = cbor_value_get_int(&map, &tmp_int);
            if (err) {
                DEBUG("cbor_value_get_int failed\n");
                return 1;
            }
            *content_type = (rest_client_content_type_t)tmp_int;
            break;
        case KEY_BODY:
            if (tmp_int < 0) {
                DEBUG("content type unknown\n");
                return 1;
            }
            else {
                switch (*content_type) {
                case REST_CLIENT_CONTENT_TYPE_APPLICATION_CBOR:
                    if (!cbor_value_is_byte_string(&map)) return 1;
                    tmp_size = *cbor_payload_buf_size;
                    err = cbor_value_copy_byte_string(&map, cbor_payload_buf, cbor_payload_buf_size, &map);
                    if (err || tmp_size == *cbor_payload_buf_size) {
                        DEBUG("null-termination is missing\n");
                        return 1;
                    }
                    break;
                case REST_CLIENT_CONTENT_TYPE_TEXT_PLAIN:
                    if (!cbor_value_is_text_string(&map)) return 1;
                    tmp_size = *cbor_payload_buf_size;
                    err = cbor_value_copy_text_string(&map, (char *)cbor_payload_buf, cbor_payload_buf_size, &map);
                    if (err || tmp_size == *cbor_payload_buf_size) {
                        DEBUG("null-termination is missing\n");
                        return 1;
                    }
                    break;
                default:
                    DEBUG("unsupported content type value\n");
                    return 1;
                }
            }

            break;
        default:
            DEBUG("unexpected value\n");
            return 1;
        }

        if (i != map_size - 1) {
            /* advance to the next key */
            err = cbor_value_advance(&map);
            if (err) {
                DEBUG("cbor_value_advance failed\n");
                return 1;
            }
        }
    }

    return 0;
}
