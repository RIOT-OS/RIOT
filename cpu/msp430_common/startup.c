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

extern void board_init(void);

/**
 * Leave some extra space in the stack to allows us to finish the kernel
 * initialization procedure. __heap_end is set the current stack, minus
 * STACK_EXTRA since there is still code to execute.
 */
#define STACK_EXTRA 32

__attribute__((constructor)) static void startup(void)
{
    /* use putchar so the linker links it in: */
    putchar('\n');

    board_init();

    LOG_INFO("RIOT MSP430 hardware initialization complete.\n");

    /* save current stack pointer as top of heap before enter the thread mode */
    extern char *__heap_end;
    __asm__ __volatile__("mov r1, %0" : "=r"(__heap_end));
    __heap_end -= STACK_EXTRA;

    kernel_init();
}
