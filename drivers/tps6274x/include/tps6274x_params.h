/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tps6274x
 *
 * @{
 * @file
 * @brief       Default configuration for TPS6274x DC-DC Converter
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef TPS6274X_PARAMS_H
#define TPS6274X_PARAMS_H

#include "board.h"
#include "tps6274x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the TPS6274x
 * @{
 */
#ifndef TPS6274X_PARAMS
#define TPS6274X_PARAMS { .vsel = { GPIO_PIN(0, 0), \
                                    GPIO_PIN(0, 1), \
                                    GPIO_PIN(0, 2), \
                                    GPIO_PIN(0, 3), \
                                   },               \
                         .ctrl_pin = GPIO_PIN(0, 4) \
                        }
#endif
/**@}*/

/**
 * @brief   Configure TPS6274X
 */
static const tps6274x_params_t tps6274x_params[] =
{
    TPS6274X_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TPS6274X_PARAMS_H */
