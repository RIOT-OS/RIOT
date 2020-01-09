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

#ifndef RCVBUF_H
#define RCVBUF_H

#include <stdint.h>
#include "mutex.h"
#include "net/gnrc/tcp/config.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Receive buffer entry.
 */
typedef struct rcvbuf_entry {
    uint8_t used;                          /**< Flag: Is buffer in use? */
    uint8_t buffer[GNRC_TCP_RCV_BUF_SIZE]; /**< Receive buffer storage */
} rcvbuf_entry_t;

/**
 * @brief   Struct holding receive buffers.
 */
typedef struct rcvbuf {
    mutex_t lock;                                 /**< Lock for allocation synchronization */
    rcvbuf_entry_t entries[GNRC_TCP_RCV_BUFFERS]; /**< Maintained receive buffers */
} rcvbuf_t;

/**
 * @brief   Initializes global receive buffer.
 */
void _rcvbuf_init(void);

/**
 * @brief Allocate receive buffer and assign it to TCB.
 *
 * @param[in,out] tcb   TCB that acquires receive buffer.
 *
 * @returns   Zero  on success.
 *            -ENOMEM if all receive buffers are currently used.
 */
int _rcvbuf_get_buffer(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Release allocated receive buffer.
 *
 * @param[in,out] tcb   TCB holding the receive buffer that should be released.
 */
void _rcvbuf_release_buffer(gnrc_tcp_tcb_t *tcb);

#ifdef __cplusplus
}
#endif

#endif /* RCVBUF_H */
/** @} */
