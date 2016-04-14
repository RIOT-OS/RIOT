/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>

#include <termios.h>

#define MTU 9000

#define TRACE(x)

static void usage(void)
{
    fprintf(stderr, "usage: ethos <tap> <serial>\n");
}

static void checked_write(int handle, void *buffer, int nbyte)
{
    if (write(handle, buffer, nbyte) != nbyte) {
        fprintf(stderr, "write to fd %i failed: %s\n", handle, strerror(errno));
    }
}

int set_serial_attribs (int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        perror ("error in tcgetattr");
        return -1;
    }

    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8-bit chars*/
                                        /* disable IGNBRK for mismatched speed
                                         * tests; otherwise receive break*/
                                        /* as \000 chars*/
    tty.c_iflag &= ~IGNBRK;             /* disable break processing*/
    tty.c_lflag = 0;                    /* no signaling chars, no echo,*/
                                        /* no canonical processing*/
    tty.c_oflag = 0;                    /* no remapping, no delays*/
    tty.c_cc[VMIN]  = 0;                /* read doesn't block*/
    tty.c_cc[VTIME] = 5;                /* 0.5 seconds read timeout*/

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* shut off xon/xoff ctrl*/

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls,*/
                                        /* enable reading*/
    tty.c_cflag &= ~(PARENB | PARODD);  /* shut off parity*/
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    cfmakeraw(&tty);

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
        perror("error from tcsetattr");
        return -1;
    }
    return 0;
}

void set_blocking (int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        perror("error from tggetattr");
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            /* 0.5 seconds read timeout*/

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
        perror("error setting term attributes");
}

/**
 * @brief Escape char definitions
 * @{
 */
#define LINE_FRAME_DELIMITER        (0x7E)
#define LINE_ESC_CHAR               (0x7D)
#define LINE_FRAME_TYPE_DATA        (0x00)
#define LINE_FRAME_TYPE_TEXT        (0x01)
#define LINE_FRAME_TYPE_HELLO       (0x02)
#define LINE_FRAME_TYPE_HELLO_REPLY (0x03)
/** @} */

typedef enum {
    WAIT_FRAMESTART,
    IN_FRAME,
    IN_ESCAPE
} line_state_t;

typedef struct {
    line_state_t state;
    int frametype;
    size_t framebytes;
    char frame[MTU];
    char local_l2_addr[6];
    char remote_l2_addr[6];
} serial_t;

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. The caller     *
 *            needs to reserve enough space in *dev.                      *
 **************************************************************************/
int tun_alloc(char *dev, int flags) {

  struct ifreq ifr;
  int fd, err;

  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
    perror("Opening /dev/net/tun");
    return fd;
  }

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
    perror("ioctl(TUNSETIFF)");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);

  return fd;
}

static void _handle_char(serial_t *serial, char c)
{
    serial->frame[serial->framebytes] = c;
    serial->framebytes++;
}

static int _serial_handle_byte(serial_t *serial, char c)
{
    switch (serial->state) {
        case WAIT_FRAMESTART:
            if (c == LINE_FRAME_DELIMITER) {
                TRACE("frame start");
                serial->state = IN_FRAME;
                serial->frametype = 0;
                serial->framebytes = 0;
            }
            break;
        case IN_FRAME:
            if (c == LINE_ESC_CHAR) {
                TRACE("i esc");
                serial->state = IN_ESCAPE;
            }
            else if (c == LINE_FRAME_DELIMITER) {
                if (!serial->framebytes) {
                    TRACE("i del unexp");
                } else {
                    TRACE("i del");
                    serial->state = WAIT_FRAMESTART;
                    return serial->framebytes;
                }
            }
            else {
                _handle_char(serial, c);
            }
            break;
        case IN_ESCAPE:
            if (c == (LINE_FRAME_DELIMITER ^ 0x20)) {
                _handle_char(serial, LINE_FRAME_DELIMITER);
            }
            else if (c == (LINE_ESC_CHAR ^ 0x20)) {
                _handle_char(serial, LINE_ESC_CHAR);
            }
            else if (c == (LINE_FRAME_TYPE_TEXT ^ 0x20)) {
                TRACE("esc txt");
                serial->frametype = LINE_FRAME_TYPE_TEXT;
            }
            else if (c == (LINE_FRAME_TYPE_HELLO ^ 0x20)) {
                TRACE("esc hello");
                serial->frametype = LINE_FRAME_TYPE_HELLO;
            }
            else if (c == (LINE_FRAME_TYPE_HELLO_REPLY ^ 0x20)) {
                TRACE("esc hello_reply");
                serial->frametype = LINE_FRAME_TYPE_HELLO_REPLY;
            }
            else if (c == LINE_FRAME_DELIMITER) {
                TRACE("esc -del");
            }
            serial->state = IN_FRAME;
            break;
    }

    return 0;
}

static void _write_escaped(int fd, char* buf, ssize_t n)
{
    uint8_t out[2];
    size_t escaped = 0;

    while(n--) {
        char c = *buf++;
        switch(c) {
            case LINE_FRAME_DELIMITER:
                out[0] = LINE_ESC_CHAR;
                out[1] = (LINE_FRAME_DELIMITER ^ 0x20);
                checked_write(fd, out, 2);
                escaped++;
                break;
            case LINE_ESC_CHAR:
                out[0] = LINE_ESC_CHAR;
                out[1] = (LINE_ESC_CHAR ^ 0x20);
                checked_write(fd, out, 2);
                escaped++;
                break;
            default:
                checked_write(fd, &c, 1);
        }
    }
}

static void _send_hello(int serial_fd, serial_t *serial, unsigned type)
{
    char delim = LINE_FRAME_DELIMITER;
    char head[] = { LINE_FRAME_DELIMITER, LINE_ESC_CHAR, (type ^ 0x20) };
    checked_write(serial_fd, head, sizeof(head));
    checked_write(serial_fd, serial->local_l2_addr, 6);
    checked_write(serial_fd, &delim, 1);
}

static void _clear_neighbor_cache(const char *ifname)
{
    char tmp[20 + IFNAMSIZ];
    snprintf(tmp, sizeof(tmp), "ip neigh flush dev %s", ifname);
    if (system(tmp) < 0) {
        fprintf(stderr, "error while flushing device neighbor cache\n");
    }
}

int main(int argc, char *argv[])
{
    char inbuf[MTU];

    serial_t serial = {0};

    if (argc < 3) {
        usage();
        return 1;
    }

    char ifname[IFNAMSIZ];
    strncpy(ifname, argv[1], IFNAMSIZ);
    int tap_fd = tun_alloc(ifname, IFF_TAP | IFF_NO_PI);

    if (tap_fd < 0) {
        return 1;
    }

    int serial_fd = open(argv[2], O_RDWR | O_NOCTTY | O_SYNC);

    if (serial_fd < 0) {
        fprintf(stderr, "Error opening serial device %s\n", argv[2]);
        return 1;
    }

    set_serial_attribs(serial_fd, B115200, 0);
    set_blocking(serial_fd, 1);

    fd_set readfds;
    int max_fd = (serial_fd > tap_fd) ? serial_fd : tap_fd;

    fprintf(stderr, "----> ethos: sending hello.\n");
    _send_hello(serial_fd, &serial, LINE_FRAME_TYPE_HELLO);

    fprintf(stderr, "----> ethos: activating serial pass through.\n");
    _send_hello(serial_fd, &serial, LINE_FRAME_TYPE_HELLO);
    while(1) {
        int activity;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(tap_fd, &readfds);
        FD_SET(serial_fd, &readfds);
        activity = select( max_fd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("select error");
        }

        if (FD_ISSET(serial_fd, &readfds)) {
            ssize_t n = read(serial_fd, inbuf, sizeof(inbuf));
            if (n > 0) {
                char *ptr = inbuf;
                while (n--) {
                    size_t res = _serial_handle_byte(&serial, *ptr++);
                    if (res) {
                        switch (serial.frametype) {
                            case LINE_FRAME_TYPE_DATA:
                                checked_write(tap_fd, serial.frame, serial.framebytes);
                                break;
                            case LINE_FRAME_TYPE_TEXT:
                                checked_write(STDOUT_FILENO, serial.frame, serial.framebytes);
                                break;
                            case LINE_FRAME_TYPE_HELLO:
                            case LINE_FRAME_TYPE_HELLO_REPLY:
                                if (serial.framebytes == 6) {
                                    memcpy(serial.remote_l2_addr, serial.frame, 6);
                                    if (serial.frametype == LINE_FRAME_TYPE_HELLO) {
                                        fprintf(stderr, "----> ethos: hello received\n");
                                        _send_hello(serial_fd, &serial, LINE_FRAME_TYPE_HELLO_REPLY);
                                    } else {
                                        fprintf(stderr, "----> ethos: hello reply received\n");
                                    }
                                    _clear_neighbor_cache(ifname);
                                }
                                break;
                        }
                        memset(&serial, '\0', sizeof(serial));
                    }
                }
            }
            else {
                fprintf(stderr, "lost serial connection.\n");
                exit(1);
            }
        }

        if (FD_ISSET(tap_fd, &readfds)) {
            ssize_t res = read(tap_fd, inbuf, sizeof(inbuf));
            if (res <= 0) {
                fprintf(stderr, "error reading from tap device. res=%zi\n", res);
                continue;
            }
            char delim = LINE_FRAME_DELIMITER;
            checked_write(serial_fd, &delim, 1);
            _write_escaped(serial_fd, inbuf, res);
            checked_write(serial_fd, &delim, 1);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            ssize_t res = read(STDIN_FILENO, inbuf, sizeof(inbuf));
            if (res <= 0) {
                fprintf(stderr, "error reading from stdio. res=%zi\n", res);
                continue;
            }

            if (res) {
                char delim = LINE_FRAME_DELIMITER;
                char head[] = { LINE_FRAME_DELIMITER, LINE_ESC_CHAR, (LINE_FRAME_TYPE_TEXT ^ 0x20) };
                checked_write(serial_fd, head, sizeof(head));
                _write_escaped(serial_fd, inbuf, res);
                checked_write(serial_fd, &delim, 1);
            }
        }
    }

    return 0;
}
