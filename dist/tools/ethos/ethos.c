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

#include <netdb.h>

#include <termios.h>

#define MTU 9000

#define TRACE(x)
#define TTY_TIMEOUT_MS (500)

#define case_baudrate(val)    \
    case val:                 \
        *baudrate = B ## val; \
        break

#define BAUDRATE_DEFAULT B115200

#define TCP_DEV "tcp:"
#define IOTLAB_TCP_PORT "20000"

/* Size of serial write buffer */
#define SERIAL_BUFFER_SIZE 64

static void usage(void)
{
    fprintf(stderr, "Usage: ethos <tap> <serial> [baudrate]\n");
    fprintf(stderr, "       ethos <tap> tcp:<host> [port]\n");
}

static void checked_write(int handle, void *buffer, int nbyte)
{
    while (nbyte > 0) {
        ssize_t res = write(handle, buffer, nbyte);
        if (res <= 0) {
            fprintf(stderr, "write to fd %i failed: %s\n", handle, strerror(errno));
            return;
        }
        nbyte -= res;
    }
}

int set_serial_attribs(int fd, int speed, int parity)
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
    tty.c_cc[VTIME] = TTY_TIMEOUT_MS / 100; /* 0.5 seconds read timeout*/
                                            /* in tenths of a second*/

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

void set_blocking(int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        perror("error from tggetattr");
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = TTY_TIMEOUT_MS / 100; /* 0.5 seconds read timeout*/
                                            /* in tenths of a second*/

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
    /*
     * Certain USB-to-UART adapters/drivers will immediately send a USB packet
     * with a single byte instead of buffering internally when the application
     * does writes one byte at a time. Since USB Full Speed can only send 1
     * packet per 1 ms, this causes huge latencies for the network, because each
     * byte of data will then add at least 1 ms on the latency.
     * Observed on NXP OpenSDAv2 (Kinetis FRDM boards), both CMSIS/mbed DAPlink
     * and Segger Jlink firmware are affected.
     */
    /* Our workaround is to prepare the data to send in a local buffer and then
     * call write() on the buffer instead of one char at a time */
    uint8_t out[SERIAL_BUFFER_SIZE];
    size_t escaped = 0;
    size_t buffered = 0;

    while(n--) {
        unsigned char c = *buf++;
        if (c == LINE_FRAME_DELIMITER || c == LINE_ESC_CHAR) {
            out[buffered++] = LINE_ESC_CHAR;
            c ^= 0x20;
            ++escaped;
            if (buffered >= SERIAL_BUFFER_SIZE) {
                checked_write(fd, out, buffered);
                buffered = 0;
            }
        }
        out[buffered++] = c;
        if (buffered >= SERIAL_BUFFER_SIZE) {
            checked_write(fd, out, buffered);
            buffered = 0;
        }
    }
    if (buffered) {
        checked_write(fd, out, buffered);
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

static int _parse_baudrate(const char *arg, unsigned *baudrate)
{
    if (arg == NULL) {
        *baudrate = BAUDRATE_DEFAULT;
        return 0;
    }

    switch(strtol(arg, (char**)NULL, 10)) {
    case 9600:
        *baudrate = B9600;
        break;
    case 19200:
        *baudrate = B19200;
        break;
    case 38400:
        *baudrate = B38400;
        break;
    case 57600:
        *baudrate = B57600;
        break;
    case 115200:
        *baudrate = B115200;
        break;
    /* the following baudrates might not be available on all platforms */
    #ifdef B234000
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
        return -1;
    }

    return 0;
}

int _parse_tcp_arg(char *name, char *port_arg, char **host, char **port)
{
    /* Remove 'tcp:' */
    name = &name[sizeof(TCP_DEV) - 1];

    /* Set default if NULL */
    if (!port_arg) {
        port_arg = IOTLAB_TCP_PORT;
    }

    *host = name;
    *port = port_arg;

    return 0;
}

/* Adapted from 'getaddrinfo' manpage example */
int _tcp_connect(char *host, char *port)
{
    int sfd = -1;
    struct addrinfo hints, *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(host, port, &hints, &result);
    if (s) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(sfd);
    }

    freeaddrinfo(result);

    if (rp == NULL) {
        fprintf(stderr, "Could not connect to '%s:%s'\n", host, port);
        return -1;
    }

    return sfd;
}

int _set_socket_timeout(int sfd)
{
    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = TTY_TIMEOUT_MS * 1000,
    };

    if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO,
                   (char *)&timeout, sizeof(timeout)) == -1) {
        perror("setsockopt failed\n");
        return 1;
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO,
                   (char *)&timeout, sizeof(timeout)) == -1) {
        perror("setsockopt failed\n");
        return 1;
    }
    return 0;
}

int _open_tcp_connection(char *name, char *port_arg)
{
    char *host;
    char *port;

    int ret = _parse_tcp_arg(name, port_arg, &host, &port);
    if (ret) {
        fprintf(stderr, "Error while parsing tcp arguments\n");
        return -1;
    }

    int sfd = _tcp_connect(host, port);
    if (_set_socket_timeout(sfd)) {
        fprintf(stderr, "Error while setting socket options\n");
        return -1;
    }
    return sfd;
}

int _open_serial_connection(char *name, char *baudrate_arg)
{
    unsigned baudrate = 0;
    if (_parse_baudrate(baudrate_arg, &baudrate) == -1) {
        fprintf(stderr, "Invalid baudrate specified: %s\n", baudrate_arg);
        return 1;
    }

    int serial_fd = open(name, O_RDWR | O_NOCTTY | O_SYNC);

    if (serial_fd < 0) {
        fprintf(stderr, "Error opening serial device %s\n", name);
        return -1;
    }

    set_serial_attribs(serial_fd, baudrate, 0);
    set_blocking(serial_fd, 1);

    return serial_fd;
}

int _open_connection(char *name, char* option)
{
    if (strncmp(name, TCP_DEV, strlen(TCP_DEV)) == 0) {
        return _open_tcp_connection(name, option);
    } else {
        return _open_serial_connection(name, option);
    }
}

int main(int argc, char *argv[])
{
    char inbuf[MTU];
    char *serial_option = NULL;

    serial_t serial = {0};

    if (argc < 3) {
        usage();
        return 1;
    }

    if (argc >= 4) {
        serial_option = argv[3];
    }

    char ifname[IFNAMSIZ];
    strncpy(ifname, argv[1], IFNAMSIZ);
    int tap_fd = tun_alloc(ifname, IFF_TAP | IFF_NO_PI);

    if (tap_fd < 0) {
        return 1;
    }


    int serial_fd = _open_connection(argv[2], serial_option);
    if (serial_fd < 0) {
        fprintf(stderr, "Error opening serial device %s\n", argv[2]);
        return 1;
    }

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
