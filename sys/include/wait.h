/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_wait   Backend agnostic functions to wait program execution
 * @ingroup     sys
 * @brief       Backend agnostic functions to wait program execution
 *
 * # Usage
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * #include "wait.h"
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{Makefile}
 * # Only if wait_us_spinning() is needed:
 * USEMODULE += wait_us_spinning
 * # If overshoots in the wait_at_least_*() family of functions should
 * # not be ridiculously high:
 * USEMODULE += ztimer_msec
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * # Motivation
 *
 * This wait functions have in common that they abstract the actual
 * implementation of the wait for an increased flexibility. This is useful
 * for two narrow and distinct use cases:
 *
 * ## Low Accuracy Wait
 *
 * For use cases such as waiting 1 ms for a device to reset during start up it
 * matters little if the wait ends up being 1 ms, 2 ms or even 10 ms. From the
 * driver perspective it is also not important if that wait is allowing the
 * MCU to enter a lower power mode or not - the driver will work correctly
 * either way. However, if the application developer enabled a low power
 * clock anyway, this can be taken as a hint that low power sleeps are
 * preferable. Hence, a friendly driver would take that hint it use a low
 * power wait if that is available anyway.
 *
 * This is what the `wait_at_least_<GRANULARY>()` family of functions provide.
 *
 * ## High Accuracy Wait
 *
 * Short and highly accurate waits can be challenging on lower end hardware
 * using standard infrastructure. @ref wait_us_spinning allows slow platforms
 * to provide a specialized wait function using methods like cycle-counting
 * to still achieve high accuracy. While this still is a best effort approach,
 * it will often allow tasks such as bit-banging to work on lower end hardware
 * that otherwise wouldn't be able to meet the timing requirements.
 *
 * ## Comparison with `ztimer`
 *
 * @ref sys_ztimer already provides @ref ztimer_sleep to allow waiting the
 * execution of the running thread. @ref ztimer_sleep allows via the
 * selection of the clock to select the best fitting trade-off between
 * resolution, accuracy (clock drift), power consumption and so on. The
 * low accuracy wait functions provided here do not allow to pick the right
 * clock for the task and, hence, are in general inferior. But for use cases
 * where the actually used backend is not important, it can be better to not
 * add a strict dependency and let the application developer decide.
 *
 * @{
 *
 * @file
 * @brief       Convenience functions to wait program execution
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef WAIT_H
#define WAIT_H

#include <assert.h>
#include <stdint.h>

#include "clk.h"
#include "compiler_hints.h"
#include "time_units.h"
#include "wait_arch.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CPU_CYCLES_PER_WAIT_LOOP_ITERATION) || defined(DOXYGEN)
/**
 * @brief   Number of CPU cycles spent for each iteration of the wait loop
 *
 * @note    This can be fine-tuned per arch to get better accuracy. However, it
 *          *must not* be overestimated: Overshooting (waiting longer than
 *          asked for) is fine, but not the other way around.
 *
 * @warning Beware modern CPU architectures: On Cortex M7 CPUs with dual issue
 *          a two instruction delay loop indeed takes only one cycle per
 *          iteration once the dynamic branch prediction converges.
 */
#define CPU_CYCLES_PER_WAIT_LOOP_ITERATION 1
#endif

/**
 * @name    Low accuracy "whatever is available" waits
 *
 * This group of functions wait execution with a low accuracy and use whatever
 * backend is available. They do prefer low power variants if available anyway,
 * but it is up the application developer to actually enable such backends
 * (e.g. `ztimer_msec`) - if they are supported.
 * @{
 */

/**
 * @brief   Internal function to do busy wait using a down-counting loop
 * @internal
 *
 * @param[in]   iterations  Number of loop iterations to wait
 */
static inline void wait_busy_loop(uint32_t iterations)
{
    /* The loop will not terminate if iterations is non-zero. We don't care
     * about an overflow here, as `wait_busy_loop(UINT32_MAX)` is an insane
     * thing to do. */
    iterations++;
    do {
        iterations--;
        /* tell compiler that iterations have been changed via inline asm
         * constraints. No asm is actually needed */
        __asm__ volatile(
                ""
                : "+r"(iterations)
                : /* no inputs */
                : /* no clobbers */
        );
    } while (iterations);
}

/**
 * @brief   Delay program execution for roughly the given amount of
 *          microseconds
 *
 * @param[in]   usecs       Duration to wait execution in microseconds
 *
 * @warning     This function gives **no guarantees** regarding accuracy or
 *              power saving. Use @ref ztimer_sleep instead when you need
 *              accuracy or power saving.
 */
static inline void wait_at_least_us(uint16_t usecs)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_sleep(ZTIMER_USEC, usecs);
    }
    else {
        uint32_t iterations = usecs * (uint64_t)coreclk() / US_PER_SEC;
        wait_busy_loop(iterations);
    }
}

/**
 * @brief   Delay program execution for roughly the given amount of
 *          milliseconds
 *
 * @param[in]   msecs       Duration to wait execution in milliseconds
 *
 * @warning     This function gives **no guarantees** regarding accuracy or
 *              power saving. Use @ref ztimer_sleep instead when you need
 *              accuracy or power saving.
 */
static inline void wait_at_least_ms(uint16_t msecs)
{
    if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_sleep(ZTIMER_MSEC, msecs);
    }
    else if (IS_USED(MODULE_ZTIMER_USEC)) {
        /* msecs is 16 bit, so multiplication by 1000 won't overflow */
        ztimer_sleep(ZTIMER_USEC, (uint32_t)msecs * US_PER_MS);
    }
    else {
        uint32_t iterations = msecs * (uint64_t)coreclk() / MS_PER_SEC;
        wait_busy_loop(iterations);
    }
}

/**
 * @brief   Delay program execution for roughly the given amount of
 *          econds
 *
 * @param[in]   secs        Duration to wait execution in seconds
 *
 * @warning     This function gives **no guarantees** regarding accuracy or
 *              power saving. Use @ref ztimer_sleep instead when you need
 *              accuracy or power saving.
 */
static inline void wait_at_least_s(uint8_t secs)
{
    if (IS_USED(MODULE_ZTIMER_MSEC)) {
        /* secs is 8 bit, so multiplication by 1000 won't overflow */
        ztimer_sleep(ZTIMER_MSEC, (uint32_t)secs * MS_PER_SEC);
    }
    else {
        wait_at_least_ms((uint16_t)secs * 1000);
    }
}
/** @} */

#if !defined(HAS_WAIT_US_SPINNING) || defined(DOXYGEN)
/**
 * @brief   Delay program execution for the given amount of microseconds
 *
 * @param[in]   usecs       Duration to wait execution in microseconds
 *
 * @warning This wait is done *busy waiting*. No lower priority thread will
 *          run and no power saving mode will be entered during this wait.
 *          It may be useful for short waits that require additional accuracy.
 *
 * @note    Some MCU families will provide platform specific implementations
 *          that surpass the accuracy of @ref ztimer_spin in an attempt to
 *          allow even low end MCUs to perform tasks such as bit-banging.
 *
 * @warning @p usecs needs to be a compile time constant.
 *
 * @warning It is expected that the caller disables IRQs prior to the call, if
 *          upper bound latency matters. But if not, why using this API in
 *          the first place?
 *
 * This is provided by the module `wait_us_spinning`.
 *
 * While the implementation indeed could disable IRQs during the spinning, this
 * alone is not sufficient anyway: The caller likely wants to do something with
 * an accurate wait. E.g. to have an accurate 50 μs pulse on a GPIO pin, the
 * caller would have to do something like
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *     unsigned state = irq_disable();
 *     gpio_ll_set(port, 1 << pin);
 *     wait_us_spinning(50);
 *     gpio_ll_clear(port, 1 << pin);
 *     irq_restore(state);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * anyway. Hence, IRQs will be disabled for any serious use of this API anyway,
 * so no need to introduce further overhead by disabling them again.
 */
static inline void wait_us_spinning(uint32_t usecs)
{
    assert(IS_CT_CONSTANT(usecs));
    ztimer_spin(ZTIMER_USEC, usecs);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* WAIT_H */
/** @} */
