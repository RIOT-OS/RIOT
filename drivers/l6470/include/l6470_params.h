/*
 * Copyright (C) 2022 Samuel Kayode
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l6470
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Samuel Kayode <samkay014@gmail.com>
 */

#ifndef L6470_PARAMS_H
#define L6470_PARAMS_H

#include "board.h"
#include "l6470.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef L6470_PARAM_SPI_DEV
#define L6470_PARAM_SPI_DEV             (SPI_DEV(0))
#endif

#ifndef L6470_PARAM_SPI_CS
#define L6470_PARAM_SPI_CS              (GPIO_PIN(0, 4))
#endif

#ifndef L6470_PARAM_SPI_MODE
#define L6470_PARAM_SPI_MODE            (SPI_MODE_3)
#endif

#ifndef L6470_PARAM_SPI_CLK
#define L6470_PARAM_SPI_CLK             (SPI_CLK_1MHZ)
#endif

#ifndef L6470_PARAM_STEP_MODE
#define L6470_PARAM_STEP_MODE           (L6470_STEP_FULL)
#endif

#if IS_USED(MODULE_L6470_STBY) || DOXYGEN
#ifndef L6470_RESET_PIN
#define L6470_RESET_PIN                 (GPIO_PIN(PORT_B, 5))
#endif
#endif /* MODULE_L6470_STBY || DOXYGEN */

#if IS_USED(MODULE_L6470_DAISY_CHAIN) || DOXYGEN
#define L6470_DEVS          (2)
#else
#define L6470_DEVS          (1)
#endif /* MODULE_L6470_DAISY_CHAIN || DOXYGEN */

#if IS_USED(MODULE_L6470_STBY) || DOXYGEN
#define L6470_PARAM_RESET  .stby             = L6470_RESET_PIN,
#else
#define L6470_PARAM_RESET
#endif

#ifndef L6470_PARAMS
#define L6470_PARAMS { \
                           .dev              = L6470_PARAM_SPI_DEV, \
                           .cs               = L6470_PARAM_SPI_CS, \
                           .spi_mode         = L6470_PARAM_SPI_MODE, \
                           .clk              = L6470_PARAM_SPI_CLK, \
                           .step_mode        = L6470_PARAM_STEP_MODE, \
                           .spi_daisy_chain_devs = L6470_DEVS, \
                           L6470_PARAM_RESET \
                     }
#endif /* L6470_PARAMS */
/**@}*/

/**
 * @brief   Configuration struct
 */
static const l6470_params_t l6470_params[] =
{
    L6470_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* L6470_PARAMS_H */
/** @} */
