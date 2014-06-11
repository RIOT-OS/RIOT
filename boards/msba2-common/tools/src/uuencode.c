/*
 * LPC 2000 Loader, http://www.pjrc.com/arm/lpc2k_pgm
 * Copyright (c) 2004, PJRC.COM, LLC, <paul@pjrc.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* If this code fails to build, please provide at least the following
 * information when requesting (free) technical support.
 *
 * 1: Complete copy of all messages during the build.
 * 2: Output of "gtk-config --version"
 * 3: Output of "gtk-config --libs"
 * 4: Output of "gtk-config --cflags"
 * 5: Output of "uname -a"
 * 6: Version of GTK installed... eg, type: ls -l /lib/libgtk*
 * 7: Other info... which linux distribution, version, other software
 */

#include "uuencode.h"

static char uuchar(unsigned int val);


void uuencode(char *str, const unsigned char *data, int num)
{
    int i, n;
    unsigned int val;

    *str++ = uuchar(num);

    n = (num + 2) / 3;

    for (i = 0; i < n; i++) {
        val = ((data[0] & 0xFF) << 16)
              | ((data[1] & 0xFF) << 8)
              | ((data[2] & 0xFF) << 0);
        *str++ = uuchar(val >> 18);
        *str++ = uuchar(val >> 12);
        *str++ = uuchar(val >>  6);
        *str++ = uuchar(val >>  0);
        data += 3;
    }

    *str = '\0';
}

int uudecode(const char *str, unsigned char *data, int max)
{
    int num = 0;
    int i, n;
    unsigned int val;

    if (*str == '\0') {
        return 0;
    }

    num = *str++ - 32;

    if (num < 1 || num > 45) {
        return 0;
    }

    n = (num + 2) / 3;

    for (i = 0; i < n; i++) {
        if (str[0] < 32 || str[0] > 96) {
            return 0;
        }

        if (str[1] < 32 || str[1] > 96) {
            return 0;
        }

        if (str[2] < 32 || str[2] > 96) {
            return 0;
        }

        if (str[3] < 32 || str[3] > 96) {
            return 0;
        }

        val = (((str[0] - 32) & 0x3F) << 18)
              | (((str[1] - 32) & 0x3F) << 12)
              | (((str[2] - 32) & 0x3F) << 6)
              | (((str[3] - 32) & 0x3F) << 0);
        *data++ = (val >> 16) & 0xFF;
        *data++ = (val >> 8) & 0xFF;
        *data++ = (val >> 0) & 0xFF;
        str += 4;
    }

    return num;
}


static char uuchar(unsigned int val)
{
    val &= 0x3F;
    val += 0x20;

    if (val == 0x20) {
        val = 0x60;
    }

    return val;
}
