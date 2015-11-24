/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef GNRC_TCP_INTERNAL_FSM_H_
#define GNRC_TCP_INTERNAL_FSM_H_

#include "net/gnrc/pkt.h"
#include "net/gnrc/tcp/fsm.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

extern kernel_pid_t _tcp_pid;

/**
 * TODO
 */
ssize_t _fsm(gnrc_tcp_tcb_t *tcb, gnrc_tcp_fsm_event_t event, gnrc_pktsnip_t *in_pkt, void* buf, size_t nByte);

#ifdef __cplusplus
}
#endif

#endif
