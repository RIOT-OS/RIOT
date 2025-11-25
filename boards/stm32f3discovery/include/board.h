/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F3Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN_NUM        9
#define LED0_PORT_NUM       PORT_E

#define LED1_PIN_NUM        8
#define LED1_PORT_NUM       PORT_E

#define LED2_PIN_NUM        10
#define LED2_PORT_NUM       PORT_E

#define LED3_PIN_NUM        15
#define LED3_PORT_NUM       PORT_E

#define LED4_PIN_NUM        11
#define LED4_PORT_NUM       PORT_E

#define LED5_PIN_NUM        14
#define LED5_PORT_NUM       PORT_E

#define LED6_PIN_NUM        12
#define LED6_PORT_NUM       PORT_E

#define LED7_PIN_NUM        13
#define LED7_PORT_NUM       PORT_E
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name LSM303DLHC magnetometer data ready pin
 * @{
 */
#define LSM303DLHC_PARAM_MAG_PIN    GPIO_PIN(PORT_E, 2)
/** @} */

/**
 * @name L3GD20 (Rev. C01, D01), I3G4250D (Rev. E02)
 * @{
 */
#define L3GXXXX_SPI_DEV     SPI_DEV(0)          /**< SPI bus used for L3Gxxxx */
#define L3GXXXX_SPI_CS      GPIO_PIN(PORT_E, 3) /**< SPI CS pin used for L3Gxxxx */
#define L3GXXXX_INT1_PIN    GPIO_PIN(PORT_E, 0) /**< INT1 pin used for L3Gxxxx */
#define L3GXXXX_INT2_PIN    GPIO_PIN(PORT_E, 1) /**< INT2/DRDY pin used for L3Gxxxx */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
