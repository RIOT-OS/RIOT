/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GNRC_TCP_FSM_H_
#define GNRC_TCP_FSM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief States for the TCP State Maschine
 */
typedef enum {CLOSED,
              LISTEN,
              SYN_SENT,
              SYN_RCVD,
              ESTABLISHED,
              CLOSE_WAIT,
              LAST_ACK,
              FIN_WAIT_1,
              FIN_WAIT_2,
              CLOSING,
              TIME_WAIT
} gnrc_tcp_fsm_state_t;

/**
 *  @brief Events for the TCP State Maschine
 */
typedef enum {
    CALL_OPEN,
    CALL_SEND,
    CALL_RECV,
    CALL_CLOSE,
    CALL_ABORT,
    RCVD_PKT,
    TIMEOUT_TIMEWAIT,
    TIMEOUT_RETRANSMIT,
    TIMEOUT_USER
} gnrc_tcp_fsm_event_t;

#ifdef __cplusplus
}
#endif

#endif
