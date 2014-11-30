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
 * Writing to the videoram.
 *
 * @ingroup x86
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__VIDEORAM_H__
#define CPU__X86__VIDEORAM_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Print out a single character on the graphical device.
 *
 * This method can be used before the UART system was initialized.
 *
 * Special characters that are understood: '\\n' and '\\r'.
 * Use DOS newlines.
 */
void videoram_putc(char c);

/**
 * @brief   Print out multiple characters on the graphical device.
 *
 * This is the same as calling videoram_putc() repeatedly.
 */
void videoram_put(const char *s);

/**
 * @brief   Print out multiple characters on the graphical device, then go to the next line.
 *
 * This is the same as calling `videoram_put(s); videoram_put("\r\n");`.
 */
void videoram_puts(const char *s);

/**
 * @brief   Print out a hexadecimal number on the graphical device, including "0x" at the start.
 */
void videoram_put_hex(unsigned long v);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
