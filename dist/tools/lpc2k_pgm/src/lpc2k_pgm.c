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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "lpc2k_pgm.h"
#include "serial.h"
#include "download.h"

int programming_done = 0;
int programming_status = 0;

int done_program(int i)
{
    printf("Programming done.\n");
    programming_done = 1;
    programming_status = i;
    return 0;
}

void handle_port_input()
{
    unsigned char buf[256];
    int num;

    num = read_serial_port(buf, sizeof(buf));

    if (num > 0) {
        download_rx_port(buf, num);
    }
}

void usage()
{
    printf("usage: lpc2k_pgm <port> <ihex-file>\n");
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage();
        exit(1);
    }

    char *port_name = argv[1];
    char *file_name = argv[2];

    if (open_serial_port(port_name) < 0) {
        return (1);
    }

    if (!download_begin(file_name)) {
        return 1;
    }

    while (!programming_done) {
        handle_port_input();
    }

    close_serial_port();

    return programming_status;
}
