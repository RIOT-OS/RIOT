/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level timer peripheral driver
 * @{
 *
 * @file        periph/timer.h
 * @brief       Low-level timer peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __TIMER_H
#define __TIMER_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Definition of available timers
 *
 * Each timer is based on a hardware timer, which can further have 1 or more channels.
 * To this point 4 timers are possible, might need to be expanded for some cases.
 */
typedef enum {
#if TIMER_0_EN
    TIMER_0 = 0,            /**< 1st timer */
#endif
#if TIMER_1_EN
    TIMER_1,                /**< 2nd timer */
#endif
#if TIMER_2_EN
    TIMER_2,                /**< 3rd timer */
#endif
#if TIMER_3_EN
    TIMER_3,                /**< 4th timer */
#endif
    TIMER_UNDEFINED         /**< fall-back if no timer is defined */
} tim_t; /* named tim instead of timer to avoid conflicts with vendor libraries */

/**
 * @brief Initialize the given timer
 *
 * Each timer device is running with the given speed. Each can contain one or more channels
 * as defined in periph_conf.h. The timer is configured in up-counting mode and will count
 * until TIMER_x_MAX_VALUE as defined in used board's periph_conf.h until overflowing.
 *
 * The timer will be started automatically after initialization with interrupts enabled.
 *
 * @param[in] dev           the timer to initialize
 * @param[in] ticks_per_us  the timers speed in ticks per us
 * @param[in] callback      this callback is called in interrupt context, the emitting channel is
 *                          passed as argument
 *
 * @return                  returns 0 on success, -1 if speed not applicable of unknown device given
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int));

/**
 * @brief Set a given timer channel for the given timer device. The callback given during
 * initialization is called when timeout ticks have passed after calling this function
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] timeout       timeout in ticks after that the registered callback is executed
 *
 * @return                  1 on success, -1 on error
 */
int timer_set(tim_t dev, int channel, unsigned int timeout);

/**
 * @brief Set an absolute timeout value for the given channel of the given timer device
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] value         the absolute compare value when the callback will be triggered
 *
 * @return                  1 on success, -1 on error
 */
int timer_set_absolute(tim_t dev, int channel, unsigned int value);

/**
 * @brief Clear the given channel of the given timer device
 *
 * @param[in] dev           the timer device to clear
 * @param[in] channel       the channel on the given device to clear
 *
 * @return                  1 on success, -1 on error
 */
int timer_clear(tim_t dev, int channel);

/**
 * @brief Read the current value of the given timer device
 *
 * @param[in] dev           the timer to read the current value from
 *
 * @return                  the timers current value
 */
unsigned int timer_read(tim_t dev);

/**
 * @brief Start the given timer. This function is only needed if the timer was stopped manually before
 *
 * @param[in] dev           the timer device to stop
 */
void timer_start(tim_t dev);

/**
 * @brief Stop the given timer - this will effect all of the timer's channels
 *
 * @param[in] dev           the timer to stop
 */
void timer_stop(tim_t dev);

/**
 * @brief Enable the interrupts for the given timer
 *
 * @param[in] dev           timer to enable interrupts for
 */
void timer_irq_enable(tim_t dev);

/**
 * @brief Disable interrupts for the given timer
 *
 * @param[in] dev           the timer to disable interrupts for
 */
void timer_irq_disable(tim_t dev);

/**
 * @brief Reset the up-counting value to zero for the given timer
 *
 * Note that this function effects all currently set channels and it can lead to non-deterministic timeouts
 * if any channel is active when this function is called.
 *
 * @param[in] dev           the timer to reset
 */
void timer_reset(tim_t dev);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H */
/** @} */
