/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_shtc1
 * @name        Default parameters for the SHTC1 Temperature and humidity sensor
 * @{
 *
 * @file
 * @brief       Default parameters for the SHTC1 Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef SHTC1_PARAMS_H
#define SHTC1_PARAMS_H

#include "board.h"
#include "shtc1.h"

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * @brief   Default configuration parameters for SHTC1 sensors
 * @{
 */
#ifndef SHTC1_PARAM_I2C
#define SHTC1_PARAM_I2C        (I2C_DEV(0))
#endif
#ifndef SHTC1_PARAM_ADDR
#define SHTC1_PARAM_ADDR       (0x70)
#endif
#ifndef SHTC1_PARAM_CRC
#define SHTC1_PARAM_CRC        CRC_ENABLED
#endif

#ifndef SHTC1_PARAMS
#define SHTC1_PARAMS           { .bus = SHTC1_PARAM_I2C, \
                                 .addr = SHTC1_PARAM_ADDR, \
                                 .crc = SHTC1_PARAM_CRC }
#endif
/** @} */

/**
 * @brief   Allocation of SHTC1 configuration
 */
static const shtc1_params_t shtc1_params[] = {
    #ifdef SHTC1_PARAMS_BOARD
        SHTC1_PARAMS_BOARD
    #else
        SHTC1_PARAMS
    #endif
};
#ifdef __cplusplus
}
#endif
#endif /*SHTC1_PARAMS_C*/
/** @} */
