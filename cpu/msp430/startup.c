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

#include "periph_conf.h"
#include "periph/init.h"
#include "kernel_init.h"
#include "stdio_base.h"
#include "cpu.h"
#include "irq.h"
#include "log.h"
#ifdef MODULE_DBGPIN
#include "dbgpin.h"
#endif

extern void board_init(void);
extern void msp430_cpu_init(void);

__attribute__((constructor)) static void startup(void)
{
    msp430_cpu_init();

    board_init();

#ifdef MODULE_DBGPIN
    dbgpin_init();
#endif

#ifdef MODULE_NEWLIB
    void _init(void);
    _init();
#endif

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();
    /* trigger static peripheral initialization */
    periph_init();
    /* continue with kernel initialization */
    kernel_init();

    __builtin_unreachable();
}
