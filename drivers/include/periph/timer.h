/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_timer Timer
 * @ingroup     drivers_periph
 * @brief       Low-level timer peripheral driver
 * @{
 *
 * @file
 * @brief       Low-level timer peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_TIMER_H
#define PERIPH_TIMER_H

#include <limits.h>

#include "periph_cpu.h"
/** @todo remove dev_enums.h include once all platforms are ported to the updated periph interface */
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default timer definition macro
 *
 * Overwrite this in your CPUs periph_cpu.h file if needed
 */
#ifndef TIMER_DEV
#define TIMER_DEV(x)        (x)
#endif

/**
 * @brief   Default value for timer not defined
 */
#ifndef TIMER_UNDEF
#define TIMER_UNDEF         (UINT_MAX)
#endif

/**
 * @brief   Default timer type
 *
 * We chose the name of tim_t here to avoid naming clashes with other libraries
 * and vendor device header.
 */
#ifndef HAVE_TIMER_T
typedef unsigned int tim_t;
#endif

/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 * @param[in] channel   timer channel that triggered the interrupt
 */
typedef void (*timer_cb_t)(void *arg, int channel);

/**
 * @brief   Default interrupt context entry holding callback and argument
 * @{
 */
#ifndef HAVE_TIMER_ISR_CTX_T
typedef struct {
    timer_cb_t cb;          /**< callback executed from timer interrupt */
    void *arg;              /**< optional argument given to that callback */
} timer_isr_ctx_t;
#endif
/** @} */

/**
 * @brief Initialize the given timer
 *
 * Each timer device is running with the given speed. Each can contain one or
 * more channels as defined in periph_conf.h. The timer is configured in
 * up-counting mode and will count until TIMER_x_MAX_VALUE as defined in used
 * board's periph_conf.h until overflowing.
 *
 * The timer will be started automatically after initialization with interrupts
 * enabled.
 *
 * @param[in] dev           the timer to initialize
 * @param[in] freq          requested number of ticks per second
 * @param[in] callback      this callback is called in interrupt context, the
 *                          emitting channel is passed as argument
 * @param[in] arg           argument to the callback
 *
 * @return                  0 on success
 * @return                  -1 if speed not applicable or unknown device given
 */
int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg);

/**
 * @brief Set a given timer channel for the given timer device
 *
 * The callback given during initialization is called when timeout ticks have
 * passed after calling this function
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] timeout       timeout in ticks after that the registered callback
 *                          is executed
 *
 * @return                  1 on success
 * @return                  -1 on error
 */
int timer_set(tim_t dev, int channel, unsigned int timeout);

/**
 * @brief Set an absolute timeout value for the given channel of the given timer
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] value         the absolute compare value when the callback will be
 *                          triggered
 *
 * @return                  1 on success
 * @return                  -1 on error
 */
int timer_set_absolute(tim_t dev, int channel, unsigned int value);

/**
 * @brief Clear the given channel of the given timer device
 *
 * @param[in] dev           the timer device to clear
 * @param[in] channel       the channel on the given device to clear
 *
 * @return                  1 on success
 * @return                  -1 on error
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
 * @brief Start the given timer
 *
 * This function is only needed if the timer was stopped manually before.
 *
 * @param[in] dev           the timer device to start
 */
void timer_start(tim_t dev);

/**
 * @brief Stop the given timer
 *
 * This will effect all of the timer's channels.
 *
 * @param[in] dev           the timer to stop
 */
void timer_stop(tim_t dev);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_TIMER_H */
/** @} */
