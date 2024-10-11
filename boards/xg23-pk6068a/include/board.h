/*
 * Copyright (C) 2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_xg23-pk6068a
 * @{
 *
 * @file
 * @brief       Board specific definitions for the xG23-PK6068A
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize pm_layered with no pre-defined blockers
 *
 * All peripheral drivers keep track of required pm modes.
 */
#define PM_BLOCKER_INITIAL { 0, 0, 0 }

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_LPTIMER_DEV            TIMER_DEV(1)
#define CONFIG_ZTIMER_LPTIMER_FREQ           LFXO_FREQ
#define CONFIG_ZTIMER_LPTIMER_WIDTH          24
#define CONFIG_ZTIMER_LPTIMER_BLOCK_PM_MODE  EFM32_PM_MODE_EM3
/** @} */

/**
 * @brief   Power mode required for GPIO IRQs
 *
 * If all GPIO IRQs are expected on port A or B, EM3 is the lowest allowed
 * power mode. No power level has to be blocked in this case.
 * If GPIO IRQs shall be fired on port C or D, we must stay in EM1.
 * Uncomment the line down below in this case.
 */
/* #define GPIO_INT_PM_BLOCKER PM_MODE_EM2 */

/**
 * @name    Board VCOM interface
 *
 * Define the GPIO pin to enable the VCOM interface
 */
#define VCOM_UART             UART_DEV(0)
#define VCOM_EN_PIN           GPIO_PIN(PB, 0)

/**
 * @name    Push button pin definitions
 * @{
 */
#define PB0_PIN               GPIO_PIN(PB, 1)
#define PB1_PIN               GPIO_PIN(PB, 3)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED0_PIN              GPIO_PIN(PB, 2)
#define LED1_PIN              GPIO_PIN(PD, 3)
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_ON               gpio_set(LED0_PIN)
#define LED0_OFF              gpio_clear(LED0_PIN)
#define LED0_TOGGLE           gpio_toggle(LED0_PIN)
#define LED1_ON               gpio_set(LED1_PIN)
#define LED1_OFF              gpio_clear(LED1_PIN)
#define LED1_TOGGLE           gpio_toggle(LED1_PIN)
/** @} */

/**
 * @name    Display configuration
 *
 * Connection to the on-board Sharp Memory LCD (LS013B7DH03).
 * @{
 */
#define DISP_SPI              SPI_DEV(0)
#define DISP_SCS_PIN          GPIO_PIN(PC, 8)
#define DISP_EXTCOMIN_PIN     GPIO_PIN(PC, 6)
#define DISP_ENABLE_PIN       GPIO_PIN(PC, 9)
/** @} */

/**
 * @name    Flash configuration
 *
 * Connection to the on-board MX25R8035F.
 * @{
 */
#define FLASH_SPI             SPI_DEV(0)
#define FLASH_CS_PIN          GPIO_PIN(PC, 4)
/** @} */

/**
 * @name    Sensor configuration
 *
 * Connection to the on-board Si7021 temperature & humidity sensor.
 * @{
 */
#define SI70XX_PARAM_I2C_DEV  I2C_DEV(0)
#define SI7021_EN_PIN         GPIO_PIN(PC, 9)
/** @} */

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
