/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     kinetis
 * @{
 *
 * @file
 * @brief       Kinetis Periodic Interrupt Timer implementation
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#ifndef __K20_PIT_H
#define __K20_PIT_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the PIT module and start the timers
 */
int kinetis_pit_timers_init(void);

/**
 * @brief Deinitialize the PIT module
 *
 * @note Also disables the clock gate to save power
 */
int kinetis_pit_timers_deinit(void);

/**
 * @brief Start the PIT timers
 */
void kinetis_pit_timers_start(void);

/**
 * @brief Stop the PIT timers
 */
void kinetis_pit_timers_stop(void);

/**
 * @brief Enable all PIT timer interrupts
 */
void kinetis_pit_timers_irq_enable(void);

/**
 * @brief Disable all PIT timer interrupts
 */
void kinetis_pit_timers_irq_disable(void);

/**
 * @brief Enable the Interrupt for a timer
 *
 * @param[in]   timer   The timer to enable interrupts for
 * @return      0 if successful, -1 if the timer number is invalid
 */
int kinetis_pit_timer_irq_enable(short timer);

/**
 * @brief Disable the timer interrupt for a timer
 *
 * @param[in]   timer   The timer to disable interrupts for
 * @return      0 if successful, -1 if the timer number is invalid
 */
int kinetis_pit_timer_irq_disable(short timer);



/**
 * @brief Set a new timer
 *
 * @param[in]   timer           The timer to configure
 * @param[in]   ticks           The ticks the timer should run
 * @param[in]   is_continuous   true if periodic, false if not repeating
 * @param[in]   callback        The callback to call when the timer expired
 * @param[in]   callback_data   Optional data to pass to the callback
 *
 * @return      0 on success, -1 if the timer is already configured or nonexistent
 */
int kinetis_pit_timer_set(short timer, uint32_t ticks, bool is_continuous, void (*callback)(short), void *callback_data);

/**
 * @brief Stop a timer
 *
 * @param[in]   timer       The timer to stop
 *
 * @return      0 on success, -1 if the timer is already stopped or nonexistent
 */
void kinetis_pit_timer_stop(short timer);

/**
 * @brief Read the current timer value
 *
 * @param[in]   timer       The timer to read
 *
 * @return      0 on success, -1 if the timer is nonexistent
 * @{
 */
uint32_t kinetis_pit_timer_read(short timer);

#ifdef __cplusplus
}
#endif

#endif /* __K20_PIT_H */
