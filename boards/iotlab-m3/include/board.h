/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_iotlab-m3 IoT-LAB M3 open node
 * @ingroup     boards
 * @brief       Board specific files for the iotlab-m3 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the iotlab-m3 board.
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the interface for the connected flash memory
 * @{
 */
#define EXTFLASH_SPI        SPI_DEV(1)
#define EXTFLASH_CS         GPIO_PIN(PORT_A,11)
#define EXTFLASH_WRITE      GPIO_PIN(PORT_C,6)
#define EXTFLASH_HOLD       GPIO_PIN(PORT_C,9)
/** @} */

/**
 * @name Define the interface to the ISL29020 light sensor
 * @{
 */
#define ISL29020_I2C        I2C_0
#define ISL29020_ADDR       0x44
/** @} */

/**
 * @name Define the interface to the LPS331AP pressure sensor
 * @{
 */
#define LPS331AP_I2C        I2C_0
#define LPS331AP_ADDR       0x5c
/** @} */

/**
 * @name Define the interface for the L3G4200D gyroscope
 * @{
 */
#define L3G4200D_I2C        I2C_0
#define L3G4200D_ADDR       0x68
#define L3G4200D_DRDY       GPIO_PIN(PORT_C,0)
#define L3G4200D_INT        GPIO_PIN(PORT_C,5)
/** @} */

/**
 * @name Define the interface to the LSM303DLHC accelerometer and magnetometer
 * @{
 */
#define LSM303DLHC_I2C      I2C_0
#define LSM303DLHC_ACC_ADDR (0x19)
#define LSM303DLHC_MAG_ADDR (0x1e)
#define LSM303DLHC_INT1     GPIO_PIN(PORT_B,12)
#define LSM303DLHC_INT2     GPIO_PIN(PORT_B,1)
#define LSM303DLHC_DRDY     GPIO_PIN(PORT_B,2)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
