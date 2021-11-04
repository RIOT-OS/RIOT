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
#define SX126X_PARAM_TX_PA_MODE             SX126X_RF_MODE_TX_HPA
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
#define LED0_PORT           GPIOB
#define LED0_PIN            GPIO_PIN(PORT_B, 5)
#define LED0_MASK           (1 << 5)
#define LED0_OFF            (LED0_PORT->BSRR = LED0_MASK)
#define LED0_ON             (LED0_PORT->BSRR = (LED0_MASK << 5))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
/** @} */

/**
 * @brief   Lora-E5-Dev always use LED0, as there is no dual use of its pin
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
#define BTN0_PIN            GPIO_PIN(PORT_B, 13)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(PORT_A, 0)
#define BTN1_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    RF 3-port switch (SP3T) control
 *
 * Refer Section 6.6.3 RF Overview in User Manual (UM2592)
 * @{
 */
#define FE_CTRL1            GPIO_PIN(PORT_A, 4)
#define FE_CTRL2            GPIO_PIN(PORT_A, 5)
/** @} */

/**
 * @brief    Enable 3.3V output
 */
#ifndef CONFIG_LORA_E5_DEV_ENABLE_3P3V
#define CONFIG_LORA_E5_DEV_ENABLE_3P3V      1
#endif
/**
 * @brief    Enable 5V output
 */
#ifndef CONFIG_LORA_E5_DEV_ENABLE_5V
#define CONFIG_LORA_E5_DEV_ENABLE_5V        1
#endif
/**
 * @brief    lora-e5-dev 3.3V gpio enable pin
 */
#define LORA_E5_DEV_3P3V_ENABLE_PIN         GPIO_PIN(PORT_A, 9)
/**
 * @brief    lora-e5-dev 5V gpio enable pin
 */
#define LORA_E5_DEV_5V_ENABLE_PIN           GPIO_PIN(PORT_B, 10)

/**
 * @brief   Board level initialization
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
