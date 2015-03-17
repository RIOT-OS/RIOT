/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
