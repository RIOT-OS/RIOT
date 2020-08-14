/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 * @{
 *
 * @file
 * @brief       Timer abstraction layer RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef HAL_HAL_TIMER_H
#define HAL_HAL_TIMER_H

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   HAL timer callback
 */
typedef xtimer_callback_t hal_timer_cb;

/**
 * @brief   The HAL timer structure.
 */
struct hal_timer {
    xtimer_t timer;     /**< the timer */
};

#ifdef __cplusplus
}
#endif

#endif /* HAL_HAL_TIMER_H */
