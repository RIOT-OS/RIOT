/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_lora-e5-dev
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options for
 *              LoRa-E5 Development Board
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.yyfr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "kernel_defines.h"
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
#define SX126X_PARAM_TX_PA_MODE             SX126X_RF_MODE_TX_LPA
#if IS_USED(MODULE_SX126X_STM32WL)
extern void lora_e5_dev_sx126x_set_rf_mode(sx126x_t *dev, sx126x_rf_mode_t rf_mode);
#define SX126X_PARAM_SET_RF_MODE_CB         lora_e5_dev_sx126x_set_rf_mode
#define SX126X_PARAM_TYPE                   SX126X_TYPE_STM32WL
#endif
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PORT_NUM       PORT_A
#define LED0_PIN_NUM        4
#define LED1_PORT_NUM       PORT_A
#define LED1_PIN_NUM        5
#define LED2_PORT_NUM       PORT_C
#define LED2_PIN_NUM        13
#define LED3_PORT_NUM       PORT_C
#define LED3_PIN_NUM        14
/** @} */

/**
 * @name    User button
 * @{
 */
//#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
//#define BTN0_MODE           GPIO_IN_PU

/** @} */

/**
 * @name    RF 3-port switch (SP3T) control
 *
 * Refer Section 6.6.3 RF Overview in User Manual (UM2592)
 * @{
 */
#define FE_CTRL1            GPIO_PIN(PORT_A, 7)
#define FE_CTRL2            GPIO_PIN(PORT_A, 6)
/** @} */


#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
