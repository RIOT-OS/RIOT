/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>
#include <string.h>
#include <assert.h>

#include "periph/uart.h"
#include "periph/timer.h"
#include "arch/panic_arch.h"
#include "kernel_init.h"
#include "cpu.h"
#include "board.h"

void mips_start(void);

extern void _fini(void);
extern void atexit(void (*)(void));
extern void _init(void);
extern void exit(int);

#ifdef FLASH_XIP
extern char _rom_data_copy __attribute__((section("data")));
extern char _fdata __attribute__((section("data")));
extern char _edata __attribute__((section("data")));
#endif

/*
 * Note the mips toolchain crt expects to jump to main but RIOT wants the user
 * code to start at main for some perverse reason, but thankfully the crt
 * does provide this hook function which gets called fairly close to the jump
 * to main, thus if we finish off the job of the crt here and never returns
 * we can support this madness.
 */
void software_init_hook(void)
{
#ifdef FLASH_XIP
    /* copy initialised data from its LMA to VMA */
    memcpy(&_fdata, &_rom_data_copy, (int)&_edata - (int)&_fdata);
#endif

    atexit(_fini);
    _init();

    mips_start();

    exit(-1);
}


void mips_start(void)
{
    board_init();

#if MODULE_NEWLIB
#error "This Port is designed to work with the (newlib) C library provided with the mips sdk toolchain"
#endif

    /* kernel_init */
    kernel_init();
}

void panic_arch(void)
{
    printf("\nPANIC!\n");
    assert(0);
    while (1) {
    }
}
