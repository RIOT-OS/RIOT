/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_same54-xpro
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Microchip SAM E54 Xplained Pro
 *              board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    AT24MAC402 configuration
 * @{
 */
#define AT24MAC_PARAM_I2C_DEV   I2C_DEV(1)
#define AT24MAC_PARAM_I2C_ADDR  (0x5E)
#define AT24MAC_PARAM_TYPE      AT24MAC4XX
#define AT24CXXX_PARAM_I2C      I2C_DEV(1)
#define AT24CXXX_PARAM_ADDR     (0x56)
/** @} */

/**
 * @name    ATECC508A configuration
 * @{
 */
#define ATCA_PARAM_I2C           I2C_DEV(1)
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PC, 18)

#define LED_PORT            PORT->Group[PC]
#define LED0_MASK           (1 << 18)

#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PC, 15)

#define LED_PORT            PORT->Group[PC]
#define LED1_MASK           (1 << 15)

#define LED1_ON             (LED_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_OFF            (LED_PORT.OUTSET.reg = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL.reg = LED1_MASK)
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PB, 31)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0   mtd_dev_get(0)  /**< MTD device for the QSPI Flash */
#define MTD_1   mtd_dev_get(1)  /**< MTD device for the AT24MAC402 serial EEPROM */
#define MTD_2   mtd_dev_get(2)  /**< MTD device for the SD/MMC Card */

#define CONFIG_SDMMC_GENERIC_MTD_OFFSET     2   /**< MTD_2 is used for the SD Card */
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (32)
#define XTIMER_HZ                   (1000000ul)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
