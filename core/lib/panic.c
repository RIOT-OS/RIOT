/*
 * SPDX-FileCopyrightText: 2015 INRIA
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Crash handling functions
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include "kernel_defines.h"
#include "cpu.h"
#include "irq.h"
#include "panic.h"
#include "periph/pm.h"

#if defined(DEVELHELP) && defined(MODULE_PS)
#include "ps.h"
#endif

/* If a device is flashed over USB bootloader, try to launch
 * the bootloader again on crash so the user can re-flash it.
 */
#if defined(DEVELHELP) && defined(MODULE_USB_BOARD_RESET)
#include "usb_board_reset.h"
#endif

/* flag preventing "recursive crash printing loop" */
static int crashed = 0;

void __attribute__((weak)) panic_arch(void)
{}
void __attribute__((weak)) panic_app(core_panic_t crash_code,
                                     const char *message)
{
    (void)crash_code;
    (void)message;
}

/* WARNING: this function NEVER returns! */
NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
#ifdef NDEBUG
    (void)crash_code;
#endif

    if (crashed == 0) {
        /* print panic message to console (if possible) */
        crashed = 1;

        /* Call back app in case it wants to store some context */
        panic_app(crash_code, message);
        printf("*** RIOT kernel panic:\n%s\n\n", message);
#ifdef DEVELHELP
#ifdef MODULE_PS
        ps();
        printf("\n");
#endif

        printf("*** halted.\n\n");
#else
        printf("*** rebooting...\n\n");
#endif
    }
    /* disable watchdog and all possible sources of interrupts */
    irq_disable();
    panic_arch();
#if CONFIG_CORE_REBOOT_ON_PANIC && defined(MODULE_PERIPH_PM)
    /* DEVELHELP not set => reboot system */
    pm_reboot();
#else
    /* DEVELHELP set => power off system */
    /*               or start bootloader */
#if defined(MODULE_USB_BOARD_RESET)
    usb_board_reset_in_bootloader();
#elif defined(MODULE_PERIPH_PM)
    pm_off();
#else
    while (1) {}
#endif
#endif /* DEVELHELP */

    /* tell the compiler that we won't return from this function
       (even if we actually won't even get here...) */
    UNREACHABLE();
}
