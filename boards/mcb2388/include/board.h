/*
 * Copyright (C) 2019 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mcb2388
 * @{
 *
 * @file
 * @brief       Basic definitions for the MCB2388 board
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "lpc23xx.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(2, 0)
#define LED1_PIN            GPIO_PIN(2, 1)
#define LED2_PIN            GPIO_PIN(2, 2)
#define LED3_PIN            GPIO_PIN(2, 3)
#define LED4_PIN            GPIO_PIN(2, 4)
#define LED5_PIN            GPIO_PIN(2, 5)
#define LED6_PIN            GPIO_PIN(2, 6)
#define LED7_PIN            GPIO_PIN(2, 7)

#define LED0_MASK           (BIT0)
#define LED1_MASK           (BIT1)
#define LED2_MASK           (BIT2)
#define LED3_MASK           (BIT3)
#define LED4_MASK           (BIT4)
#define LED5_MASK           (BIT5)
#define LED6_MASK           (BIT6)
#define LED7_MASK           (BIT7)

#define LED0_ON             (FIO2SET  = LED0_MASK)
#define LED0_OFF            (FIO2CLR  = LED0_MASK)
#define LED0_TOGGLE         (FIO2PIN ^= LED0_MASK)

#define LED1_ON             (FIO2SET  = LED1_MASK)
#define LED1_OFF            (FIO2CLR  = LED1_MASK)
#define LED1_TOGGLE         (FIO2PIN ^= LED1_MASK)

#define LED2_ON             (FIO2SET  = LED2_MASK)
#define LED2_OFF            (FIO2CLR  = LED2_MASK)
#define LED2_TOGGLE         (FIO2PIN ^= LED2_MASK)

#define LED3_ON             (FIO2SET  = LED3_MASK)
#define LED3_OFF            (FIO2CLR  = LED3_MASK)
#define LED3_TOGGLE         (FIO2PIN ^= LED3_MASK)

#define LED4_ON             (FIO2SET  = LED4_MASK)
#define LED4_OFF            (FIO2CLR  = LED4_MASK)
#define LED4_TOGGLE         (FIO2PIN ^= LED4_MASK)

#define LED5_ON             (FIO2SET  = LED5_MASK)
#define LED5_OFF            (FIO2CLR  = LED5_MASK)
#define LED5_TOGGLE         (FIO2PIN ^= LED5_MASK)

#define LED6_ON             (FIO2SET  = LED6_MASK)
#define LED6_OFF            (FIO2CLR  = LED6_MASK)
#define LED6_TOGGLE         (FIO2PIN ^= LED6_MASK)

#define LED7_ON             (FIO2SET  = LED7_MASK)
#define LED7_OFF            (FIO2CLR  = LED7_MASK)
#define LED7_TOGGLE         (FIO2PIN ^= LED7_MASK)
/** @} */

/**
 * @name    INT0 (Button) pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(2, 10)
#define BTN0_MODE           GPIO_IN
#define BTN0_INT_FLANK      GPIO_FALLING
/** @} */

/**
 * @name    Configuration for the 16x2 character LCD display
 * @{
 */
#define HD44780_PARAM_COLS              (16U)
#define HD44780_PARAM_ROWS              (2U)
#define HD44780_PARAM_PIN_RS            GPIO_PIN(1, 28)
#define HD44780_PARAM_PIN_RW            GPIO_PIN(1, 29)
#define HD44780_PARAM_PIN_ENABLE        GPIO_PIN(1, 31)
#define HD44780_PARAM_PINS_DATA         {   GPIO_PIN(1,24), GPIO_PIN(1,25), \
                                            GPIO_PIN(1,26), GPIO_PIN(1,27), \
                                            GPIO_UNDEF,     GPIO_UNDEF,     \
                                            GPIO_UNDEF,     GPIO_UNDEF      }
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#ifdef MODULE_MTD_MCI
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
