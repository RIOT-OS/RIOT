/*
 * Copyright (C) 2017 HAW HAmburg
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp23x17
 * @{
 *
 * @file
 * @brief       Default configuration for Microchip MCP23x17 I/O expanders
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 */

#ifndef MCP23X17_PARAMS_H
#define MCP23X17_PARAMS_H

#include "board.h"
#include "mcp23x17.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef MCP23X17_PARAM_DEV
/** Default device is I2C_DEV(0) */
#define MCP23X17_PARAM_DEV          (I2C_DEV(0))
#endif

#ifndef MCP23X17_PARAM_ADDR
/** Default device address offset is 0 */
#define MCP23X17_PARAM_ADDR         (MCP23X17_BASE_ADDR + 0)
#endif

#ifndef MCP23X17_PARAM_CS
/** I2C is used by default, SPI CS signal pin is undefined therefore */
#define MCP23X17_PARAM_CS           (GPIO_UNDEF)
#endif

#ifndef MCP23X17_PARAM_CLK
/** If SPI is used, the default SPI clock speed is 10 MHz */
#define MCP23X17_PARAM_CLK          (SPI_CLK_10MHZ)
#endif

#ifndef MCP23X17_PARAM_INT_PIN
/** Default MCU pin for INTA and INTB signal */
#define MCP23X17_PARAM_INT_PIN      (GPIO_PIN(0, 0))
#endif

#ifndef MCP23X17_PARAM_RESET_PIN
/** MCU pin for RESET signal is undefinded by default */
#define MCP23X17_PARAM_RESET_PIN    (GPIO_UNDEF)
#endif

#ifndef MCP23X17_PARAMS
#define MCP23X17_PARAMS { \
                             .dev = MCP23X17_PARAM_DEV, \
                             .addr = MCP23X17_PARAM_ADDR, \
                             .cs = MCP23X17_PARAM_CS, \
                             .clk = MCP23X17_PARAM_CLK, \
                             .int_pin = MCP23X17_PARAM_INT_PIN, \
                             .reset_pin = MCP23X17_PARAM_RESET_PIN, \
                        }
#endif

#ifndef MCP23X17_SAUL_GPIO_PARAMS
/** Example for mapping expander pins to SAUL */
#define MCP23X17_SAUL_GPIO_PARAMS { \
                                        .dev = 0, \
                                        .gpio = { \
                                            .name = "PA0 Input", \
                                            .pin = MCP23X17_GPIO_PIN(0,0), \
                                            .mode = GPIO_IN, \
                                            .flags = 0, \
                                        } \
                                 }, \
                                 { \
                                        .dev = 0, \
                                        .gpio = { \
                                            .name = "PB5 Output", \
                                            .pin = MCP23X17_GPIO_PIN(1,5), \
                                            .mode = GPIO_OUT, \
                                            .flags = SAUL_GPIO_INIT_CLEAR, \
                                        } \
                                 },
#endif
/**@}*/

/**
 * @brief   Allocation of MCP23x17 configuration
 */
static const mcp23x17_params_t mcp23x17_params[] =
{
    MCP23X17_PARAMS
};

#if MODULE_SAUL_GPIO || DOXYGEN
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

#endif /* MCP23X17_PARAMS_H */
/** @} */
