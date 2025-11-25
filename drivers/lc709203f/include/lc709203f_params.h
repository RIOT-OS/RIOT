/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lc709203f
 * @name        Default parameters for the SHTC1 Temperature and humidity sensor
 * @{
 *
 * @file
 * @brief       Default parameters for the SHTC1 Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */
#ifndef LC709203F_PARAMS_H
#define LC709203F_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"  /* THIS INCLUDE IS MANDATORY */
#include "lc709203f.h"

/**
 * @brief   Default configuration parameters for LC709203F sensors
 * @{
 */
#ifndef LC709203F_PARAMS_I2C
#define LC709203F_PARAMS_I2C            (I2C_DEV(0))
#endif
#ifndef LC709203F_PARAMS_ADDR
#define LC709203F_PARAMS_ADDR           (0xb)
#endif
#ifndef LC709203F_PARAMS_ALARM_PIN
#define LC709203F_PARAMS_ALARM_PIN      GPIO_PIN(4, 6)
#endif

#ifndef LC709203F_PARAMS
#define LC709203F_PARAMS            { .alarm_pin = LC709203F_PARAMS_ALARM_PIN, \
                                      .bus  = LC709203F_PARAMS_I2C, \
                                      .addr = LC709203F_PARAMS_ADDR }
#endif
/** @} */

/**
 * @brief   Allocation of LC709203f configuration
 */
static const lc709203f_params_t params_default[] = {
    #ifdef LC709203F_PARAMS_BOARD
        LC709203F_PARAMS_BOARD
    #else
        LC709203F_PARAMS
    #endif
};

#ifdef __cplusplus
}
#endif
#endif /* LC709203F_PARAMS_H */
/** @} */
