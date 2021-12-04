/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

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

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "board_common.h"
#include "mtd.h"

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
extern mtd_dev_t *mtd0;     /**< SPI NOR Flash device */
#define MTD_0     mtd0      /**< Indicate presence of MTD device */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
