/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_msba2
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-A2 board
 *
 * @author      unknown
 */

#ifndef BOARD_H
#define BOARD_H

#include "lpc23xx.h"
#include "mtd.h"
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(2, 25)
#define LED1_PIN            GPIO_PIN(2, 26)

#define LED0_MASK           (BIT25)
#define LED1_MASK           (BIT26)

#define LED0_OFF            (FIO3SET  = LED0_MASK)
#define LED0_ON             (FIO3CLR  = LED0_MASK)
#define LED0_TOGGLE         (FIO3PIN ^= LED0_MASK)

#define LED1_OFF            (FIO3SET  = LED1_MASK)
#define LED1_ON             (FIO3CLR  = LED1_MASK)
#define LED1_TOGGLE         (FIO3PIN ^= LED1_MASK)
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
