/*
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_periph_rtt RTT
 * @ingroup     drivers_periph
 * @brief       Low-level RTT (Real Time Timer) peripheral driver
 *
 * # (Low-) Power Implications
 *
 * After the RTT has been initialized (i.e. after calling rtt_init()), the RTT
 * should be powered on and running. The RTT can then be powered off manually
 * at a later point in time by calling the rtt_poweroff() function. When the RTT
 * is powered back on using the rtt_poweron() function, it **should**
 * transparently continue its previously configured operation.
 *
 * On many CPUs, certain power states might need to be blocked in rtt_init(), so
 * that it is ensured that the RTT will function properly while it is enabled.
 *
 * @{
 * @file
 * @brief       Low-level RTT (Real Time Timer) peripheral driver interface
 *              definitions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_RTT_H
#define PERIPH_RTT_H

#include <stdint.h>

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     RTT_FREQUENCY
 *
 * @brief   The desired frequency for the RTT
 */
#ifdef DOXYGEN
#define RTT_FREQUENCY
#endif

/**
 * @def     RTT_MAX_VALUE
 *
 * @brief   The maximum value for the RTT counter, must be (2^n - 1)
 *
 */
#ifdef DOXYGEN
#define RTT_MAX_VALUE
#endif

/**
 * @def     RTT_MIN_OFFSET
 *
 * @brief   The minimum offset to correctly set an rtt callback.
 *
 * If the callback is taking into account rtt_get_counter() then the rtt
 * might advance right between the call to rtt_get_counter() and
 * rtt_set_alarm(). If that happens with val==1, the alarm would be
 * set to the current time, which would then underflow. To avoid this,
 * the alarm should be set at least two ticks in the future.
 *
 * This value can vary depending on the platform.
 *
 */
#ifndef RTT_MIN_OFFSET
#define RTT_MIN_OFFSET (2U)
#endif

/* Allow mock-RTT for unit tests */
#ifdef MOCK_RTT_FREQUENCY
#undef RTT_FREQUENCY
#define RTT_FREQUENCY MOCK_RTT_FREQUENCY
#else
#ifndef RTT_FREQUENCY
#warning "RTT_FREQUENCY undefined. Set RTT_FREQUENCY to the number of ticks " \
         "per second for the current architecture."
#endif
#endif

/* Allow mock-RTT for unit tests */
#ifdef MOCK_RTT_MAX_VALUE
#undef RTT_MAX_VALUE
#define RTT_MAX_VALUE MOCK_RTT_MAX_VALUE
#else
#ifndef RTT_MAX_VALUE
#warning "RTT_MAX_VALUE is undefined. Set RTT_MAX_VALUE to the maximum value " \
         "for the RTT counter, ensure it is (2^n - 1)."
#endif
#endif

/**
 * @brief       Convert microseconds to rtt ticks
 * @param[in]   us      number of microseconds
 * @return              rtt ticks
 */
#define RTT_US_TO_TICKS(us)     (RTT_SEC_TO_TICKS(us) / 1000000UL)

/**
 * @brief       Convert milliseconds to rtt ticks
 * @param[in]   ms      number of milliseconds
 * @return              rtt ticks
 */
#define RTT_MS_TO_TICKS(ms)     (RTT_SEC_TO_TICKS(ms) / 1000UL)

/**
 * @brief       Convert seconds to rtt ticks
 * @param[in]   sec     number of seconds
 * @return              rtt ticks
 */
#define RTT_SEC_TO_TICKS(sec)   (sec * RTT_FREQUENCY)

/**
 * @brief       Convert minutes to rtt ticks
 * @param[in]   min     number of minutes
 * @return              rtt ticks
 */
#define RTT_MIN_TO_TICKS(min)   (RTT_SEC_TO_TICKS((min) * 60))

/**
 * @brief       Convert rtt ticks to microseconds
 * @param[in]   ticks   rtt ticks
 * @return              number of microseconds
 */
#define RTT_TICKS_TO_US(ticks)  ((uint64_t)(ticks) * 1000000UL / RTT_FREQUENCY)

/**
 * @brief       Convert rtt ticks to milliseconds
 * @param[in]   ticks   rtt ticks
 * @return              number of milliseconds
 */
#define RTT_TICKS_TO_MS(ticks)  (RTT_TICKS_TO_US(ticks) / 1000)

/**
 * @brief       Convert rtt ticks to seconds
 * @param[in]   ticks   rtt ticks
 * @return              number of seconds
 */
#define RTT_TICKS_TO_SEC(ticks)  (RTT_TICKS_TO_MS(ticks) / 1000)

/**
 * @brief       Convert rtt ticks to minutes
 * @param[in]   ticks   rtt ticks
 * @return              number of minutes
 */
#define RTT_TICKS_TO_MIN(ticks)  (RTT_TICKS_TO_SEC(ticks) / 60)

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
 * @brief   Set the RTT counter to a specified value.
 *
 * @param[in] counter       The value to set the RTT to.
 *
 * @note    This function is only provided when the feature `periph_rtt_set_counter` is provided
 */
void rtt_set_counter(uint32_t counter);

/**
 * @brief Set an alarm for RTT to the specified absolute target time.
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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_RTT_H */
/** @} */
