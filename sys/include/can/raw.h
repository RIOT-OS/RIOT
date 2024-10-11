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
 * @brief       Definitions high-level RAW CAN interface
 *
 * This file defines the high-level CAN interface to send and receive RAW CAN
 * frame.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 */

#ifndef CAN_RAW_H
#define CAN_RAW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sched.h"
#include "can/can.h"
#include "can/common.h"
#include "can/device.h"

#ifdef MODULE_CAN_MBOX
#include "mbox.h"
#endif

#ifdef MODULE_TRX
#include "can/can_trx.h"
#endif

/**
 * @brief Default value for undefined interface number
 */
#define RAW_CAN_DEV_UNDEF (-1)

/**
 * @brief Send a CAN frame
 *
 * Send a CAN @p frame through the @p ifnum interface. The result is
 * sent to the @p pid thread via IPC.
 *
 * @param[in] ifnum the interface number to send to
 * @param[in] frame the frame to send
 * @param[in] pid   the user thread id to whom the result msg will be sent
 *                  it can be THREAD_PID_UNDEF if no feedback is expected
 *
 * @return a positive handle identifying the sent frame on success
 * @return < 0 on error
 */
int raw_can_send(int ifnum, const struct can_frame *frame, kernel_pid_t pid);

/**
 * @brief Abort a CAN frame
 *
 * Abort the frame identified by @p handle in the interface @p ifnum
 * If no tx confirmation is received, this function must be called by the upper layer
 * to ensure the driver frees its tx mailbox. The driver is not responsible of tx timeouts.
 *
 * @param[in] ifnum  the interface number used to send the frame
 * @param[in] handle the handle of the frame to abort,
 *                   it must be the value returned by raw_can_send
 * @return 0 on success
 * @return < 0 on error (-ENODEV)
 */
int raw_can_abort(int ifnum, int handle);

/**
 * @brief Subscribe to a CAN filter
 *
 * This function must be called if a user thread @p pid wants to receive the CAN frame matching @p filter
 * on the interface @p ifnum.
 * The user thread will then receive msg via IPC on reception of frame matching @p filters.
 *
 * @param[in] ifnum      the interface number to listen
 * @param[in] filter     the list of filter to receive
 * @param[in] pid        the thread id of the user
 * @param[in] param      optional user parameter
 *
 * @return the @p ifnum on success
 * @return < 0 on error
 */
int raw_can_subscribe_rx(int ifnum, struct can_filter *filter, kernel_pid_t pid, void *param);

/**
 * @brief Unsubscribe from reception for the given CAN @p filter on @p pid thread
 *
 * @param[in] ifnum    the interface number
 * @param[in] filter   the filter to remove
 * @param[in] pid      the thread id of the user
 * @param[in] param    optional user parameter
 *
 * @return 0 on success
 * @return < 0 on error
 */
int raw_can_unsubscribe_rx(int ifnum, struct can_filter *filter, kernel_pid_t pid, void *param);

/**
 * @brief Free a received frame
 *
 * This function must be called by the user when a received frame is not needed anymore.
 *
 * @param[in] frame the frame to free, it must be a pointer to a frame received by the stack
 *
 * @return 0 on success
 * @return < 0 on error
 */
int raw_can_free_frame(can_rx_data_t *frame);

/**
 * @brief Get a CAN option @p opt from interface @p ifnum
 *
 * @param[in] ifnum   the interface number
 * @param[in,out] opt the option to get
 *
 * @return 0 on success
 * @return < 0 on error
 */
int raw_can_get_can_opt(int ifnum, can_opt_t *opt);

/**
 * @brief Set a CAN option @p opt to interface @p ifnum
 *
 * @param[in] ifnum   the interface number
 * @param[in,out] opt the option to set
 *
 * @return 0 on success
 * @return < 0 on error
 */
int raw_can_set_can_opt(int ifnum, can_opt_t *opt);

#if  defined(MODULE_CAN_MBOX) || defined(DOXYGEN)
/**
 * @brief Send a CAN frame
 *
 * Send a CAN @p frame through the @p ifnum interface. The result is
 * sent to the @p mbox thread via mailbox IPC.
 *
 * @param[in] ifnum the interface number to send to
 * @param[in] frame the frame to send
 * @param[in] mbox  the user mbox to whom the result msg will be sent
 *                  it can be NULL if no feedback is expected
 *
 * @return a positive handle identifying the sent frame on success
 * @return < 0 on error
 */
int raw_can_send_mbox(int ifnum, const struct can_frame *frame, mbox_t *mbox);

/**
 * @brief Subscribe to a CAN filter
 *
 * This function must be called if a user thread waiting on @p mbox wants to receive
 * the CAN frame matching @p filter on the interface @p ifnum.
 * The user thread will then receive msg via mailbox IPC on reception of frame matching @p filters.
 *
 * Currently only single frame ID (i.e. filters->can_mask = 0xFFFFFFFF) are supported.
 *
 * @param[in] ifnum      the interface number to listen
 * @param[in] filter     the list of filter to receive
 * @param[in] mbox       the mbox of the user
 * @param[in] param      optional user parameter
 *
 * @return the @p ifnum on success
 * @return < 0 on error
 */
int raw_can_subscribe_rx_mbox(int ifnum, struct can_filter *filter, mbox_t *mbox, void *param);

/**
 * @brief Unsubscribe from reception for the given CAN @p filter and @p mbox
 *
 * @param[in] ifnum    the interface number
 * @param[in] filter   the filter to remove
 * @param[in] mbox     the mbox of the user
 * @param[in] param    optional user parameter
 *
 * @return 0 on success
 * @return < 0 on error
 */
int raw_can_unsubscribe_rx_mbox(int ifnum, struct can_filter *filter, mbox_t *mbox, void *param);
#endif

/**
 * @brief Power down a given interface
 *
 * @param[in] ifnum    the interface number to power down
 *
 * @return 0 on success
 * @return < 0 on error
 */
int raw_can_power_down(int ifnum);

/**
 * @brief Power up a given interface
 *
 * @param[in] ifnum    the interface number to power up
 *
 * @return 0 on success
 * @return < 0 on error
 */
int raw_can_power_up(int ifnum);

/**
 * @brief Get the interface number of a given interface
 *
 * @param[in] name   interface name
 *
 * @return the interface number, RAW_CAN_DEV_UNDEF if not defined
 */
int raw_can_get_ifnum_by_name(const char *name);

/**
 * @brief Get the interface name of a given interface number
 *
 * @param[in] ifnum   interface number
 *
 * @return the interface name, NULL if no interface registered with this number
 */
const char *raw_can_get_name_by_ifnum(int ifnum);

/**
 * @brief Get the candev descriptor from a given interface number
 *
 * @param[in] ifnum   interface number
 *
 * @return pointer to a candev descriptor, NULL if no interface is registered with
 *         this number
 */
candev_dev_t *raw_can_get_dev_by_ifnum(int ifnum);

/**
 * @brief Set the given bitrate/sample_point to the given ifnum
 *
 * Set the given @p bitrate and @p sample_point to the given @p ifnum. This is a
 * helper function which calculates the right bittiming from @p bitrate and
 * @p sample_point.
 *
 * @param[in] ifnum          the interface number
 * @param[in] bitrate        the bitrate in bits/s
 * @param[in] sample_point   the sample point in tenth of percent (875 = 87.5%)
 *                           if not set, the default value of 87.5% is used
 * @return 0 on success
 * @return 1 if the bitrate/sample_point couple can not be reached precisely but the bitrate is set
 * @return < 0  on error
 */
int raw_can_set_bitrate(int ifnum, uint32_t bitrate, uint32_t sample_point);

#if defined(MODULE_CAN_TRX) || defined(DOXYGEN)
/**
 * @brief Set a transceiver for a given interface
 *
 * The interface must be powered down before changing the transceiver.
 *
 * @param[in] ifnum   the interface number
 * @param[in] trx     the transceiver to set
 *
 * @return 0 on success
 * @return < 0 on error (-EBUSY if device is not powered down)
 */
int raw_can_set_trx(int ifnum, can_trx_t *trx);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CAN_RAW_H */
