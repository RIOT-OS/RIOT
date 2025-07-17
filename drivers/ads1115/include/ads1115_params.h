/*
 * Copyright (C) Baptiste Le Duc <baptiste.leduc38@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#pragma once

/**
 * @ingroup     drivers_ads1115
 * @{
 *
 * @file
 * @brief       Default configuration for ADS1115 device
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 */

#include "board.h"
#include "ads1115.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    Set default configuration parameters for the ADS1115 driver
 * @{
 */
#ifndef ADS1115_PARAM_I2C
#define ADS1115_PARAM_I2C        (I2C_DEV(0))
#endif
#ifndef ADS1115_PARAM_ADDR
#define ADS1115_PARAM_ADDR       (0x48)
#endif
#ifndef ADS1115_PARAM_MUX
#define ADS1115_PARAM_MUX        (ADS1115_MUX_AIN0_AIN1)
#endif
#ifndef ADS1115_PARAM_PGA
#define ADS1115_PARAM_PGA        (ADS1115_PGA_2_048V)
#endif
#ifndef ADS1115_PARAM_MODE
#define ADS1115_PARAM_MODE       (ADS1115_MODE_SINGLE)
#endif
#ifndef ADS1115_PARAM_DR
#define ADS1115_PARAM_DR         (ADS1115_DR_128)
#endif
#ifndef ADS1115_PARAM_COMP_MODE
#define ADS1115_PARAM_COMP_MODE  (ADS1115_COMP_MODE_TRADITIONAL)
#endif
#ifndef ADS1115_PARAM_COMP_POLARITY
#define ADS1115_PARAM_COMP_POLARITY (ADS1115_COMP_POLARITY_LOW)
#endif
#ifndef ADS1115_PARAM_COMP_LATCH
#define ADS1115_PARAM_COMP_LATCH (ADS1115_COMP_LATCH_DISABLE)
#endif
#ifndef ADS1115_PARAM_COMP_QUEUE
#define ADS1115_PARAM_COMP_QUEUE (ADS1115_COMP_QUEUE_DISABLE)
#endif

#ifndef ADS1115_PARAMS
#define ADS1115_PARAMS          { \
            .i2c = ADS1115_PARAM_I2C, \
            .addr = ADS1115_PARAM_ADDR, \
            .mux = ADS1115_PARAM_MUX, \
            .pga = ADS1115_PARAM_PGA, \
            .mode = ADS1115_PARAM_MODE, \
            .dr = ADS1115_PARAM_DR, \
            .comp_mode = ADS1115_PARAM_COMP_MODE, \
            .comp_polarity = ADS1115_PARAM_COMP_POLARITY, \
            .comp_latch = ADS1115_PARAM_COMP_LATCH, \
            .comp_queue = ADS1115_PARAM_COMP_QUEUE \
}
#endif

static const ads1115_params_t ads1115_params[] = {
    ADS1115_PARAMS
};



#ifdef __cplusplus
}
#endif
