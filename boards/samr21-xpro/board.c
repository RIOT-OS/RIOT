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

void board_antenna_config(uint8_t antenna)
{
    if (antenna == RFCTL_ANTENNA_EXT){
        gpio_set(RFCTL1_PIN);
        gpio_clear(RFCTL2_PIN);
    }
    else if (antenna == RFCTL_ANTENNA_BOARD){
        gpio_clear(RFCTL1_PIN);
        gpio_set(RFCTL2_PIN);
    }
}

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;

    /* initialize the on-board antenna switch */
    gpio_init(RFCTL1_PIN, GPIO_OUT);
    gpio_init(RFCTL2_PIN, GPIO_OUT);
    /* set default antenna switch configuration */
    board_antenna_config(RFCTL_ANTENNA_DEFAULT);

    /* initialize the CPU */
    cpu_init();
}
