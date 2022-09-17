/*
 * Copyright (C) 2022 samkno1
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_a3g4250d
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Samuel Kayode <bossmansk.sk@gmail.com>
 */

#ifndef A3G4250D_PARAMS_H
#define A3G4250D_PARAMS_H

#include "board.h"
#include "a3g4250d.h"
#include "a3g4250d_constants.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */

#if MODULE_PERIPH_I2C || DOXYGEN
#ifndef A3G4250D_PARAM_I2C_DEV
#define A3G4250D_PARAM_I2C_DEV      (I2C_DEV(0))
#endif

#ifndef A3G4250D_PARAM_I2C_ADDR
#define A3G4250D_PARAM_I2C_ADDR     (A3G4250D_I2C_ADD_H)
#endif
#endif /* MODULE_PERIPH_I2C */

#if MODULE_PERIPH_SPI || DOXYGEN
#ifndef A3G4250D_PARAM_SPI_DEV
#define A3G4250D_PARAM_SPI_DEV        (SPI_DEV(0))
#endif

#ifndef A3G4250D_PARAM_SPI_NSS_PIN
#define A3G4250D_PARAM_SPI_NSS_PIN     GPIO_PIN(0, 6)
#endif
#endif /* MODULE_PERIPH_SPI */

/**
 * @brief   Default I2C parameters if non provided
 */
#define A3G4250D_PARAMS_I2C              
{
    .i2c.dev = A3G4250D_PARAM_I2C_DEV,                 
    .i2c.addr = A3G4250D_PARAM_I2C_ADDR,               
    .i2c_data_rate = A3G4250D_ODR_100Hz,
    .i2c_bandwidth = A3G4250D_CUT_OFF_LOW,           
    .i2c_filt_mode = A3G4250D_HP_NORMAL_MODE,
    .i2c_filt_freq = A3G4250D_HP_LEVEL_0,
    .i2c_endian = A3G4250D_AUX_MSB_AT_LOW_ADD,
    .i2c_fifo_mode = A3G4250D_FIFO_BYPASS_MODE,               
}

/**
 * @brief Default SPI parameters if none provided
 * 
 */
#define A3G4250D_PARAMS_SPI              
{
    .spi.dev = A3G4250D_PARAM_SPI_DEV,                 
    .spi.nss_pin = A3G4250D_PARAM_SPI_NSS_PIN,              
    .spi_data_rate = A3G4250D_ODR_100Hz,
    .spi_bandwidth = A3G4250D_CUT_OFF_LOW,           
    .spi_filt_mode = A3G4250D_HP_NORMAL_MODE,
    .spi_filt_freq = A3G4250D_HP_LEVEL_0,
    .spi_endian = A3G4250D_AUX_MSB_AT_LOW_ADD,
    .spi_fifo_mode = A3G4250D_FIFO_BYPASS_MODE,                               
}

/**
 * @brief   Default SAUL meta information
 */
#ifndef A3G4250D_SAUL_INFO
#if MODULE_A3G4250D_I2C && MODULE_A3G4250D_SPI
#define A3G4250D_SAUL_INFO    { .name = "a3g4250d:0" }, \
                              { .name = "a3g4250d:1" },
#else /* MODULE_A3G4250D_I2C && MODULE_A3G4250D_SPI */
#define A3G4250D_SAUL_INFO    { .name = "a3g4250d" }
#endif /* MODULE_A3G4250D_I2C && MODULE_A3G4250D_SPI */
#endif /* A3G4250D_SAUL_INFO */
/**@}*/

/**
 * @brief Configure params for A3G4250D
 * 
 */
static const a3g4250d_params_t a3g4250d_params[] =
{
#if MODULE_A3G4250D_I2C || DOXYGEN
    A3G4250D_PARAMS_I2C,
#endif
#if MODULE_A3G4250D_SPI || DOXYGEN
    A3G4250D_PARAMS_SPI,
#endif
};

/**
 * @brief Additional meta information to keep in the SAUL registry
 *
 */
static const saul_reg_info_t a3g4250d_saul_info[] =
{
    A3G4250D_SAUL_INFO
};

/**
 * @brief Number of configured sensors
 * 
 */
#define A3G4250D_NUMOF      ARRAY_SIZE(a3g4250d_params)

#ifdef __cplusplus
}
#endif

#endif /* A3G4250D_PARAMS_H */
/** @} */
