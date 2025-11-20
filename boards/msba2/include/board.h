/*
 * SPDX-FileCopyrightText: 2013 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_msba2
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-A2 board
 *
 * @author      unknown
 */

#include "lpc23xx.h"
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
#define MTD_0 mtd_dev_get(0)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
