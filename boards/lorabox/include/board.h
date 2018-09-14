/*
 * Copyright (C) 2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    Lorabox
 * @ingroup     boards
 * @brief       Support for Lobaro LoraBox
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PORT           GPIOA
#define LED0_PIN            GPIO_PIN(PORT_A, 1)
#define LED0_MASK           (1 << 3)

#define LED0_ON             (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_OFF            (LED0_PORT->BSRR = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)

#define EN3V3_PORT          GPIOA
#define EN3V3_PIN           GPIO_PIN(PORT_A, 11)
#define EN3V3_MASK          (1 << 11)

#define EN3V3_ON            (EN3V3_PORT->BSRR = EN3V3_MASK)
#define EN3V3_OFF           (EN3V3_PORT->BSRR = (EN3V3_MASK << 16))
#define EN3V3_TOGGLE        (EN3V3_PORT->ODR  ^= EN3V3_MASK)
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
