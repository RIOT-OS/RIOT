/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "clock_conf.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "periph/uart.h"
#include "periph_conf.h"

#include "stdio_uart.h"
#include <stdio.h>
#include <sys/unistd.h>

void gpio_reset(void)
{
    reset_component(RESET_PADS_BANK0, RESET_PADS_BANK0);
    reset_component(RESET_IO_BANK0, RESET_IO_BANK0);
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks, peripheral
 */
void cpu_init(void)
{
    riscv_init();

    uint32_t ra_register = 0;

    __asm__ volatile ("mv %0, ra" : "=r"(ra_register));

    /* Reset GPIO state */
    gpio_reset();

    /* Reset clock to default state */
    clock_reset();

    /* initialize the CPU clock */
    cpu_clock_init();

    /* initialize the early peripherals */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();

    board_init();

    xosc_sleep(1000);
    printf("Enabling IRQ 50\n");

    printf("ra register at start of cpu_init: 0x%lx\n", ra_register);

    __asm__ volatile ("nop");
}
