/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup    sys_can_dll
 * @{
 *
 * @file
 * @brief       Definitions of low-level CAN DLL interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#ifndef CAN_DLL_H
#define CAN_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "can/common.h"
#include "can/pkt.h"
#include "can/device.h"
#include "thread.h"

/**
 * @brief Initialize the CAN DLL
 *
 * @return 0 on success
 */
int can_dll_init(void);

/**
 * @brief Register a CAN device into the DLL
 *
 * This function must be called by the device thread to register the device into the DLL
 *
 * @param[in] candev  the candev to register
 *
 * @return interface number on success
 * @return -ENODEV if ifnum is invalid
 */
int can_dll_register_candev(candev_dev_t *candev);

/**
 * @brief Dispatch a received frame
 *
 * This function is used to send a message to the DLL thread when a @p frame is received
 * from the device identified by its @p pid
 *
 * @param[in] frame the received frame
 * @param[in] pid   the pid of the receiver device
 *
 * @return 0 on success
 * @return -ENOMEM if the message can not be sent
 */
int can_dll_dispatch_rx_frame(struct can_frame *frame, kernel_pid_t pid);

/**
 * @brief Dispatch a tx confirmation
 *
 * This function is used to send a message to the sender thread when the
 * @p pkt has been sent correctly.
 *
 * @param[in] pkt   the pkt which has been sent
 *
 * @return 0 on success
 * @return -ENOMEM if the message can not be sent
 */
int can_dll_dispatch_tx_conf(can_pkt_t *pkt);

/**
 * @brief Dispatch a tx error
 *
 * This function is used to send a message to the sender thread when the
 * @p pkt has not been sent correctly
 *
 * @param[in] pkt   the pkt which has not been sent correctly
 *
 * @return 0 on success
 * @return -ENOMEM if the message can not be sent
 */
int can_dll_dispatch_tx_error(can_pkt_t *pkt);

/**
 * @brief Dispatch RX error from a device
 *
 * Dispatch RX error from a device to receivers threads
 * which have subscribed to frames on that interface
 *
 * @param[in] pid   the device thread pid
 *
 * @return 0 on success
 */
int can_dll_dispatch_bus_off(kernel_pid_t pid);

#ifdef __cplusplus
}
#endif

#endif /* CAN_DLL_H */

/** @} */
