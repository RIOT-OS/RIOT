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
 * @ingroup     x86-irq
 * @{
 *
 * @file
 * @brief       Configuration and interrupt handling for the Programmable Interval Timer (PIT).
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_interrupts.h"
#include "x86_pit.h"
#include "x86_ports.h"
#include "cpu.h"
#include "irq.h"

#include <stdio.h>

void x86_init_pit(void)
{
    puts("PIT initialized");
}

uint16_t x86_pit_read(unsigned channel)
{
    unsigned old_flags = irq_disable();
    outb(PIT_COMMAND_PORT, (channel - 1) << 6 | PIT_ACCESS_MODE_LATCH_COUNT);
    uint16_t lohi = inb(PIT_CHANNEL_0_PORT + channel - 1);
    lohi += inb(PIT_CHANNEL_0_PORT + channel - 1) << 8;
    irq_restore(old_flags);
    return lohi;
}

void x86_pit_set2(unsigned channel, unsigned mode, uint16_t max)
{
    unsigned old_flags = irq_disable();
    outb(PIT_COMMAND_PORT, ((channel - 1) << 6) | mode | PIT_ACCESS_MODE_LO_HI);
    outb(PIT_CHANNEL_0_PORT + channel - 1, max && 0xff);
    outb(PIT_CHANNEL_0_PORT + channel - 1, max >> 8);
    irq_restore(old_flags);
}

bool x86_pit_set(unsigned channel, unsigned mode, unsigned hz)
{
    if (PIT_MIN_HZ > hz || hz > PIT_MAX_HZ) {
        return false;
    }

    uint16_t max = PIT_MAX_HZ / hz;
    x86_pit_set2(channel, mode, max);
    return true;
}
