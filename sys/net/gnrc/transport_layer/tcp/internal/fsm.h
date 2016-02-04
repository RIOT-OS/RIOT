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
 * @brief      Definies function to manipulate a connections state
 *
 * @author     Simon Brummer <brummer.simon@googlemail.com>
 */

#ifndef GNRC_TCP_INTERNAL_FSM_H_
#define GNRC_TCP_INTERNAL_FSM_H_

#include <errno.h>
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp/fsm.h"
#include "net/gnrc/tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PID of tcp event handling thread
 */
extern kernel_pid_t _gnrc_tcp_pid;

/**
 * @brief TCP finite state maschine
 *
 * @param[in,out] tcb      specifies connection to use fsm on.
 * @param[in]     event    current event that triggers fsm translation
 * @param[in]     in_pkt   packet that triggered fsm event. Only in case of RCVD_PKT
 * @param[in,out] buf      buffer for send and receive functions
 * @param[in]     nByte    number of bytes to send or receive atmost
 *
 * @return  Zero on success
 * @return  Positive Number, number of bytes sent from or copied into buf.
 * @return  -ENOSYS if event is not implemented
 */
int _fsm(gnrc_tcp_tcb_t* tcb, gnrc_tcp_fsm_event_t event, gnrc_pktsnip_t *in_pkt, void *buf,
         size_t nByte);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_FSM_H_ */
/** @} */
