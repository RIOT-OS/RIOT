/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_mqtt
 * @{
 *
 * @file
 * @brief       Internal functions for MQTT-based transport modules
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
*/

#ifndef REST_CLIENT_INTERNAL_MQTT_H
#define REST_CLIENT_INTERNAL_MQTT_H

#include "uuid.h"

#include "rest_client.h"

/**
 * @brief   Struct for custom MQTT payload, including header fields and payload
 */
typedef struct {
    uuid_t *request_id;
    rest_client_content_type_t content_type;
    rest_client_content_type_t accept;
    rest_client_content_type_t target_content_type;
    rest_client_content_type_t target_accept;
    char *authorization;
    char *query_string;
    uint8_t *body;
    int body_len;
} _mqtt_payload_t;

/**
 * @brief   Create a request topic string from access method and path.
 *          Example request topic: GET/api/user/profile
 *
 * @param[out] buf      Output buffer
 * @param[in]  buf_len  Output buffer size
 * @param[in]  method   Access method (like GET or POST)
 * @param[in]  path     Resource path
 */
void _rest_client_internal_mqtt_build_mqtt_request_topic(
        char *buf, size_t buf_len, rest_client_method_t method, const char *path);

/**
 * @brief   Create response topic from access method, path and a uuid
 *          Example response topic: GET/api/user/profile
 *
 * @param[out] buf      Output buffer
 * @param[in]  buf_len  Output buffer size
 * @param[in]  method   Access method (like GET or POST)
 * @param[in]  path     Resource path
 * @param[in]  uuid     Unique request ID
 */
void _rest_client_internal_mqtt_build_mqtt_response_topic(
        char *buf, size_t buf_len, rest_client_method_t method, const char *path, const char *uuid);

/**
 * @brief   Encode custom MQTT payload
 *
 * @param[out] mqtt_payload     MQTT payload struct
 * @param[in]  encoder_buf      Encoder buffer
 * @param[in]  encoder_buf_len  Encoder buffer size
 *
 * @return  Length of encoded data
 */
size_t _rest_client_internal_mqtt_encode_mqtt_payload(
        _mqtt_payload_t *mqtt_payload, uint8_t *encoder_buf, size_t encoder_buf_len);

/**
 * @brief   Parse custom MQTT payload
 *
 * @param[in] buf                   Input buffer
 * @param[in] buf_size              Input buffer size
 * @param[in] http_status           HTTP status
 * @param[in] content_type          Content type
 * @param[in] cbor_payload_buf      Buffer for extracted body
 * @param[in] cbor_payload_buf_size Length of the extracted body
 *
 * @retval  0   On success
 * @retval  1   On error
 */
int _rest_client_internal_mqtt_parse_mqtt_payload(
        const uint8_t *buf, size_t buf_size,
        int *http_status, rest_client_content_type_t *content_type,
        uint8_t *cbor_payload_buf, size_t *cbor_payload_buf_size);

#endif /* REST_CLIENT_INTERNAL_MQTT_H */
