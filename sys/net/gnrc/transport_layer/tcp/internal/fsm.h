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
 * @brief       RIOT's TCP implementation for the GNRC network stack.
 *
 * @{
 *
 * @file
 * @brief       TCP finite state maschine declarations.
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief The TCP FSM states.
 */
typedef enum {
    FSM_STATE_CLOSED = 0,
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
 *  @brief Events that trigger transitions in TCP FSM.
 */
typedef enum {
    FSM_EVENT_CALL_OPEN,          /* User function call: open */
    FSM_EVENT_CALL_SEND,          /* User function call: send */
    FSM_EVENT_CALL_RECV,          /* User function call: recv */
    FSM_EVENT_CALL_CLOSE,         /* User function call: close */
    FSM_EVENT_CALL_ABORT,         /* User function call: abort */
    FSM_EVENT_RCVD_PKT,           /* Paket received from peer */
    FSM_EVENT_TIMEOUT_TIMEWAIT,   /* Timeout: timewait */
    FSM_EVENT_TIMEOUT_RETRANSMIT, /* Timeout: retransmit */
    FSM_EVENT_TIMEOUT_CONNECTION, /* Timeout: connection */
    FSM_EVENT_SEND_PROBE,         /* Send zero window probe */
    FSM_EVENT_CLEAR_RETRANSMIT    /* Clear retransmission mechanism */
} fsm_event_t;

/**
 * @brief TCP finite state maschine
 *
 * @param[in,out] tcb     TCB holding the connection information.
 * @param[in]     event   Current event that triggers FSM transition.
 * @param[in]     in_pkt  Incomming packet. Only not NULL in case of event RCVD_PKT.
 * @param[in,out] buf     Buffer for send and receive functions.
 * @param[in]     len     Number of bytes to send or receive.
 *
 * @returns   Zero on success
 *            Positive Number, number of bytes sent from or copied into @p buf.
 *            -ENOSYS if event is not implemented
 */
int _fsm(gnrc_tcp_tcb_t *tcb, fsm_event_t event, gnrc_pktsnip_t *in_pkt, void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* FSM_H */
/** @} */
