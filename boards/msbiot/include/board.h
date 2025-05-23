/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_msbiot
 * @{
 *
 * @file
 * @brief       Board specific definitions for the MSB-IoT board
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Configure connected CC1101 (radio) device
 * @{
 */
#define CC110X_PARAM_SPI        SPI_DEV(0)              /**< SPI interface CC1101 is connected to */
#define CC110X_PARAM_CS         GPIO_PIN(PORT_B, 12)    /**< CS pin of CC1101 */
#define CC110X_PARAM_GDO0       GPIO_PIN(PORT_C, 4)     /**< GDO0 pin of CC1101 */
#define CC110X_PARAM_GDO2       GPIO_PIN(PORT_C, 5)     /**< GDO2 pin of CC1101 */
#define CC110X_PARAM_SPI_CLOCK  SPI_CLK_1MHZ            /**< SPI clock (reduced to work around hw bug) */
/** @} */

/**
 * @name    Configure connected MPU-9150 device
 * @{
 */
#define MPU9X50_PARAM_COMP_ADDR   (0x0E)            /**< I2C address of the MPU9150 */
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        8       /**< Pin number of red LED */
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B  /**< Port number of red LED */

#define LED1_PIN_NUM        14      /**< Pin number of yellow LED */
#define LED1_PORT           GPIO_PORT_B /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_B  /**< Port number of yellow LED */

#define LED2_PIN_NUM        15      /**< Pin number of green LED */
#define LED2_PORT           GPIO_PORT_B /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_B  /**< Port number of green LED */
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
#define BUTTON0_PIN         GPIO_PIN(PORT_B, 13)    /**< Pin of left button */
#define BUTTON1_PIN         GPIO_PIN(PORT_A, 0)     /**< Pin of right button */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
