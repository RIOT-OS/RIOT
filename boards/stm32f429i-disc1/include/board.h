/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f429i-disc1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F429I-DISC1 evaluation board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN_NUM        13
#define LED0_PORT           GPIO_PORT_G /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_G

#define LED1_PIN_NUM        14
#define LED1_PORT           GPIO_PORT_G /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_G
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0) /**< GPIO used for BTN0 */
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name L3GD20 (Rev. B01, C01), I3G4250D (Rev. D02, E01)
 * @{
 */
#define L3GXXXX_SPI_DEV     SPI_DEV(0)          /**< SPI bus used for L3Gxxxx */
#define L3GXXXX_SPI_CS      GPIO_PIN(PORT_C, 1) /**< SPI CS pin used for L3Gxxxx */
#define L3GXXXX_INT1_PIN    GPIO_PIN(PORT_A, 1) /**< INT1 pin used for L3Gxxxx */
#define L3GXXXX_INT2_PIN    GPIO_PIN(PORT_A, 2) /**< INT2/DRDY pin used for L3Gxxxx */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
