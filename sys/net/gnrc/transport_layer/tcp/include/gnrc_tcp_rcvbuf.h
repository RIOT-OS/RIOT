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
 * @brief       Functions for allocating and freeing the receive buffer.
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_RCVBUF_H
#define GNRC_TCP_RCVBUF_H

#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes global receive buffer.
 */
void _gnrc_tcp_rcvbuf_init(void);

/**
 * @brief Allocate receive buffer and assign it to TCB.
 *
 * @param[in,out] tcb   TCB that acquires receive buffer.
 *
 * @returns   Zero  on success.
 *            -ENOMEM if all receive buffers are currently used.
 */
int _gnrc_tcp_rcvbuf_get_buffer(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Release allocated receive buffer.
 *
 * @param[in,out] tcb   TCB holding the receive buffer that should be released.
 */
void _gnrc_tcp_rcvbuf_release_buffer(gnrc_tcp_tcb_t *tcb);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GNRC_TCP_RCVBUF_H */
