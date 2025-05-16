/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ata8520e
 * @brief       Default initialization parameters for the ATA8520E device
 * @{
 * @file
 * @brief       Default initialization parameters for the ATA8520E device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#include "board.h"
#include "ata8520e.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ATA8520E driver
 * @{
 */
#ifndef ATA8520E_PARAM_SPI
#define ATA8520E_PARAM_SPI         SPI_DEV(0)
#endif
#ifndef ATA8520E_PARAM_SPI_CLK
#define ATA8520E_PARAM_SPI_CLK     (SPI_CLK_100KHZ)
#endif
#ifndef ATA8520E_PARAM_CS_PIN
#define ATA8520E_PARAM_CS_PIN      GPIO_PIN(0, 0)
#endif
#ifndef ATA8520E_PARAM_INT_PIN
#define ATA8520E_PARAM_INT_PIN     GPIO_PIN(0, 1)
#endif
#ifndef ATA8520E_PARAM_POWER_PIN
#define ATA8520E_PARAM_POWER_PIN   GPIO_PIN(0, 2)
#endif
#ifndef ATA8520E_PARAM_RESET_PIN
#define ATA8520E_PARAM_RESET_PIN   GPIO_PIN(0, 3)
#endif

#ifndef ATA8520E_PARAMS
#define ATA8520E_PARAMS            { .spi = ATA8520E_PARAM_SPI,             \
                                     .spi_clk = ATA8520E_PARAM_SPI_CLK,     \
                                     .cs_pin = ATA8520E_PARAM_CS_PIN,       \
                                     .int_pin = ATA8520E_PARAM_INT_PIN,     \
                                     .power_pin = ATA8520E_PARAM_POWER_PIN, \
                                     .reset_pin = ATA8520E_PARAM_RESET_PIN }
#endif
/**@}*/

/**
 * @brief   ATA8520E configuration
 */
static const ata8520e_params_t ata8520e_params[] =
{
    ATA8520E_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
