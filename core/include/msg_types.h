/*
 * Copyright (C) 2014 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_msg
 *
 * @{
 *
 * @file        msg_types.h
 * @brief       Global definitions of all message types
 *
 * This is the global list of all defined message types in RIOT.
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef __MSG_TYPES_H_
#define __MSG_TYPES_H_

/*
 * This list is sorted alphabetically by module name.
 * Within a module, message types are sorted alphabetically.
 * The message declaration format is MT_<MODULE>_<NAME>.
 */
typedef enum {
    /* core message type: */
    MT_MSG_UNDEF = 0,                   /**< undefined message type */

#ifdef MODULE_PIR
    MT_PIR_STATUS_HI,                   /**< motion was detected */
    MT_PIR_STATUS_LO,                   /**< no motion is detected */
#endif

#ifdef MODULE_TRANSCEIVER
    MT_TRANSCEIVER_RCV_PKT_CC1020,      /**< packet was received by CC1020 transceiver */
    MT_TRANSCEIVER_RCV_PKT_CC1100,      /**< packet was received by CC1100 transceiver */
    MT_TRANSCEIVER_RCV_PKT_CC2420,      /**< packet was received by CC2420 transceiver */
    MT_TRANSCEIVER_RCV_PKT_MC1322X,     /**< packet was received by mc1322x transceiver */
    MT_TRANSCEIVER_RCV_PKT_NATIVE,      /**< packet was received by native transceiver */
    MT_TRANSCEIVER_RCV_PKT_AT86RF231,   /**< packet was received by AT86RF231 transceiver */

    /* Message types for transceiver <-> upper layer communication */
    MT_TRANSCEIVER_PKT_PENDING,         /**< packet pending in transceiver buffer */
    MT_TRANSCEIVER_SND_PKT,             /**< request for sending a packet */
    MT_TRANSCEIVER_SND_ACK,             /**< request for sending an acknowledgement */
    MT_TRANSCEIVER_SWITCH_RX,           /**< switch transceiver to RX sate */
    MT_TRANSCEIVER_POWERDOWN,           /**< power down transceiver */
    MT_TRANSCEIVER_GET_CHANNEL,         /**< Get current channel */
    MT_TRANSCEIVER_SET_CHANNEL,         /**< Set a new channel */
    MT_TRANSCEIVER_GET_ADDRESS,         /**< Get the radio address */
    MT_TRANSCEIVER_SET_ADDRESS,         /**< Set the radio address */
    MT_TRANSCEIVER_GET_LONG_ADDR,       /**< Get the long radio address, if existing */
    MT_TRANSCEIVER_SET_LONG_ADDR,       /**< Set the long radio address, if supported by hardware */
    MT_TRANSCEIVER_SET_MONITOR,         /**< Set transceiver to monitor mode (disable address checking) */
    MT_TRANSCEIVER_GET_PAN,             /**< Get current pan */
    MT_TRANSCEIVER_SET_PAN,             /**< Set a new pan */

    /* debug message types */
    MT_TRANSCEIVER_DBG_IGN,             /**< add a physical address to the ignore list */

    /* Error messages */
    MT_TRANSCEIVER_ENOBUFFER,           /**< No buffer left */

    /* reserve message types for higher layer notifications */
    MT_TRANSCEIVER_UPPER_LAYER_1,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_2,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_3,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_4,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_5,       /**< reserved */
#endif

    /* core message type: */
    MT_MSG_MAX,                         /**< highest defined message type */
} msg_type_t;

#endif /* __MSG_TYPES_H_ */
/** @} */
