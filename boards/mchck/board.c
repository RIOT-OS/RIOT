/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_mchck
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */


#include <stdlib.h>

#include "board.h"
#include "MK20DZ10.h"
#include <k20_gpio.h>
#include <k20_uart.h>
#include <periph/uart.h>

static void leds_init(void);


/**
 * @brief   Initialize the board
 */
void board_init(void)
{
    /* initialize the boards LEDs, this is done first for debugging purposes */
    leds_init();

    /* initialize the CPU */
    cpu_init();

    /* select the UART0 at PIN PA1/2 */
    k20_uart0_select_output(K20_UART0_OUTPUT_PTA_1_2);
}

/**
 * @brief Initialize the boards on-board LED
 *
 * The LED initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to their CPU pins.
 *
 * The LED is connected to PTB16
 */
static void leds_init(void)
{
    k20_gpio_enable_output(PTB_BASE_PTR, 16, true);
}
