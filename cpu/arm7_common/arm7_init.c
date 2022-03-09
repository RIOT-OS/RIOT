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

#include "cpu.h"
#include "log.h"

static inline void _init_data(void)
{
    /* (linker script ensures that data is 32-bit aligned) */
    extern unsigned int _etext;
    extern unsigned int _srelocate; /* .data section */
    extern unsigned int _erelocate;
    extern unsigned int _szero;     /* .bss section */
    extern unsigned int _ezero;

/* Support for Battery Backup RAM */
#ifdef CPU_HAS_BACKUP_RAM
    extern unsigned int _sbackup_data_load[];
    extern unsigned int _sbackup_data[];
    extern unsigned int _ebackup_data[];
    extern unsigned int _sbackup_bss[];
    extern unsigned int _ebackup_bss[];
#endif

    register unsigned int *src;
    register unsigned int *dst;
    register unsigned int *end;

#ifdef DEVELHELP
    /* Fill user stack with canary values up until the current stack pointer */
    /* Read current stack pointer from CPU register */
    __asm__ volatile ("mov %[end], sp" : [end] "=r" (end) : : );
    dst = &__stack_start;
    while (dst < end) {
        *(dst++) = STACK_CANARY_WORD;
    }

    /* fill the interrupt stacks with canary values */
    extern unsigned int __stack_usr_start;
    extern unsigned int __stack_end;

    dst = &__stack_usr_start;
    end = &__stack_end;
    /* cppcheck-suppress comparePointers
     * (addresses exported as symbols via linker script and look unrelated
     * to cppcheck) */
    while (dst < end) {
        *(dst++) = STACK_CANARY_WORD;
    }
#endif

    /* initialize data from flash */
    src = &_etext;
    dst = &_srelocate;
    end = &_erelocate;

    /* cppcheck-suppress comparePointers
     * (addresses exported as symbols via linker script and look unrelated
     * to cppcheck) */
    while (dst < end) {
        *dst++ = *src++;
    }

    /* clear bss */
    dst = &_szero;
    end = &_ezero;

    /* cppcheck-suppress comparePointers
     * (addresses exported as symbols via linker script and look unrelated
     * to cppcheck) */
    while (dst < end) {
        *dst++ = 0;
    }

#ifdef CPU_HAS_BACKUP_RAM
    /* only initialize battery backup on cold boot */
    if (cpu_backup_ram_is_initialized()) {
        return;
    }

    /* load low-power data section. */
    src = _sbackup_data_load;
    dst = _sbackup_data;
    end = _ebackup_data;

    /* cppcheck-suppress comparePointers
     * (addresses exported as symbols via linker script and look unrelated
     * to cppcheck) */
    while (dst < end) {
        *dst++ = *src++;
    }

    /* zero-out low-power bss. */
    dst = _sbackup_bss;
    end = _ebackup_bss;

    /* cppcheck-suppress comparePointers
     * (addresses exported as symbols via linker script and look unrelated
     * to cppcheck) */
    while (dst < end) {
        *dst++ = 0;
    }
#endif /* CPU_HAS_BACKUP_RAM */
}

void bootloader(void)
{
    extern void cpu_init(void);

    /* initialize bss and data */
    _init_data();

#ifdef MODULE_PUF_SRAM
    /* use uninitialized heap */
    extern unsigned _sheap;
    puf_sram_init((uint8_t *) &_sheap, SEED_RAM_LEN);
#endif

    /* cpu specific setup of clocks, peripherals */
    cpu_init();

#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    extern void __libc_init_array(void);
    __libc_init_array();
#endif
}

/** @} */
