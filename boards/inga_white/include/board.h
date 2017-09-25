/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef INGA_COMMON_BOARD_H_
#define INGA_COMMON_BOARD_H_

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    User LED pin definitions and handlers
 */
#define LED1_PIN            GPIO_PIN(PORT_D, 5)
#define LED1_MASK           (1 << 5)
#define LED1_OFF            (PORTD |= LED1_MASK)
#define LED1_ON             (PORTD &= ~LED1_MASK)
#define LED1_TOGGLE         (PORTD ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_D, 7)
#define LED2_MASK           (1 << 7)
#define LED2_OFF            (PORTD |= LED2_MASK)
#define LED2_ON             (PORTD &= ~LED2_MASK)
#define LED2_TOGGLE         (PORTD ^= LED2_MASK)
/** @} */

/**
 * @name    User button pin definitions
 */
#define BTN0_PIN            GPIO_PIN(PORT_B,2) /* PB2 */
#define BTN0_MODE           GPIO_IN
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  INGA_COMMON_BOARD_H_ */
