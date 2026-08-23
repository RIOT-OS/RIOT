/*
 * SPDX-FileCopyrightText: 2015-2017 Simon Brummer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
