/*
 * Copyright (C) 2017 Shuguo Zhuo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_gomach
 * @{
 *
 * @file
 * @brief       Internal used types of GoMacH
 * @internal
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */


#include <stdint.h>
#include <stdbool.h>

#include "xtimer.h"
#include "net/gnrc/gomach/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GoMacH RTT event type.
 */
#define GNRC_GOMACH_EVENT_RTT_TYPE          (0x4300)

/**
 * @brief   GoMacH RTT new cycle start event type.
 */
#define GNRC_GOMACH_EVENT_RTT_NEW_CYCLE     (0x4301)

/**
 * @brief   GoMacH timeout event type.
 */
#define GNRC_GOMACH_EVENT_TIMEOUT_TYPE      (0x4400)

/**
 * @brief   GoMacH uninitialized phase value.
 */
#define GNRC_GOMACH_PHASE_UNINITIALIZED     (0)

/**
 * @brief   GoMacH max phase value.
 */
#define GNRC_GOMACH_PHASE_MAX               (-1)

/**
 * @brief   GoMacH check duplicate packet buffer size.
 */
#define GNRC_GOMACH_DUPCHK_BUFFER_SIZE             (8U)

/**
 * @brief The default largest number of parallel timeouts in GoMacH.
 */
#define GNRC_GOMACH_TIMEOUT_COUNT             (6U)

/**
 * @brief The default number of slot-schedule-unit
 *        @ref gnrc_gomach_slosch_unit_t in GoMacH.
 */
#define GNRC_GOMACH_SLOSCH_UNIT_COUNT           (11U)

/**
 * @brief MAC type of unknown in GoMacH for indicating that
 *        the node's phase is unknown.
 */
#define GNRC_GOMACH_TYPE_UNKNOWN           (0U)

/**
 * @brief MAC type of known in GoMacH for indicating that the
 *        node's phase is known.
 */
#define GNRC_GOMACH_TYPE_KNOWN             (1U)

/**
 * @brief   State-machine states of Broadcast procedure of GoMacH.
 */
typedef enum {
    GNRC_GOMACH_BCAST_INIT,             /**< Initiate broadcast. */
    GNRC_GOMACH_BCAST_SEND,             /**< Send broadcast packet. */
    GNRC_GOMACH_BCAST_WAIT_TX_FINISH,   /**< Wait for broadcast TX finish. */
    GNRC_GOMACH_BCAST_WAIT_NEXT_TX,     /**< Wait for next broadcast TX timing. */
    GNRC_GOMACH_BCAST_END               /**< End of broadcast procedure. */
} gnrc_gomach_bcast_state_t;

/**
 * @brief   State-machine states of Transmission-to-phase-known
 *          (t2k) procedure of GoMacH.
 */
typedef enum {
    GNRC_GOMACH_T2K_INIT,                   /**< Initiate t2k procedure. */
    GNRC_GOMACH_T2K_WAIT_CP,                /**< Wait for receiver's CP (wake-up) period. */
    GNRC_GOMACH_T2K_TRANS_IN_CP,            /**< Transmit data in receiver's CP period. */
    GNRC_GOMACH_T2K_WAIT_CPTX_FEEDBACK,     /**< Wait for CP transmission finish. */
    GNRC_GOMACH_T2K_WAIT_BEACON,            /**< Wait receiver's beacon. */
    GNRC_GOMACH_T2K_WAIT_SLOTS,             /**< Wait for the node's allocated slots period. */
    GNRC_GOMACH_T2K_VTDMA_TRANS,            /**< Transmit data in allocate slots. */
    GNRC_GOMACH_T2K_WAIT_VTDMA_FEEDBACK,    /**< Wait for TX finish in TX-slot. */
    GNRC_GOMACH_T2K_END                     /**< End of t2k procedure. */
} gnrc_gomach_t2k_state_t;

/**
 * @brief   State-machine states of Transmission-to-phase-unknown
 *          (t2u) procedure of GoMacH.
 */
typedef enum {
    GNRC_GOMACH_T2U_INIT,                   /**< Initiate t2u procedure. */
    GNRC_GOMACH_T2U_PREAMBLE_PREPARE,       /**< Prepare settings before sending preamble. */
    GNRC_GOMACH_T2U_SEND_PREAMBLE,          /**< Send preamble in t2u. */
    GNRC_GOMACH_T2U_WAIT_PREAMBLE_TX,       /**< Wait for Send preamble TX finish. */
    GNRC_GOMACH_T2U_WAIT_PREAMBLE_ACK,      /**< Wait for preamble-ACK from receiver. */
    GNRC_GOMACH_T2U_SEND_DATA,              /**< Send data packet to the receiver. */
    GNRC_GOMACH_T2U_WAIT_DATA_TX,           /**< Wait for Send data TX finish. */
    GNRC_GOMACH_T2U_END                     /**< End of t2u procedure. */
} gnrc_gomach_t2u_state_t;

/**
 * @brief   State-machine states of basic management procedure of GoMacH.
 */
typedef enum {
    GNRC_GOMACH_INIT,           /**< Initiate GoMacH. */
    GNRC_GOMACH_LISTEN,         /**< GoMacH's duty-cycled listen procedure. */
    GNRC_GOMACH_TRANSMIT        /**< GoMacH's transmission procedure. */
} gnrc_gomach_basic_state_t;

/**
 * @brief   State-machine states of initialization procedure of GoMacH.
 */
typedef enum {
    GNRC_GOMACH_INIT_PREPARE,           /**< Prepare the initialization procedure of GoMacH. */
    GNRC_GOMACH_INIT_ANNC_SUBCHAN,      /**< Announce the sub-channel sequence of the node. */
    GNRC_GOMACH_INIT_WAIT_FEEDBACK,     /**< Wait for announce TX finish. */
    GNRC_GOMACH_INIT_END                /**< End of the initialization procedure of GoMacH. */
} gnrc_gomach_init_state_t;

/**
 * @brief   State-machine states of duty-cycled listening procedure of GoMacH.
 */
typedef enum {
    GNRC_GOMACH_LISTEN_CP_INIT,             /**< Initiate the listen period. */
    GNRC_GOMACH_LISTEN_CP_LISTEN,           /**< Listen for incoming packets. */
    GNRC_GOMACH_LISTEN_CP_END,              /**< End of packet listen period. */
    GNRC_GOMACH_LISTEN_SEND_BEACON,         /**< Send beacon packet when needed. */
    GNRC_GOMACH_LISTEN_WAIT_BEACON_TX,      /**< Wait for send beacon TX finish. */
    GNRC_GOMACH_LISTEN_VTDMA_INIT,          /**< Initiate the vTDMA period. */
    GNRC_GOMACH_LISTEN_VTDMA,               /**< Listen for incoming packets in vTDMA. */
    GNRC_GOMACH_LISTEN_VTDMA_END,           /**< End of the vTDMA period. */
    GNRC_GOMACH_LISTEN_SLEEP_INIT,          /**< Initiate the sleep period. */
    GNRC_GOMACH_LISTEN_SLEEP,               /**< Turn radio off to sleep. */
    GNRC_GOMACH_LISTEN_SLEEP_END            /**< End of the sleep period. */
} gnrc_gomach_listen_state_t;

/**
 * @brief   State-machine states of basic transmission management procedure of GoMacH.
 */
typedef enum {
    GNRC_GOMACH_TRANS_TO_UNKNOWN,       /**< Transmit to phase-unknown node in GoMacH. */
    GNRC_GOMACH_TRANS_TO_KNOWN,         /**< Transmit to phase-known node in GoMacH. */
    GNRC_GOMACH_BROADCAST               /**< Broadcast packet in GoMacH. */
} gnrc_gomach_transmit_state_t;

/**
 * @brief   GoMacH timeout types.
 */
typedef enum {
    GNRC_GOMACH_TIMEOUT_DISABLED,               /**< Timeout is disabled. */
    GNRC_GOMACH_TIMEOUT_BCAST_FINISH,           /**< Timeout of broadcast procedure end. */
    GNRC_GOMACH_TIMEOUT_BCAST_INTERVAL,         /**< Timeout of next broadcast transmission. */
    GNRC_GOMACH_TIMEOUT_PREAMBLE,               /**< Timeout of next preamble transmission. */
    GNRC_GOMACH_TIMEOUT_MAX_PREAM_INTERVAL,     /**< Timeout of maximum preamble interval. */
    GNRC_GOMACH_TIMEOUT_PREAM_DURATION,         /**< Timeout of maximum preamble duration. */
    GNRC_GOMACH_TIMEOUT_WAIT_CP,                /**< Timeout of waiting receiver's CP period. */
    GNRC_GOMACH_TIMEOUT_WAIT_BEACON,            /**< Timeout of waiting beacon. */
    GNRC_GOMACH_TIMEOUT_WAIT_SLOTS,             /**< Timeout of waiting own slots. */
    GNRC_GOMACH_TIMEOUT_CP_END,                 /**< Timeout of CP (wake-up) period ending. */
    GNRC_GOMACH_TIMEOUT_CP_MAX,                 /**< Timeout of maximum CP duration. */
    GNRC_GOMACH_TIMEOUT_WAIT_RX_END,            /**< Timeout of waiting reception complete. */
    GNRC_GOMACH_TIMEOUT_VTDMA,                  /**< Timeout of vTDMA period end. */
    GNRC_GOMACH_TIMEOUT_NO_TX_ISR,              /**< Timeout of maximum transmission duration. */
} gnrc_gomach_timeout_type_t;

/**
 * @brief   GoMacH internal L2 address structure
 */
typedef struct {
    uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];  /**< Node's address. */
} gnrc_gomach_l2_id_t;

/**
 * @brief   GoMacH's slot-schedule-unit.
 *
 * This slot-schedule-unit is used to record related information of a node for
 * running the dynamic slots allocation scheme in GoMacH.
 */
typedef struct {
    gnrc_gomach_l2_addr_t node_addr;    /**< Node's address. */
    uint8_t queue_indicator;            /**< Node's queue-length indicator. */
} gnrc_gomach_slosch_unit_t;

/**
 * @brief   GoMacH's vTDMA (dynamic slots allocation) management unit.
 */
typedef struct {
    uint8_t total_slots_num;        /**< Number of total allocated transmission slots. */
    uint8_t sub_channel_seq;        /**< Receiver's sub-channel sequence. */
} gnrc_gomach_vtdma_manag_t;

/**
 * @brief   GoMacH's data structure for recording sender's past TX information.
 */
typedef struct {
    gnrc_gomach_l2_addr_t node_addr;    /**< Node's address. */
    uint8_t seq;                        /**< Node's MAC packet sequence. */
    uint8_t life_cycle;                 /**< Lifetime of this record unit. */
} gnrc_gomach_dupchk_unit_t;

/**
 * @brief   GoMacH's data structure for recording TX information for avoiding receiving
 *          duplicate packets.
 */
typedef struct {
    gnrc_gomach_dupchk_unit_t last_nodes[GNRC_GOMACH_DUPCHK_BUFFER_SIZE]; /**< Duplicate
                                                                               check unit. */
    uint8_t queue_head;                                                   /**< Check queue's
                                                                               head. */
} gnrc_gomach_dupchk_t;

/**
 * @brief   Type to pass information about packet parsing.
 */
typedef struct {
    gnrc_gomach_hdr_t *header;          /**< GoMacH header of packet. */
    gnrc_gomach_l2_addr_t src_addr;     /**< Copied source address of packet.  */
    gnrc_gomach_l2_addr_t dst_addr;     /**< Copied destination address of packet. */
    uint8_t seq;                        /**< MAC sequence of the received packet. */
} gnrc_gomach_packet_info_t;

/**
 * @brief   Sender's vTDMA (dynamic slots allocation) management unit.
 */
typedef struct {
    uint16_t sub_channel_seq;       /**< Receiver's sub-channel sequence. */
    uint8_t slots_position;         /**< Node's own slots position. */
    uint8_t slots_num;              /**< Node's allocated slots number. */
} gnrc_gomach_vtdma_t;

/**
 * @brief   GoMacH's timeout structure
 */
typedef struct {
    xtimer_t timer;                     /**< xtimer entity. */
    msg_t msg;                          /**< msg entity. */
    bool expired;                       /**< Timeout expiration indicator. */
    gnrc_gomach_timeout_type_t type;    /**< GoMacH timeout type. */
} gnrc_gomach_timeout_t;

/**
 * @brief   Static initializer for @ref gnrc_gomach_timeout_t.
 */
#define GNRC_GOMACH_TIMEOUT_INIT  { {}, {}, false, GNRC_GOMACH_TIMEOUT_DISABLED }

/**
 * @brief   GoMacH's specific structure for storing internal states.
 */
typedef struct gomach {
    gnrc_gomach_basic_state_t basic_state;                      /**< Basic state. */
    gnrc_gomach_init_state_t init_state;                        /**< Initialization state. */
    gnrc_gomach_timeout_t timeouts[GNRC_GOMACH_TIMEOUT_COUNT];  /**< GoMacH's timeouts. */
    uint16_t subchannel_occu_flags;                             /**< Sub-channel
                                                                     usage indicator. */
    uint16_t sub_channel_seq;                                   /**< Sub-channel sequence. */
    uint16_t pub_channel_1;                                     /**< Public channel 1. */
    uint16_t pub_channel_2;                                     /**< Public channel 2. */
    uint16_t cur_pub_channel;                                   /**< Current public channel. */
    uint8_t cp_extend_count;                                    /**< CP extend count. */
    uint32_t last_wakeup;                                       /**< Last wake-up timing. */
    uint32_t backoff_phase_us;                                  /**< Phase backoff time. */
    uint16_t gomach_info;                                       /**< GoMacH's internal
                                                                     information. */
    uint64_t last_wakeup_phase_us;                              /**< Last cycle wakeup phase. */
    int16_t rx_pkt_rssi;                                        /**< RSSI of latest received
                                                                     packet in dBm */
    uint8_t rx_pkt_lqi;                                         /**< LQI of latest received
                                                                     packet */

#if (GNRC_MAC_ENABLE_DUTYCYCLE_RECORD == 1)
    /* Parameters for recording duty-cycle */
    uint64_t last_radio_on_time_ms;                          /**< The last time in ms
                                                                  when radio is on */
    uint64_t radio_off_time_ms;                              /**< The time in ms when
                                                                  radio is off */
    uint64_t system_start_time_ms;                           /**< The time in ms when
                                                                  chip is started */
    uint64_t awake_duration_sum_ms;                          /**< The sum of time in ms
                                                                  when radio is on */
#endif
} gnrc_gomach_t;

#ifdef __cplusplus
}
#endif

/** @} */
