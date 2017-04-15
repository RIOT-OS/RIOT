/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief      Definies states and events for TCP finite state maschine
 *
 * @author     Simon Brummer <brummer.simon@googlemail.com>
 */

#ifndef GNRC_TCP_FSM_H_
#define GNRC_TCP_FSM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief The TCP FSM States.
 */
typedef enum {
    GNRC_TCP_FSM_STATE_CLOSED,
    GNRC_TCP_FSM_STATE_LISTEN,
    GNRC_TCP_FSM_STATE_SYN_SENT,
    GNRC_TCP_FSM_STATE_SYN_RCVD,
    GNRC_TCP_FSM_STATE_ESTABLISHED,
    GNRC_TCP_FSM_STATE_CLOSE_WAIT,
    GNRC_TCP_FSM_STATE_LAST_ACK,
    GNRC_TCP_FSM_STATE_FIN_WAIT_1,
    GNRC_TCP_FSM_STATE_FIN_WAIT_2,
    GNRC_TCP_FSM_STATE_CLOSING,
    GNRC_TCP_FSM_STATE_TIME_WAIT
} gnrc_tcp_fsm_state_t;

/**
 *  @brief Events that trigger translations in TCP FSM.
 */
typedef enum {
    GNRC_TCP_FSM_EVENT_CALL_OPEN,          /* User function call: open */
    GNRC_TCP_FSM_EVENT_CALL_SEND,          /* User function call: send */
    GNRC_TCP_FSM_EVENT_CALL_RECV,          /* User function call: recv */
    GNRC_TCP_FSM_EVENT_CALL_CLOSE,         /* User function call: close */
    GNRC_TCP_FSM_EVENT_CALL_ABORT,         /* User function call: abort */
    GNRC_TCP_FSM_EVENT_RCVD_PKT,           /* Paket received from peer */
    GNRC_TCP_FSM_EVENT_TIMEOUT_TIMEWAIT,   /* Timeout: Timewait */
    GNRC_TCP_FSM_EVENT_TIMEOUT_RETRANSMIT, /* Timeout: Retransmit */
    GNRC_TCP_FSM_EVENT_TIMEOUT_CONNECTION, /* Timeout: Connection */
    GNRC_TCP_FSM_EVENT_SEND_PROBE,         /* Send a Zero Window Probe */
    GNRC_TCP_FSM_EVENT_CLEAR_RETRANSMIT    /* Clear Retransmission Mechanism */
} gnrc_tcp_fsm_event_t;

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_FSM_H_ */
/** @} */
