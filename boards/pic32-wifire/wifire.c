/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include <stdint.h>
#include "periph/gpio.h"
#include "periph/hwrng.h"
#include "bitarithm.h"
#include "board.h"
#include "cpu.h"

extern void dummy(void);

void board_init(void)
{
    /* Turn off all LED's */
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_init(LED4_PIN, GPIO_OUT);
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    LED4_OFF;


    /* Stop the linker from throwing away the PIC32 config register settings */
    dummy();
}
