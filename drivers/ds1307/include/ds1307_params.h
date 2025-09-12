/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup drivers_ds1307
 * @{
 *
 * @file
 * @brief   Default configuration for DS1307 devices
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "board.h"
#include "ds1307.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the DS1307 driver
 * @{
 */
#ifndef DS1307_PARAM_I2C
#define DS1307_PARAM_I2C        (I2C_DEV(0))
#endif

#ifndef DS1307_PARAMS
#define DS1307_PARAMS           { .i2c = DS1307_PARAM_I2C }
#endif
/** @} */

/**
 * @brief   DS1307 configuration
 */
static const ds1307_params_t ds1307_params[] =
{
    DS1307_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
