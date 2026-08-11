/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#if MODULE_DBGPIN
#include "dbgpin.h"
#endif

extern void board_init(void);
extern void msp430_cpu_init(void);

__attribute__((constructor)) static void startup(void)
{
    msp430_cpu_init();

    board_init();

#if MODULE_DBGPIN
    dbgpin_init();
#endif

#if MODULE_NEWLIB
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
