/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nrf6310
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nRF51 evaluation board nrf6310
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 8)
#define LED1_PIN            GPIO_PIN(0, 9)
#define LED2_PIN            GPIO_PIN(0, 10)

#define LED0_MASK           (1 << 8)
#define LED1_MASK           (1 << 9)
#define LED2_MASK           (1 << 10)

#define LED0_ON             (NRF_GPIO->OUTCLR = LED0_MASK)
#define LED0_OFF            (NRF_GPIO->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (NRF_GPIO->OUT   ^= LED0_MASK)

#define LED1_ON             (NRF_GPIO->OUTCLR = LED1_MASK)
#define LED1_OFF            (NRF_GPIO->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (NRF_GPIO->OUT   ^= LED1_MASK)

#define LED2_ON             (NRF_GPIO->OUTCLR = LED2_MASK)
#define LED2_OFF            (NRF_GPIO->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (NRF_GPIO->OUT   ^= LED2_MASK)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
