/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2018 Matthew Blue
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega1284p
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

/* For Catchall-Loop */
#include "board.h"


/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This pair of functions hook circumvent the call to main
 *
 * avr-libc normally uses the .init9 section for a call to main. This call
 * seems to be not replaceable without hacking inside the library. We
 * circumvent the call to main by using section .init7 to call the function
 * reset_handler which therefore is the real entry point and  section .init8
 * which should never be reached but just in case jumps to exit.
 * This way there should be no way to call main directly.
 */
void init7_ovr(void) __attribute__((naked)) __attribute__((section(".init7")));
void init8_ovr(void) __attribute__((naked)) __attribute__((section(".init8")));


void init7_ovr(void)
{
    __asm__("call reset_handler");
}

void init8_ovr(void)
{
    __asm__("jmp exit");
}
/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. initialize the board (sync clock, setup std-IO)
 * 2. initialize and start RIOTs kernel
 */
void reset_handler(void)
{
    /* initialize the board and startup the kernel */
    board_init();
    /* startup the kernel */
    kernel_init();
}
