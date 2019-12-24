/*
 * Copyright (C) 2016 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_hamilton HamiltonIoT Hamilton
 * @ingroup     boards
 * @brief       Support for the HamiltonIoT Hamilton board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Hamilton board
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(1)
#define XTIMER_CHAN         (0)
#define XTIMER_OVERHEAD     (0)
/** @} */

/**
 * @name AT86RF233 configuration
 * @{
 */
#define AT86RF2XX_PARAM_SPI      SPI_DEV(0)
#define AT86RF2XX_PARAM_SPI_CLK  SPI_CLK_5MHZ
#define AT86RF2XX_PARAM_CS       GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT      GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP    GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET    GPIO_PIN(PB, 15)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                 GPIO_PIN(0, 19)

#define LED0_PORT                PORT->Group[0]
#define LED0_MASK                (1 << 19)

#define LED0_ON                  (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF                 (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE              (LED0_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
#define BTN0_PORT                PORT->Group[0]
#define BTN0_PIN                 GPIO_PIN(0, 18)
#define BTN0_MODE                GPIO_IN_PU
/** @} */

/**
 * @name FXOS8700 configuration
 * Note that another fxos8700 operation option, FXOS8700_USE_ACC_RAW_VALUES,
 * need to be set according to the application purposes
 * @{
 */
#define FXOS8700_PARAM_I2C            I2C_DEV(0)
#define FXOS8700_PARAM_ADDR           (0x1E)
#define FXOS8700_PARAM_RENEW_INTERVAL (1000000ul)
/** @} */

/**
 * @name HDC1080 configuration
 * @{
 */
#define HDC1000_PARAM_I2C             I2C_DEV(0)
#define HDC1000_PARAM_ADDR            (0x40)
#define HDC1000_PARAM_RES             HDC1000_14BIT
#define HDC1000_PARAM_RENEW_INTERVAL  (1000000ul)
/** @} */

/**
 * @name EKMB (PIR motion sensor) configuration
 * @{
 */
#define PIR_PARAM_GPIO          GPIO_PIN(PA, 6)
#define PIR_PARAM_ACTIVE_HIGH   (1)
/** @} */

/**
 * @name PULSE_COUNTER configuration
 * @{
 */
#define PULSE_COUNTER_GPIO          BTN0_PIN
#define PULSE_COUNTER_GPIO_FLANK    GPIO_FALLING
/** @} */

/**
 * @name TMP006 configuration
 * Note that two other tmp006 operation options, TMP006_USE_LOW_POWER and
 * TMP006_USE_RAW_VALUES, need to be set according to the application purpose
 * @{
 */
#define TMP006_PARAM_I2C            I2C_DEV(0)
#define TMP006_PARAM_ADDR           (0x44)
#define TMP006_PARAM_RATE           TMP006_CONFIG_CR_AS2
/** @} */

/**
 * @name ToDo: APDS9007 configuration
 * @{
 */
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
