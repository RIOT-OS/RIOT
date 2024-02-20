/*
 * Copyright (C) 2024 CNRS, France
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_abp2
 * @{
 *
 * @file
 * @brief       Default configuration for ABP2 capaticance-to-digital
 *              converter
 *
 * @author      David Picard <david.picard@clermont.in2p3.fr>
 */

#ifndef ABP2_PARAMS_H
#define ABP2_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "abp2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ABP2 driver
 * @{
 */

#ifdef MODULE_ABP2_SPI              /* default configuration for SPI version */

#ifndef ABP2_PARAM_SPI
#define ABP2_PARAM_SPI              (SPI_DEV(0))
#endif
#ifndef ABP2_PARAM_CLK
#define ABP2_PARAM_CLK              (SPI_CLK_400KHZ)
#endif
#ifndef ABP2_PARAM_CS
#define ABP2_PARAM_CS               (GPIO_PIN(0, 0))
#endif
#ifndef ABP2_PARAM_BUSCFG
#define ABP2_PARAM_BUSCFG           .spi = ABP2_PARAM_SPI, \
                                    .cs  = ABP2_PARAM_CS, \
                                    .clk = ABP2_PARAM_CLK
#endif

#elif defined(MODULE_ABP2_I2C)      /* default configuration for I2C version */

#ifndef ABP2_PARAM_I2C
#define ABP2_PARAM_I2C              I2C_DEV(0)
#endif
#ifndef ABP2_PARAM_BUSCFG
#define ABP2_PARAM_BUSCFG           .i2c  = ABP2_PARAM_I2C
#endif
#endif                              /* end of interface-specific configuration */

/* By default, pick a sensor with a 0..160mbar range. */
#ifndef ABP2_PARAM_RNGMIN
/** @brief Default lower range: 0mbar converted to µbar. */
#define ABP2_PARAM_RNGMIN           (0)
#endif
#ifndef ABP2_PARAM_RNGMAX
/** @brief Default upper range: 160mbar converted to µbar. */
#define ABP2_PARAM_RNGMAX           (160000)
#endif

#ifndef ABP2_PARAMS
/** @brief Default sensor parameters. */
#define ABP2_PARAMS             { \
                                    ABP2_PARAM_BUSCFG, \
                                    .rangeMin = ABP2_PARAM_RNGMIN, \
                                    .rangeMax = ABP2_PARAM_RNGMAX \
                                }
#endif
#ifndef ABP2_SAUL_INFO
/** @brief Sensor driver name in the SAUL framework */
#define ABP2_SAUL_INFO          { .name = "abp2" }
#endif
/** @} */

/**
 * @brief   ABP2 default configuration.
 *
 * This variable can be used to initialize the sensor with default values as
 * demonstrated below.
 *
 * \code
 * abp2_t dev;
 * abp2_init(&dev, &abp2_params);
 * \endcode
 */
static const abp2_params_t abp2_params[] =
{
    ABP2_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t abp2_saul_info[] =
{
    ABP2_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* ABP2_PARAMS_H */
/** @} */
