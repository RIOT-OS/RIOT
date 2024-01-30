/*
 * Copyright (C) 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_countdown Countdown timer
 * @ingroup     drivers_periph
 * @brief       Low-level countdown peripheral driver
 *
 *              Countdown timers are cheap timers that count down from a set
 *              number, then execute an alarm callback.
 *              Focus is on speedy execution of the .set() / .start() functions
 *              which allows them to be used in time critical situations.
 *
 *              Typically the .set() function will be more costly, so a timeout
 *              can be configured in advance that then only needs to be started
 *              by calling the light-weight .start() function.
 * @{
 * @file
 * @brief       Low-level countdown peripheral driver interface definitions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef PERIPH_COUNTDOWN_H
#define PERIPH_COUNTDOWN_H

#include <limits.h>
#include <stdint.h>

#include "timex.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Countdown API flags
 * @{
 */
#define COUNTDOWN_FLAG_UNIT_NS  (0 << 0)    /**< time is in ns  */
#define COUNTDOWN_FLAG_UNIT_US  (1 << 0)    /**< time is in µs  */
#define COUNTDOWN_FLAG_UNIT_MS  (2 << 0)    /**< time is in ms  */
#define COUNTDOWN_FLAG_UNIT_SEC (3 << 0)    /**< time is in s   */
#define COUNTDOWN_FLAG_UNIT_MASK (0x3)      /**< time unit mask */

#define COUNTDOWN_FLAG_PERIODIC (1 << 2)    /**< Countdown will restart when it
                                                 reaches zero */
/** @} */

/**
 * @brief Type of a countdown timer
 */
typedef unsigned countdown_t;

/**
 * @brief Countdown alarm callback function
 *
 * @param[in] arg   Optional user supplied function argument
 */
typedef void (*countdown_cb_t)(void *arg);

/**
 * @brief Countdown driver API implementation
 */
typedef struct countdown_desc countdown_desc_t;

/**
 * @brief   Countdown device
 */
typedef struct {
    const countdown_desc_t *driver; /**< API implementation */
    const void *ctx;                /**< driver context */
} countdown_dev_t;

/**
 * @brief   Countdown device run-time config
 */
typedef struct {
#if IS_USED(MODULE_PERIPH_COUNTDOWN_LONG)
    uint32_t max_timeout;       /**< largest timeout period in base unit */
    uint32_t skip_cb[2];        /**< number of max timeout periods remaining / total */
    uint32_t remaining;         /**< remaining ticks after max timeout periods */
    countdown_cb_t real_cb;     /**< original user callback */
    void *real_ctx;             /**< original user callback argument */
#endif
    uint32_t ticks_per_unit;    /**< counter ticks per base unit */
    uint8_t base_unit;          /**< base unit (ns, µs, …) of the counter */
} countdown_cfg_t;

/**
 * @brief   Countdown device low-level API
 */
struct countdown_desc {

    /**
     * @brief   Initialize Countdown timer
     *
     * @param[in] ctx       Device specific driver context
     * @param[in] max_range Greatest expected timeout
     * @param[in] flags     Unit of the timeout
     * @param[out] mask     timer value mask
     *
     * @returns frequency in Hz on success
     * @returns 0 on error
     */
    uint32_t (*init)(const void *ctx, uint32_t max_range, unsigned flags, uint32_t *mask);

    /**
     * @brief     Set alarm callback
     *
     * @param[in] ctx       Device specific driver context
     * @param[in] cb        Callback function
     * @param[in] arg       Callback function argument
     */
    void (*set_cb)(const void *ctx, countdown_cb_t cb, void *arg);

    /**
     * @brief     Get alarm callback
     *
     * @param[in] ctx       Device specific driver context
     */
    void (*get_cb)(const void *ctx, countdown_cb_t *cb, void **arg);

    /**
     * @brief     Set the alarm
     *
     * @note      This function must cancel any pending alarm
     *
     * @param[in] ctx       Device specific driver context
     * @param[in] timeout   Timeout in ticks
     * @param[in] one_shot  Timer should not reload after alarm
     */
    void (*set)(const void *ctx, uint32_t timeout, bool one_shot);

    /**
     * @brief     Start the countdown
     *
     * @param[in] ctx       Device specific driver context
     */
    void (*start)(const void *ctx);

    /**
     * @brief     Cancel the countdown
     *
     * @note      This function will reset the countdown
     *
     * @param[in] ctx       Device specific driver context
     */
    void (*cancel)(const void *ctx);
};

/**
 * @brief   Initialize a countdown timer
 *
 * @param[in] dev       The Countdown timer
 * @param[in] max_range The range that the countdown timer should have
 * @param[in] flags     The unit of the timeout range
 * @param[in] cb        Callback function to be called on alarm
 * @param[in] arg       Argument of the callback function
 *
 * @return true if an interval of @p max_range can be configured
 * @return false if no configuration exists where @p max_range can be mapped
 *         to a single countdown interval
 */
bool countdown_init(countdown_t dev, uint32_t max_range, unsigned flags,
                    countdown_cb_t cb, void *arg);

/**
 * @brief   Set an alarm time
 *
 * @note    This will cancel any pending alarm.
 *
 * @param dev       The Countdown timer
 * @param timeout   Timeout to configure
 * @param flags     Unit of the timeout, periodic alarm
 */
void countdown_set(countdown_t dev, uint32_t timeout, unsigned flags);

/**
 * @brief   Set an alarm time with an extended period
 *          This allows to set alarms that are longer into the future than the
 *          period of the hardware timer allows.
 *
 *          This software extension requires the `periph_countdown_long` module.
 *
 * @note    This will cancel any pending alarm.
 *
 * @param dev       The Countdown timer
 * @param timeout   Timeout to configure
 * @param flags     Unit of the timeout, periodic alarm
 */
void countdown_set_long(countdown_t dev, uint32_t timeout, unsigned flags);

/**
 * @brief   Start a previously configured alarm
 *
 * @pre     @ref countdown_set() or @ref countdown_set_long() have been called
 *          on the timer
 *
 * @param dev   The Countdown timer
 */
void countdown_start(countdown_t dev);

/**
 * @brief   Cancel a timeout
 *          This stops the alarm, but the timeout remains configured.
 *          The count is reset.
 *
 * @param dev   The Countdown timer
 */
void countdown_cancel(countdown_t dev);

/**
 * @brief   Helper function to convert time value into µs
 *
 * @param time  Time in user supplied unit
 * @param flags Unit of @p time
 *
 * @return Time in µs
 */
static inline uint32_t countdown_unit_mul_us(uint32_t time, unsigned flags)
{
    switch (flags & COUNTDOWN_FLAG_UNIT_MASK) {
    default:
    case COUNTDOWN_FLAG_UNIT_US:
        return time;
    case COUNTDOWN_FLAG_UNIT_MS:
        return time * US_PER_MS;
    case COUNTDOWN_FLAG_UNIT_SEC:
        return time * US_PER_SEC;
    case COUNTDOWN_FLAG_UNIT_NS:
        return time / NS_PER_US;
    }
}

#ifndef _PERIPH_COUNTDOWN_XFA_INIT
/**
 * @brief Array of Countdown timer devices
 */
XFA_USE_CONST(countdown_dev_t, countdown_timers_xfa);
#endif

/**
 * @brief Number of Countdown timers
 */
#define COUNTDOWN_NUMOF XFA_LEN(countdown_dev_t, countdown_timers_xfa)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_COUNTDOWN_H */
/** @} */
