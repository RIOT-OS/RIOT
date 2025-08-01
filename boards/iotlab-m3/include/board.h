/*
 * SPDX-FileCopyrightText: 2014-2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_iotlab-m3
 * @{
 *
 * @file
 * @brief       Board specific definitions for the iotlab-m3 board
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the interface for the connected flash memory
 * @{
 */
#define EXTFLASH_SPI        SPI_DEV(1)
#define EXTFLASH_CS         GPIO_PIN(PORT_A, 11)
#define EXTFLASH_WRITE      GPIO_PIN(PORT_C, 6)
#define EXTFLASH_HOLD       GPIO_PIN(PORT_C, 9)
/** @} */

/**
 * @name Define the interface to the LPS331AP pressure sensor
 * @{
 */
#define LPSXXX_PARAM_ADDR          (0x5C)
/** @} */

/**
 * @name Define the interface for the L3G4200D gyroscope
 * @{
 */
#define L3G4200D_PARAM_INT2        GPIO_PIN(PORT_C, 0)
#define L3G4200D_PARAM_INT1        GPIO_PIN(PORT_C, 5)
/** @} */

/**
 * @name Define the interface to the LSM303DLHC accelerometer and magnetometer
 * @{
 */
#define LSM303DLHC_PARAM_ACC_PIN   GPIO_PIN(PORT_B, 12)
#define LSM303DLHC_PARAM_MAG_PIN   GPIO_PIN(PORT_B, 2)
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0     mtd_dev_get(0)      /**< SPI NOR Flash device */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
