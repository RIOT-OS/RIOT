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

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <net/if.h>
#ifdef __linux__
#include <linux/if.h>
#include <linux/if_tun.h>
#endif
#include <netdb.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <termios.h>
#include <unistd.h>

#define OPTSTRING               "hI:"

#define TUN_IFACE_DEFAULT       "sl0"
#define BAUDRATE_DEFAULT        (B115200)
#define TCP_PORT_STR_DEFAULT    "20000"

#define TCP_SPEC                "tcp:"

#define TUN_MTU                 (1500U)
#define SERIAL_BUFFER_SIZE      (64U)
#define TTY_TIMEOUT_MS          (500U)

#define SLIP_START_STDIO        0x0aU
#define SLIP_START_NET_MASK     0xf0U
#define SLIP_START_NET_IPV4     0x40U
#define SLIP_START_NET_IPV6     0x60U

#define SLIP_END                0xc0U
#define SLIP_ESC                0xdbU
#define SLIP_END_ESC            0xdcU
#define SLIP_ESC_ESC            0xddU

static void _usage(char *cmd)
{
    fprintf(stderr, "Usage: %s [-I %s] <serial> [baudrate]\n",
            cmd, TUN_IFACE_DEFAULT);
    fprintf(stderr, "Usage: %s [-I %s] tcp:<host> [port]\n",
            cmd, TUN_IFACE_DEFAULT);
}

#ifndef __linux__
static int devopen(const char *dev, int flags)
{
    char devpath[1024];

    strcpy(devpath, "/dev/");
    strncat(devpath, dev, sizeof(devpath) - (sizeof("/dev/") - 1));
    return open(devpath, flags);
}
#endif

static int tun_alloc(char *dev)
{
#ifdef __linux__
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    if (*dev != 0) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        close(fd);
        return err;
    }

    return fd;
#else
    char ifname[IFNAMSIZ];

    strncpy(ifname, dev, IFNAMSIZ);
    return devopen(ifname, O_RDWR);
#endif
}

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

static int _open_tcp(int argc, char **argv)
{
    struct addrinfo hints, *result, *rp;
    char *host = &argv[0][sizeof(TCP_SPEC) - 1];    /* remove TCP_SPEC */
    char *port = TCP_PORT_STR_DEFAULT;
    int res;

    if (argc > 1) {
        port = argv[1];
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    res = getaddrinfo(host, port, &hints, &result);
    if (res) {
        fprintf(stderr, "%s. ", gai_strerror(res));
        return -1;
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        res = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (res < 0) {
            continue;
        }

        if (connect(res, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }

        close(res);
    }

    freeaddrinfo(result);
    if (rp == NULL) {
        return -1;
    }
    return res;
}

static int _open_serial(int argc, char **argv)
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
    _set_blocking(fd, 1);
    return fd;
}

static int _open_stream(int argc, char **argv)
{
    if (strncmp(argv[0], TCP_SPEC, sizeof(TCP_SPEC) - 1) == 0) {
        return _open_tcp(argc, argv);
    }
    else {
        return _open_serial(argc, argv);
    }
}

static int _checked_write(int fd, const uint8_t *buffer, size_t buffer_size)
{
    while (buffer_size > 0) {
        ssize_t res = write(fd, buffer, buffer_size);
        if (res <= 0) {
            fprintf(stderr, "fd %d - ", fd);
            perror("write failed");
            return -1;
        }
        buffer_size -= res;
    }
    return 0;
}

static int _write_char(int fd, uint8_t c)
{
    return _checked_write(fd, &c, 1);
}

static void _write_escaped(int fd, const uint8_t *buffer, size_t buffer_size)
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
     * call write() on the buffer instead of one character at a time */
    uint8_t out[SERIAL_BUFFER_SIZE];
    size_t buffered = 0;

#define push(chr) \
    out[buffered++] = (chr); \
    if (buffered >= SERIAL_BUFFER_SIZE) { \
        _checked_write(fd, out, buffered); \
        buffered = 0; \
    }

    while (buffer_size--) {
        uint8_t c = *(buffer++);
        switch (c) {
            case SLIP_END:
                push(SLIP_ESC);
                push(SLIP_END_ESC);
                break;
            case SLIP_ESC:
                push(SLIP_ESC);
                push(SLIP_ESC_ESC);
                break;
            default:
                push(c);
                break;
        }
    }
#undef push
    out[buffered++] = SLIP_END;
    _checked_write(fd, out, buffered);
}

static void _flush_stream(FILE *f)
{
    for (int res = 0; (res != EOF) && (res != SLIP_END); res = fgetc(f)) {
        if (res == SLIP_START_STDIO) {
            ungetc(res, f);
            break;
        }
    }
}

static int _demultiplex_slip(FILE *stream, int iface_fd, int out_fd)
{
    FILE *target;
    int escaped = 0;
    int byte;

    if ((byte = fgetc(stream)) == EOF) {
        _flush_stream(stream);
        return -1;
    }
    if (byte == SLIP_START_STDIO) {
        if ((byte = fgetc(stream)) == EOF) {
            _flush_stream(stream);
            return -1;
        }
        target = fdopen(dup(out_fd), "w");
        /* don't print initial new line */
    }
    else {
        switch (byte & SLIP_START_NET_MASK) {
            case SLIP_START_NET_IPV4:
            case SLIP_START_NET_IPV6:
                target = fdopen(dup(iface_fd), "w");
                break;
            default:
                fprintf(stderr, "Unknown packet type 0x%02x\n", byte);
                _flush_stream(stream);
                return 0;
        }
    }
    while (byte != SLIP_END) {
        switch ((uint8_t)byte) {
            case SLIP_ESC:
                escaped = 1;
                break;
            case SLIP_END:
                fflush(target);
                fclose(target);
                return 0;
            case SLIP_END_ESC:
                if (escaped) {
                    byte = SLIP_END;
                    escaped = 0;
                }
                /* Intentionally falls through */
                /* to default when !escaped */
            case SLIP_ESC_ESC:
                if (escaped) {
                    byte = SLIP_ESC;
                    escaped = 0;
                }
                /* Intentionally falls through */
            default:
                fputc(byte, target);
                break;
        }
        if ((byte = fgetc(stream)) == EOF) {
            _flush_stream(stream);
            fclose(target);
            return -1;
        }
    }
    fclose(target);
    return 0;
}

static int _dispatch(int iface_fd, int stream_fd, int in_fd, int out_fd)
{
    static uint8_t buffer[TUN_MTU];
    fd_set readfds;
    int max_fd = (iface_fd > stream_fd)
               ? ((iface_fd > in_fd) ? iface_fd : in_fd)
               : ((stream_fd > in_fd) ? stream_fd : in_fd);
    int input_open = 1;
    int ret = 0;
    /* reasons for using FILE pointer for stream are two-fold:
     * - able to write back characters previously read (ungetc())
     * - fgetc() */
    FILE *stream = fdopen(stream_fd, "r");

    fprintf(stderr, "Starting dispatch. TUN: %d, Stream: %d, In: %d, Out: %d\n",
            iface_fd, stream_fd, in_fd, out_fd);
    while (1) {
        int res;

        FD_ZERO(&readfds);
        FD_SET(iface_fd, &readfds);
        FD_SET(stream_fd, &readfds);
        if (input_open) {
            FD_SET(in_fd, &readfds);
        }
        res = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if ((res < 0) && (errno != EINTR)) {
            perror("select");
        }

        if (FD_ISSET(stream_fd, &readfds)) {
            if (_demultiplex_slip(stream, iface_fd, out_fd) < 0) {
                fputs("Connection lost to serial.\n", stderr);
                ret = -1;
                close(stream_fd);
                break;
            }
        }
        if (FD_ISSET(iface_fd, &readfds)) {
            ssize_t res = read(iface_fd, buffer, sizeof(buffer));
            if (res <= 0) {
                perror("Error reading TUN interface");
                continue;
            }
            _write_escaped(stream_fd, buffer, res);
        }
        if (FD_ISSET(in_fd, &readfds)) {
            ssize_t res = read(in_fd, buffer, sizeof(buffer));
            if (res == 0) {
                /* handle EOF */
                fputs("EOF from input\n", stderr);
                if (isatty(in_fd)) {
                    /* EOF from terminal means we exit */
                    fputs("Exit by user.\n", stderr);
                    break;
                }
                else {
                    /* ignore EOF when input is not a terminal */
                    close(in_fd);
                    input_open = 0;
                }
                continue;
            }
            if (res < 0) {
                perror("Error reading from input");
                continue;
            }
            if (_write_char(stream_fd, SLIP_START_STDIO) < 0) {
                continue;
            }
            _write_escaped(stream_fd, buffer, res);
        }
    }
    fclose(stream);
    return ret;
}

int main(int argc, char **argv)
{
    int c;
    int stream_fd = -1;
    int iface_fd = -1;

    while ((c = getopt(argc, argv, OPTSTRING)) >= 0) {
        switch (c) {
            case 'h':
                _usage(argv[0]);
                return 0;
            case 'I':
                iface_fd = tun_alloc(optarg);
                if (iface_fd < 0) {
                    perror("Could not open TUN interface");
                    return 1;
                }
                break;
            case '?':
                switch (optopt) {
                    case 'I':
                        fprintf(stderr, "Option -%c requires an argument",
                                optopt);
                        break;
                    default:
                        if (isprint(optopt)) {
                            fprintf(stderr, "Unknown option '-%c'\n", optopt);
                        }
                        else {
                            fprintf(stderr, "Unknown option character \\%02x\n",
                                    optopt);
                        }
                        break;
                }
                _usage(argv[0]);
                return 1;
            default:
                abort();
        }
    }
    if ((argc - optind) < 1) {
        fputs("Serial or TCP connection required\n", stderr);
        _usage(argv[0]);
        return 1;
    }
    stream_fd = _open_stream(argc - optind, &argv[optind]);
    if (stream_fd < 0) {
        perror("Could not open serial or TCP connection");
        return 1;
    }
    if (iface_fd < 0) {
        iface_fd = tun_alloc(TUN_IFACE_DEFAULT);
        if (iface_fd < 0) {
            perror("Could not open TUN interface");
            return 1;
        }
    }
    while ((stream_fd < 0) ||
           (_dispatch(iface_fd, stream_fd, STDIN_FILENO, STDOUT_FILENO) < 0)) {
        fprintf(stderr, "Reconnecting %s ... ", argv[optind]);
        stream_fd = _open_stream(argc - optind, &argv[optind]);
        if (stream_fd < 0) {
            perror("");
        }
        else {
            fputs("Success\n", stderr);
        }
        sleep(2);
    }
    return 0;
}
