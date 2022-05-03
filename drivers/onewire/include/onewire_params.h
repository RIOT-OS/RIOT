/*
 * Copyright (C) 2017 Frits Kuipers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ds18
 *
 * @{
 * @file
 * @brief       Default configuration for DS1822 and DS18B20 temperature sensors
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 */

#ifndef ONEWIRE_PARAMS_H
#define ONEWIRE_PARAMS_H

#include "board.h"
#include "onewire.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the ds18
 * @{
 */
#ifndef ONEWIRE_PARAMS_PIN
#define ONEWIRE_PARAMS_PIN (GPIO_PIN(0, 0))
#endif

#ifndef ONEWIRE_PARAMS_OUT_MODE
#define ONEWIRE_PARAMS_OUT_MODE (GPIO_OD_PU)
#endif

#ifndef ONEWIRE_PARAMS
#define ONEWIRE_PARAMS \
    { \
        .pin = ONEWIRE_PARAMS_PIN, \
        .out_mode = ONEWIRE_PARAMS_OUT_MODE \
    }
#endif

/**@}*/

/**
 * @brief   Configure ds18
 */
static const onewire_params_t onewire_params[] = {
    ONEWIRE_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* ONEWIRE_PARAMS_H */
/** @} */
