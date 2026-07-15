/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup    sys_can_dll
 * @{
 *
 * @file
 * @brief       Definitions of CAN device interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "can/candev.h"
#include "sched.h"

#ifdef MODULE_CAN_PM
#include "ztimer.h"
#endif
#ifdef MODULE_CAN_TRX
#include "can/can_trx.h"
#endif

#ifndef CAN_MAX_RATE_ERROR
/**
 * Maximum bit-rate error allowed when computing bittimings
 * in tenth of percent
 */
#define CAN_MAX_RATE_ERROR  (50) /* 5 % */
#endif

#ifndef CAN_DLL_NUMOF
/**
 * Maximum number of interfaces which can be registered on DLL
 */
#define CAN_DLL_NUMOF       (1)
#endif

/**
 * @brief Parameters to initialize a candev
 */
typedef struct candev_params {
    const char *name;                /**< candev name to set */
#if defined(MODULE_CAN_TRX) || defined(DOXYGEN)
    can_trx_t *trx;                  /**< transceiver to set */
#endif
#if defined(MODULE_FDCAN)
    uint16_t loop_delay;             /**< CAN FD transceiver loop delay */
#endif
#if defined(MODULE_CAN_PM) || defined(DOXYGEN)
    uint32_t rx_inactivity_timeout;  /**< power management rx timeout value */
    uint32_t tx_wakeup_timeout;      /**< power management tx wake up value */
#endif
} candev_params_t;

/**
 * @brief candev descriptor to pass to the device thread
 */
typedef struct candev_dev {
    candev_t *dev;    /**< the device */
    int ifnum;        /**< interface number */
    kernel_pid_t pid; /**< pid */
    const char *name; /**< device name */
#if defined(MODULE_CAN_TRX) || defined(DOXYGEN)
    can_trx_t *trx;   /**< transceiver attached to the device */
#endif
#if defined(MODULE_FDCAN)
    uint16_t loop_delay;    /**< CAN FD transceiver loop delay */
#endif
#if defined(MODULE_CAN_PM) || defined(DOXYGEN)
    uint32_t rx_inactivity_timeout; /**< Min timeout loaded when a frame is received */
    uint32_t tx_wakeup_timeout;     /**< Min timeout loaded when a frame is sent */
    uint32_t last_pm_update;   /**< time when the pm was updated */
    uint32_t last_pm_value;    /**< last pm timer value set */
    ztimer_t pm_timer;         /**< timer for power management */
#endif
} candev_dev_t;

/**
 * @brief Initialize a CAN device thread
 *
 * This function sets up a CAN device thread
 *
 * @param[in] stack      the device thread stack
 * @param[in] stacksize  the device thread stack size
 * @param[in] priority   the device thread priority
 * @param[in] name       the device thread name
 * @param[in] params     the parameters containing the device pointer and the ifnum
 *
 * @return the pid of the created thread
 */
kernel_pid_t can_device_init(char *stack, int stacksize, char priority,
                             const char *name, candev_dev_t *params);

/**
 * @brief Fill in a @p bittiming structure from @p bittiming->bitrate and @p timing_const
 *
 * @param[in] clock         the clock of the CAN controller
 * @param[in] timing_const  the timing parameter of the CAN controller
 * @param[in,out] bittiming the calculated bittiming (bitrate field must be set)
 *
 * @return 0 on success
 * @return < 0 on error
 */
int can_device_calc_bittiming(uint32_t clock, const struct can_bittiming_const *timing_const,
                              struct can_bittiming *bittiming);

#ifdef __cplusplus
}
#endif

/** @} */
