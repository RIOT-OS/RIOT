/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_thingy52
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Nordic Thingy:52 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 11)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Power switch pins definitions
 * @{
 */
#define SPK_PWR_CTRL_PIN    GPIO_PIN(0, 29) /**< Speaker power switch */
#define VDD_PWR_CTRL_PIN    GPIO_PIN(0, 30) /**< VDD power switch */
/** @} */

/**
 * @name    LIS2DH12 low power accelerometer configuration
 * @{
 */
#define LIS2DH12_PARAM_I2C  I2C_DEV(1)
/** @} */

/**
 * @name    LPS22HB device address
 * @{
 */
#define LPSXXX_PARAM_ADDR   (0x5c)
/** @} */

#ifdef MODULE_PWM_DAC
/**
 * @name    The emulated DAC channels
 *
 * The static value is actually the result of the @ref pwm_dac_init in
 * board_init, and it is checked there whether the values are identical.
 *
 * @{
 */
#define DAC_NUMOF 1
#undef DAC_LINE
#define DAC_LINE(x) (0x600)
/** @} */
#endif /* MODULE_PWM_DAC */

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

/**
 * @brief   Enable the speaker
 *
 * Before this is called, it's a good time to run
 * `dac_poweron(DAC_LINE(0));`.
 */
void board_speaker_on(void);

/**
 * @brief   Disable the speaker
 *
 * After this is called, it's a good time to run
 * `dac_poweroff(DAC_LINE(0));`.
 */
void board_speaker_off(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
