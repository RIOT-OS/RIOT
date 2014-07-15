/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_pca10005
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the nRF51822 evaluation board pca10005
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "periph/timer.h"
#include "board.h"
#include "cpu.h"
#include "nrf51.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "radio.h"

extern void SystemInit(void);
void leds_init(void);

void board_init(void)
{
    /* initialize core clocks via STM-lib given function */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /**
     * initialize UART
     * please see periph_conf.h for UART pins!
     **/
    gpio_init_in(GPIO_9, GPIO_NOPULL);
    gpio_init_out(GPIO_11, GPIO_NOPULL);

    uart_init_blocking(0, 115200);
}