/*
 * Copyright (C) 2017 George Psimenos (gp7g14@soton.ac.uk)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht2x
 * @{
 *
 * @file
 * @brief       Default configuration for SHT2x devices
 *
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 */

#ifndef SHT2X_PARAMS_H
#define SHT2X_PARAMS_H

#include "board.h"
#include "sht2x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the SHT2x driver
 * @{
 */
#ifndef SHT2X_PARAM_I2C
#define SHT2X_PARAM_I2C       	(I2C_DEV(0))
#endif
#ifndef SHT2X_PARAM_ADDR
#define SHT2X_PARAM_ADDR		(SHT2X_I2C_ADDRESS)
#endif
#ifndef SHT2X_PARAM_TYPE
#define SHT2X_PARAM_TYPE		(SHT2X_TYPE_SHT25)
#endif
#ifndef SHT2X_PARAM_RESOLUTION
#define SHT2X_PARAM_RESOLUTION 	(SHT2x_RES_12_14BIT)
#endif

#ifndef SHT2X_PARAMS
#define SHT2X_PARAMS          { .i2c 		= SHT2X_PARAM_I2C, \
                                .addr		= SHT2X_PARAM_ADDR, \
                                .type   	= SHT2X_PARAM_TYPE, \
                                .resolution = SHT2X_PARAM_RESOLUTION }
#endif
/**@}*/

/**
 * @brief   SHT2x configuration
 */
static const sht2x_params_t sht2x_params[] =
{
    SHT2X_PARAMS
};


#ifdef __cplusplus
}
#endif

#endif /* SHT2X_PARAMS_H */
/** @} */
