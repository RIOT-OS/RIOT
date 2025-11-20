/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke
 * SPDX-FileCopyrightText: 2023 Hugues Larrive
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 * @}
 */

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

/* For Catchall-Loop */
#include "board.h"
#ifdef MODULE_PUF_SRAM
#include "puf_sram.h"
#endif
#ifdef MODULE_DBGPIN
#include "dbgpin.h"
#endif

/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void cpu_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This pair of functions hook circumvent the call to main
 *
 * avr-libc normally uses the .init9 section for a call to main. This call
 * seems to be not replaceable without hacking inside the library. We
 * circumvent the call to main by using section .init7 to call the function
 * reset_handler which therefore is the real entry point and section .init8
 * which should never be reached but just in case jumps to exit.
 * This way there should be no way to call main directly.
 */
void init7_ovr(void) __attribute__((section(".init7")));
void init8_ovr(void) __attribute__((section(".init8")));

__attribute__((used, naked)) void init7_ovr(void)
{
#if (FLASHEND <= 0x1FFF)
    __asm__ ("rjmp reset_handler");
#else
    __asm__ ("call reset_handler");
#endif
}

__attribute__((used, naked)) void init8_ovr(void)
{
#if (FLASHEND <= 0x1FFF)
    __asm__ ("rjmp exit");
#else
    __asm__ ("jmp exit");
#endif
}

/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. initialize the board (sync clock, setup std-IO)
 * 2. initialize and start RIOTs kernel
 */
__attribute__((used)) void reset_handler(void)
{
#ifdef MODULE_PUF_SRAM
    puf_sram_init((uint8_t *)RAMEND-SEED_RAM_LEN, SEED_RAM_LEN);
#endif

#ifdef MODULE_DBGPIN
    dbgpin_init();
#endif

    /* initialize the CPU */
    cpu_init();
    /* initialize the board and startup the kernel */
    board_init();
    /* startup the kernel */
    kernel_init();
}
