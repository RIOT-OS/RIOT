/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_emcute
 * @{
 *
 * @file
 * @brief       emCute internals
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef EMCUTE_INTERNAL_H
#define EMCUTE_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   MQTT-SN message types
 */
enum {
    ADVERTISE       = 0x00,     /**< advertise message */
    SEARCHGW        = 0x01,     /**< search gateway message */
    GWINFO          = 0x02,     /**< gateway info message */
    CONNECT         = 0x04,     /**< connect message */
    CONNACK         = 0x05,     /**< connection acknowledgment message */
    WILLTOPICREQ    = 0x06,     /**< will topic request */
    WILLTOPIC       = 0x07,     /**< will topic */
    WILLMSGREQ      = 0x08,     /**< will message request */
    WILLMSG         = 0x09,     /**< will message */
    REGISTER        = 0x0a,     /**< topic registration request */
    REGACK          = 0x0b,     /**< topic registration acknowledgment */
    PUBLISH         = 0x0c,     /**< publish message */
    PUBACK          = 0x0d,     /**< publish acknowledgment */
    PUBCOMP         = 0x0e,     /**< publish received (QoS 2) */
    PUBREC          = 0x0f,     /**< publish complete (QoS 2) */
    PUBREL          = 0x10,     /**< publish release (QoS 2) */
    SUBSCRIBE       = 0x12,     /**< subscribe message */
    SUBACK          = 0x13,     /**< subscription acknowledgment */
    UNSUBSCRIBE     = 0x14,     /**< unsubscribe message */
    UNSUBACK        = 0x15,     /**< unsubscription acknowledgment */
    PINGREQ         = 0x16,     /**< ping request */
    PINGRESP        = 0x17,     /**< ping response */
    DISCONNECT      = 0x18,     /**< disconnect message */
    WILLTOPICUPD    = 0x1a,     /**< will topic update request */
    WILLTOPICRESP   = 0x1b,     /**< will topic update response */
    WILLMSGUPD      = 0x1c,     /**< will message update request */
    WILLMSGRESP     = 0x1d      /**< will topic update response */
};

/**
 * @brief   MQTT-SN return codes
 */
enum {
    ACCEPT          = 0x00,     /**< all good */
    REJ_CONG        = 0x01,     /**< reject, reason: congestions */
    REJ_INVTID      = 0x02,     /**< reject, reason: invalid topic ID */
    REJ_NOTSUP      = 0x03      /**< reject, reason: operation not supported */
};

#ifdef __cplusplus
}
#endif

#endif /* EMCUTE_INTERNAL_H */
/** @} */
