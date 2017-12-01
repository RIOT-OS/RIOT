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
 *
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uuencode.h"


unsigned int sum = 0;



void cksum(const char *str)
{
    int num, i;
    unsigned char data[256];

    if (str == NULL) {
        return;
    }

    num = uudecode(str, data, sizeof(data));

    for (i = 0; i < num; i++) {
        sum += data[i];
    }

}


int main()
{
    char buf[4096];

    while (!feof(stdin)) {
        fgets(buf, sizeof(buf), stdin);

        if (strcmp(buf, "\n") == 0) {
            break;
        }

        cksum(buf);
    }

    printf("sum = %u\n", sum);

    return 0;
}
