/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
