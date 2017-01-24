/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_udoo UDOO
 * @ingroup     boards
 * @brief       Support for the UDOO board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the UDOO board.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PB, 27)

#define LED_PORT            PIOB
#define LED0_MASK           PIO_PB27

#define LED0_ON             (LED_PORT->PIO_SODR =  LED0_MASK)
#define LED0_OFF            (LED_PORT->PIO_CODR =  LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->PIO_ODSR ^= LED0_MASK)
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
