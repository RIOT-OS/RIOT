/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_jellyfish
 * @{
 *
 * @file
 * @brief       Board specific implementations for the JF4418 evaluation board
 *
 * @author      Dang Minh Phuong <kamejoko80@yahoo.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "board.h"
#include "uart_stdio.h"

#include "s5p4418_clk.h"
#include "s5p4418_irq.h"
#include "s5p4418_timer.h"
#include "s5p4418_uart.h"
#include "cp15.h"

extern void kernel_init(void);

void board_init(void)
{
    /* Initialize peripherals */
    s5p4418_clk_init();
    s5p4418_irq_init();
    s5p4418_timer_init();

#ifdef MODULE_NEWLIB
    extern void __libc_init_array(void);

    /* Disable IRQ */
    __irq_disable();

    /* Init newlib array */
    __libc_init_array();

    /* Enable IRQ */
    __irq_enable();

    printf("Board low level init done\r\n");
#endif

    kernel_init();
}
