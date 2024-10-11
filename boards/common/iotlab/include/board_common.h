/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_common_iotlab IoTlab Common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for the IoTlab boards.
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

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set the default baudrate to 500K for this board
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#   define STDIO_UART_BAUDRATE (500000U)
#endif
/** @} */

/**
 * @name    Xtimer configuration
 *
 * Tell the xtimer that we use a 16-bit peripheral timer
 * @{
 */
#define XTIMER_WIDTH        (16U)
/** @} */

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_USEC_TYPE    ZTIMER_TYPE_PERIPH_TIMER
#define CONFIG_ZTIMER_USEC_DEV     TIMER_DEV(0)
#define CONFIG_ZTIMER_USEC_MIN     (2)
/** @} */

/**
 * @name    Define the interface to the AT86RF231 radio
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 * @{
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PORT_A, 4)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PORT_C, 4)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PORT_A, 2)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PORT_C, 1)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        2
#define LED0_PORT           GPIO_PORT_D /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_D

#define LED1_PIN_NUM        5
#define LED1_PORT           GPIO_PORT_B /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_B

#define LED2_PIN_NUM        10
#define LED2_PORT           GPIO_PORT_C /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_C
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
#endif /* BOARD_COMMON_H */
