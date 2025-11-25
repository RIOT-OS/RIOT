/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mcp23x17
 * @{
 *
 * @file
 * @brief       Default configuration for Microchip MCP23x17 I/O expanders
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"
#include "mcp23x17.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default hardware configuration parameters
 * @{
 */

#if IS_USED(MODULE_MCP23X17_SPI) || DOXYGEN

#ifndef MCP23X17_PARAM_SPI_ADDR
/**
 * Default hardware address, if the SPI interface is used.
 * The use of hardware addressing also for SPI devices allows the use of
 * up to eight SPI devices with the same CS signal.
 */
#define MCP23X17_PARAM_SPI_ADDR     (0)
#endif

#ifndef MCP23X17_PARAM_SPI_DEV
/** Default SPI device, if the SPI interface is used */
#define MCP23X17_PARAM_SPI_DEV      (SPI_DEV(0))
#endif

#ifndef MCP23X17_PARAM_SPI_CLK
/** Default SPI clock frequency, if the SPI interface is used */
#define MCP23X17_PARAM_SPI_CLK      (SPI_CLK_10MHZ)
#endif

#ifndef MCP23X17_PARAM_SPI_CS
/** Default SPI CS signal, if the SPI interface is used */
#define MCP23X17_PARAM_SPI_CS       (GPIO_PIN(0, 0))
#endif

#ifndef MCP23X17_PARAM_SPI_INT
/** Default MCU pin for INTA and INTB signal of the default SPI device */
#define MCP23X17_PARAM_SPI_INT      (GPIO_PIN(0, 1))
#endif

#endif /* MODULE_MCP23X17_SPI || DOXYGEN */

#if IS_USED(MODULE_MCP23X17_I2C) || DOXYGEN

#ifndef MCP23X17_PARAM_I2C_ADDR
/** Default hardware address, if the I2C interface is used */
#define MCP23X17_PARAM_I2C_ADDR     (0)
#endif

#ifndef MCP23X17_PARAM_I2C_DEV
/** Default I2C device, if the I2C interface is used */
#define MCP23X17_PARAM_I2C_DEV      (I2C_DEV(0))
#endif

#ifndef MCP23X17_PARAM_I2C_INT
/** Default MCU pin for INTA and INTB signal of the default I2C device */
#define MCP23X17_PARAM_I2C_INT      (GPIO_PIN(0, 2))
#endif

#endif /* MODULE_MCP23X17_I2C || DOXYGEN */

#ifndef MCP23X17_PARAM_RESET_PIN
/** MCU pin for RESET signal is undefinded by default */
#define MCP23X17_PARAM_RESET_PIN    (GPIO_UNDEF)
#endif

#if IS_USED(MODULE_MCP23X17_SPI) || DOXYGEN
#ifndef MCP23X17_SPI_PARAMS
/** Default device parameters, if SPI interface is used */
#define MCP23X17_SPI_PARAMS { \
                                .addr = MCP23X17_PARAM_SPI_ADDR, \
                                .int_pin = MCP23X17_PARAM_SPI_INT, \
                                .reset_pin = MCP23X17_PARAM_RESET_PIN, \
                                .if_params.type = MCP23X17_SPI, \
                                .if_params.spi.dev = MCP23X17_PARAM_SPI_DEV, \
                                .if_params.spi.cs = MCP23X17_PARAM_SPI_CS, \
                                .if_params.spi.clk = MCP23X17_PARAM_SPI_CLK, \
                            }
#endif /* MCP23X17_SPI_PARAMS */
#endif /* MODULE_MCP23X17_SPI || DOXYGEN */

#if IS_USED(MODULE_MCP23X17_I2C) || DOXYGEN
#ifndef MCP23X17_I2C_PARAMS
/** Default device parameters, if I2C interface is used */
#define MCP23X17_I2C_PARAMS { \
                                .addr = MCP23X17_PARAM_I2C_ADDR, \
                                .int_pin = MCP23X17_PARAM_I2C_INT, \
                                .reset_pin = MCP23X17_PARAM_RESET_PIN, \
                                .if_params.type = MCP23X17_I2C, \
                                .if_params.i2c.dev = MCP23X17_PARAM_I2C_DEV, \
                            }
#endif /* MCP23X17_I2C_PARAMS */
#endif /* MODULE_MCP23X17_I2C || DOXYGEN */

#if IS_USED(MODULE_SAUL_GPIO) || DOXYGEN

#ifndef MCP23X17_SAUL_GPIO_PARAMS
/** Example for mapping expander pins to SAUL */
#define MCP23X17_SAUL_GPIO_PARAMS { \
                                        .dev = 0, \
                                        .gpio = { \
                                            .name = "MCP23x17_0 PA0 Input", \
                                            .pin = MCP23X17_GPIO_PIN(0, 0), \
                                            .mode = GPIO_IN, \
                                            .flags = 0, \
                                        } \
                                 }, \
                                 { \
                                        .dev = 0, \
                                        .gpio = { \
                                            .name = "MCP23x17_0 PB5 Output", \
                                            .pin = MCP23X17_GPIO_PIN(1, 5), \
                                            .mode = GPIO_OUT, \
                                            .flags = SAUL_GPIO_INIT_CLEAR, \
                                        } \
                                 },
#endif /* MCP23X17_SAUL_GPIO_PARAMS */
#endif /* MODULE_SAUL_GPIO || DOXYGEN */
/**@}*/

/**
 * @brief   Allocation of MCP23x17 configuration
 */
static const mcp23x17_params_t mcp23x17_params[] =
{
#if IS_USED(MODULE_MCP23X17_SPI) || DOXYGEN
    MCP23X17_SPI_PARAMS,
#endif
#if IS_USED(MODULE_MCP23X17_I2C) || DOXYGEN
    MCP23X17_I2C_PARAMS,
#endif
};

#if IS_USED(MODULE_SAUL_GPIO) || DOXYGEN
/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const  mcp23x17_saul_gpio_params_t mcp23x17_saul_gpio_params[] =
{
    MCP23X17_SAUL_GPIO_PARAMS
};
#endif /* MODULE_SAUL_GPIO || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
