/*
 * Copyright (C) 2014 volatiles UG (haftungsbeschränkt)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_ek-tm4c123gxl
 * @ingroup     boards
 * @brief       Board specific files for the EK-TM4C123GXL Launchpad evaluation board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the EK-TM4C123GXL Launchpad evaluation board
 *
 * @author      Benjamin Valentin <benjamin.valentin@volatiles.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (80000000UL)

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Define the UART used for stdio
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)

#define _RED_LED            (*((volatile unsigned int *)(GPIOF_BASE + ((0x2 << 2)))))
#define _BLUE_LED           (*((volatile unsigned int *)(GPIOF_BASE + ((0x4 << 2)))))
#define _GREEN_LED          (*((volatile unsigned int *)(GPIOF_BASE + ((0x8 << 2)))))

#define RED_LED_ON          (_RED_LED = 0x2)
#define RED_LED_OFF         (_RED_LED = 0x0)
#define RED_LED_TOGGLE      (_RED_LED^= 0x2)

#define BLUE_LED_ON         (_BLUE_LED = 0x4)
#define BLUE_LED_OFF        (_BLUE_LED = 0x0)
#define BLUE_LED_TOGGLE     (_BLUE_LED^= 0x4)

#define GREEN_LED_ON        (_GREEN_LED = 0x8)
#define GREEN_LED_OFF       (_GREEN_LED = 0x0)
#define GREEN_LED_TOGGLE    (_GREEN_LED^= 0x8)

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /** __BOARD_H */
/** @} */
