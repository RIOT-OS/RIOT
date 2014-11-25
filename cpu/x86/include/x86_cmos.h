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
 * Functions to interact with the CMOS memory of the BIOS.
 *
 * @ingroup x86
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__CMOS__H__
#define CPU__X86__CMOS__H__

#include "x86_ports.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CMOS_ADDRESS (0x70)
#define CMOS_DATA    (0x71)

#define CMOS_SERIAL_OFFS (0x41)
#define CMOS_SERIAL_LEN  (6)

/**
 * @brief   Read a CMOS register.
 * @returns The value of the CMOS register.
 */
uint8_t x86_cmos_read(int reg);

/**
 * @brief   Write a CMOS register.
 */
void x86_cmos_write(int reg, uint8_t value);

/**
 * @brief   Read serial number of the BIOS.
 *
 * Most -- if not all current -- BIOSes do not seem to store any serial number here.
 * This function won't cause an error, but the read serial might be some more or less random data.
 * The implementor of the board specific code should know whether the BIOS contains a serial number.
 */
void x86_cmos_serial(uint8_t (*serial)[CMOS_SERIAL_LEN]);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
