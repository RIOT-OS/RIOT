/**
 * Native CPU entry code
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include <dlfcn.h>
#else
#include <dlfcn.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <err.h>


#include "kernel_internal.h"
#include "cpu.h"

#include "native_internal.h"
#include "tap.h"

__attribute__((constructor)) static void startup(int argc, char **argv)
{
    /* get system read/write */
    *(void **)(&real_read) = dlsym(RTLD_NEXT, "read");
    *(void **)(&real_write) = dlsym(RTLD_NEXT, "write");

#ifdef MODULE_NATIVENET
    if (argc < 2) {
        printf("usage: %s <tap interface>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
#else /* args unused here */
    (void) argc;
    (void) argv;
#endif

    native_hwtimer_pre_init();
    native_cpu_init();
    native_interrupt_init();
#ifdef MODULE_NATIVENET
    tap_init(argv[1]);
#endif

    board_init();

    puts("RIOT native hardware initialization complete.\n");
    kernel_init();
}
