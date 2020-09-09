/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "at24mac.h"

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
#define AT24CXXX_PARAM_ADDR     (0x56)
/** @} */

/**
 * @name    ATECC508A configuration
 * @{
 */
#define ATCA_PARAM_I2C           I2C_DEV(1)
/** @} */

/**
 * @brief    AT24Mac provides a EUI-48
 */
static inline int _at24mac_get_eui48(const void *arg, eui48_t *addr)
{
    return at24mac_get_eui48((uintptr_t)arg, addr);
}

/**
 * @name    EUI-48 sources on the board
 *          AT24Mac is present on the board
 * @{
 */
#define EUI48_PROVIDER_FUNC   _at24mac_get_eui48
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
/** @} */

/**
 * @name SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           PORT->Group[PB]
#define BTN0_PIN            GPIO_PIN(PB, 31)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (32)
#define XTIMER_HZ                   (1000000ul)
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
