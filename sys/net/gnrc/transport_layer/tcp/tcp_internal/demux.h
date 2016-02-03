/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef GNRC_TCP_INTERNAL_DEMUX_H_
#define GNRC_TCP_INTERNAL_DEMUX_H_

#include <stdint.h>
#include "net/gnrc/netreg.h"
#include "net/gnrc/tcp/tcb.h"

/* PORT 0 is reserved, according to rfc 1700(https://www.ietf.org/rfc/rfc1700.txt) */
/* Here used to indicate, that tcb internal port is currently not assigned */
#define PORT_UNSPEC 0

/* Range of Port Numbers, that are not controlled by IANA. */
#define BEG_REG_PORTS 1024
#define END_REG_PORTS 65535

/**
 * @brief Builds a context for multiplexing(combines peer and local port to a context)
 *
 * @param[in] peer_port    peer portnumber
 * @param[in] local_port   local portnumber
 *
 * @return    context generated from parameters
 */
uint32_t _demux_build_context(const uint16_t peer_port, const uint16_t local_port);

/**
 * TODO
 */
uint8_t _demux_context_unique(const uint16_t peer_port, const uint16_t local_port);

/**
 * @brief Registers a thread on in a multiplexing context.
 *
 * The context is evaluated by the portnumbers in a given tcb
 *
 * @param[in] tcb   tcb used to evaluate context.
 *
 * @return          zero on success
 */
int8_t _demux_register(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Removes thread from a multiplexing context.
 *
 * The context is evaluated by the portnumbers in a given tcb
 *
 * @param[in] tcb   tcb used to evaluate context.
 *
 * @return          zero on success
 */
int8_t _demux_remove(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Changes multiplexing context to new context.
 *
 * The old context is specified by a given tcb. The new context is stored in tcb
 * after this call.
 *
 * @param[in/out] tcb              tcb used to evaluate current context
 * @param[in]     new_peer_port    new_peer_port to store in tcb
 * @param[in]     new_local_port   new_local_port to store in tcb
 *
 * @return                         zero on success
 */
int8_t _demux_change_context(gnrc_tcp_tcb_t *tcb, uint16_t new_peer_port, uint16_t new_local_port);

/**
 * @brief Search for an unused Portnumber between Port 1024 and Port 65535.
 *        Those Ports are not controlled by the IANA. They are free to use.
 *
 * @param[in] dst_port   TODO
 *
 * @return    currently unused Port Number in [1024, 65535]
 */
uint16_t _demux_find_free_port(const uint8_t dst_port);

#endif
