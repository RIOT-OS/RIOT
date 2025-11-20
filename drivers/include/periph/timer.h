/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_periph_timer Timer
 * @ingroup     drivers_periph
 * @brief       Low-level timer peripheral driver
 *
 * # (Low-) Power Implications
 *
 * After calling timer_init(), the underlying hardware timer **should** be
 * powered on and running. When a timer is explicitly stopped by calling
 * timer_stop(), the timer **should** be stopped and powered down (e.g. by
 * peripheral clock gating). Once the timer is started again (by calling
 * timer_start()), it should transparently continue its previously configured
 * operation.
 *
 * While the timer is active, the implementation might need to block certain
 * power modes on specific CPU implementation.
 *
 * @{
 * @file
 * @brief       Low-level timer peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

#include "architecture.h"
#include "periph_cpu.h"
#include "periph_conf.h"

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
#define TIMER_UNDEF         (UINT_FAST8_MAX)
#endif

/**
 * @brief   Default timer type
 *
 * We chose the name of tim_t here to avoid naming clashes with other libraries
 * and vendor device header.
 */
#ifndef HAVE_TIMER_T
typedef uint_fast8_t tim_t;
#endif

/**
 * @brief   Reset the timer when the set() function is called
 *
 * When set, calling the timer_set_periodic() function resets the timer count value.
 */
#ifndef TIM_FLAG_RESET_ON_SET
#define TIM_FLAG_RESET_ON_SET   (0x01)
#endif

/**
 * @brief   Reset the timer on match
 *
 * When set, a match on this channel will reset the timer count value.
 * When set on multiple channels, only the channel with the lowest match value
 * will be reached.
 */
#ifndef TIM_FLAG_RESET_ON_MATCH
#define TIM_FLAG_RESET_ON_MATCH (0x02)
#endif

/**
 * @brief   Keep the timer stopped after setting alarm.
 *
 * When set, the timer will remained stopped after a timer_set_periodic() and
 * can be started manually with timer_start().
 */
#ifndef TIM_FLAG_SET_STOPPED
#define TIM_FLAG_SET_STOPPED    (0x04)
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
 */
#ifndef HAVE_TIMER_ISR_CTX_T
typedef struct {
    timer_cb_t cb;          /**< callback executed from timer interrupt */
    void *arg;              /**< optional argument given to that callback */
} timer_isr_ctx_t;
#endif

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
 * @param[in] cb            this callback is called in interrupt context, the
 *                          emitting channel is passed as argument
 * @param[in] arg           argument to the callback
 *
 * @return                  0 on success
 * @return                  -1 if speed not applicable or unknown device given
 */
int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg);

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
 * @return                  0 on success
 * @return                  -1 on error
 */
int timer_set(tim_t dev, int channel, unsigned int timeout);

/**
 * @brief Set an absolute timeout value for the given channel of the given timer
 *
 * Timers that are less wide than `unsigned int` accept and truncate overflown
 * values.
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] value         the absolute compare value when the callback will be
 *                          triggered
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int timer_set_absolute(tim_t dev, int channel, unsigned int value);

/**
 * @brief Set an absolute timeout value for the given channel of the given timer.
 *        The timeout will be called periodically for each iteration.
 *
 * @note  Only one channel with `TIM_FLAG_RESET_ON_MATCH` can be active.
 *        Some platforms (Atmel) only allow to use the first channel as TOP value.
 *
 * @note  Needs to be enabled with `FEATURES_REQUIRED += periph_timer_periodic`.
 *
 * @param[in] dev           the timer device to set
 * @param[in] channel       the channel to set
 * @param[in] value         the absolute compare value when the callback will be
 *                          triggered
 * @param[in] flags         options
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int timer_set_periodic(tim_t dev, int channel, unsigned int value, uint8_t flags);

/**
 * @brief Clear the given channel of the given timer device
 *
 * @param[in] dev           the timer device to clear
 * @param[in] channel       the channel on the given device to clear
 *
 * @return                  0 on success
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
 * When the timer is stopped, the underlying timer peripheral should be
 * completely powered off.
 *
 * @param[in] dev           the timer to stop
 */
void timer_stop(tim_t dev);

/**
 * @brief   Get the number of different frequencies supported by the given
 *          timer
 *
 * If calling @ref timer_query_freqs_numof for the same timer with an index
 * smaller this number, it hence MUST return a frequency (and not zero).
 *
 * @details This function is marked with attribute pure to tell the compiler
 *          that this function has no side affects and will return the same
 *          value when called with the same parameter. (E.g. to not call this
 *          function in every loop iteration when iterating over all
 *          supported frequencies.)
 */
__attribute__((pure))
uword_t timer_query_freqs_numof(tim_t dev);

/**
 * @brief   Get the number of timer channels for the given timer
 *
 * @details This function is marked with attribute pure to tell the compiler
 *          that this function has no side affects and will return the same
 *          value when called with the same timer as parameter.
 * @details There is a weak default implementation that returns the value of
 *          `TIMER_CHANNEL_NUMOF`. For some MCUs the number of supported
 *          channels depends on @p dev - those are expected to provide there
 *          own implementation of this function.
 */
__attribute__((pure))
uword_t timer_query_channel_numof(tim_t dev);

/**
 * @brief   Iterate over supported frequencies
 *
 * @param   dev     The timer to get the next supported frequency of
 * @param   index   Index of the frequency to get
 * @return          The @p index highest frequency supported by the timer
 * @retval  0       @p index is too high
 *
 * @note    Add `FEATURES_REQUIRED += periph_timer_query_freqs` to your `Makefile`.
 *
 * When called with a value of 0 for @p index, the highest supported frequency
 * is returned. For a value 1 the second highest is returned, and so on. For
 * values out of range, 0 is returned. A program hence can iterate over all
 * supported frequencies using:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * uint32_t freq:
 * for (uword_t i = 0; (freq = timer_query_freqs(dev, i)); i++) {
 *     work_with_frequency(freq);
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Or alternatively:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * for (uword_t i = 0; i < timer_query_freqs_numof(dev); i++) {
 *     work_with_frequency(timer_query_freqs(dev, i));
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint32_t timer_query_freqs(tim_t dev, uword_t index);

/**
 * @brief   Search the frequency supported by the timer that is closest to
 *          a given target frequency, efficiently
 *
 * @param   dev     The timer to get the closest supported frequency for
 * @param   target  Ideal frequency to match
 * @return          The frequency supported by the timer @p dev that is
 *                  closest to @p target
 *
 * @details This will use binary search internally to have an O(log(n))
 *          runtime. This can be relevant on hardware with 16 bit or 32 bit
 *          prescaler registers.
 *
 * @note    Add `FEATURES_REQUIRED += periph_timer_query_freqs` to your
 *          `Makefile`.
 */
uint32_t timer_get_closest_freq(tim_t dev, uint32_t target);

#if defined(DOXYGEN)
/**
 * @brief Check whether a compare channel has matched
 *
 * @return true once after the channel has matched.
 *
 * It is currently not defined whether this keeps returning true after a
 * channel has been polled until that channel is set, or whether later calls
 * return false.
 *
 * This is typically used in spin loops that wait for a timer's completion:
 *
 * ~~~
 * while (!timer_poll_channel(tim, chan)) {};
 * ~~~
 *
 * This function is only available on platforms that implement the
 * `periph_timer_poll` peripheral in addition to `periph_timer`.
 *
 */
/* As this function is polled, it needs to be inlined, so it is typically
 * provided through timer_arch.h. If a platform ever does not need to go
 * through static inline here, this declaration's condition can be extended to
 * be `(defined(MODULE_PERIPH_TIMER_POLL) &&
 * !defined(PERIPH_TIMER_PROVIDES_INLINE_POLL_CHANNEL) || defined(DOXYGEN)` or
 * similar. */
bool timer_poll_channel(tim_t dev, int channel);
#endif

#if defined(MODULE_PERIPH_TIMER_POLL)
#include "timer_arch.h" /* IWYU pragma: export */
#endif

#ifdef __cplusplus
}
#endif

/** @} */
