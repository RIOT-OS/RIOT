/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_systick SysTick Timer
 * @ingroup     cpu_cortexm_common
 * @ingroup     drivers_periph
 * @brief       Low-level timer based on SysTick
 *
 * SysTick is a 24 bit down-counting timer that is implemented on every Cortex-M
 * processor. It runs at the same frequency as the CPU and will generate an
 * interrupt when it reaches zero.
 *
 * To comply with the RIOT timer interface that expects an up-counting, monotonic
 * timer, SysTick is augmented by software to fulfill these requirements.
 * This also adds a software prescaler to simulate arbitrary frequencies and introduces
 * a virtual, 32 bit counter.
 * That allows for longer intervals than would be possible with the SysTick hardware
 * peripheral alone.
 *
 * @{
 *
 * @file
 * @brief       Low-level SysTick timer peripheral driver interface definitions
 *              The API makes no guarantees about thread-safety.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SysTick is extended to 32 bit by software
 */
#define SYSTICK_TIMER_MAX_VALUE UINT32_MAX

/**
 * @brief Initialize the SysTick timer
 *
 * The timer is configured in up-counting mode and will count until
 * SYSTICK_TIMER_MAX_VALUE until overflowing.
 *
 * The timer will be started automatically after initialization with interrupts
 * enabled.
 *
 * The user must implement a `void systick_callback(void)` function that will
 * be called when the timer expires.
 *
 * @param[in] freq          requested number of ticks per second
 *
 *                          If you want to set this to anything other than `CLOCK_CORECLOCK`
 *                          the `systick_prescaler` pseudo-module is required.
 */
void systick_timer_init(unsigned long freq);

/**
 * @brief Set the SysTick timer
 *
 * The callback given during initialization is called when timeout ticks have
 * passed after calling this function
 *
 * @param[in] timeout       timeout in ticks after that the registered callback
 *                          is executed
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int systick_timer_set(unsigned int timeout);

/**
 * @brief Set an absolute timeout value for the SysTick timer
 *
 * @param[in] timeout       the absolute compare value when the callback will be
 *                          triggered
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int systick_timer_set_absolute(unsigned int timeout);

/**
 * @brief Disable the alarm from the SysTick timer
 */
void systick_timer_clear(void);

/**
 * @brief Read the current (virtual) value of the SysTick timer
 *
 * @return                  the timers current value
 */
unsigned int systick_timer_read(void);

/**
 * @brief Start the SysTick timer
 *
 * This function is only needed if the timer was stopped manually before.
 */
void systick_timer_start(void);

/**
 * @brief Stop the SysTick timer
 *
 * When the timer is stopped, no alarms will be executed and the counter
 * does not advance.
 */
void systick_timer_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTICK_H */
/** @} */
