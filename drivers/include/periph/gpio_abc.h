/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_gpio_abc GPIO Advanced Bitbanging Capabilities (ABC)
 * @ingroup     drivers_periph_gpio
 * @brief       GPIO extension for precisely timed GPIO accesses
 *
 * Description
 * ===========
 *
 * This submodule extends @ref drivers_periph_gpio to allow precisely timed
 * GPIO accesses with a sub-microsecond resolution. The goal of this extension
 * is to provide a platform independent API that allows to implement bit banging
 * protocols with high data rates and tight timing constraints (timing accuracy
 * of 150ns) in a portable manner.
 *
 * Why is this Extension Needed
 * ============================
 *
 * The accuracy of 150ns cannot be be implemented with the default GPIO API, as
 * the function calls add a significant overhead (in the order of hundred
 * nanoseconds on ARM, in the order of microseconds on AVR). Therefore, the user
 * would have to compensate this platform specific delay, making portable bit
 * banging implementations with a timing accuracy of 150ns and better
 * impossible. Each platform specific GPIO ABC implementation will therefore
 * transparently compensate for the platform specific delay.
 *
 * Secondly, no platform independent way of sub microsecond delays is possible,
 * so platform specific implementations under the umbrella of a common API are
 * required.
 *
 * And lastly, as the overhead of a function call has the biggest impact on the
 * frequency at which GPIO accesses can be performed, this overhead should not
 * be paid more than needed. By combining the GPIO access with the high accuracy
 * timing, the overhead of a function call needs to be paid only once.
 *
 * General Design Idea
 * ===================
 *
 * The general idea of this API is to combine the GPIO interface
 * (@ref drivers_periph_gpio) with a high resolution delay, which takes the
 * overhead of calling the API into account and compensates it. Further, it is
 * assumed that the duration of high and low pulses are known ahead of time -
 * usually at compile time. Therefore the macro @ref GPIO_ABC_TICKS or the
 * function @ref gpio_abc_ticks have to be used to translate the delay in
 * nanoseconds into an opaque number of delay ticks, which has to be passed
 * to the GPIO ABC API. It is further assumed that the relation between the
 * ticks and the actual delay can be described by an linear function, with the
 * y-intercept being the duration of a call to @ref gpio_set_for with zero ticks
 * delay and the slope being the number of nanoseconds each tick takes. Each
 * platform specific implementation will provide the parameters of this linear
 * function, so that correct number of delay ticks can be calculated with
 * @ref GPIO_ABC_TICKS or @ref gpio_abc_ticks.
 *
 * Accuracy
 * ========
 *
 * The accuracy of GPIO ABC depends on two aspects
 *
 * 1. On the correctness of the parameters for the delay calculation
 * 2. On how much CPU time is spend between calls to GPIO ABC functions, as this
 *    time is not taken into account
 *
 * It is therefore mandatory that users of GPIO ABC do ***NOT*** perform any
 * non-trival (in terms of CPU cycles) calculations *during* bit banging.
 *
 * Adding Support for GPIO ABC
 * ===========================
 *
 * In order to add support for GPIO ABC to a platform, three things need to
 * be provided:
 *
 * 1. Implementations of @ref gpio_set_for and @ref gpio_clear_for
 * 2. The underlying clock frequency of the timing used as @ref GPIO_ABC_CLOCK,
 *    unless the CPU frequency is used. (If @ref GPIO_ABC_CLOCK is not defined
 *    in `cpu_gpio_abc.h`, it will be defined to @ref CLOCK_CORECLOCK.)
 * 3. The overhead of a function call in cycles of @ref GPIO_ABC_CLOCK as
 *    `GPIO_ABC_OVERHEAD_CYCLES`, or in nanoseconds as
 *    @ref GPIO_ABC_OVERHEAD_NS. (If @ref GPIO_ABC_OVERHEAD_NS is not provided
 *    in `cpu_gpio_abc.h`, it will be calculated in this header based on
 *    @ref GPIO_ABC_CLOCK.)
 * 4. The number of @ref GPIO_ABC_CLOCK cycles the duration of a call to
 *    @ref gpio_set_for or @ref gpio_clear_for is increased when the
 *    `ticks` argument is increased by one. (In other words: The length of one
 *    delay loop iteration in terms of @ref GPIO_ABC_CLOCK.)
 *
 * Implementation Hints
 * --------------------
 *
 * The implementations of @ref gpio_set_for and @ref gpio_clear_for have to
 * inline the logic to set/clear the GPIO pin and the delay loop instead of
 * calling @ref gpio_set and @ref gpio_clear. Otherwise the overhead of the
 * function calls would prevent sending short pulses. The logic for
 * setting/clearing the pin can (and should) be implemented in C. This logic
 * is rather simple and therefore has little potential for compiler
 * optimizations and, thus, will take about the same time independent of the
 * compiler. If CPU cycles counting is used in the delay loop, inline assembly
 * must be used, as even one CPU cycle more or less accumulates over each spin
 * of the loop.  On more sophisticated platforms with features like dynamic
 * branch prediction, special care needs to be taken that each spin of the delay
 * loop requires the same number of CPU cycles.
 *
 * If the delay loop uses a high precision hardware timer, a C implementation
 * should be feasible.
 *
 * Determination of Overhead Cycles and Loop Cycles
 * ------------------------------------------------
 *
 * The test application in `tests/periph_gpio_abc` can be used to determine
 * (or validate) the parameters. You need however to define @ref GPIO_ABC_CLOCK
 * in advance, if it differs from the CPU core clock.
 *
 * You can verify the parameters with an oscilloscope or a logic analyzer (with
 * at least 20 MHz sample rate) using that test application. This is highly
 * encouraged on sophisticated platforms with features like dynamic branch
 * prediction, when CPU cycle counting is used in the delay loop: The dynamic
 * branch prediction will add ***significant*** jitter on the length of the
 * pulses, if the delay loop is not implemented with care.
 *
 * @{
 * @file
 * @brief       GPIO Advanced Bitbanging Capabilities (ABC)
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PERIPH_GPIO_ABC_H
#define PERIPH_GPIO_ABC_H

#include "board.h"
#include "cpu_gpio_abc.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The minimum accuracy a GPIO ABC implementation has to provide in nano seconds
 */
#define GPIO_ABC_MIN_ACCURACY       (150U)

#ifndef GPIO_ABC_CLOCK
/**
 * @brief   The frequency of the clock used for timing in the GPIO ABC
 *          implementation
 *
 * @details If not provided by the implementation, the CPU frequency is used
 */
#define GPIO_ABC_CLOCK              CLOCK_CORECLOCK
#endif /* GPIO_ABC_CLOCK */

#ifndef GPIO_ABC_OVERHEAD_NS
/**
 * @brief   Overhead to compensate for in delay loop measured in nano seconds
 *
 * @details An GPIO ABC implementation has to either provide this macro, or
 *          provide the overhead in terms of `GPIO_ABC_CLOCK` via
 *          `GPIO_ABC_OVERHEAD_CYCLES`
 */
#define GPIO_ABC_OVERHEAD_NS        ((1000000000ULL * GPIO_ABC_OVERHEAD_CYCLES + GPIO_ABC_CLOCK / 2) / GPIO_ABC_CLOCK)
#endif /* GPIO_ABC_MIN_ACCURACY */

/**
 * @brief   Length of one GPIO ABC "tick" (one delay loop spin) in nanoseconds
 */
#define GPIO_ABC_TICK_NS            ((1000000000ULL * GPIO_ABC_TICK_CYCLES + GPIO_ABC_CLOCK / 2) / GPIO_ABC_CLOCK)

/**
 * @brief   Minimum pulse length (in nanosecond) supported by the used GPIO ABC
 *          implementation
 */
#define GPIO_ABC_MIN_PULSE_LEN      GPIO_ABC_OVERHEAD_NS

/**
 * @brief   Expected accuracy of the timer in nanoseconds
 *
 * This assumes that the parameter for overhead and cycles per loop are 100%
 * correct, but the desired delay would be achieved just in the middle of
 * a delay loop iteration (worst case). The value is rounded up.
 */
#define GPIO_ABC_ACCURACY_NS        ((GPIO_ABC_TICK_NS + 1) / 2)

#if GPIO_ABC_ACCURACY_NS > GPIO_ABC_MIN_ACCURACY
#warning "The GPIO ABC implementation has an accuracy of less than 150ns"
#endif

/**
 * @brief   Calculates the ticks parameter from a pulse length in nano seconds
 *
 * This macro can be used instead of @ref gpio_abc_ticks when the pulse length
 * is a compile time constant. When doing so, please check against
 * @ref GPIO_ABC_MIN_PULSE_LEN if the required pulse length is not too short
 * for the GPIO ABC implementation.
 */
#define GPIO_ABC_TICKS(ns) \
    ((int)( \
        ((ns) < GPIO_ABC_OVERHEAD_NS) ? 0 : \
        ((uint64_t)(ns) - GPIO_ABC_OVERHEAD_NS) * GPIO_ABC_CLOCK / (1000000000ULL * GPIO_ABC_TICK_CYCLES)) \
    )

#ifndef GPIO_ABC_HAS_BEGIN
/**
 * @brief   Prepares the board for a bit banging transfer with GPIO ABC
 *
 * Depending on the GPIO ABC implementation, some preparation - such as setting
 * up a high resolution timer - might be required before a bit banging transfer.
 * This function does this if required and adds no overhead if not.
 *
 * @see     gpio_abc_end
 */
static inline void gpio_abc_begin(void)
{
    /* Nothing to do */
}
#endif /* GPIO_ABC_HAS_BEGIN */

#ifndef GPIO_ABC_HAS_END
/**
 * @brief   Function to call at the end of a GPIO ABC bit banging transfer
 *
 * Depending on the GPIO ABC implementation, some configuration done in
 * @ref gpio_abc_begin needs to be undone after the GPIO ABC session, e.g.
 * powering of a high resolution timer in order to reduce power consumption.
 */
static inline void gpio_abc_end(void)
{
    /* Nothing to do */
}
#endif /* GPIO_ABC_HAS_END */

/**
 * @brief   Calculate the delay parameter for precise timing
 *
 * @param   ns      Intended pulse length in nano seconds
 *
 * @return  The delay parameter to pass to @ref gpio_set_for and
 *          @ref gpio_clear_for
 * @retval  -1      The pulse length is too short to be feasible
 *
 * @details If the delay is a compile time constant, use @ref GPIO_ABC_DLEAY
 *          instead
 */
int gpio_abc_ticks(uint16_t ns);

/**
 * @brief   Set the given pin to HIGH and wait for the given duration
 *
 * @param[in] pin       The pin to set
 * @param[in] delay     Number of delay loop iterations calculated with
 *                      @p gpio_abc_ticks
 *
 * @post    The GPIO pin identified by @p pin has been set and afterwards a
 *          delay loop spun for @p delay iterations
 *
 * @details If @p delay is zero or negative, this function returns as soon as
 *          possible
 * @warning Wrap calls to @ref gpio_set_for and @ref gpio_clear_for in a pair
 *          of @ref gpio_abc_begin and @ref gpio_abc_end
 */
void gpio_set_for(gpio_t pin, int delay);

/**
 * @brief   Set the given pin to LOW and wait for the given duration
 *
 * @param[in] pin       The pin to clear
 * @param[in] delay     Number of delay loop iterations calculated with
 *                      @p gpio_abc_ticks
 *
 * @post    The GPIO pin identified by @p pin has been cleared and afterwards a
 *          delay loop spun for @p delay iterations
 *
 * @details If @p delay is zero or negative, this function returns as soon as
 *          possible
 * @warning Wrap calls to @ref gpio_set_for and @ref gpio_clear_for in a pair
 *          of @ref gpio_abc_begin and @ref gpio_abc_end
 */
void gpio_clear_for(gpio_t pin, int delay);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_GPIO_ABC_H */
/** @} */
