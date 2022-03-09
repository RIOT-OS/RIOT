/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       TCP finite state maschine declarations.
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_FSM_H
#define GNRC_TCP_FSM_H

#include <stdint.h>
#include "mbox.h"
#include "net/gnrc.h"
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
} _gnrc_tcp_fsm_state_t;

/**
 *  @brief Events that trigger transitions in TCP FSM.
 */
typedef enum {
    FSM_EVENT_CALL_OPEN,          /* User function call: open */
    FSM_EVENT_CALL_SEND,          /* User function call: send */
    FSM_EVENT_CALL_RECV,          /* User function call: recv */
    FSM_EVENT_CALL_CLOSE,         /* User function call: close */
    FSM_EVENT_CALL_ABORT,         /* User function call: abort */
    FSM_EVENT_RCVD_PKT,           /* Packet received from peer */
    FSM_EVENT_TIMEOUT_TIMEWAIT,   /* Timeout: timewait */
    FSM_EVENT_TIMEOUT_RETRANSMIT, /* Timeout: retransmit */
    FSM_EVENT_TIMEOUT_CONNECTION, /* Timeout: connection */
    FSM_EVENT_SEND_PROBE,         /* Send zero window probe */
    FSM_EVENT_CLEAR_RETRANSMIT    /* Clear retransmission mechanism */
} _gnrc_tcp_fsm_event_t;

/**
 * @brief TCP finite state maschine
 *
 * @param[in,out] tcb     TCB holding the connection information.
 * @param[in]     event   Current event that triggers FSM transition.
 * @param[in]     in_pkt  Incoming packet. Only not NULL in case of event RCVD_PKT.
 * @param[in,out] buf     Buffer for send and receive functions.
 * @param[in]     len     Number of bytes to send or receive.
 *
 * @returns   Zero on success
 *            Positive Number, number of bytes sent from or copied into @p buf.
 *            -ENOSYS if event is not implemented
 */
int _gnrc_tcp_fsm(gnrc_tcp_tcb_t *tcb, _gnrc_tcp_fsm_event_t event,
                  gnrc_pktsnip_t *in_pkt, void *buf, size_t len);

/**
 * @brief Associate mbox with tcb. Messages sent from the FSM will be stored in the mbox.
 *
 * @param[in, out] tcb   TCB to set message box on.
 * @param[in]      mbox  Message box used to store messages from the FSM.
 *                       If @p mbox is NULL, no messages will be stored.
 */
void _gnrc_tcp_fsm_set_mbox(gnrc_tcp_tcb_t *tcb, mbox_t *mbox);

/**
 * @brief Get latest FSM state from given TCB.
 *
 * @param[in] tcb   TCB to get state from.
 *
 * @return   Current TCB state.
 */
_gnrc_tcp_fsm_state_t _gnrc_tcp_fsm_get_state(gnrc_tcp_tcb_t *tcb);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_FSM_H */
/** @} */
