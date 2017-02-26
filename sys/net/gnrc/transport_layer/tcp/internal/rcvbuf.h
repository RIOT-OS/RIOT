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
 * @brief       Functions for allocating and freeing the receive buffer
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_INTERNAL_RCVBUF_H
#define GNRC_TCP_INTERNAL_RCVBUF_H

#include <stdint.h>
#include "mutex.h"
#include "net/gnrc/tcp/config.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Struct for a single connections receive buffer
 * @internal
 */
typedef struct rcvbuf_entry {
    uint8_t used;                            /**< Is entry currently in use */
    uint8_t buffer[GNRC_TCP_RCV_BUF_SIZE];   /**< Raw Buffer Data */
} rcvbuf_entry_t;

/**
 * @brief   Stuct holding receive buffers
 * @internal
 */
typedef struct rcvbuf {
    mutex_t lock;                                   /**< Lock for synchronization */
    rcvbuf_entry_t entries[GNRC_TCP_RCV_BUFFERS];   /**< Number of receive buffers */
} rcvbuf_t;

/**
 * @brief   Initializes global receive Buffer
 * @internal
 */
void _rcvbuf_init(void);

/**
 * @brief Initializes and assigns receive Buffer to tcb.
 *
 * @param[in] tcb   Transmission control block that should hold the buffer.
 *
 * @return  zero  on success
 * @return  -ENOMEM If receive buffer is out of memory.
 */
int _rcvbuf_get_buffer(gnrc_tcp_tcb_t *tcb);

/**
 * @brief Free allocated receive buffer
 *
 * @param[in] tcb   Transmission control block that buffer should be freed.
 */
void _rcvbuf_release_buffer(gnrc_tcp_tcb_t *tcb);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_RCVBUF_H */
/** @} */
