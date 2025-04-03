/*
 * Copyright (C) 2016-2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef CAN_ROUTER_H
#define CAN_ROUTER_H

/**
 * @ingroup    sys_can_dll
 * @{
 *
 * @file
 * @brief       Functions for routing RX can frames
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "can/can.h"
#include "can/pkt.h"

/**
 * @brief Initialize CAN router
 */
void can_router_init(void);

/**
 * @brief Register a user @p entry to receive a frame @p can_id
 *
 * @param[in] entry   the entry containing ifnum and user info
 * @param[in] can_id  the CAN ID of the frame to receive
 * @param[in] mask    the mask of the frame to receive
 * @param[in] param   a user private pointer
 *
 * @return 0 on success
 * @return < 0 on error
 */
int can_router_register(can_reg_entry_t *entry, canid_t can_id, canid_t mask, void *param);

/**
 * @brief Unregister a user @p entry from receiving @p can_id
 *
 * The filter is unregistered from the 'router' layer if @p can_id, @p mask and @p param
 * matches a registered entry.
 *
 * @param[in] entry   the entry containing ifnum and user info which was registered
 * @param[in] can_id  the CAN ID of the frame to stop receiving
 * @param[in] mask    the mask of the frame to stop receiving
 * @param[in] param   a user private pointer
 *
 * @return 0 if @p can_id is not used anymore
 * @return 1 if @p can_id is still used by another pid
 * @return < 0 on error
 */
int can_router_unregister(can_reg_entry_t *entry, canid_t can_id, canid_t mask, void *param);

/**
 * @brief Free a received frame
 *
 * This function decrements the ref counter of the packet and frees it if the packet
 * is no more in use.
 *
 * @param[in] frame    the frame to free, it must be a frame returned by the stack
 *
 * @return 0 on success
 * @return < 0 on error
 */
int can_router_free_frame(can_frame_t *frame);

/**
 * @brief Dispatch a RX indication to subscribers threads
 *
 * This function goes through the list of subscribed filters to send a message to each
 * subscriber's thread. If all the subscriber's threads cannot receive message,
 * the packet is freed.
 *
 * @param[in] pkt   the packet to dispatch
 *
 * @return 0 on success
 * @return < 0 on error, if at least a thread cannot receive message
 */
int can_router_dispatch_rx_indic(can_pkt_t *pkt);

/**
 * @brief Dispatch a TX confirmation to the sender's thread
 *
 * @param[in] pkt  the correctly sent packet
 *
 * @return 0 on success
 * @return < 0 on error
 */
int can_router_dispatch_tx_conf(can_pkt_t *pkt);

/**
 * @brief Dispatch a TX error to the sender's thread
 *
 * @param[in] pkt  the error packet
 *
 * @return 0 on success
 * @return < 0 on error
 */
int can_router_dispatch_tx_error(can_pkt_t *pkt);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CAN_ROUTER_H */
