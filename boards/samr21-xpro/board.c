/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM R21 Xplained
 *              Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

int board_choose_antenna(uint8_t antenna)
{
	if (antenna == 2){
		gpio_clear(RFCTL2_PIN);
		gpio_set(RFCTL1_PIN);
		return antenna;
  	}
	else if (antenna == 1){
		gpio_clear(RFCTL1_PIN);
		gpio_set(RFCTL2_PIN);
		return antenna;
  	}
	else{
		return -1;
	}
}

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    /* initialize the on-board antenna switch */
    gpio_init(RFCTL1_PIN, GPIO_OUT);
    gpio_init(RFCTL2_PIN, GPIO_OUT);
    /* choose antenna 1 (chip antenna) as default */
    board_choose_antenna(1);

    /* initialize the CPU */
    cpu_init();
}
