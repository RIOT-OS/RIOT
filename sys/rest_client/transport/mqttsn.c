/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @defgroup    sys_rest_client_transport_mqttsn REST client MQTT-SN transport
 * @ingroup     sys_rest_client
 * @brief       REST client communicating over MQTT-SN
 *
 * @{
 * @file
 * @brief       REST client communicating over MQTT-SN
 * @note        Code was derived from the wolfMQTT repository (see https://github.com/wolfSSL/wolfMQTT/blob/master/examples/sn-client/sn-client.c)
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include "cbor.h"
#include "uuid.h"

#include "wolfmqtt/mqtt_types.h"
#include "wolfmqtt/mqtt_client.h"

#include "rest_client/util.h"
#include "rest_client/internal/mqtt.h"
#include "rest_client/mqttnet.h"
#include "rest_client/transport/mqttsn.h"

#include "rest_client.h"

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"

/* Locals */
static int _stop_read = 0;

/* Maximum size for network read/write callbacks. */
#define MAX_BUFFER_SIZE     1024

static int _sn_message_cb(MqttClient *client, MqttMessage *msg, uint8_t msg_new, uint8_t msg_done)
{
#if ENABLE_DEBUG
    uint8_t buf[CONFIG_REST_CLIENT_MQTT_PRINT_BUFFER_SIZE + 1];
    uint32_t len;

    if (msg_new) {
        /* Determine min size to dump */
        len = msg->topic_name_len;
        if (len > CONFIG_REST_CLIENT_MQTT_PRINT_BUFFER_SIZE) {
            len = CONFIG_REST_CLIENT_MQTT_PRINT_BUFFER_SIZE;
        }
        XMEMCPY(buf, msg->topic_name, len);
        buf[len] = '\0'; /* Make sure its null terminated */

        /* Print incoming message */
        DEBUG("MQTT-SN Message: Topic %s, Qos %d, Len %u\n", buf, msg->qos, msg->total_len);

        /* Todo: Stops to listen for responses after the first response, which protects the client
         *       from too many responses, but may not be the desired behaviour */
        _stop_read = 1;
    }

    /* Print message payload */
    DEBUG("........Payload (MQTT; CBOR)\n");
    for (size_t i = 0; i < msg->buffer_len; i++) {
        DEBUG("%02x", msg->buffer[i]);
    }
    DEBUG("\n");

    if (msg_done) {
        DEBUG("....MQTT-SN Message: Done\n");
    }
#else
    (void)msg_done;

    if (msg_new) {
        /* Todo: Stops to listen for responses after the first response, which protects the client
         *       from too many responses, but may not be the desired behaviour */
        _stop_read = 1;
    }
#endif

    rest_client_mqtt_context_t *mqtt_context = (rest_client_mqtt_context_t *)client->ctx;

    assert(mqtt_context->listener != NULL);

    mqtt_context->listener->body_len = mqtt_context->listener->body_max_len;

    int rc = _rest_client_internal_mqtt_parse_mqtt_payload(
            msg->buffer, msg->buffer_len,
            &(mqtt_context->listener->status_code), &(mqtt_context->listener->content_type),
            mqtt_context->listener->body_buf, &(mqtt_context->listener->body_len));

    assert(rc == 0);
    (void)rc;

    mqtt_context->listener->listener(mqtt_context->listener);

    return MQTT_CODE_SUCCESS;
}

static int _wolfmqtt_exit(rest_client_mqtt_context_t *mqtt_context)
{
    /* Free resources */
    if (mqtt_context->tx_buf) WOLFMQTT_FREE(mqtt_context->tx_buf);
    if (mqtt_context->rx_buf) WOLFMQTT_FREE(mqtt_context->rx_buf);

    /* Cleanup network */
    mqttnet_deinit(&mqtt_context->net);

    MqttClient_DeInit(&mqtt_context->client);

    return MQTT_CODE_SUCCESS;
}

static int _register_topic_name(char *topic_name, uint16_t *topic_id, rest_client_mqtt_context_t *mqtt_context)
{
    uint16_t registered_id;
    /* Register topic name to get the assigned topic ID */
    SN_Register regist_s, *regist = &regist_s;

    XMEMSET(regist, 0, sizeof(SN_Register));
    regist->packet_id = mqtt_get_packetid();
    regist->topicName = topic_name;

    DEBUG("MQTT-SN Register: topic = %s\n", regist->topicName);
    int rc = SN_Client_Register(&mqtt_context->client, regist);

    if ((rc == MQTT_CODE_SUCCESS) && (regist->regack.return_code == SN_RC_ACCEPTED)) {
        /* Topic ID is returned in RegAck */
        registered_id = regist->regack.topicId;
        DEBUG("....MQTT-SN Register Ack: rc = %d, topic id = %d\n",
               regist->regack.return_code, regist->regack.topicId);
        *topic_id = registered_id;

        return MQTT_CODE_SUCCESS;
    }

    return rc;
}

static int _subscribe_on_topic_name(char *topic_name, uint16_t *topic_id, rest_client_mqtt_context_t *mqtt_context, MqttQoS qos)
{
    /* Subscribe Topic */
    SN_Subscribe subscribe;

    XMEMSET(&subscribe, 0, sizeof(SN_Subscribe));

    subscribe.duplicate = 0;
    subscribe.qos = qos;
    subscribe.topic_type = SN_TOPIC_ID_TYPE_NORMAL;
    subscribe.topicNameId = topic_name;
    subscribe.packet_id = mqtt_get_packetid();

    DEBUG("MQTT-SN Subscribe: topic name = %s\n", subscribe.topicNameId);
    int rc = SN_Client_Subscribe(&mqtt_context->client, &subscribe);

    DEBUG("....MQTT-SN Subscribe Ack: topic id = %d, rc = %d\n",
           subscribe.subAck.topicId, subscribe.subAck.return_code);

    if ((rc == MQTT_CODE_SUCCESS) && (subscribe.subAck.return_code == SN_RC_ACCEPTED)) {
        /* Topic ID is returned in SubAck */
        *topic_id = subscribe.subAck.topicId;
        return MQTT_CODE_SUCCESS;
    }

    return rc;
}

static int _publish_by_topic_id(
        uint16_t topic_id, uint8_t *message, uint16_t message_len, rest_client_mqtt_context_t *mqtt_context, MqttQoS qos)
{
    /* Publish Topic */
    XMEMSET(&mqtt_context->publish_sn, 0, sizeof(SN_Publish));
    mqtt_context->publish_sn.retain = 0;
    mqtt_context->publish_sn.qos = qos;
    mqtt_context->publish_sn.duplicate = 0;
    mqtt_context->publish_sn.topic_type = SN_TOPIC_ID_TYPE_NORMAL;

    mqtt_context->publish_sn.topic_name = (char *)&topic_id;
    if (mqtt_context->publish_sn.qos > MQTT_QOS_0) {
        mqtt_context->publish_sn.packet_id = mqtt_get_packetid();
    }
    else {
        mqtt_context->publish_sn.packet_id = 0x00;
    }

    mqtt_context->publish_sn.buffer = message;
    mqtt_context->publish_sn.total_len = message_len;

    int rc = SN_Client_Publish(&mqtt_context->client, &mqtt_context->publish_sn);

    DEBUG("MQTT-SN Publish: topic id = %d, rc = %d\n",
           (word16)*mqtt_context->publish_sn.topic_name,
           mqtt_context->publish_sn.return_code);

    return rc;
}

rest_client_result_t rest_client_transport_mqttsn_send(
        rest_client_t *rest_client, rest_client_response_listener_t *listener,
        rest_client_method_t method, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len)
{
    uuid_t request_uuid;

    _stop_read = 0;

    DEBUG("MQTT-SN Client: QoS %d\n", qos);

    _mqtt_payload_t mqtt_payload = {
        .request_id = NULL,
        .content_type = REST_CLIENT_CONTENT_TYPE_NOT_SET,
        .accept = REST_CLIENT_CONTENT_TYPE_NOT_SET,
        .target_content_type = REST_CLIENT_CONTENT_TYPE_NOT_SET,
        .target_accept = REST_CLIENT_CONTENT_TYPE_NOT_SET,
        .authorization = NULL,
        .query_string = query_string,
        .body = body,
        .body_len = body_len,
    };

    int i = 0;
    while (headers[i].key != REST_CLIENT_HEADERS_END) {

        switch (headers[i].key) {
        case REST_CLIENT_HEADER_CONTENT_TYPE:
            mqtt_payload.content_type = (rest_client_content_type_t)headers[i].value;
            break;
        case REST_CLIENT_HEADER_ACCEPT:
            mqtt_payload.accept = (rest_client_content_type_t)headers[i].value;
            break;
        case REST_CLIENT_HEADER_TARGET_CONTENT_TYPE:
            mqtt_payload.target_content_type = (rest_client_content_type_t)headers[i].value;
            break;
        case REST_CLIENT_HEADER_TARGET_ACCEPT:
            mqtt_payload.target_accept = (rest_client_content_type_t)headers[i].value;
            break;
        case REST_CLIENT_HEADER_AUTHORIZATION:
            mqtt_payload.authorization = (char *)headers[i].value;
            break;
        case REST_CLIENT_HEADERS_END:
            continue;
        /* no default case in order to get an error when we forgot to handle an enum value */
        }

        i++;
    }

    int rc;

    char request_topic[128];
    _rest_client_internal_mqtt_build_mqtt_request_topic(request_topic, sizeof(request_topic), method, path);

    char response_topic[128];

    if (listener != NULL) {
        uuid_v4(&request_uuid);
        mqtt_payload.request_id = &request_uuid;

        char uuid_buf[UUID_STR_LEN + 1];
        uuid_to_string(&request_uuid, uuid_buf);
        _rest_client_internal_mqtt_build_mqtt_response_topic(response_topic, sizeof(response_topic), method, path, uuid_buf);
    }

    uint8_t buf[256];
    size_t buf_count = _rest_client_internal_mqtt_encode_mqtt_payload(&mqtt_payload, buf, sizeof(buf));

    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);
    mqtt_context->listener = listener;

    uint16_t publish_topic_id = 0;
    uint16_t subscribe_topic_id = 0;

    rc = _register_topic_name(request_topic, &publish_topic_id, mqtt_context);
    if (rc != MQTT_CODE_SUCCESS) {
        return REST_CLIENT_ERROR_CONNECTION;
    }

    if (listener != NULL) {
        rc = _subscribe_on_topic_name(response_topic, &subscribe_topic_id, mqtt_context, (MqttQoS)qos);
        if (rc != MQTT_CODE_SUCCESS) {
            return REST_CLIENT_ERROR_CONNECTION;
        }
    }

    rc = _publish_by_topic_id(publish_topic_id, buf, buf_count, mqtt_context, (MqttQoS)qos);
    if (rc != MQTT_CODE_SUCCESS) {
        return REST_CLIENT_ERROR_CONNECTION;
    }

    if (listener != NULL) {
        /* Read Loop */
        do {
            /* check for test mode */
            if (_stop_read) {
                rc = MQTT_CODE_SUCCESS;
                DEBUG("MQTT Exiting...\n");
                break;
            }

            /* Try and read packet */
            DEBUG("MQTT Waiting for message...\n");
            rc = SN_Client_WaitMessage(&mqtt_context->client, CONFIG_REST_CLIENT_MQTT_CMD_TIMEOUT_MS);

            /* check for test mode */
            if (_stop_read) {
                rc = MQTT_CODE_SUCCESS;
                DEBUG("MQTT Exiting...\n");
                break;
            }
            else if (rc == MQTT_CODE_ERROR_TIMEOUT) {
                /* Keep Alive */
                DEBUG("Keep-alive timeout, sending ping\n");

                rc = SN_Client_Ping(&mqtt_context->client, NULL);
                if (rc != MQTT_CODE_SUCCESS) {
                    DEBUG("MQTT Ping Keep Alive Error: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
                    break;
                }
            }
            else if (rc != MQTT_CODE_SUCCESS) {
                /* There was an error */
                DEBUG("MQTT-SN Message Wait Error: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
                break;
            }
            /* Todo: check cancelled state after timeout (e.g. with _stop_read) */
        } while (1);

        /* Check for error */
        if (rc != MQTT_CODE_SUCCESS) {
            return REST_CLIENT_ERROR_CONNECTION;
        }

        /* Unsubscribe Topics */
        SN_Unsubscribe unsubscribe;
        XMEMSET(&unsubscribe, 0, sizeof(SN_Unsubscribe));
        unsubscribe.topicNameId = response_topic;
        unsubscribe.packet_id = mqtt_get_packetid();
        rc = SN_Client_Unsubscribe(&mqtt_context->client, &unsubscribe);

        DEBUG("MQTT Unsubscribe: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
        if (rc != MQTT_CODE_SUCCESS) {
            return REST_CLIENT_ERROR_CONNECTION;
        }
    }

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqttsn_init(rest_client_t *rest_client)
{
    int rc = MQTT_CODE_SUCCESS;

    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);

    /* init defaults */
    XMEMSET(mqtt_context, 0, sizeof(rest_client_mqtt_context_t));
    mqtt_context->app_name = "rest_client_transport_mqttsn";
    mqtt_context->host = rest_client->mqttsn_gateway->hostname;
    mqtt_context->is_numeric_host = rest_client->mqttsn_gateway->is_numeric_hostname;
    mqtt_context->addr = &rest_client->mqttsn_gateway->_addr;
    mqtt_context->port = rest_client->mqttsn_gateway->port;
#ifdef WOLFMQTT_CLIENT_ID
    mqtt_context->clean_session = 1; /* Todo: why ??? */
    mqtt_context->client_id = CONFIG_REST_CLIENT_MQTT_CLIENT_ID;
#endif

    /* Initialize Network */
    rc = mqttnet_sn_init(&mqtt_context->net, mqtt_context);
    DEBUG("MQTT-SN Net Init: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        _wolfmqtt_exit(mqtt_context);
        return REST_CLIENT_ERROR_CONNECTION;
    }

    /* setup tx/rx buffers */
    mqtt_context->tx_buf = (uint8_t *)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);
    mqtt_context->rx_buf = (uint8_t *)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);

    /* Initialize MqttClient structure */
    rc = MqttClient_Init(&mqtt_context->client, &mqtt_context->net,
                         _sn_message_cb,
                         mqtt_context->tx_buf, MAX_BUFFER_SIZE,
                         mqtt_context->rx_buf, MAX_BUFFER_SIZE,
                         CONFIG_REST_CLIENT_MQTT_CMD_TIMEOUT_MS);

    DEBUG("MQTT-SN Init: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        _wolfmqtt_exit(mqtt_context);
        return REST_CLIENT_ERROR_CONNECTION;
    }

    /* The client.ctx will be used in _sn_message_cb */
    mqtt_context->client.ctx = mqtt_context;

    /* Setup socket direct to gateway (UDP network, so no TLS) */
    rc = MqttClient_NetConnect(&mqtt_context->client, mqtt_context->host,
                               mqtt_context->port, CONFIG_REST_CLIENT_MQTT_CON_TIMEOUT_MS,
                               0, NULL);

    DEBUG("MQTT-SN Socket Connect: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        _wolfmqtt_exit(mqtt_context);
        return REST_CLIENT_ERROR_CONNECTION;
    }

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqttsn_deinit(rest_client_t *rest_client)
{
    /* Todo: Why was this not included in sn-client from wolfMQTT pkg */
    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);
    mqtt_free_ctx(mqtt_context);

    /* Todo: shouldn't be _wolfmqtt_exit called here too? */

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqttsn_connect(rest_client_t *rest_client)
{
    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);

    int rc;

    SN_Connect connect_s, *connect = &connect_s;
    /* Build connect packet */
    XMEMSET(connect, 0, sizeof(SN_Connect));
    connect_s.keep_alive_sec = CONFIG_REST_CLIENT_MQTT_DEFAULT_KEEP_ALIVE_SEC;
#ifdef WOLFMQTT_CLIENT_ID
    connect_s.clean_session = mqtt_context->clean_session;
    connect_s.client_id = mqtt_context->client_id;
#else
    connect_s.clean_session = 1; /* must be 1 when client_id is empty */
    connect_s.client_id = "";
#endif
    connect->protocol_level = SN_PROTOCOL_ID;

    DEBUG("MQTT-SN Connect: gateway = %s : %d\n", mqtt_context->host, mqtt_context->port);
    /* Send Connect and wait for Connect Ack */
    rc = SN_Client_Connect(&mqtt_context->client, connect);

    /* Validate Connect Ack info */
    DEBUG("....MQTT-SN Connect Ack: Return Code %u\n", mqtt_context->connect.ack.return_code);
    if (rc != MQTT_CODE_SUCCESS) {
        return REST_CLIENT_ERROR_CONNECTION;
    }

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqttsn_disconnect(rest_client_t *rest_client)
{
    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);

    /* Disconnect */
    int rc = SN_Client_Disconnect(&mqtt_context->client);

    DEBUG("MQTT Disconnect: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        return rc;
    }

    rc = MqttClient_NetDisconnect(&mqtt_context->client);

    DEBUG("MQTT Socket Disconnect: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);

    return REST_CLIENT_RESULT_OK;
}
