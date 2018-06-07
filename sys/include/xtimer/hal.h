/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_xtimer

 * @{
 * @file
 * @brief   xtimer hardware abstraction layer
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 */
#ifndef XTIMER_HAL_H
#define XTIMER_HAL_H

#ifndef XTIMER_H
#error "Do not include this file directly! Use xtimer.h instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   xtimer low-level paramters for timer
 */
typedef struct xtimer_llparams_timer {
    uint8_t devid;    /**< device timer number */
    uint8_t chan;     /**< device channel number */
} xtimer_llparams_timer_t;

/**
 * @brief   xtimer hal parameters
 */
typedef struct xtimer_params {
    uint32_t hz;             /**< device frequency */
    uint32_t backoff;        /**< backoff time in microseconds */
    uint32_t backoff_isr;    /**< ISR backoff time in microseconds */
    void *ll;                /**< low-level device parameters */
    uint8_t width;           /**< number of bits in device counter */
    uint8_t overhead;        /**< overhead time in ticks */
    uint8_t flags;           /**< configuration flags */
} xtimer_params_t;

/**
 * @brief   xtimer hal device descriptor
 */
typedef struct xtimer_hal {
    xtimer_params_t *params;    /**< device parameters */
    void *ll;                   /**< low-level device descriptor details */
} xtimer_hal_t;

/**
 * @brief   xtimer hal callback
 */
typedef void (*xtimer_hal_cb_t)(void *);

/**
 * @brief   Initialize a low-level timer (callback typedef)
 *
 * Some drivers utilize the callback when initializing, and others utilize it
 * when setting the alarm.
 *
 * @param[in] dev       device descriptor of timer to initialize
 * @param[in] cb        callback to be executed when alarm sounds
 * @param[in] arg       argument for callback
 *
 * @return    >=0 when successful, <0 on failure
 */
typedef int (*xtimer_driver_init_t)(xtimer_hal_t *dev, xtimer_hal_cb_t cb, void *arg);

/**
 * @brief   Set an alarm for a low-level timer (callback typedef)
 *
 * Some drivers utilize the callback when initializing, and others utilize it
 * when setting the alarm.
 *
 * @param[in] dev       device descriptor of timer to set alarm for
 * @param[in] target    target time, in ticks
 * @param[in] cb        callback to be executed when alarm sounds
 * @param[in] arg       argument for callback
 */
typedef void (*xtimer_driver_set_t)(xtimer_hal_t *dev, uint32_t target,
                                    xtimer_hal_cb_t cb, void *arg);

/**
 * @brief   Get current time from low-level timer (callback typedef)
 *
 * @param[in] dev    device descriptor of timer to get time from
 *
 * @return    current time of that timer in ticks
 */
typedef uint32_t (*xtimer_driver_now_t)(xtimer_hal_t *dev);

/**
 * @brief   Start a low-level timer (callback typedef)
 *
 * @param[in] dev    device descriptor of timer to start
 */
typedef void (*xtimer_driver_start_t)(xtimer_hal_t *dev);

/**
 * @brief   Stop a low-level timer (callback typedef)
 *
 * This is expected to prevent the stopped timer from blocking power
 * management modes
 *
 * @param[in] dev    device descriptor of timer to stop
 */
typedef void (*xtimer_driver_stop_t)(xtimer_hal_t *dev);

/**
 * @brief   xtimer low-level driver entry
 */
typedef struct xtimer_driver {
    xtimer_driver_init_t init;
    xtimer_driver_set_t set;
    xtimer_driver_now_t now;
    xtimer_driver_start_t start;
    xtimer_driver_stop_t stop;
} xtimer_driver_t;

/**
 * @brief drop bits of a value that don't fit into the low-level timer.
 */
static inline uint32_t _xtimer_lltimer_mask(const uint8_t width, uint32_t val)
{
    if (width >= 32) {
        return val;
    }

    return val & (((uint32_t)(-1) << width) >> width);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* XTIMER_HAL_H */
