/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86
 * @{
 *
 * @file
 * @brief       Configurable reboot handler for x86.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_interrupts.h"
#include "x86_ports.h"
#include "x86_reboot.h"

#define KBC_DATA    (0x60)
#define KBC_STATUS  (0x64)
#define KBC_COMMAND (0x64)

#define KBC_OUTPUT_FULL (1u << 0)
#define KBC_INPUT_FULL  (1u << 1)

#define KBC_RESET ((uint8_t) ~(1u << 0))

static const struct idtr_t EMPTY_IDT = {
    .limit = 0,
    .base = NULL,
};

void x86_load_empty_idt(void)
{
    asm volatile ("lidt %0" :: "m"(EMPTY_IDT));
}

static bool fail_violently;

void NORETURN x86_kbc_reboot(void)
{
    /* First load an empty IDT. Any interrupt will cause a tripple fault. */
    x86_load_empty_idt();

    while (1) {
        if (fail_violently) {
            asm volatile ("int3"); /* Cause a tripple fault. Won't return. */
        }
        fail_violently = true;

        for (unsigned i = 0; i < 0x100; ++i) {
            for (unsigned j = 0; j < 0x10000; ++j) {
                uint8_t c = inb(KBC_STATUS);
                if (c & KBC_OUTPUT_FULL) {
                    (void) inb(KBC_DATA);
                }
                else if (!(c & KBC_OUTPUT_FULL)) {
                    outb(KBC_COMMAND, KBC_RESET);
                    break;
                }
            }
        }

        asm volatile ("int3"); /* Cause a tripple fault. Won't return. */
    }
}

static x86_reboot_t reboot_fun;
static bool reboot_twice;

int reboot_arch(int mode)
{
    switch (mode) {
        case RB_AUTOBOOT:
            asm volatile ("cli");
            if (!reboot_twice) {
                reboot_twice = true;
                if (reboot_fun) {
                    reboot_fun();
                }
            }
            x86_kbc_reboot();

        default:
            return -1;
    }
}

void x86_set_reboot_fun(x86_reboot_t fun)
{
    reboot_fun = fun;
}

static x86_shutdown_t shutdown_fun;

bool x86_shutdown(void)
{
    if (!shutdown_fun) {
        return false;
    }
    return shutdown_fun();
}

void x86_set_shutdown_fun(x86_shutdown_t fun)
{
    shutdown_fun = fun;
}
