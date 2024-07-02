/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_max31855
 *
 * @{
 * @file
 * @brief       Default configuration for the MAX31855 driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef MAX31855_PARAMS_H
#define MAX31855_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#include "max31855.h"
#include "max31855_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration for the MAX31855 driver
 * @{
 */
/**
 * @brief    Default SPI bus for the MAX31855 driver
 */
#ifndef MAX31855_PARAM_PARAM_SPI
#define MAX31855_PARAM_PARAM_SPI        (SPI_DEV(0))
#endif
/**
 * @brief    Default CS pin for the MAX31855 driver
 */
#ifndef MAX31855_PARAM_PARAM_CS_PIN
#define MAX31855_PARAM_PARAM_CS_PIN     (GPIO_PIN(0, 4))
#endif
/**
 * @brief    Default parameters for the MAX31855 driver
 */
#ifndef MAX31855_PARAMS
#define MAX31855_PARAMS {                   \
            .spi = MAX31855_PARAM_PARAM_SPI,        \
            .cs_pin = MAX31855_PARAM_PARAM_CS_PIN,  \
}
#endif
/**
 * @brief    Default SAUL info for the MAX31855 driver
 */
#ifndef MAX31855_SAULINFO
#define MAX31855_SAULINFO { .name = "max31855_thermo" },{ .name = "max31855_internal" }
#endif
/**@}*/

/**
 * @brief   Configuration structs for the MAX31855 driver
 */
static const max31855_params_t max31855_params[] =
{
    MAX31855_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t max31855_saul_info[] =
{
    MAX31855_SAULINFO
};

#ifdef __cplusplus
}
#endif

#endif /* MAX31855_PARAMS_H */
/** @} */
