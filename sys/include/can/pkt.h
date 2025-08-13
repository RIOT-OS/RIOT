/*
 * Copyright (C) 2016-2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup    sys_can_dll
 * @{
 *
 * @file
 * @brief       CAN memory allocation module
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdatomic.h>

#include "can/common.h"
#include "can/can.h"
#include "msg.h"
#ifdef MODULE_CAN_MBOX
#include "mbox.h"
#endif

/**
 * @brief A CAN packet
 *
 * A CAN packet is used to add stack-related data around a CAN frame
 */
typedef struct {
    can_reg_entry_t entry;   /**< entry containing ifnum and upper layer info */
    atomic_uint ref_count;   /**< Reference counter (for rx frames) */
    int handle;              /**< handle (for tx frames */
    can_frame_t frame;  /**< CAN Frame */
} can_pkt_t;

/**
 * @brief Initialize the CAN packet module
 *
 * This must be called by the DLL to initialize the module
 */
void can_pkt_init(void);

/**
 * @brief Allocate a CAN packet to transmit
 *
 * This function allocates a CAN packet and associates it to the @p ifnum and @p tx_pid.
 * The provided @p frame is copied into the CAN packet and a unique handle is set.
 *
 * @param[in] ifnum  the interface number
 * @param[in] frame  the frame to copy
 * @param[in] tx_pid the pid of the sender's device thread
 *
 * @return an allocated CAN packet, NULL if an error occurred
 */
can_pkt_t *can_pkt_alloc_tx(int ifnum, const can_frame_t *frame, kernel_pid_t tx_pid);

/**
 * @brief Allocate an incoming CAN packet
 *
 * @param[in] ifnum  the interface number
 * @param[in] frame  the received frame
 *
 * @return an allocated CAN packet, NULL if an error occurred
 */
can_pkt_t *can_pkt_alloc_rx(int ifnum, const can_frame_t *frame);

#if defined(MODULE_CAN_MBOX) || defined(DOXYGEN)
/**
 * @brief Allocate a CAN packet for a mbox to transmit
 *
 * This function allocates a CAN packet and associate it to the @p ifnum and @p mbox.
 * The provided @p frame is copied into the CAN packet and a unique handle is set.
 *
 * @param[in] ifnum  the interface number
 * @param[in] frame  the frame to copy
 * @param[in] mbox   the pointer to the sender's mbox
 *
 * @return an allocated CAN packet, NULL if an error occurred
 */
can_pkt_t *can_pkt_alloc_mbox_tx(int ifnum, const can_frame_t *frame, mbox_t *mbox);
#endif

/**
 * @brief Free a CAN packet
 *
 * @param[in] pkt     the packet to free, it must be a pointer returned
 *                    by @ref can_pkt_alloc_tx or @ref can_pkt_alloc_rx
 */
void can_pkt_free(can_pkt_t *pkt);

/**
 * @brief Allocate a @p can_rx_data_t and initialize it with gieven parameters
 *
 * This is used to allocate a return value to the upper layer
 *
 * @param[in] data  data which will be returned
 * @param[in] len   length of @p data
 * @param[in] arg   optional argument for the upper layer
 *
 * @return a @p can_rx_data_t pointer, NULL if out of memory
 */
can_rx_data_t *can_pkt_alloc_rx_data(void *data, size_t len, void *arg);

/**
 * @brief Free rx data previously allocated by can_pkt_alloc_rx_data()
 *
 * @param[in] data  the pointer to free
 */
void can_pkt_free_rx_data(can_rx_data_t *data);

/**
 * @brief Allocate @p size bytes and return the pointer
 *
 * This function has been copied from gnrc_pktbuf_static
 *
 * @param[in] size  the number of bytes to allocate
 *
 * @return the pointer to thje allocated data, NULL if out of memory
 */
void *can_pkt_buf_alloc(size_t size);

/**
 * @brief Free the data allocated by can_pkt_buf_alloc()
 *
 * @param[in] data  the pointer to free
 * @param[in] size  the size of the data to free
 */
void can_pkt_buf_free(void *data, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
