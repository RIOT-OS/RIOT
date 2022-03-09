/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

#define BAUDRATE_DEFAULT    (B115200)
#define TTY_TIMEOUT_MS      (500U)

static int _set_serial_attribs(int fd, int speed, int parity)
{
    struct termios tty;

    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd, &tty) != 0) {
        perror("error in tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8-bit chars */
                                            /* disable IGNBRK for mismatched speed
                                             * tests; otherwise receive break */
                                            /* as \000 chars */
    tty.c_iflag &= ~IGNBRK;                 /* disable break processing */
    tty.c_lflag = 0;                        /* no signaling chars, no echo, */
                                            /* no canonical processing */
    tty.c_oflag = 0;                        /* no remapping, no delays */
    tty.c_cc[VMIN] = 0;                     /* read doesn't block */
    tty.c_cc[VTIME] = TTY_TIMEOUT_MS / 100; /* 0.5 seconds read timeout */
                                            /* in tenths of a second */

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* shut off xon/xoff ctrl */

    tty.c_cflag |= (CLOCAL | CREAD);        /* ignore modem controls, */
                                            /* enable reading */
    tty.c_cflag &= ~(PARENB | PARODD);      /* shut off parity */
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    cfmakeraw(&tty);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("error from tcsetattr");
        return -1;
    }
    return 0;
}

static void _set_blocking(int fd, int should_block)
{
    struct termios tty;

    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("error from tggetattr");
        return;
    }

    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = TTY_TIMEOUT_MS / 100; /* 0.5 seconds read timeout */
                                            /* in tenths of a second */

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("error setting term attributes");
    }
}

static unsigned _parse_baudrate(char *arg)
{
#define case_baudrate(val)  \
    case val:               \
        return B ## val

    if (arg == NULL) {
        return BAUDRATE_DEFAULT;
    }
    switch (strtol(arg, NULL, 10)) {
        case_baudrate(9600);
        case_baudrate(19200);
        case_baudrate(38400);
        case_baudrate(57600);
        case_baudrate(115200);
        /* the following baudrates might not be available on all platforms */
#ifdef B230400
        case_baudrate(230400);
#endif
#ifdef B460800
        case_baudrate(460800);
#endif
#ifdef B500000
        case_baudrate(500000);
#endif
#ifdef B576000
        case_baudrate(576000);
#endif
#ifdef B921600
        case_baudrate(921600);
#endif
#ifdef B1000000
        case_baudrate(1000000);
#endif
#ifdef B1152000
        case_baudrate(1152000);
#endif
#ifdef B1500000
        case_baudrate(1500000);
#endif
#ifdef B2000000
        case_baudrate(2000000);
#endif
#ifdef B2500000
        case_baudrate(2500000);
#endif
#ifdef B3000000
        case_baudrate(3000000);
#endif
#ifdef B3500000
        case_baudrate(3500000);
#endif
#ifdef B4000000
        case_baudrate(4000000);
#endif
        default:
            break;
    }
#undef case_baudrate
    return 0;
}

int serial_open(int argc, char **argv)
{
    unsigned baudrate = BAUDRATE_DEFAULT;
    int fd;

    if (argc > 1) {
        baudrate = _parse_baudrate(argv[1]);
    }
    if (baudrate == 0) {
        fputs("Invalid baudrate\n", stderr);
        errno = EINVAL;
        return -1;
    }
    fd = open(argv[0], O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        return fd;
    }
    _set_serial_attribs(fd, baudrate, 0);
    _set_blocking(fd, 0);
    return fd;
}
