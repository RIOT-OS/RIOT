/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Definition of internal types used by LWMAC
 *
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_LWMAC_TYPES_H
#define NET_GNRC_LWMAC_TYPES_H

#include "msg.h"
#include "xtimer.h"
#include "net/gnrc/lwmac/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LWMAC RTT event type.
 */
#define GNRC_LWMAC_EVENT_RTT_TYPE            (0x4300)

/**
 * @brief   LWMAC RTT start event type.
 */
#define GNRC_LWMAC_EVENT_RTT_START           (0x4301)

/**
 * @brief   LWMAC RTT stop event type.
 */
#define GNRC_LWMAC_EVENT_RTT_STOP            (0x4302)

/**
 * @brief   LWMAC RTT pause event type.
 */
#define GNRC_LWMAC_EVENT_RTT_PAUSE           (0x4303)

/**
 * @brief   LWMAC RTT resume event type.
 */
#define GNRC_LWMAC_EVENT_RTT_RESUME          (0x4304)

/**
 * @brief   LWMAC RTT wakeup pending event type.
 */
#define GNRC_LWMAC_EVENT_RTT_WAKEUP_PENDING  (0x4305)

/**
 * @brief   LWMAC RTT sleep pending event type.
 */
#define GNRC_LWMAC_EVENT_RTT_SLEEP_PENDING   (0x4306)

/**
 * @brief   LWMAC timeout event type.
 */
#define GNRC_LWMAC_EVENT_TIMEOUT_TYPE        (0x4400)

/**
 * @brief   LWMAC duty-cycle active flag.
 *
 * Keep track of duty cycling to avoid late RTT events after stopping.
 */
#define GNRC_LWMAC_DUTYCYCLE_ACTIVE          (0x01)

/**
 * @brief   LWMAC needs reschedule flag.
 *
 * Used internally for rescheduling state machine update, e.g. after state
 * transition caused in update.
 */
#define GNRC_LWMAC_NEEDS_RESCHEDULE          (0x02)

/**
 * @brief   LWMAC check radio's on/off state flag.
 */
#define GNRC_LWMAC_RADIO_IS_ON               (0x04)

/**
 * @brief The default largest number of parallel timeouts in LWMAC
 */
#ifndef GNRC_LWMAC_TIMEOUT_COUNT
#define GNRC_LWMAC_TIMEOUT_COUNT             (3U)
#endif

/**
 * @brief   Internal states of LWMAC
 */
typedef enum {
    GNRC_LWMAC_UNDEF = -1,      /**< Undefined state of LWMAC */
    GNRC_LWMAC_STOPPED,         /**< LWMAC's main state machine has been stopped */
    GNRC_LWMAC_START,           /**< Start LWMAC's main state machine */
    GNRC_LWMAC_STOP,            /**< Stop LWMAC's main state machine */
    GNRC_LWMAC_RESET,           /**< Reset LWMAC's main state machine */
    GNRC_LWMAC_LISTENING,       /**< Listen the channel for receiving packets */
    GNRC_LWMAC_RECEIVING,       /**< RX is handled in own state machine */
    GNRC_LWMAC_TRANSMITTING,    /**< TX is handled in own state machine */
    GNRC_LWMAC_SLEEPING,        /**< Turn off radio to conserve power */
    GNRC_LWMAC_STATE_COUNT      /**< Count of LWMAC's states */
} gnrc_lwmac_state_t;

/**
 * @brief   TX states of LWMAC
 */
typedef enum {
    GNRC_LWMAC_TX_STATE_STOPPED,            /**< Tx schedule stopped, stop sending packet */
    GNRC_LWMAC_TX_STATE_INIT,               /**< Initiate transmission */
    GNRC_LWMAC_TX_STATE_SEND_BROADCAST,     /**< directly goes to SUCCESSFUL or FAILED when finished */
    GNRC_LWMAC_TX_STATE_SEND_WR,            /**< Send a wakeup request */
    GNRC_LWMAC_TX_STATE_WAIT_WR_SENT,       /**< Wait until WR sent to set timeout */
    GNRC_LWMAC_TX_STATE_WAIT_FOR_WA,        /**< Wait for dest node's wakeup ackknowledge */
    GNRC_LWMAC_TX_STATE_SEND_DATA,          /**< Send the actual payload data */
    GNRC_LWMAC_TX_STATE_WAIT_FEEDBACK,      /**< Wait if packet was ACKed */
    GNRC_LWMAC_TX_STATE_SUCCESSFUL,         /**< Transmission has finished successfully */
    GNRC_LWMAC_TX_STATE_FAILED              /**< Payload data couldn't be delivered to dest */
} gnrc_lwmac_tx_state_t;

/**
 * @brief   Static initializer for gnrc_lwmac_tx_state_t.
 */
#define GNRC_LWMAC_TX_STATE_INITIAL GNRC_LWMAC_TX_STATE_STOPPED

/**
 * @brief   RX states of LWMAC
 */
typedef enum {
    GNRC_LWMAC_RX_STATE_STOPPED,        /**< Rx schedule stopped */
    GNRC_LWMAC_RX_STATE_INIT,           /**< Initiate reception */
    GNRC_LWMAC_RX_STATE_WAIT_FOR_WR,    /**< Wait for a wakeup request */
    GNRC_LWMAC_RX_STATE_SEND_WA,        /**< Send wakeup ackknowledge to requesting node */
    GNRC_LWMAC_RX_STATE_WAIT_WA_SENT,   /**< Wait until WA sent to set timeout */
    GNRC_LWMAC_RX_STATE_WAIT_FOR_DATA,  /**< Wait for actual payload data */
    GNRC_LWMAC_RX_STATE_SUCCESSFUL,     /**< Recption has finished successfully */
    GNRC_LWMAC_RX_STATE_FAILED          /**< Reception over, but nothing received */
} gnrc_lwmac_rx_state_t;

/**
 * @brief   Static initializer for gnrc_lwmac_rx_state_t.
 */
#define GNRC_LWMAC_RX_STATE_INITIAL GNRC_LWMAC_RX_STATE_STOPPED

/**
 * @brief   LWMAC uninitialized phase value
 */
#define GNRC_LWMAC_PHASE_UNINITIALIZED   (0)

/**
 * @brief   LWMAC max phase value
 */
#define GNRC_LWMAC_PHASE_MAX             (-1)

/**
 * @brief   LWMAC timeout types
 */
typedef enum {
    GNRC_LWMAC_TIMEOUT_DISABLED,                /**< Timeout is disabled */
    GNRC_LWMAC_TIMEOUT_WR,                      /**< WR timeout, waiting WA */
    GNRC_LWMAC_TIMEOUT_NO_RESPONSE,             /**< Maximum WR duration timeout awaiting WA */
    GNRC_LWMAC_TIMEOUT_DATA,                    /**< Timeout awaiting data packet from receiver */
    GNRC_LWMAC_TIMEOUT_WAIT_DEST_WAKEUP,        /**< Timeout for waiting receiver's wake-up phase */
    GNRC_LWMAC_TIMEOUT_WAKEUP_PERIOD,           /**< Wake up period timeout for going to sleep */
    GNRC_LWMAC_TIMEOUT_NEXT_BROADCAST,          /**< Timeout for waiting to send the next broadcast packet */
    GNRC_LWMAC_TIMEOUT_BROADCAST_END,           /**< Timeout awaiting the end of the whole broadcast period */
} gnrc_lwmac_timeout_type_t;

/**
 * @brief   LWMAC timeout structure
 */
typedef struct {
    xtimer_t timer;                 /**< xtimer entity */
    msg_t msg;                      /**< msg entity */
    bool expired;                   /**< If type != DISABLED, this indicates if timeout has expired */
    gnrc_lwmac_timeout_type_t type; /**< timeout type */
} gnrc_lwmac_timeout_t;

/**
 * @brief   LWMAC specific structure for storing internal states.
 */
typedef struct lwmac {
    gnrc_lwmac_state_t state;                                   /**< Internal state of MAC layer */
    uint32_t last_wakeup;                                       /**< Used to calculate wakeup times */
    uint8_t lwmac_info;                                         /**< LWMAC's internal information (flags) */
    gnrc_lwmac_timeout_t timeouts[GNRC_LWMAC_TIMEOUT_COUNT];    /**< Store timeouts used for protocol */

#if (GNRC_MAC_ENABLE_DUTYCYCLE_RECORD == 1)
    /* Parameters for recording duty-cycle */
    uint32_t last_radio_on_time_ticks;                          /**< The last time in ticks when radio is on */
    uint32_t radio_off_time_ticks;                              /**< The time in ticks when radio is off */
    uint32_t system_start_time_ticks;                           /**< The time in ticks when chip is started */
    uint32_t awake_duration_sum_ticks;                          /**< The sum of time in ticks when radio is on */
    uint32_t pkt_start_sending_time_ticks;                      /**< The time in ticks when the packet is started
                                                                     to be sent */
#endif
} gnrc_lwmac_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LWMAC_TYPES_H */
/** @} */
