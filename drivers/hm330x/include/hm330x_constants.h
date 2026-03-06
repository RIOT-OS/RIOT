/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_hm330x
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     HM330X default i2c address
 */
#define HM330X_I2C_ADDRESS      (0x40U)

/**
 * @brief     HM330X data length
 */
#define HM330X_DATA_LENGTH      (29U)

/**
 * @brief     HM330X cmd to switch to i2c mode
 */
#define HM330X_CMD_I2C_MODE     (0x88U)

#ifdef __cplusplus
}
#endif

/** @} */
