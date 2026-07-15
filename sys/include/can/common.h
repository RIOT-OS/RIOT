/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup   sys_can_common  Common definitions
 * @ingroup    sys_can
 * @brief      CAN stack common definitions
 *
 * This module defines the common part of the CAN stack, including structures
 * and messages.
 *
 * @{
 *
 *
 * @file
 * @brief       Definitions of high-level CAN interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/uio.h>

#include "timex.h"
#include "thread.h"
#ifdef MODULE_CAN_MBOX
#include "mbox.h"
#endif

/**
 * @brief Default CAN maximum data length
 */
#ifdef MODULE_FDCAN
#define DEFAULT_CAN_MAX_DLEN CANFD_MAX_DLEN
#else
#define DEFAULT_CAN_MAX_DLEN CAN_MAX_DLEN
#endif

/**
 * @brief CAN options
 */
typedef enum {
    CANOPT_BITTIMING,       /**< bit timing parameter */
    CANOPT_RX_FILTERS,      /**< rx filters */
    CANOPT_TEC,             /**< Transmit Error Counter */
    CANOPT_REC,             /**< Receive Error Counter*/
    CANOPT_LEC,             /**< Last Error Code */
    CANOPT_CLOCK,           /**< controller main clock */
    CANOPT_BITTIMING_CONST, /**< controller bittiming parameters */
    CANOPT_STATE,           /**< set controller state @ref canopt_state_t */
#ifdef MODULE_FDCAN
    CANOPT_FD_BITTIMING,    /**< bit timing parameter for FDCAN data payload */
    CANOPT_FD_BITTIMING_CONST, /**< controller bit timing parameter for FDCAN data payload */
#endif
} canopt_t;

/**
 * @brief CAN state options
 *
 * CAN state options to be used with @p CANOPT_STATE
 */
typedef enum {
    CANOPT_STATE_OFF,             /**< powered off */
    CANOPT_STATE_SLEEP,           /**< sleep mode */
    CANOPT_STATE_LISTEN_ONLY,     /**< listen only mode */
    CANOPT_STATE_ON,              /**< power on, rx / tx mode */
    CANOPT_STATE_LOOPBACK,        /**< loopback mode */
} canopt_state_t;

/**
 * @brief Structure to pass a CAN option
 */
typedef struct {
    canopt_t opt;               /**< the option to get/set */
    uint16_t context;           /**< (optional) context for that option */
    void *data;                 /**< data to set or buffer to read into */
    uint16_t data_len;          /**< size of the data / the buffer */
} can_opt_t;

/**
 * @brief Messages which can be sent through the CAN stack
 */
enum can_msg {
    /* High level messages */
    CAN_MSG_ACK = 0x100,      /**< acknowledgment */
    CAN_MSG_SEND_FRAME,       /**< send a frame */
    CAN_MSG_ABORT_FRAME,      /**< abort a frame */
    CAN_MSG_SET,              /**< set an option */
    CAN_MSG_GET,              /**< get an option */
    CAN_MSG_SET_FILTER,       /**< set a filter */
    CAN_MSG_REMOVE_FILTER,    /**< remove a filter */
    CAN_MSG_POWER_UP,         /**< power up */
    CAN_MSG_POWER_DOWN,       /**< power down */
#if defined(MODULE_CAN_TRX) || defined(DOXYGEN)
    CAN_MSG_SET_TRX,          /**< set a transceiver */
#endif
    /* candev internal messages */
    CAN_MSG_EVENT = 0x200,    /**< driver event */
    CAN_MSG_WAKE_UP,          /**< driver has been woken up by bus */
    CAN_MSG_TX_CONFIRMATION,  /**< a frame has been sent */
    CAN_MSG_RX_INDICATION,    /**< a frame has been received */
    CAN_MSG_TX_ERROR,         /**< there was an error when transmitting */
    CAN_MSG_RX_ERROR,         /**< there was an error when receiving */
    CAN_MSG_BUS_OFF,          /**< bus-off detected */
    CAN_MSG_ERROR_PASSIVE,    /**< driver switched in error passive */
    CAN_MSG_ERROR_WARNING,    /**< driver reached error warning */
#if defined(MODULE_CAN_PM) || defined(DOXYGEN)
    CAN_MSG_PM,               /**< power management event */
#endif
    /* isotp messages */
#if defined(MODULE_CAN_ISOTP) || defined(DOXYGEN)
    CAN_MSG_ISOTP_RX_TIMEOUT = 0x400,  /**< isotp rx timeout */
    CAN_MSG_ISOTP_TX_TIMEOUT,          /**< isotp tx timeout */
#endif
};

/**
 * @brief Received data structure
 *
 * This structure is used when a layer sends received data
 * to the upper layer
 */
typedef struct can_rx_data {
    struct iovec data;    /**< iovec containing received data */
    void *arg;            /**< upper layer private param */
} can_rx_data_t;

/**
 * @brief registry entry types
 */
typedef enum {
    CAN_TYPE_DEFAULT = 0, /**< default entry (use msg) */
#if defined(MODULE_CAN_MBOX) || defined(DOXYGEN)
    CAN_TYPE_MBOX,        /**< mbox entry */
#endif
} can_reg_type_t;

/**
 * @brief registry entry
 *
 * This structure is used through the stack to describe how to contact
 * the upper layer and which CAN interface to use
 */
typedef struct can_reg_entry {
    struct can_reg_entry *next; /**< next for linked list */
    int ifnum;                  /**< interface number for the entry */
    union {
        kernel_pid_t pid;       /**< pid of the thread when using msg */
#if defined(MODULE_CAN_MBOX) || defined(DOXYGEN)
        mbox_t *mbox;           /**< mbox pointer */
#endif
    } target;                   /**< entry target */
#if defined(MODULE_CAN_MBOX) || defined(DOXYGEN)
    can_reg_type_t type;        /**< entry type */
#endif
} can_reg_entry_t;

#ifdef __cplusplus
}
#endif

/** @} */
