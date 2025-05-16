/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_l3gxxxx
 * @brief       Default configuration for ST L3Gxxxx 3-axis gyroscope sensor family
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */


#include "board.h"
#include "l3gxxxx.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default hardware configuration
 * @{
 */
#if IS_USED(MODULE_L3GXXXX_I2C) || DOXYGEN

#ifndef L3GXXXX_I2C_DEV
/** Default I2C device, if the I2C interface is used */
#define L3GXXXX_I2C_DEV     (I2C_DEV(0))
#endif

#ifndef L3GXXXX_I2C_ADDR
/** Default I2C address, if the I2C interface is used */
#define L3GXXXX_I2C_ADDR    (L3GXXXX_I2C_ADDR_2)
#endif

#ifndef L3GXXXX_I2C_IF_PARAMS
/** Default I2C interface parameter set */
#define L3GXXXX_I2C_IF_PARAMS   .if_params.type = L3GXXXX_I2C, \
                                .if_params.i2c.dev = L3GXXXX_I2C_DEV, \
                                .if_params.i2c.addr = L3GXXXX_I2C_ADDR,
#endif

#endif /* MODULE_L3GXXXX_I2C || DOXYGEN */

#if IS_USED(MODULE_L3GXXXX_SPI) || DOXYGEN

#ifndef L3GXXXX_SPI_DEV
/** Default SPI device, if the SPI interface is used */
#define L3GXXXX_SPI_DEV     SPI_DEV(0)
#endif

#ifndef L3GXXXX_SPI_CLK
/** Default SPI clock frequency, if the SPI interface is used */
#define L3GXXXX_SPI_CLK     (SPI_CLK_1MHZ)
#endif

#ifndef L3GXXXX_SPI_CS
/** Default SPI CS signal, if the SPI interface is used */
#define L3GXXXX_SPI_CS      (GPIO_PIN(0, 0))
#endif

#ifndef L3GXXXX_SPI_IF_PARAMS
/** Default SPI interface parameter set */
#define L3GXXXX_SPI_IF_PARAMS   .if_params.type = L3GXXXX_SPI, \
                                .if_params.spi.dev = L3GXXXX_SPI_DEV, \
                                .if_params.spi.clk = L3GXXXX_SPI_CLK, \
                                .if_params.spi.cs = L3GXXXX_SPI_CS,
#endif

#endif /* MODULE_L3GXXXX_SPI || DOXYGEN */

#ifndef L3GXXXX_INT1_PIN
/** Default MCU pin for INT1 signal */
#define L3GXXXX_INT1_PIN    (GPIO_PIN(0, 1))
#endif

#ifndef L3GXXXX_INT2_PIN
/** Default MCU pin for INT2/DRDY signal */
#define L3GXXXX_INT2_PIN    (GPIO_PIN(0, 2))
#endif

/** @} */

/**
 * @name    Default sensor configuration
 * @{
 */
#if !DOXYGEN
/* Mapping of Kconfig defines to the respective driver enumeration values */

#ifdef CONFIG_L3GXXXX_ODR_100_12
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_100_12)
#elif CONFIG_L3GXXXX_ODR_100_25
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_100_25)
#elif CONFIG_L3GXXXX_ODR_200_12
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_200_12)
#elif CONFIG_L3GXXXX_ODR_200_25
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_200_25)
#elif CONFIG_L3GXXXX_ODR_200_50
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_200_50)
#elif CONFIG_L3GXXXX_ODR_200_70
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_200_70)
#elif CONFIG_L3GXXXX_ODR_400_20
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_400_20)
#elif CONFIG_L3GXXXX_ODR_400_25
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_400_25)
#elif CONFIG_L3GXXXX_ODR_400_50
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_400_50)
#elif CONFIG_L3GXXXX_ODR_400_110
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_400_110)
#elif CONFIG_L3GXXXX_ODR_800_30
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_800_30)
#elif CONFIG_L3GXXXX_ODR_800_35
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_800_35)
#elif CONFIG_L3GXXXX_ODR_800_50
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_800_50)
#elif CONFIG_L3GXXXX_ODR_800_100
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_800_100)
#elif CONFIG_L3GXXXX_ODR_12
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_12)
#elif CONFIG_L3GXXXX_ODR_25
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_25)
#elif CONFIG_L3GXXXX_ODR_50
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_50)
#endif

#ifdef CONFIG_L3GXXXX_SCALE_245_DPS
#define CONFIG_L3GXXXX_SCALE            (L3GXXXX_SCALE_245_DPS)
#elif CONFIG_L3GXXXX_SCALE_500_DPS
#define CONFIG_L3GXXXX_SCALE            (L3GXXXX_SCALE_500_DPS)
#elif CONFIG_L3GXXXX_SCALE_2000_DPS
#define CONFIG_L3GXXXX_SCALE            (L3GXXXX_SCALE_2000_DPS)
#endif

#ifdef CONFIG_L3GXXXX_NO_FILTER
#define CONFIG_L3GXXXX_FILTER_SEL       (L3GXXXX_NO_FILTER)
#elif CONFIG_L3GXXXX_HPF_ONLY
#define CONFIG_L3GXXXX_FILTER_SEL       (L3GXXXX_HPF_ONLY)
#elif CONFIG_L3GXXXX_LPF2_ONLY
#define CONFIG_L3GXXXX_FILTER_SEL       (L3GXXXX_LPF2_ONLY)
#elif CONFIG_L3GXXXX_HPF_AND_LPF2
#define CONFIG_L3GXXXX_FILTER_SEL       (L3GXXXX_HPF_AND_LPF2)
#endif

#ifdef CONFIG_L3GXXXX_HPF_NORMAL
#define CONFIG_L3GXXXX_HPF_MODE         (L3GXXXX_HPF_NORMAL)
#elif CONFIG_L3GXXXX_HPF_REFERENCE
#define CONFIG_L3GXXXX_HPF_MODE         (L3GXXXX_HPF_REFERENCE)
#elif CONFIG_L3GXXXX_HPF_AUTORESET
#define CONFIG_L3GXXXX_HPF_MODE         (L3GXXXX_HPF_AUTORESET)
#endif

#ifdef CONFIG_L3GXXXX_FIFO_MODE_BYPASS
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_BYPASS)
#elif CONFIG_L3GXXXX_FIFO_MODE_FIFO
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_FIFO)
#elif CONFIG_L3GXXXX_FIFO_MODE_STREAM
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_STREAM)
#elif CONFIG_L3GXXXX_FIFO_MODE_STREAM_TO_FIFO
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_STREAM_TO_FIFO)
#elif CONFIG_L3GXXXX_FIFO_MODE_BYPASS_TO_STREAM
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_BYPASS_TO_STREAM)
#elif CONFIG_L3GXXXX_FIFO_MODE_DYNAMIC_STREAM
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_DYNAMIC_STREAM
#elif CONFIG_L3GXXXX_FIFO_MODE_BYPASS_TO_FIFO
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_BYPASS_TO_FIFO)
#endif

#ifdef CONFIG_L3GXXXX_INT1_NO_FILTER
#define CONFIG_L3GXXXX_INT1_FILTER      (L3GXXXX_NO_FILTER)
#elif CONFIG_L3GXXXX_INT1_HPF_ONLY
#define CONFIG_L3GXXXX_INT1_FILTER      (L3GXXXX_HPF_ONLY)
#elif CONFIG_L3GXXXX_INT1_LPF2_ONLY
#define CONFIG_L3GXXXX_INT1_FILTER      (L3GXXXX_LPF2_ONLY)
#elif CONFIG_L3GXXXX_INT1_HPF_AND_LPF2
#define CONFIG_L3GXXXX_INT1_FILTER      (L3GXXXX_HPF_AND_LPF2)
#endif

#endif /* !DOXYGEN */

#ifndef CONFIG_L3GXXXX_ODR
/** Default ODR and cut-off frequency */
#define CONFIG_L3GXXXX_ODR              (L3GXXXX_ODR_100_25)
#endif

#ifndef CONFIG_L3GXXXX_SCALE
/** Default full scale */
#define CONFIG_L3GXXXX_SCALE            (L3GXXXX_SCALE_245_DPS)
#endif

#ifndef CONFIG_L3GXXXX_FILTER_SEL
/** Default filter selection used for output data */
#define CONFIG_L3GXXXX_FILTER_SEL       (L3GXXXX_HPF_AND_LPF2)
#endif

#ifndef CONFIG_L3GXXXX_HPF_MODE
/** Default HPF mode used for output data */
#define CONFIG_L3GXXXX_HPF_MODE         (L3GXXXX_HPF_NORMAL)
#endif

#ifndef CONFIG_L3GXXXX_HPF_CUTOFF
/** Default HPF cutoff frequency 8 Hz */
#define CONFIG_L3GXXXX_HPF_CUTOFF       (0)
#endif

#ifndef CONFIG_L3GXXXX_FIFO_MODE
/** Default FIFO mode if FIO is used */
#define CONFIG_L3GXXXX_FIFO_MODE        (L3GXXXX_FIFO)
#endif

#ifndef CONFIG_L3GXXXX_FIFO_WATERMARK
/** Default FIFO watermark level (threshold) value if FIO is used */
#define CONFIG_L3GXXXX_FIFO_WATERMARK   (23)
#endif

#ifndef CONFIG_L3GXXXX_INT1_X_THRESH
/** Default INT1 threshold for X axis events (~30 dps at fulls scale of ±245 dps) */
#define CONFIG_L3GXXXX_INT1_X_THRESH    (4012)
#endif

#ifndef CONFIG_L3GXXXX_INT1_X_GT_THRESH
/** Default INT1 interrupt enable for |X| > X threshold (X high event) */
#define CONFIG_L3GXXXX_INT1_X_GT_THRESH (true)
#endif

#ifndef CONFIG_L3GXXXX_INT1_X_LT_THRESH
/** Default INT1 interrupt enable for |X| < X threshold (X low event) */
#define CONFIG_L3GXXXX_INT1_X_LT_THRESH (false)
#endif

#ifndef CONFIG_L3GXXXX_INT1_Y_THRESH
/** Default INT1 threshold for Y axis events (~30 dps at fulls scale of ±245 dps) */
#define CONFIG_L3GXXXX_INT1_Y_THRESH    (4012)
#endif

#ifndef CONFIG_L3GXXXX_INT1_Y_GT_THRESH
/** Default INT1 interrupt enable for |Y| < Y threshold (Y low event) */
#define CONFIG_L3GXXXX_INT1_Y_GT_THRESH (true)
#endif

#ifndef CONFIG_L3GXXXX_INT1_Y_LT_THRESH
/** Default INT1 interrupt enable for |Y| > Y threshold (Y high event) */
#define CONFIG_L3GXXXX_INT1_Y_LT_THRESH (false)
#endif

#ifndef CONFIG_L3GXXXX_INT1_Z_THRESH
/** Default INT1 threshold for Z axis events (~30 dps at fulls scale of ±245 dps) */
#define CONFIG_L3GXXXX_INT1_Z_THRESH    (4012)
#endif

#ifndef CONFIG_L3GXXXX_INT1_Z_GT_THRESH
/** Default INT1 interrupt enable for |Z| < Z threshold (Z low event) */
#define CONFIG_L3GXXXX_INT1_Z_GT_THRESH (true)
#endif

#ifndef CONFIG_L3GXXXX_INT1_Z_LT_THRESH
/** Default INT1 interrupt enable for |Z| > Z threshold (Z high event) */
#define CONFIG_L3GXXXX_INT1_Z_LT_THRESH (false)
#endif

#ifndef CONFIG_L3GXXXX_INT1_FILTER
/** Default filter selection used for INT1 interrupt */
#define CONFIG_L3GXXXX_INT1_FILTER      (L3GXXXX_HPF_AND_LPF2)
#endif

#ifndef CONFIG_L3GXXXX_INT1_AND
/** Default event interrupt combination is OR */
#define CONFIG_L3GXXXX_INT1_AND         (false)
#endif

#ifndef CONFIG_L3GXXXX_INT1_LATCH
/** Default INT1 event interrupt latch enabled */
#define CONFIG_L3GXXXX_INT1_LATCH       (true)
#endif

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT) || DOXYGEN
/** Default INT1 parameter set */
#define L3GXXXX_INT1_PARAMS     .int1_pin =  L3GXXXX_INT1_PIN, \
                                .int1_cfg.x_high_enabled = CONFIG_L3GXXXX_INT1_X_GT_THRESH, \
                                .int1_cfg.y_high_enabled = CONFIG_L3GXXXX_INT1_Y_GT_THRESH, \
                                .int1_cfg.z_high_enabled = CONFIG_L3GXXXX_INT1_Z_GT_THRESH, \
                                .int1_cfg.x_low_enabled  = CONFIG_L3GXXXX_INT1_X_LT_THRESH, \
                                .int1_cfg.y_low_enabled  = CONFIG_L3GXXXX_INT1_Y_LT_THRESH, \
                                .int1_cfg.z_low_enabled  = CONFIG_L3GXXXX_INT1_Z_LT_THRESH, \
                                .int1_cfg.x_threshold = CONFIG_L3GXXXX_INT1_X_THRESH, \
                                .int1_cfg.y_threshold = CONFIG_L3GXXXX_INT1_Y_THRESH, \
                                .int1_cfg.z_threshold = CONFIG_L3GXXXX_INT1_Z_THRESH, \
                                .int1_cfg.filter = CONFIG_L3GXXXX_INT1_FILTER, \
                                .int1_cfg.and_or = CONFIG_L3GXXXX_INT1_AND, \
                                .int1_cfg.latch = CONFIG_L3GXXXX_INT1_LATCH,
#else
#define L3GXXXX_INT1_PARAMS
#endif

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA) || DOXYGEN
/** Default INT2 parameter set */
#define L3GXXXX_INT2_PARAMS     .int2_pin =  L3GXXXX_INT2_PIN,
#else
#define L3GXXXX_INT2_PARAMS
#endif

#if IS_USED(MODULE_L3GXXXX_FIFO) || DOXYGEN
/** Default FIFO parameter set */
#define L3GXXXX_FIFO_PARAMS     .fifo_mode =  CONFIG_L3GXXXX_FIFO_MODE, \
                                .fifo_watermark  =  CONFIG_L3GXXXX_FIFO_WATERMARK,
#else
#define L3GXXXX_FIFO_PARAMS
#endif

#if IS_USED(MODULE_L3GXXXX_I2C) || DOXYGEN

#ifndef L3GXXXX_I2C_PARAMS
/** Default I2C device parameter set */
#define L3GXXXX_I2C_PARAMS  { \
                                L3GXXXX_I2C_IF_PARAMS \
                                .odr = CONFIG_L3GXXXX_ODR, \
                                .scale = CONFIG_L3GXXXX_SCALE, \
                                .filter_sel = CONFIG_L3GXXXX_FILTER_SEL, \
                                .hpf_mode = CONFIG_L3GXXXX_HPF_MODE, \
                                .hpf_cutoff = CONFIG_L3GXXXX_HPF_CUTOFF, \
                                L3GXXXX_FIFO_PARAMS \
                                L3GXXXX_INT1_PARAMS \
                                L3GXXXX_INT2_PARAMS \
                            }
#endif
#endif /* MODULE_L3GXXXX_I2C || DOXYGEN */

#if IS_USED(MODULE_L3GXXXX_SPI) || DOXYGEN
#ifndef L3GXXXX_SPI_PARAMS
/** Default SPI device parameter set */
#define L3GXXXX_SPI_PARAMS  { \
                                L3GXXXX_SPI_IF_PARAMS \
                                .odr = CONFIG_L3GXXXX_ODR, \
                                .scale = CONFIG_L3GXXXX_SCALE, \
                                .filter_sel = CONFIG_L3GXXXX_FILTER_SEL, \
                                .hpf_mode = CONFIG_L3GXXXX_HPF_MODE, \
                                .hpf_cutoff = CONFIG_L3GXXXX_HPF_CUTOFF, \
                                L3GXXXX_FIFO_PARAMS \
                                L3GXXXX_INT1_PARAMS \
                                L3GXXXX_INT2_PARAMS \
                            }
#endif
#endif /* MODULE_L3GXXXX_SPI || DOXYGEN */

#ifndef L3GXXXX_SAUL_INFO
/** Default SAUL device info */
#define L3GXXXX_SAUL_INFO   { .name = "l3gxxxx" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const l3gxxxx_params_t l3gxxxx_params[] =
{
#if IS_USED(MODULE_L3GXXXX_I2C) || DOXYGEN
    L3GXXXX_I2C_PARAMS,
#endif
#if IS_USED(MODULE_L3GXXXX_SPI) || DOXYGEN
    L3GXXXX_SPI_PARAMS,
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t l3gxxxx_saul_info[] =
{
    L3GXXXX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
