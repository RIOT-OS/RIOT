/*
 * Copyright 2018, Anatoliy Atanasov, Iliyan Stoyanov All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bq27441
 * @brief       Default parameters for the BQ27441 fuel gauge for single-cell Li-Ion batteries
 * @{
 *
 * @file
 * @brief       Default parameters for the BQ27441 fuel gauge
 *
 * @author      Anatoliy Atanasov <anatoliy@6lowpan.io>
 * @author      Iliyan Stoyanov <iliyan@6lowpan.io>
 */

#ifndef BQ27441_PARAMS_H
#define BQ27441_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"  /* THIS INCLUDE IS MANDATORY */
#include "bq27441.h"

/**
 * @brief   Default configuration parameters for BQ27441 sensors
 * @{
 */
#ifndef BQ27441_PARAMS_I2C
#define BQ27441_PARAMS_I2C            (I2C_DEV(0))
#endif
#ifndef BQ27441_PARAMS_ADDR
#define BQ27441_PARAMS_ADDR           (BQ72441_I2C_ADDRESS)
#endif
#ifndef BQ27441_PARAMS_ALARM_PIN
#define BQ27441_PARAMS_ALARM_PIN      GPIO_PIN(2, 3)
#endif

#ifndef BQ27441_PARAMS
#define BQ27441_PARAMS            { .alarm_pin = BQ27441_PARAMS_ALARM_PIN, \
                                      .bus  = BQ27441_PARAMS_I2C, \
                                      .addr = BQ27441_PARAMS_ADDR }
#endif
/** @} */
/**
 * @brief   Allocation of BQ27441 configuration
 */
static const bq27441_param_t params_default[] = {
    #ifdef BQ27441_PARAMS_BOARD
        BQ27441_PARAMS_BOARD
    #else
        BQ27441_PARAMS
    #endif
};

#ifdef __cplusplus
}
#endif
#endif /* BQ27441_PARAMS_H */
