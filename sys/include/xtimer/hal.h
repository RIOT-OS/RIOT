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

typedef struct xtimer_llparams_timer {
    uint8_t devid;
    uint8_t chan;
} xtimer_llparams_timer_t;

typedef struct xtimer_params {
    uint32_t hz;
    void *ll;
    uint8_t width;
    uint8_t backoff;
    uint8_t backoff_isr;
    uint8_t overhead;
} xtimer_params_t;

typedef struct xtimer_hal {
    xtimer_params_t *params;
    void *ll;
} xtimer_hal_t;

/**
 * @brief   xtimer hal callback
 */
typedef void (*xtimer_hal_cb_t)(void *);

typedef int (*xtimer_driver_init_t)(xtimer_hal_t *dev, xtimer_hal_cb_t cb, void *arg);

typedef void (*xtimer_driver_set_t)(xtimer_hal_t *dev, uint32_t target,
                                    xtimer_hal_cb_t cb, void *arg);

typedef uint32_t (*xtimer_driver_now_t)(xtimer_hal_t *dev);

typedef void (*xtimer_driver_start_t)(xtimer_hal_t *dev);

typedef void (*xtimer_driver_stop_t)(xtimer_hal_t *dev);

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
