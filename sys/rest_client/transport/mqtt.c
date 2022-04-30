/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @defgroup    sys_rest_client_transport_mqtt REST client MQTT transport
 * @ingroup     sys_rest_client
 * @brief       REST client communicating over MQTT
 *
 * @{
 * @file
 * @brief       REST client communicating over MQTT
 * @note        Code was derived from the wolfMQTT repository (see https://github.com/wolfSSL/wolfMQTT/blob/master/examples/mqttclient/mqttclient.c)
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
#include "rest_client/transport/mqtt.h"

#include "rest_client.h"

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"

/* Locals */
static int _stop_read = 0;

/* Maximum size for network read/write callbacks. There is also a v5 define that
   describes the max MQTT control packet size, DEFAULT_MAX_PKT_SZ. */
#define MAX_BUFFER_SIZE 1024

static int _mqtt_message_cb(MqttClient *client, MqttMessage *msg, uint8_t msg_new, uint8_t msg_done)
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
        DEBUG("MQTT Message: Topic %s, Qos %d, Len %u\n", buf, msg->qos, msg->total_len);

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
        DEBUG("MQTT Message: Done\n");
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

rest_client_result_t rest_client_transport_mqtt_send(
        rest_client_t *rest_client, rest_client_response_listener_t *listener,
        rest_client_method_t method, rest_client_qos_t qos,
        rest_client_header_t *headers, char *path, char *query_string, uint8_t *body, int body_len)
{
    uuid_t request_uuid;

    _stop_read = 0;

    DEBUG("MQTT Client: QoS %d\n", qos);

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

    if (listener != NULL) {
        /* Build list of topics */
        XMEMSET(&mqtt_context->subscribe, 0, sizeof(MqttSubscribe));

        mqtt_context->topics[0].topic_filter = response_topic;
        mqtt_context->topics[0].qos = (MqttQoS)qos;

        /* Subscribe Topic */
        mqtt_context->subscribe.packet_id = mqtt_get_packetid();
        mqtt_context->subscribe.topic_count =
                sizeof(mqtt_context->topics) / sizeof(MqttTopic);
        mqtt_context->subscribe.topics = mqtt_context->topics;

        rc = MqttClient_Subscribe(&mqtt_context->client, &mqtt_context->subscribe);

        DEBUG("MQTT Subscribe: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
        if (rc != MQTT_CODE_SUCCESS) {
            return REST_CLIENT_ERROR_CONNECTION;
        }

        /* show subscribe results */
        for (int i = 0; i < mqtt_context->subscribe.topic_count; i++) {
            MqttTopic *topic = &mqtt_context->subscribe.topics[i];
            DEBUG("  Topic %s, Qos %u, Return Code %u\n",
                  topic->topic_filter,
                  topic->qos, topic->return_code);
        }
    }

    /* Publish Topic */
    XMEMSET(&mqtt_context->publish, 0, sizeof(MqttPublish));
    mqtt_context->publish.retain = 0;
    mqtt_context->publish.qos = (MqttQoS)qos;
    mqtt_context->publish.duplicate = 0;
    mqtt_context->publish.topic_name = request_topic;
    mqtt_context->publish.packet_id = mqtt_get_packetid();
    mqtt_context->publish.buffer = (uint8_t *)buf;
    mqtt_context->publish.total_len = (uint16_t)buf_count;

    rc = MqttClient_Publish(&mqtt_context->client, &mqtt_context->publish);

    DEBUG("MQTT Publish: Topic %s, %s (%d)\n", mqtt_context->publish.topic_name,
           MqttClient_ReturnCodeToString(rc), rc);

    if (rc != MQTT_CODE_SUCCESS) {
        return REST_CLIENT_ERROR_CONNECTION;
    }

    if (listener != NULL) {
        /* Read Loop */
        DEBUG("MQTT Waiting for message...\n");

        do {
            /* Try and read packet */
            rc = MqttClient_WaitMessage(&mqtt_context->client, CONFIG_REST_CLIENT_MQTT_CMD_TIMEOUT_MS);

    #ifdef WOLFMQTT_NONBLOCK
            /* Track elapsed time with no activity and trigger timeout */
            rc = mqtt_check_timeout(rc, &mqtt_context->start_sec, (CONFIG_REST_CLIENT_MQTT_CMD_TIMEOUT_MS / 1000));
    #endif

            /* check for test mode */
            if (_stop_read) {
                rc = MQTT_CODE_SUCCESS;
                DEBUG("MQTT Exiting...\n");
                break;
            }
            else if (rc == MQTT_CODE_ERROR_TIMEOUT) {
                /* Keep Alive */
                DEBUG("Keep-alive timeout, sending ping\n");

                rc = MqttClient_Ping(&mqtt_context->client);
                if (rc != MQTT_CODE_SUCCESS) {
                    DEBUG("MQTT Ping Keep Alive Error: %s (%d)\n",
                          MqttClient_ReturnCodeToString(rc), rc);
                    break;
                }
            }
            else if (rc != MQTT_CODE_SUCCESS) {
                /* There was an error */
                DEBUG("MQTT Message Wait: %s (%d)\n",
                      MqttClient_ReturnCodeToString(rc), rc);
                break;
            }
        } while (1);

        /* Check for error */
        if (rc != MQTT_CODE_SUCCESS) {
            return REST_CLIENT_ERROR_CONNECTION;
        }

        /* Unsubscribe Topics */
        XMEMSET(&mqtt_context->unsubscribe, 0, sizeof(MqttUnsubscribe));
        mqtt_context->unsubscribe.packet_id = mqtt_get_packetid();
        mqtt_context->unsubscribe.topic_count =
                sizeof(mqtt_context->topics) / sizeof(MqttTopic);
        mqtt_context->unsubscribe.topics = mqtt_context->topics;

        /* Unsubscribe Topics */
        rc = MqttClient_Unsubscribe(&mqtt_context->client,
                                    &mqtt_context->unsubscribe);

        DEBUG("MQTT Unsubscribe: %s (%d)\n",
              MqttClient_ReturnCodeToString(rc), rc);
        if (rc != MQTT_CODE_SUCCESS) {
            return REST_CLIENT_ERROR_CONNECTION;
        }
    }

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqtt_init(rest_client_t *rest_client)
{
    int rc = MQTT_CODE_SUCCESS;

    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);
    XMEMSET(mqtt_context, 0, sizeof(rest_client_mqtt_context_t));
    mqtt_context->app_name = "rest_client_transport_mqtt";
    mqtt_context->host = rest_client->hostname;
    mqtt_context->is_numeric_host = rest_client->is_numeric_hostname;
    mqtt_context->addr = &rest_client->_addr;
    mqtt_context->port = rest_client->port;
#ifdef WOLFMQTT_CLIENT_ID
    mqtt_context->clean_session = 1;
    mqtt_context->client_id = CONFIG_REST_CLIENT_MQTT_CLIENT_ID;
#endif

    /* Initialize Network */
    rc = mqttnet_init(&mqtt_context->net, mqtt_context);
    DEBUG("MQTT Net Init: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        _wolfmqtt_exit(mqtt_context);
        return REST_CLIENT_ERROR_CONNECTION;
    }

    /* setup tx/rx buffers */
    mqtt_context->tx_buf = (uint8_t *)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);
    mqtt_context->rx_buf = (uint8_t *)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);

    /* Initialize MqttClient structure */
    rc = MqttClient_Init(&mqtt_context->client, &mqtt_context->net, _mqtt_message_cb,
                         mqtt_context->tx_buf, MAX_BUFFER_SIZE,
                         mqtt_context->rx_buf, MAX_BUFFER_SIZE,
                         CONFIG_REST_CLIENT_MQTT_CMD_TIMEOUT_MS);

    DEBUG("MQTT Init: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        _wolfmqtt_exit(mqtt_context);
        return REST_CLIENT_ERROR_CONNECTION;
    }
    /* The client.ctx will be stored in the cert callback ctx during
       MqttSocket_Connect for use by mqtt_tls_verify_cb */
    mqtt_context->client.ctx = mqtt_context;

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqtt_deinit(rest_client_t *rest_client)
{
    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);
    mqtt_free_ctx(mqtt_context);

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqtt_connect(rest_client_t *rest_client)
{
    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);

    DEBUG("MqttClient_NetConnect\n");
    /* Connect to broker */
    int rc = MqttClient_NetConnect(&mqtt_context->client, mqtt_context->host,
                                   mqtt_context->port,
                                   CONFIG_REST_CLIENT_MQTT_CON_TIMEOUT_MS, false, NULL);

    DEBUG("MQTT Socket Connect: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        _wolfmqtt_exit(mqtt_context);
        return REST_CLIENT_ERROR_CONNECTION;
    }

    /* Build connect packet */
    XMEMSET(&mqtt_context->connect, 0, sizeof(MqttConnect));
    mqtt_context->connect.keep_alive_sec = CONFIG_REST_CLIENT_MQTT_DEFAULT_KEEP_ALIVE_SEC;
#ifdef WOLFMQTT_CLIENT_ID
    mqtt_context->connect.clean_session = mqtt_context->clean_session;
    mqtt_context->connect.client_id = mqtt_context->client_id;
#else
    mqtt_context->connect.clean_session = 1; /* must be 1 when client_id is empty */
    mqtt_context->connect.client_id = "";
#endif

    /* Optional authentication */
    mqtt_context->connect.username = mqtt_context->username;
    mqtt_context->connect.password = mqtt_context->password;

    /* Send Connect and wait for Connect Ack */
    rc = MqttClient_Connect(&mqtt_context->client, &mqtt_context->connect);

    DEBUG("MQTT Connect: Proto (%s), %s (%d)\n",
          MqttClient_GetProtocolVersionString(&mqtt_context->client),
          MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        return REST_CLIENT_ERROR_CONNECTION;
    }

    /* Validate Connect Ack info */
    DEBUG("MQTT Connect Ack: Return Code %u, Session Present %d\n",
          mqtt_context->connect.ack.return_code,
          (mqtt_context->connect.ack.flags &
          MQTT_CONNECT_ACK_FLAG_SESSION_PRESENT) ? 1 : 0);

    return REST_CLIENT_RESULT_OK;
}

rest_client_result_t rest_client_transport_mqtt_disconnect(rest_client_t *rest_client)
{
    rest_client_mqtt_context_t *mqtt_context = &(rest_client->mqtt_context);

    /* Disconnect */
    int rc = MqttClient_Disconnect_ex(&mqtt_context->client, &mqtt_context->disconnect);

    DEBUG("MQTT Disconnect: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        return rc;
    }

    rc = MqttClient_NetDisconnect(&mqtt_context->client);

    DEBUG("MQTT Socket Disconnect: %s (%d)\n", MqttClient_ReturnCodeToString(rc), rc);

    _wolfmqtt_exit(mqtt_context);

    return REST_CLIENT_RESULT_OK;
}
