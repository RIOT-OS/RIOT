/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_emw3162 EMW3162
 * @ingroup     boards
 * @brief       Board specific files for the EMW3162 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the EMW3162 evaluation board
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER              TIMER_0
#define XTIMER_CHAN         (0)
#define XTIMER_OVERHEAD     (6)
#define XTIMER_BACKOFF      (5)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_GREEN_GPIO      GPIO_PIN(PORT_B, 0)
#define LED_RED_GPIO        GPIO_PIN(PORT_B, 1)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
