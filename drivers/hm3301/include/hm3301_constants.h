/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hm3301
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef HM3301_CONSTANTS_H
#define HM3301_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     HM3301 default i2c address
 */
#define HM3301_I2C_ADDRESS      (0x40U)

/**
 * @brief     HM3301 data length
 */
#define HM3301_DATA_LENGTH      (29U)

/**
 * @brief     HM3301 cmd to switch to i2c mode
 */
#define HM3301_CMD_I2C_MODE     (0x88U)

#ifdef __cplusplus
}
#endif

#endif /* HM3301_CONSTANTS_H */
/** @} */
