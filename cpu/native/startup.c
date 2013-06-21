/**
 * Native CPU entry code
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <kernel_intern.h>
#include <cpu.h>

extern void board_init(void);
extern void native_cpu_init(void);
extern void native_interrupt_init(void);

__attribute__((constructor)) static void startup(void)
{
    native_cpu_init();
    native_interrupt_init();

    board_init();

    puts("RIOT native hardware initialization complete.\n");
    kernel_init();
}
