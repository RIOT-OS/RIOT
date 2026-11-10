/*
 * SPDX-FileCopyrightText: 2004 PJRC.COM, LLC, <paul@pjrc.com>
 * SPDX-License-Identifier: GPL-2.0-only
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
