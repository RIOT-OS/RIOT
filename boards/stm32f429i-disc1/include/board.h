/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32f429i-disc1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F429I-DISC1 evaluation board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

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

/**
 * @brief stmpe811 driver parameters
 * @{
 */
#define STMPE811_PARAM_XYCONV (STMPE811_MIRROR_Y | STMPE811_SWAP_XY)
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
