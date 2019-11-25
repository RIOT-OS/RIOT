/*
 * Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Common ARM7 boot up code
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#ifdef MODULE_PUF_SRAM
#include "puf_sram.h"
#endif

#include "log.h"

static inline void
_init_data(void)
{
    extern unsigned int _etext;
    extern unsigned int _data;
    extern unsigned int _edata;
    extern unsigned int __bss_start;
    extern unsigned int __bss_end;

    register unsigned int *p1;
    register unsigned int *p2;
    register unsigned int *p3;

    // initialize data from flash
    // (linker script ensures that data is 32-bit aligned)
    p1 = &_etext;
    p2 = &_data;
    p3 = &_edata;

    while (p2 < p3) {
        *p2++ = *p1++;
    }

    // clear bss
    // (linker script ensures that bss is 32-bit aligned)
    p1 = &__bss_start;
    p2 = &__bss_end;

    while (p1 < p2) {
        *p1++ = 0;
    }
}

void bootloader(void)
{
    extern void cpu_init(void);

    /* initialize bss and data */
    _init_data();

#ifdef MODULE_PUF_SRAM
    /* uninitialized heap starts after bss section */
    extern unsigned int __bss_end;
    puf_sram_init((uint8_t *) __bss_end, SEED_RAM_LEN);
#endif

    /* cpu specific setup of clocks, peripherals */
    cpu_init();

#ifdef MODULE_NEWLIB
    extern void __libc_init_array(void);
    __libc_init_array();
#endif
}

/** @} */
