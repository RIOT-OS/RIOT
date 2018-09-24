/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    bootloaders    RIOT compatible bootloaders
 * @ingroup     bootloaders
 * @{
 *
 * @file
 * @brief      Minimal riot-based bootloader
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "panic.h"
#include "riotboot/slot.h"

void kernel_init(void)
{
    /* bootloader boots only slot 0 if it is valid */
    unsigned slot = 0;

    if (riotboot_slot_validate(slot) == 0) {
        riotboot_slot_jump(slot);
    }

    /* serious trouble! */
    while (1) {}
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
