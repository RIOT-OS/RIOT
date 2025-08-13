/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_srf08
 *
 * @{
 * @file
 * @brief       Default configuration for srf08 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "srf08.h"

/**
 * @name   Default configuration parameters for SRF08 device
 * @{
 */
#ifndef SRF08_PARAM_I2C
#define SRF08_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef SRF08_PARAM_ADDR
#define SRF08_PARAM_ADDR        (0x70)  /* 0xE0 shifted by 1 */
#endif

#ifndef SRF08_PARAMS
#define SRF08_PARAMS            { .i2c  = SRF08_PARAM_I2C, \
                                  .addr = SRF08_PARAM_ADDR }
#endif
/** @} */

/**
 * @brief   SRF08 configuration
 */
static const srf08_params_t srf08_params[] = {
    SRF08_PARAMS
};

/**
 * @brief   Number of SRF08 devices
 */
#define SRF08_NUMOF     ARRAY_SIZE(srf08_params)

#ifdef __cplusplus
}
#endif

/** @} */
