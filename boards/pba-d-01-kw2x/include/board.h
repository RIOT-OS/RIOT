/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_pba-d-01-kw2x
 * @{
 *
 * @file
 * @brief       Board specific definitions for the phyWAVE evaluation board
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_D, 6)
#define LED1_PIN            GPIO_PIN(PORT_D, 4)
#define LED2_PIN            GPIO_PIN(PORT_A, 4)

#define LED0_MASK           (1 << 6)
#define LED1_MASK           (1 << 4)
#define LED2_MASK           (1 << 4)

#define LED0_ON            (GPIOD->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOD->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOD->PTOR = LED0_MASK)

#define LED1_ON            (GPIOD->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOD->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOD->PTOR = LED1_MASK)

#define LED2_ON            (GPIOA->PCOR = LED2_MASK)
#define LED2_OFF           (GPIOA->PSOR = LED2_MASK)
#define LED2_TOGGLE        (GPIOA->PTOR = LED2_MASK)
/** @} */

/**
 * @name Macro for button S1/S2.
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_D, 1)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name Macro for capacitive sensor button.
 * @{
 */
#define BTN1_PORT           PORTC
#define BTN1_PIN            GPIO_PIN(PORT_C, 6)
#define BTN1_MODE           GPIO_IN
/** @} */

/**
 * @name KW2XRF configuration
 *
 * {spi bus, cs pin, int pin, spi speed,}
 * @{
 */
#define KW2XRF_PARAM_SPI           SPI_DEV(1)
#define KW2XRF_PARAM_SPI_CLK       (SPI_CLK_10MHZ)
#define KW2XRF_PARAM_CS            GPIO_PIN(KW2XDRF_PORT, KW2XDRF_PCS0_PIN)
#define KW2XRF_PARAM_INT           GPIO_PIN(KW2XDRF_PORT, KW2XDRF_IRQ_PIN)
#define KW2XRF_PARAM_RESET         GPIO_PIN(KW2XDRF_IRQ_PIN, KW2XDRF_RST_PIN)
#define KW2XRF_SHARED_SPI          (0)
/** @}*/

/**
 * @name TMP006 configuration
 *
 * @{
 */
#define TMP00X_PARAM_ADDR          (0x41)
/** @}*/

#ifdef __cplusplus
}
#endif

/** @} */
