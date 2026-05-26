/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_can_isotp ISO transport protocol over CAN
 * @ingroup     sys_can
 * @brief       ISO transport protocol over CAN (ISO15765)
 * @{
 *
 * @file
 * @brief       ISO TP high level interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "can/can.h"
#include "can/common.h"
#include "thread.h"
#include "ztimer.h"
#include "net/gnrc/pktbuf.h"

#ifndef CAN_ISOTP_BS
/**
 * @brief   Default Block Size for RX Flow Control frames
 */
#define CAN_ISOTP_BS        (10)
#endif

#ifndef CAN_ISOTP_STMIN
/**
 * @brief   Default STmin for RX Flow Control frames
 */
#define CAN_ISOTP_STMIN     (5)
#endif

#ifndef CAN_ISOTP_WFTMAX
/**
 * @brief   Default maximum WFT for TX Flow Control
 */
#define CAN_ISOTP_WFTMAX    (1)
#endif

/**
 * @brief The isotp_fc_options struct
 *
 * It describes the flow control options
 */
struct isotp_fc_options {
    uint8_t bs;    /**< blocksize provided in FC frame, 0 = off */

    /** separation time provided in FC frame
     * 0x00 - 0x7F : 0 - 127 ms
     * 0x80 - 0xF0 : reserved
     * 0xF1 - 0xF9 : 100 us - 900 us
     * 0xFA - 0xFF : reserved */
    uint8_t stmin;

    uint8_t wftmax; /**< max. number of wait frame transmiss., 0 = ignored */
};

/**
 * @brief The isotp_options struct
 *
 * It describes the ISO-TP options
 */
struct isotp_options {
    canid_t tx_id;           /**< transmit CAN ID */
    canid_t rx_id;           /**< Receive CAN ID */
    uint16_t flags;          /**< set flags for isotp behaviour. */
    uint8_t  ext_address;    /**< set address for extended addressing */
    uint8_t  txpad_content;  /**< set content of padding byte (tx) */
    uint8_t  rx_ext_address; /**< set address for extended addressing */
};

/**
 * @brief The tpcon struct
 *
 * It describes the current connection status
 */
struct tpcon {
    unsigned idx;         /**< current index in @p buf */
    uint8_t state;        /**< the protocol state */
    uint8_t bs;           /**< block size */
    uint8_t sn;           /**< current sequence number */
    int tx_handle;        /**< handle of the last sent frame */
    gnrc_pktsnip_t *snip; /**< allocated snip containing data buffer */
};

/**
 * @brief The isotp struct
 *
 * This is the main struct used by an ISO-TP channel
 */
struct isotp {
    struct isotp *next;            /**< next bound channel */
    struct isotp_options opt;      /**< channel options */
    struct isotp_fc_options rxfc;  /**< rx flow control options (defined locally) */
    struct isotp_fc_options txfc;  /**< tx flow control options (defined remotely) */
    struct tpcon tx;               /**< transmit state */
    struct tpcon rx;               /**< receive state */
    ztimer_t tx_timer;             /**< timer for tx operations */
    ztimer_t rx_timer;             /**< timer for rx operations */
    can_reg_entry_t entry;         /**< entry containing ifnum and upper layer msg system */
    uint32_t tx_gap;               /**< transmit gap from fc (in us) */
    uint8_t tx_wft;                /**< transmit wait counter */
    void *arg;                     /**< upper layer private arg */
};

/**
 * @name flags for isotp behaviour
 * @{
 */
#define CAN_ISOTP_RX_FLAGS_MASK 0x0000FFFF /**< rx flags mask */
#define CAN_ISOTP_LISTEN_MODE   0x0001     /**< listen only flag (do not send FC) */
#define CAN_ISOTP_EXTEND_ADDR   0x0002     /**< enable extended addressing */
#define CAN_ISOTP_TX_PADDING    0x0004     /**< enable CAN frame padding tx path */
#define CAN_ISOTP_HALF_DUPLEX   0x0040     /**< half duplex error state handling */
#define CAN_ISOTP_RX_EXT_ADDR   0x0200     /**< different rx extended addressing */

#define CAN_ISOTP_TX_FLAGS_MASK 0xFFFF0000 /**< tx flags mask */
#define CAN_ISOTP_TX_DONT_WAIT  0x00010000 /**< do not send a tx confirmation msg */
/** @} */

/**
 * @name default configuration values
 * @{
 */
#define CAN_ISOTP_DEFAULT_FLAGS         0
#define CAN_ISOTP_DEFAULT_EXT_ADDRESS   0x00
#define CAN_ISOTP_DEFAULT_PAD_CONTENT   0xCC /* prevent bit-stuffing */
#define CAN_ISOTP_DEFAULT_FRAME_TXTIME  0
#define CAN_ISOTP_DEFAULT_RECV_BS       0
#define CAN_ISOTP_DEFAULT_RECV_STMIN    0x00
#define CAN_ISOTP_DEFAULT_RECV_WFTMAX   0
/** @} */

/**
 * @brief Initialize the isotp layer
 *
 * @param stack           stack for the isotp thread
 * @param stacksize       size of @p stack
 * @param priority        priority of the isotp thread
 * @param name            name of the isotp thread
 *
 * @return the pid of the isotp thread
 */
kernel_pid_t isotp_init(char *stack, int stacksize, char priority, const char *name);

/**
 * @brief Send data through an isotp channel
 *
 * @param isotp           the channel to use
 * @param buf             the data to send
 * @param len             length of the data to send
 * @param flags           flags for sending
 *
 * @return the number of bytes sent
 * @return < 0 if an error occurred  (-EBUSY, -ENOMEM)
 */
int isotp_send(struct isotp *isotp, const void *buf, int len, int flags);

/**
 * @brief Bind an isotp channel
 *
 * Initialize the channel, set the filter on the DLL and add the
 * channel to the list of bound channels
 *
 * @param isotp           the channel to bind
 * @param entry           entry identifying the CAN ifnum and the upper layer
 *                        either by its pid or its mailbox
 * @param fc_options      flow control parameters, bs and stmin for rx, wftmax for tx,
 *                        if NULL, default values will be used
 * @param arg             upper layer private parameter
 *
 * @return 0 on success, < 0 on error
 */
int isotp_bind(struct isotp *isotp, can_reg_entry_t *entry, void *arg,
               struct isotp_fc_options *fc_options);

/**
 * @brief Release a bound isotp channel
 *
 * Unset the filter on the DLL and remove the channel from the list
 * of bound channels
 *
 * @param isotp           the channel to release
 *
 * @return 0 on success, < 0 on error
 */
int isotp_release(struct isotp *isotp);

/**
 * @brief Free a received buffer
 *
 * This MUST be called by the upper layer when the received data are read
 *
 * @param rx              the received data
 */
void isotp_free_rx(can_rx_data_t *rx);

#ifdef __cplusplus
}
#endif

/** @} */
