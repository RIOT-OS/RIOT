/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_transport_mqttsn
 * @{
 *
 * @file
 * @brief       REST client transport module for MQTT-SN
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#ifndef REST_CLIENT_TRANSPORT_MQTTSN_H
#define REST_CLIENT_TRANSPORT_MQTTSN_H

#include "rest_client.h"

rest_client_result_t rest_client_transport_mqttsn_send(
        rest_client_t *rest_client, rest_client_response_listener_t *listener,
        rest_client_method_t method, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len);

rest_client_result_t rest_client_transport_mqttsn_init(rest_client_t *rest_client);
rest_client_result_t rest_client_transport_mqttsn_deinit(rest_client_t *rest_client);
rest_client_result_t rest_client_transport_mqttsn_connect(rest_client_t *rest_client);
rest_client_result_t rest_client_transport_mqttsn_disconnect(rest_client_t *rest_client);


#endif /* REST_CLIENT_TRANSPORT_MQTTSN_H */
