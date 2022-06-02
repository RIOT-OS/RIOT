/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2022 Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/select.h>

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

/* Size of serial write buffer */
#define SERIAL_BUFFER_SIZE 64

static void usage(void)
{
    fprintf(stderr, "Usage: dose [-w][-b baudrate] <tap> <serial>\n");
}

static uint16_t crc16_update(uint16_t crc, uint8_t octet)
{
    crc = (uint8_t)(crc >> 8) | (crc << 8);
    crc ^= octet;
    crc ^= (uint8_t)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
    return crc;
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
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("error in tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

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

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("error from tcsetattr");
        return -1;
    }
    return 0;
}

void set_blocking(int fd, int should_block)
{
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("error from tggetattr");
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = TTY_TIMEOUT_MS / 100; /* 0.5 seconds read timeout*/
                                            /* in tenths of a second*/

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("error setting term attributes");
    }
}

/**
 * @brief Escape char definitions
 * @{
 */
#define LINE_FRAME_DELIMITER        (0xFF)
#define LINE_ESC_CHAR               (0xFE)
#define DOSE_FRAME_CRC_LEN          (2)
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
    uint8_t frame[MTU];
} serial_t;

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. The caller     *
 *            needs to reserve enough space in *dev.                      *
 **************************************************************************/
int tun_alloc(char *dev, int flags)
{
    struct ifreq ifr = { 0 };
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    ifr.ifr_flags = flags;

    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    return fd;
}

static int _handle_char(serial_t *serial, char c)
{
    if (serial->framebytes >= sizeof(serial->frame)) {
        TRACE("rx buffer overflow");
        serial->state = WAIT_FRAMESTART;
        return -1;
    }
    serial->frame[serial->framebytes] = c;
    serial->framebytes++;
    return 0;
}

static int _serial_handle_byte(serial_t *serial, uint8_t c)
{
    switch (serial->state) {
    case WAIT_FRAMESTART:
        TRACE("frame start");
        serial->state = IN_FRAME;
        serial->frametype = 0;
        serial->framebytes = 0;
        /* fall-through */
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
            return _handle_char(serial, c);
        }
        break;
    case IN_ESCAPE:
        serial->state = IN_FRAME;
        return _handle_char(serial, c);
    }

    return 0;
}

static uint16_t _write_escaped(int fd, void* buf, ssize_t n)
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
    size_t buffered = 0;
    uint8_t *_buf = buf;
    uint16_t crc = 0xffff;

    while (n--) {
        unsigned char c = *_buf++;
        crc = crc16_update(crc, c);
        if (c == LINE_FRAME_DELIMITER || c == LINE_ESC_CHAR) {
            out[buffered++] = LINE_ESC_CHAR;
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

    return htons(crc);
}

static int _parse_baudrate(const char *arg, unsigned *baudrate)
{
    if (arg == NULL) {
        *baudrate = BAUDRATE_DEFAULT;
        return 0;
    }

    switch (strtol(arg, NULL, 10)) {
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
        return -1;
    }

    return 0;
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

int main(int argc, char *argv[])
{
    uint8_t inbuf[MTU];
    char *port;
    char *serial_option = NULL;
    bool write_enabled = false;

    serial_t serial = { 0 };

    int c;
    while ((c = getopt(argc, argv, "b:w")) != -1) {
        switch (c) {
        case 'b':
            serial_option = optarg;
            break;
        case 'w':
            write_enabled = true;
            break;
        default:
            usage();
            return 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc < 2) {
        usage();
        return 1;
    }

    port = argv[1];

    char ifname[IFNAMSIZ] = { 0 };
    strncpy(ifname, argv[0], IFNAMSIZ - 1);

    int tap_fd = tun_alloc(ifname, IFF_TAP | IFF_NO_PI);
    if (tap_fd < 0) {
        return 1;
    }

    int serial_fd = _open_serial_connection(port, serial_option);
    if (serial_fd < 0) {
        fprintf(stderr, "Error opening serial device %s\n", argv[2]);
        return 1;
    }

    fd_set readfds;
    int max_fd = (serial_fd > tap_fd) ? serial_fd : tap_fd;

    int stdin_open = 1;
    while (1) {
        int activity;
        FD_ZERO(&readfds);
        if (stdin_open) {
            FD_SET(STDIN_FILENO, &readfds);
        }
        FD_SET(tap_fd, &readfds);
        FD_SET(serial_fd, &readfds);
        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select error");
        }

        if (FD_ISSET(serial_fd, &readfds)) {
            ssize_t n = read(serial_fd, inbuf, sizeof(inbuf));
            if (n < 0) {
                fprintf(stderr, "lost serial connection.\n");
                exit(1);
            }
            uint8_t *ptr = inbuf;
            while (n--) {
                size_t res = _serial_handle_byte(&serial, *ptr++);
                if (res) {
                    checked_write(tap_fd, serial.frame,
                                  serial.framebytes - DOSE_FRAME_CRC_LEN);
                    memset(&serial, '\0', sizeof(serial));
                }
            }
        }

        if (FD_ISSET(tap_fd, &readfds)) {
            ssize_t res = read(tap_fd, inbuf, sizeof(inbuf));
            if (res <= 0) {
                fprintf(stderr, "error reading from tap device. res=%zi\n", res);
                continue;
            }
            if (!write_enabled) {
                continue;
            }
            char delim = LINE_FRAME_DELIMITER;
            uint16_t crc = _write_escaped(serial_fd, inbuf, res);
            checked_write(serial_fd, &crc, sizeof(crc));
            checked_write(serial_fd, &delim, 1);
        }
    }

    return 0;
}
