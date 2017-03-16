/*
 * Copyright (C) 2015-2017 Simon Brummer
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
 * @brief       TCP finite state maschine declarations
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_INTERNAL_FSM_H
#define GNRC_TCP_INTERNAL_FSM_H

#include <stdint.h>
#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief The TCP FSM States.
 */
typedef enum {
    FSM_STATE_CLOSED,
    FSM_STATE_LISTEN,
    FSM_STATE_SYN_SENT,
    FSM_STATE_SYN_RCVD,
    FSM_STATE_ESTABLISHED,
    FSM_STATE_CLOSE_WAIT,
    FSM_STATE_LAST_ACK,
    FSM_STATE_FIN_WAIT_1,
    FSM_STATE_FIN_WAIT_2,
    FSM_STATE_CLOSING,
    FSM_STATE_TIME_WAIT
} fsm_state_t;

/**
 *  @brief Events that trigger translations in TCP FSM.
 */
typedef enum {
    FSM_EVENT_CALL_OPEN,          /* User function call: open */
    FSM_EVENT_CALL_SEND,          /* User function call: send */
    FSM_EVENT_CALL_RECV,          /* User function call: recv */
    FSM_EVENT_CALL_CLOSE,         /* User function call: close */
    FSM_EVENT_CALL_ABORT,         /* User function call: abort */
    FSM_EVENT_RCVD_PKT,           /* Paket received from peer */
    FSM_EVENT_TIMEOUT_TIMEWAIT,   /* Timeout: Timewait */
    FSM_EVENT_TIMEOUT_RETRANSMIT, /* Timeout: Retransmit */
    FSM_EVENT_TIMEOUT_CONNECTION, /* Timeout: Connection */
    FSM_EVENT_SEND_PROBE,         /* Send a Zero Window Probe */
    FSM_EVENT_CLEAR_RETRANSMIT    /* Clear Retransmission Mechanism */
} fsm_event_t;

/**
 * @brief TCP finite state maschine
 *
 * @param[in,out] tcb     specifies connection to use fsm on.
 * @param[in]     event   current event that triggers fsm translation
 * @param[in]     in_pkt  packet that triggered fsm event. Only in case of RCVD_PKT
 * @param[in,out] buf     buffer for send and receive functions
 * @param[in]     len     number of bytes to send or receive atmost
 *
 * @return  Zero on success
 * @return  Positive Number, number of bytes sent from or copied into buf.
 * @return  -ENOSYS if event is not implemented
 */
int _fsm(gnrc_tcp_tcb_t *tcb, fsm_event_t event, gnrc_pktsnip_t *in_pkt, void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_FSM_H */
/** @} */
