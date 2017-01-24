/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */


/**
 * @defgroup    boards_avsextrem Avsextrem
 * @ingroup     boards
 * @brief       Support for the Avsextrem board
 * @{
 *
 * @file
 * @brief       Boards specific drivers and configuration for the Avsextrem board
 *
 * @author      Heiko Will
 * @author      Zakaria Kasmi
 */


#ifndef BOARDCONF_H
#define BOARDCONF_H
#include "bitarithm.h"
#include "msba2_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
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
 * @brief Initialize the board's clock system
 */
void init_clks1(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARDCONF_H */
