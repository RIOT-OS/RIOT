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
 * @brief       Trivial functions to write to the videoram.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_videoram.h"

#define VIDEORAM ((char *) 0xB8000)

void videoram_putc(char c)
{
    static unsigned pos = 0;
    if (c == '\n') {
        pos += 80;
    }
    else if (c == '\r') {
        pos = (pos / 80) * 80;
    }
    else {
        VIDEORAM[2 * pos + 0] = c;
        VIDEORAM[2 * pos + 1] = (char) 0x9E; /* light blue bg, light yellow fg */
        ++pos;
    }
    if (pos >= 25 * 80) {
        pos = 0;
    }
}

void videoram_put(const char *s)
{
    for (; *s; ++s) {
        videoram_putc(*s);
    }
}

void videoram_puts(const char *s)
{
    videoram_put(s);
    videoram_put("\r\n");
}

void videoram_put_hex(unsigned long v)
{
    char s[2 + 2 * sizeof v + 1];
    char *p = &s[sizeof s];

    *--p = 0;
    do {
        char c = v % 16;
        v /= 16;
        *--p = c + (c < 10 ? '0' : 'A' - 10);
    } while (v > 0);
    *--p = 'x';
    *--p = '0';

    videoram_put(p);
}
