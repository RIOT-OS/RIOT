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
#include <string.h>
#include <ctype.h>

#include "settings.h"

#define DEFAULT_FILE ""
#define DEFAULT_PORT "/dev/ttyS0"
#define DEFAULT_BAUD "115200"
#define DEFAULT_CRYSTAL "16"

char *baud_list[] = {"115200", "57600", "38400",
                     "19200", "9600", "4800", "2400", "1200", "300", NULL
                    };

static char file[128] = {DEFAULT_FILE};
static char port[64] = {DEFAULT_PORT};
static char baud[64] = {DEFAULT_BAUD};
static char crystal[64] = {DEFAULT_CRYSTAL};

static char settings_file[256] = {'\0'};


void init_settings(void)
{
    const char *home_dir;
    char buf[1024];

    home_dir = getenv("HOME");

    if (home_dir && *home_dir) {
        snprintf(settings_file, sizeof(settings_file),
                 "%s/.lpc2k_pgm", home_dir);
        FILE *fp = fopen(settings_file, "r");

        if (fp == NULL) {
            return;
        }

        while (!feof(fp)) {
            char *p, *q;
            buf[0] = '\0';
            fgets(buf, sizeof(buf), fp);

            if (strncmp(buf, "file:", 5) == 0) {
                for (p = buf + 5; isspace(*p); p++);
                q = rindex(p, '\n');
                if (q) {
                    *q = '\0';
                }
                q = rindex(p, '\r');
                if (q) {
                    *q = '\0';
                }
                snprintf(file, sizeof(file), "%s", p);
            }

            if (strncmp(buf, "port:", 5) == 0) {
                for (p = buf + 5; isspace(*p); p++);
                q = rindex(p, '\n');
                if (q) {
                    *q = '\0';
                }
                q = rindex(p, '\r');
                if (q) {
                    *q = '\0';
                }
                snprintf(port, sizeof(port), "%s", p);
            }

            if (strncmp(buf, "baud:", 5) == 0) {
                for (p = buf + 5; isspace(*p); p++);
                q = rindex(p, '\n');
                if (q) {
                    *q = '\0';
                }
                q = rindex(p, '\r');
                if (q) {
                    *q = '\0';
                }
                snprintf(baud, sizeof(baud), "%s", p);
            }

            if (strncmp(buf, "xtal:", 5) == 0) {
                for (p = buf + 5; isspace(*p); p++) ;
                q = rindex(p, '\n');
                if (q) {
                    *q = '\0';
                }
                q = rindex(p, '\r');
                if (q) {
                    *q = '\0';
                }
                snprintf(crystal, sizeof(crystal), "%s", p);
            }
        }

        fclose(fp);
    }
}

void write_settings_file(void)
{
    FILE *fp;

    if (settings_file[0] == '\0') {
        return;
    }

    fp = fopen(settings_file, "w");

    if (fp == NULL) {
        return;
    }

    fprintf(fp, "file: %s\n", file);
    fprintf(fp, "port: %s\n", port);
    fprintf(fp, "baud: %s\n", baud);
    fprintf(fp, "xtal: %s\n", crystal);
    fflush(fp);
    fclose(fp);
}

const char *file_setting(void)
{
    return file;
}

const char *port_setting(void)
{
    return port;
}

const char *baud_setting(void)
{
    return baud;
}

const char *crystal_setting(void)
{
    return crystal;
}

void new_file_setting(const char *new_file)
{
    if (strcmp(file, new_file)) {
        snprintf(file, sizeof(file), "%s", new_file);
        write_settings_file();
    }
}

void new_port_setting(const char *new_port)
{
    if (strcmp(port, new_port)) {
        snprintf(port, sizeof(port), "%s", new_port);
        write_settings_file();
    }
}

void new_baud_setting(const char *new_baud)
{
    if (strcmp(baud, new_baud)) {
        snprintf(baud, sizeof(baud), "%s", new_baud);
        write_settings_file();
    }
}

void new_crystal_setting(const char *new_xtal)
{
    if (strcmp(crystal, new_xtal)) {
        snprintf(crystal, sizeof(crystal), "%s", new_xtal);
        write_settings_file();
    }
}
