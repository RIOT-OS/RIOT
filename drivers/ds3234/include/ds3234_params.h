/*
 * SPDX-FileCopyrightText: 2018 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ds3234
 *
 * @{
 * @file
 * @brief       Default configuration for DS3234 devices
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "board.h"
#include "ds3234.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DS3234 devices
 * @{
 */
#ifndef DS3234_PARAM_SPI
#define DS3234_PARAM_SPI                (SPI_DEV(0))
#endif
#ifndef DS3234_PARAM_CS
#define DS3234_PARAM_CS                 (GPIO_PIN(0, 0))
#endif
#ifndef DS3234_PARAM_CLK
#define DS3234_PARAM_CLK                (SPI_CLK_1MHZ)
#endif
#ifndef DS3234_PARAMS
#define DS3234_PARAMS \
    { \
        .spi = DS3234_PARAM_SPI, \
        .cs = DS3234_PARAM_CS, \
        .clk = DS3234_PARAM_CLK, \
    }
#endif
/**@}*/

/**
 * @brief   Configure DS3234 devices
 */
static const ds3234_params_t ds3234_params[] =
{
    DS3234_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
