/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_mqttsn MQTT-SN Defines
 * @ingroup     net
 * @brief       Shared definitions for MQTT-SN
 *
 * @see         http://mqtt.org/new/wp-content/uploads/2009/06/MQTT-SN_spec_v1.2.pdf
 *
 * @{
 * @file
 * @brief       Generic MQTT-SN definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    The client ID must contain 1-23 characters
 *
 * @see MQTT-SN spec v1.2, section 5.3.1
 * @{
 */
#define MQTTSN_CLI_ID_MINLEN    (1U)
#define MQTTSN_CLI_ID_MAXLEN    (23U)
/** @} */

/**
 * @brief   MQTT-SN flags
 *
 * All MQTT-SN functions only support a sub-set of the available flags. It is up
 * to the user to only supply valid/supported flags to a function.
 *
 * Refer to the MQTT-SN spec section 5.3.4 for further information.
 */
enum {
    MQTTSN_DUP        = 0x80,       /**< duplicate flag */
    MQTTSN_QOS_MASK   = 0x60,       /**< QoS level mask */
    MQTTSN_QOS_2      = 0x40,       /**< QoS level 2 */
    MQTTSN_QOS_1      = 0x20,       /**< QoS level 1 */
    MQTTSN_QOS_NEG1   = 0x60,       /**< QoS level -1 (negative 1) */
    MQTTSN_QOS_0      = 0x00,       /**< QoS level 0 */
    MQTTSN_RETAIN     = 0x10,       /**< retain flag */
    MQTTSN_WILL       = 0x08,       /**< will flag, used during CONNECT */
    MQTTSN_CS         = 0x04,       /**< clean session flag */
    MQTTSN_TIT_MASK   = 0x03,       /**< topic ID type mask */
    MQTTSN_TIT_SHORT  = 0x02,       /**< topic ID: short */
    MQTTSN_TIT_PREDEF = 0x01,       /**< topic ID: pre-defined */
    MQTTSN_TIT_NORMAL = 0x00,       /**< topic ID: normal */
};

/**
 * @brief   MQTT-SN message types
 */
enum {
    MQTTSN_ADVERTISE        = 0x00,     /**< advertise message */
    MQTTSN_SEARCHGW         = 0x01,     /**< search gateway message */
    MQTTSN_GWINFO           = 0x02,     /**< gateway info message */
    MQTTSN_CONNECT          = 0x04,     /**< connect message */
    MQTTSN_CONNACK          = 0x05,     /**< connection acknowledgment message */
    MQTTSN_WILLTOPICREQ     = 0x06,     /**< will topic request */
    MQTTSN_WILLTOPIC        = 0x07,     /**< will topic */
    MQTTSN_WILLMSGREQ       = 0x08,     /**< will message request */
    MQTTSN_WILLMSG          = 0x09,     /**< will message */
    MQTTSN_REGISTER         = 0x0a,     /**< topic registration request */
    MQTTSN_REGACK           = 0x0b,     /**< topic registration acknowledgment */
    MQTTSN_PUBLISH          = 0x0c,     /**< publish message */
    MQTTSN_PUBACK           = 0x0d,     /**< publish acknowledgment */
    MQTTSN_PUBCOMP          = 0x0e,     /**< publish received (QoS 2) */
    MQTTSN_PUBREC           = 0x0f,     /**< publish complete (QoS 2) */
    MQTTSN_PUBREL           = 0x10,     /**< publish release (QoS 2) */
    MQTTSN_SUBSCRIBE        = 0x12,     /**< subscribe message */
    MQTTSN_SUBACK           = 0x13,     /**< subscription acknowledgment */
    MQTTSN_UNSUBSCRIBE      = 0x14,     /**< unsubscribe message */
    MQTTSN_UNSUBACK         = 0x15,     /**< unsubscription acknowledgment */
    MQTTSN_PINGREQ          = 0x16,     /**< ping request */
    MQTTSN_PINGRESP         = 0x17,     /**< ping response */
    MQTTSN_DISCONNECT       = 0x18,     /**< disconnect message */
    MQTTSN_WILLTOPICUPD     = 0x1a,     /**< will topic update request */
    MQTTSN_WILLTOPICRESP    = 0x1b,     /**< will topic update response */
    MQTTSN_WILLMSGUPD       = 0x1c,     /**< will message update request */
    MQTTSN_WILLMSGRESP      = 0x1d,     /**< will topic update response */
};

/**
 * @brief   MQTT-SN return codes
 *
 * @see     MQTT-SN spec v1.2, section 5.3.10, table 5
 */
enum {
    MQTTSN_ACCEPTED         = 0x00,     /**< accepted */
    MQTTSN_REJ_CONGESTION   = 0x01,     /**< rejected: congestion */
    MQTTSN_REJ_INV_TOPIC_ID = 0x02,     /**< rejected: invalid topic id */
    MQTTSN_REJ_NOTSUP       = 0x03,     /**< rejected: not supported */
};

#ifdef __cplusplus
}
#endif

/** @} */
