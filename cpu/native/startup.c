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
#include <stdlib.h>
#include <err.h>

#include <kernel_internal.h>
#include <cpu.h>

#include "tap.h"

extern void board_init(void);
extern void native_cpu_init(void);
extern void native_interrupt_init(void);

__attribute__((constructor)) static void startup(int argc, char **argv)
{

#ifdef MODULE_NATIVENET
    if (argc < 2) {
        printf("usage: %s <tap interface>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
#else /* args unused here */
    (void) argc;
    (void) argv;
#endif

    native_cpu_init();
    native_interrupt_init();
#ifdef MODULE_NATIVENET
    tap_init(argv[1]);
#endif

    board_init();

    puts("RIOT native hardware initialization complete.\n");
    kernel_init();
}
