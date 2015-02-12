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
 * @ingroup     boards_qemu-i386
 * @{
 *
 * @file
 * @brief       Initialization code for the qemu-i386 board
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "x86_ports.h"
#include "x86_reboot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static bool qemu_shutdown(void)
{
    unsigned old_state = disableIRQ();

    DEBUG("SHUTTING DOWN.\n");

    /* (phony) ACPI shutdown (http://forum.osdev.org/viewtopic.php?t=16990) */
    /* Works for qemu and bochs. */
    outw(0xB004, 0x2000);

    restoreIRQ(old_state);
    return false;
}

void x86_init_board(void)
{
    x86_set_shutdown_fun(qemu_shutdown);
}
