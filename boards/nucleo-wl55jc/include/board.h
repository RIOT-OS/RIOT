/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-wl55jc
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options
 *
 * @author      Akshai M <akshai.m@fu-berlin.de>
 * @author      Hauke Petersen <devel@haukepetersen.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_nucleo.h"

/* Required for `nucleo_wl55jc_sx126x_set_rf_mode` */
#if IS_USED(MODULE_SX126X_STM32WL)
#include "sx126x.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Sub-GHz radio (LoRa) configuration
 * @{
 */
#define SX126X_PARAM_SPI                    (SPI_DEV(0))

#if IS_USED(MODULE_SX126X_STM32WL)
extern void nucleo_wl55jc_sx126x_set_rf_mode(sx126x_t *dev, sx126x_rf_mode_t rf_mode);
#define SX126X_PARAM_SET_RF_MODE_CB         nucleo_wl55jc_sx126x_set_rf_mode
#endif
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PORT           GPIOB
#define LED0_PIN            GPIO_PIN(PORT_B, 15)
#define LED0_MASK           (1 << 15)
#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)

#define LED1_PORT           GPIOB
#define LED1_PIN            GPIO_PIN(PORT_B, 9)
#define LED1_MASK           (1 << 9)
#define LED1_ON             (LED0_PORT->BSRR = LED1_MASK)
#define LED1_OFF            (LED0_PORT->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (LED0_PORT->ODR  ^= LED1_MASK)

#define LED2_PORT           GPIOB
#define LED2_PIN            GPIO_PIN(PORT_B, 11)
#define LED2_MASK           (1 << 11)
#define LED2_ON             (LED0_PORT->BSRR = LED2_MASK)
#define LED2_OFF            (LED0_PORT->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (LED0_PORT->ODR  ^= LED2_MASK)
/** @} */

/**
 * @brief   Nucleo-wl55jc always use LED0, as there is no dual use of its pin
 * @{
 */
#ifndef AUTO_INIT_LED0
#define AUTO_INIT_LED0
#endif
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(PORT_A, 1)
#define BTN1_MODE           GPIO_IN_PU
#define BTN2_PIN            GPIO_PIN(PORT_C, 6)
#define BTN2_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    RF 3-port switch (SP3T) control
 *
 * Refer Section 6.6.3 RF Overview in User Manual (UM2592)
 * @{
 */
#define FE_CTRL1            GPIO_PIN(PORT_C, 4)
#define FE_CTRL2            GPIO_PIN(PORT_C, 5)
#define FE_CTRL3            GPIO_PIN(PORT_C, 3)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
