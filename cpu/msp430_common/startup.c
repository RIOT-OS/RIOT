/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       Calls startup functions on MSP430-based platforms
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "kernel_init.h"
#include "irq.h"
#include "log.h"

#include "board.h"

__attribute__((constructor)) static void startup(void)
{
    /* use putchar so the linker links it in: */
    putchar('\n');
    /* init CPU core */
    msp430_cpu_init();

    board_init();

    LOG_INFO("RIOT MSP430 hardware initialization complete.\n");

    kernel_init();
}
