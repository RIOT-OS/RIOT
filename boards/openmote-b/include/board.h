/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_openmote-b
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OpenMote-B board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"
#include "cc2538_eui_primary.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                    GPIO_PIN(2, 4)
#define LED1_PIN                    GPIO_PIN(2, 7)
#define LED2_PIN                    GPIO_PIN(2, 6)
#define LED3_PIN                    GPIO_PIN(2, 5)
#define BTN0_PIN                    GPIO_PIN(3, 5)
#define BTN0_MODE                   GPIO_IN
#define RF24_SWITCH_CC2538_PIN      GPIO_PIN(3, 4)  /**< PD4 -- CC2538 */
#define RF24_SWITCH_AT86RF215_PIN   GPIO_PIN(3, 3)  /**< PD3 -- AT86RF215 */

#define LED_PORT                    GPIO_C
#define LED0_MASK                   (1 << 4)
#define LED1_MASK                   (1 << 7)
#define LED2_MASK                   (1 << 6)
#define LED3_MASK                   (1 << 5)

#define RF_SWITCH_PORT              GPIO_D
#define RF24_SWITCH_CC2538_MASK     (1 << 4)
#define RF24_SWITCH_AT86RF215_MASK  (1 << 3)

#define LED0_ON                     (LED_PORT->DATA &= ~LED0_MASK)
#define LED0_OFF                    (LED_PORT->DATA |=  LED0_MASK)
#define LED0_TOGGLE                 (LED_PORT->DATA ^=  LED0_MASK)

#define LED1_ON                     (LED_PORT->DATA &= ~LED1_MASK)
#define LED1_OFF                    (LED_PORT->DATA |=  LED1_MASK)
#define LED1_TOGGLE                 (LED_PORT->DATA ^=  LED1_MASK)

#define LED2_ON                     (LED_PORT->DATA &= ~LED2_MASK)
#define LED2_OFF                    (LED_PORT->DATA |=  LED2_MASK)
#define LED2_TOGGLE                 (LED_PORT->DATA ^=  LED2_MASK)

#define LED3_ON                     (LED_PORT->DATA &= ~LED3_MASK)
#define LED3_OFF                    (LED_PORT->DATA |=  LED3_MASK)
#define LED3_TOGGLE                 (LED_PORT->DATA ^=  LED3_MASK)

#define RF24_SWITCH_CC2538_ON       (RF_SWITCH_PORT->DATA &= ~RF24_SWITCH_CC2538_MASK)
#define RF24_SWITCH_CC2538_OFF      (RF_SWITCH_PORT->DATA |=  RF24_SWITCH_CC2538_MASK)
#define RF24_SWITCH_CC2538_TOGGLE   (RF_SWITCH_PORT->DATA ^=  RF24_SWITCH_CC2538_MASK)

#define RF24_SWITCH_AT86RF215_ON     (RF_SWITCH_PORT->DATA &= ~RF24_SWITCH_AT86RF215_MASK)
#define RF24_SWITCH_AT86RF215_OFF    (RF_SWITCH_PORT->DATA |=  RF24_SWITCH_AT86RF215_MASK)
#define RF24_SWITCH_AT86RF215_TOGGLE (RF_SWITCH_PORT->DATA ^=  RF24_SWITCH_AT86RF215_MASK)
/** @} */

/**
 * @name    RF CORE observable signals settings
 * @{
 */
#define CONFIG_CC2538_RF_OBS_SIG_0_PCX  5   /* PC5 */
#define CONFIG_CC2538_RF_OBS_SIG_1_PCX  6   /* PC6 */
#define CONFIG_CC2538_RF_OBS_SIG_2_PCX  7   /* PC7 */
/** @} */

/**
 * @name    OpenWSN leds configuration
 *
 * @note This configuration mimics the one done in OpenWSN-fw for the
 *       same platform
 * @{
 */
#define OPENWSN_LEDPIN_ERROR    LED0_PIN
#define OPENWSN_LEDPIN_SYNC     LED1_PIN
#define OPENWSN_LEDPIN_RADIO    LED3_PIN
#define OPENWSN_LEDPIN_DEBUG    LED2_PIN
/** @} */

/**
 * @name    OpenWSN debugpins configuration
 *
 * @note This configuration mimics the one done in OpenWSN-fw for the
 *       same platform
 * @{
 */
#define OPENWSN_DEBUGPIN_FRAME  GPIO_PIN(0, 7) /* A7 */
#define OPENWSN_DEBUGPIN_ISR    GPIO_PIN(2, 3) /* C3 */
#define OPENWSN_DEBUGPIN_SLOT   GPIO_PIN(1, 3) /* B3 */
#define OPENWSN_DEBUGPIN_FSM    GPIO_PIN(1, 2) /* B2 */
#define OPENWSN_DEBUGPIN_TASK   GPIO_PIN(1, 1) /* B1 */
#define OPENWSN_DEBUGPIN_RADIO  GPIO_PIN(1, 0) /* B0 */
/** @} */

/**
 * @name    AT86RF215 configuration
 * @{
 */
#define AT86RF215_PARAM_SPI        SPI_DEV(0)
#define AT86RF215_PARAM_CS         GPIO_PIN(0, 3) /* A3 */
#define AT86RF215_PARAM_INT        GPIO_PIN(3, 0) /* D0 */
#define AT86RF215_PARAM_RESET      GPIO_PIN(3, 1) /* D1 */
/** @} */

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

/**
 * @name    Flash Customer Configuration Area (CCA) parameters
 * @{
 */
#ifndef UPDATE_CCA
#define UPDATE_CCA                (1)
#endif

#define CCA_BACKDOOR_ENABLE       (1)
#define CCA_BACKDOOR_PORT_A_PIN   (6) /**< BSL_BOOT Pin */
#define CCA_BACKDOOR_ACTIVE_LEVEL (0) /**< Active low */

#define BOOT_PIN    GPIO_PIN(0, CCA_BACKDOOR_PORT_A_PIN) /**< BSL_BOOT Pin */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
