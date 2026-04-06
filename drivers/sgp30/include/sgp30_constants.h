/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sgp30
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     SGP30 I2C address, only one is defined
 */
#define SGP30_I2C_ADDRESS                   (0x58)

/**
 * @brief     Minimum required feature set version for this driver
 */
#define SGP30_REQUIRED_FEATURE_SET          (0x0020)

/**
 * @brief     Length of serial id
 */
#define SGP30_SERIAL_ID_LEN                 (6U)

/**
 * @brief     Initialization time
 */
#define SGP30_AIR_QUALITY_INIT_DELAY_US     (15 * US_PER_SEC)

/**
 * @brief     Sampling for dynamic baseline compensation algorithm
 */
#define SGP30_RECOMMENDED_SAMPLING_PERIOD        (1 * US_PER_SEC)

#ifdef __cplusplus
}
#endif

/** @} */
