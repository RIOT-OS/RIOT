/*
 * LPC 2000 Loader, http://www.pjrc.com/arm/lpc2k_pgm
 * Copyright (c) 2004, PJRC.COM, LLC, <paul@pjrc.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#ifdef LINUX
#include <linux/serial.h>
#endif

#include "serial.h"

static int port_fd = -1;

static tcflag_t baud_name_to_flags(const char *baud_name);
static void report_open_error(const char *filename, int err);

char *baud_rate = "115200";

int open_serial_port(const char *port_name)
{
    int r;
    struct termios term_setting;

    if (port_fd >= 0) {
        close(port_fd);
    }

    port_fd = open(port_name, O_RDWR);

    if (port_fd < 0) {
        report_open_error(port_name, errno);
        return -1;
    }

    bzero(&term_setting, sizeof(term_setting));
    term_setting.c_cflag = (CS8 | CREAD);
    term_setting.c_cc[VMIN] = 1;
    term_setting.c_cc[VTIME] = 1;
    r = tcsetattr(port_fd, TCSANOW, &term_setting);

    if (r != 0) {
        return -1;
    }

    r = set_baud(baud_rate);

    if (r == 0) {
        printf("Port \"%s\" opened at %s baud\r\n",
               port_name, baud_rate);
    }
    else {
        printf("Port \"%s\" opened, unable to set baud to %s\r\n",
               port_name, baud_rate);
    }

#ifdef LINUX
    {
        struct serial_struct kernel_serial_settings;
        /* attempt to set low latency mode, but don't worry if we can't */
        r = ioctl(port_fd, TIOCGSERIAL, &kernel_serial_settings);

        if (r < 0) {
            return 0;
        }

        kernel_serial_settings.flags |= ASYNC_LOW_LATENCY;
        ioctl(port_fd, TIOCSSERIAL, &kernel_serial_settings);
    }
#endif
    return 0;
}


/* if the port can't be opened, try to print as much info as
 * possible, so the problem can be resolved (usually permissions)
 */
static void report_open_error(const char *filename, int err)
{
    struct stat info;
    uid_t my_uid;
    gid_t my_gid;
    char my_uname[64], my_gname[64], file_uname[64], file_gname[64];
    struct passwd *p;
    struct group *g;
    mode_t perm;
    int r, perm_ok = 0;

    printf("\r\n");
    printf("Unable to open \"%s\"\r\n", filename);

    if (err == EACCES) {
        printf("You don't have permission to access %s\r\n", filename);
    }

    //printf("Attemping to find more information about %s....\r\n", filename);
    r = stat(filename, &info);

    if (r < 0) {
        if (errno == ENOENT) {
            printf("file %s does not exist\r\n", filename);
        }
        else if (errno == ELOOP) {
            printf("too many symbolic links\r\n");
        }
        else if (errno == EACCES) {
            printf("permission denied to get file status\r\n");
        }
        else {
            printf("Unable to get file status, err%d\r\n", errno);
        }

        return;
    }

    my_uid = getuid();
    my_gid = getgid();

    p = getpwuid(my_uid);

    if (p) {
        snprintf(my_uname, sizeof(my_uname),
                 "\"%s\" (gid=%d)", p->pw_name, (int)my_uid);
    }
    else {
        snprintf(my_uname, sizeof(my_uname),
                 "(gid=%d)", (int)my_uid);
    }

    p = getpwuid(info.st_uid);

    if (p) {
        snprintf(file_uname, sizeof(file_uname),
                 "\"%s\" (uid=%d)", p->pw_name, (int)info.st_uid);
    }
    else {
        snprintf(file_uname, sizeof(file_uname),
                 "(uid=%d)", (int)info.st_uid);
    }

    g = getgrgid(my_gid);

    if (g) {
        snprintf(my_gname, sizeof(my_gname),
                 "\"%s\" (gid=%d)", g->gr_name, (int)my_gid);
    }
    else {
        snprintf(my_gname, sizeof(my_gname),
                 "(gid=%d)", (int)my_gid);
    }

    g = getgrgid(info.st_gid);

    if (g) {
        snprintf(file_gname, sizeof(file_gname),
                 "\"%s\" (uid=%d)", g->gr_name, (int)info.st_gid);
    }
    else {
        snprintf(file_gname, sizeof(file_gname),
                 "(uid=%d)", (int)info.st_gid);
    }

    /* printf("%s is owned by: user %s, group %s\r\n",
        filename, file_uname, file_gname); */

    perm = info.st_mode;

    if ((perm & S_IROTH) && (perm & S_IWOTH)) {
        printf("%s has read/write permission for everybody\r\n",
               filename);
    }
    else {
        printf("%s is not read/write for everybody, so\r\n", filename);
        printf("  you must match either user or group permission\r\n");

        if ((perm & S_IRUSR) && (perm & S_IWUSR)) {
            printf("%s has read/write permission for user %s\r\n",
                   filename, file_uname);
            perm_ok = 1;
        }

        if ((perm & S_IRGRP) && (perm & S_IWGRP)) {
            printf("%s has read/write permission for group %s\r\n",
                   filename, file_gname);
            perm_ok = 1;
        }

        if (perm_ok == 0) {
            printf("%s does not read/write permission for user or group!\r\n",
                   filename);
        }
        else {
            printf("Your access privs: user %s, group %s\r\n",
                   my_uname, my_gname);
        }
    }

    printf("\r\n");
}



int write_serial_port(const void *buf, int num)
{
    return (write(port_fd, buf, num));
}


void input_flush_serial_port(void)
{
    tcflush(port_fd, TCIFLUSH);
}


int read_serial_port_nb(unsigned char *buf, int bufsize)
{
    int num, flags;

    flags = fcntl(port_fd, F_GETFL);
    fcntl(port_fd, F_SETFL, flags | O_NONBLOCK);
    num = read(port_fd, buf, bufsize);
    fcntl(port_fd, F_SETFL, flags);
    return num;
}

int read_serial_port(unsigned char *buf, int bufsize)
{
    int num;

    num = read(port_fd, buf, bufsize);

    return num;
}


void send_break_signal(void)
{
    tcsendbreak(port_fd, 0);
}


void close_serial_port(void)
{
    if (port_fd >= 0) {
        close(port_fd);
        port_fd = -1;
    }
}


tcflag_t baud_name_to_flags(const char *baud_name)
{
    if (strcmp(baud_name, "230400") == 0) {
        return B230400;
    }

    if (strcmp(baud_name, "115200") == 0) {
        return B115200;
    }

    if (strcmp(baud_name, "57600") == 0) {
        return B57600;
    }

    if (strcmp(baud_name, "38400") == 0) {
        return B38400;
    }

    if (strcmp(baud_name, "19200") == 0) {
        return B19200;
    }

    if (strcmp(baud_name, "9600") == 0) {
        return B9600;
    }

    if (strcmp(baud_name, "4800") == 0) {
        return B4800;
    }

    if (strcmp(baud_name, "2400") == 0) {
        return B2400;
    }

    if (strcmp(baud_name, "1200") == 0) {
        return B1200;
    }

    if (strcmp(baud_name, "300") == 0) {
        return B300;
    }

    return B0;
}


int set_baud(const char *baud_name)
{
    struct termios port_setting;
    tcflag_t baud;
    int r;

    if (port_fd < 0) {
        return -1;
    }

    baud = baud_name_to_flags(baud_name);

    if (baud == B0) {
        return -2;
    }

    r = tcgetattr(port_fd, &port_setting);

    if (r != 0) {
        return -3;
    }

#ifdef __APPLE__
    cfsetspeed(&port_setting, baud);
    port_setting.c_iflag = IGNBRK | IGNPAR;
    port_setting.c_cflag =  CS8 | CREAD | HUPCL | CLOCAL;
#else
    port_setting.c_iflag = IGNBRK | IGNPAR;
    port_setting.c_cflag = baud | CS8 | CREAD | HUPCL | CLOCAL;
#endif

    port_setting.c_oflag = 0;
    port_setting.c_lflag = 0;
    r = tcsetattr(port_fd, TCSAFLUSH, &port_setting);

    if (r != 0) {
        return -4;
    }

    return 0;
}


// Normally this should never be used... except to pass the port
// file descriptor to the GTK event monitoring loop.  All other
// use of the serial port is supposed to happen in the file.
int serial_port_fd(void)
{
    return port_fd;
}



void set_rts(int val)
{
    int flags;
    int result;

    result = ioctl(port_fd, TIOCMGET, &flags);

    if (result == -1) {
        printf("Error %i while reading port io flags\n", errno);
        return;
    }

    if (val) {
        flags |= TIOCM_RTS;
    }
    else {
        flags &= ~(TIOCM_RTS);
    }

    result = ioctl(port_fd, TIOCMSET, &flags);

    if (result == -1) {
        printf("Error %i while setting port io flags\n", errno);
    }
}







void set_dtr(int val)
{
    int flags;
    int result;

    result = ioctl(port_fd, TIOCMGET, &flags);

    if (result == -1) {
        printf("Error %i while reading port io flags\n", errno);
        return;
    }

    if (val) {
        flags |= TIOCM_DTR;
    }
    else {
        flags &= ~(TIOCM_DTR);
    }

    result = ioctl(port_fd, TIOCMSET, &flags);

    if (result == -1) {
        printf("Error %i while setting port io flags\n", errno);
    }
}
