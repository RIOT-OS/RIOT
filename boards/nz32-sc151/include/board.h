/*
 * Copyright (C) 2016 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_nz32-sc151
 * @ingroup     boards
 * @brief       Board specific files for the nz32-sc151 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nz32-sc151 board.
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   USER LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 2)

#define LED0_PORT           (GPIOB)
#define LED0_MASK           (1 << 2)

#define LED0_ON             (LED0_PORT->BSRRL = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BSRRH = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
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
