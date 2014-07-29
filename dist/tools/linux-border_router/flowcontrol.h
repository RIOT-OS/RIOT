/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file    flowcontrol.h
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @brief   Public declarations for the flow control jobs via the
 *          serial interface for the 6LoWPAN Border Router driver.
 */

#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#include <netinet/in.h>

#include "multiplex.h"

/* packet types for flowcontrol */
#define BORDER_PACKET_ACK_TYPE    1       ///< Packet type for acknowledgement packets (for flow control).

/* configuration types for flowcontrol */
#define BORDER_CONF_SYN           0       ///< Configuration packet type for SYN-Packets.
#define BORDER_CONF_SYNACK        1       ///< Configuration packet type for SYN/ACK-Packets.

#define BORDER_SWS                1       ///< Sending window size for flow control.
#define BORDER_RWS                1       ///< Receiving window size for flow control.
#define BORDER_SL_TIMEOUT         500000  ///< Timeout time (in µsec) for flow control.

/**
 * @brief   State of the sliding window algorithm, used for flow control
 * @see     "Computernetze -- Eine systemorientierte Einführung",
 *          L.L. Peterson, B.S. Davie, dpunkt-lehrbuch, 2008
 */
typedef struct flowcontrol_stat_t {
    /* Sender state */
    uint8_t last_ack;                   ///< Sequence number of the last received acknowledgement.
    uint8_t last_frame;                 ///< Sequence number of the last send frame.
    /**
     * @brief   Semaphore, that locks if sending window is full.
     */
    sem_t send_win_not_full;
    /**
     * @brief a slot in the sending window
     */
    struct send_slot {
        pthread_t resend_thread;        ///< Thread that handles the resending of this slot's frame (if needed).
        uint8_t frame[BUFFER_SIZE];     ///< This slot's frame.
        size_t frame_len;               ///< The length of this slot's frame.
    } send_win[BORDER_SWS];             ///< The sending window.

    /* Receiver state */
    uint8_t next_exp;                   ///< The next expected sequence number to be received.
    /**
     * @brief a receiving in the sending window
     */
    struct recv_slot {
        int8_t received;                ///< 0 if this slot is empty, != 0 if this slot contains a received frame.
        uint8_t frame[BUFFER_SIZE];     ///< This slot's frame
        size_t frame_len;               ///< The length of this slot's frame.
    } recv_win[BORDER_RWS];             ///< The receiving window.
} flowcontrol_stat_t;

/**
 * @brief   Describes a SYN packet for connection establishment of
 *          the serial line.
 * @extends border_conf_header_t
 */
typedef struct __attribute__((packed)) border_syn_packet_t {
    uint8_t empty;
    uint8_t type;
    /**
     * @brief Next sequence number
     *
     * Communicates the next local sequence number to be send to the
     * MSB-A2 (for flow control).
     *
     * This replaces @ref border_conf_header_t::seq_num of normal
     * configuration packets.
     */
    uint8_t next_seq_num;
    uint8_t conftype;
    /**
     * @brief Next expected sequence number
     *
     * Communicates to the MSB-A2 which sequence number the driver
     * expects next.
     */
    uint8_t next_exp;
    struct in6_addr addr;   ///< IPv6-Address of this border router.
} border_syn_packet_t;

/**
 * @brief   Sets the flow control algorithm to the initial state.
 * @param[in]   addr    The IP address that should be communicated to the
 *                      LoWPAN interface.
 */
void flowcontrol_init(const struct in6_addr *addr);

/**
 * @brief   Destroys the state struct for the flow control algorithm.
 */
void flowcontrol_destroy(void);

/**
 * @brief   Singals the flow control algorith, that an connection
 *          was established (because a SYNACK packet was received).
 */
void signal_connection_established(void);

/**
 * @brief   Sends a packet via the serial interface.
 * @param[in,out]   packet  The packet that is to be send via the
 *                          serial interface. The function sets the
 *                          sequence number of the packet for flow
 *                          control.
 * @param[in]       len     Length of the packet.
 */
void flowcontrol_send_over_tty(border_packet_t *packet, int len);

/**
 * @brief   Delivers all actions that should be done by the sliding
 *          window on receiving a packet.
 * @param[in]       packet  The packet the was received via the serial
 *                          interface.
 * @param[in]       len     Length of the packet.
 */
void flowcontrol_deliver_from_tty(const border_packet_t *packet, int len);

#endif /* FLOWCONTROL_H*/
