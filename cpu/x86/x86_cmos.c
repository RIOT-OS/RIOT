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
 * @brief       Accessing the CMOS space of the BIOS.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_cmos.h"

uint8_t x86_cmos_read(int reg)
{
    outb(CMOS_ADDRESS, reg);
    io_wait();
    return inb(CMOS_DATA);
}

void x86_cmos_write(int reg, uint8_t value)
{
    outb(CMOS_ADDRESS, reg);
    io_wait();
    outb(CMOS_DATA, value);
}

void x86_cmos_serial(uint8_t (*serial)[CMOS_SERIAL_LEN])
{
    for (unsigned i = 0; i < CMOS_SERIAL_LEN; ++i) {
        (*serial)[i] = x86_cmos_read(CMOS_SERIAL_OFFS + i);
    }
}
