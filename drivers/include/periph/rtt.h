/*
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_periph_rtt RTT
 * @ingroup     driver_periph
 * @brief       Low-level RTT (Real Time Timer) peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level RTT (Real Time Timer) peripheral driver interface
 *              definitions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __RTT_H
#define __RTT_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* guard file in case no RTT device was specified */
#if RTT_NUMOF

/**
 * @brief Signature for the alarm callback
 *
 * @param[in] arg           Optional argument which is passed to the
 *                          callback
 */
typedef void(*rtt_cb_t)(void *arg);

/**
 * @brief Initialize RTT module
 */
void rtt_init(void);

/**
 * @brief Set a callback for the counter overflow event
 *
 * @param[in] cb            Callback to execute on overflow
 * @param[in] arg           Argument passed to the callback
 */
void rtt_set_overflow_cb(rtt_cb_t cb, void *arg);

/**
 * @brief Clear the overflow callback
 */
void rtt_clear_overflow_cb(void);

/**
 * @brief Get the current RTT counter.
 *
 * @return                  Current value of the RTT counter
 */
uint32_t rtt_get_counter(void);

/**
 * @brief Set the RTT counter to a specified value.
 *
 * @param[in] counter       The value to set the RTT to.
 */
void rtt_set_counter(uint32_t counter);

/**
 * @brief Set an alarm for RTT to the specified value.
 *
 * @param[in] alarm         The value to trigger an alarm when hit
 * @param[in] cb            Callback executed when alarm is hit
 * @param[in] arg           Argument passed to callback when alarm is hit
 */
void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg);

/**
 * @brief Get the value of a set alarm.
 *
 * If no alarm is set, the return value is arbitrary.
 *
 * @return                  Value the alarm is set to
 */
uint32_t rtt_get_alarm(void);

/**
 * @brief Clear any set alarm, do nothing if nothing set
 */
void rtt_clear_alarm(void);

/**
 * @brief Turn the RTT hardware module on
 */
void rtt_poweron(void);

/**
 * @brief Turn the RTT hardware module off
 */
void rtt_poweroff(void);

#endif /* RTT_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* __RTT_H */
/** @} */
