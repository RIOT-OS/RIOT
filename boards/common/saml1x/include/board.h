/*
 * SPDX-FileCopyrightText: 2019 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_saml1x Microchip SAML1X
 * @ingroup     boards
 * @brief       Support for SAML10 and SAML11 boards
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Microchip
 *              SAML10 & SAML11 Xplained Pro board.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ATCA device type on SAML1x XPro boards
 * @{
 */
#define ATCA_DEVTYPE        ATECC508A
#define ATCA_PARAM_I2C      I2C_DEV(0)      /**< I2C bus device is connected to */
/** @} */

/**
 * @name    PORT selection macros
 * @{
 */
#ifdef CPU_FAM_SAML11
#define _PORT PORT_SEC
#else
#define _PORT PORT
#endif
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 7)

#define LED_PORT            _PORT->Group[PA]
#define LED0_MASK           (1 << 7)

#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PA, 27)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_BACKOFF      (40)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
