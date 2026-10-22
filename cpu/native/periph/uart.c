/*
 * SPDX-FileCopyrightText: 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup cpu_native
 * @ingroup drivers_periph_uart
 * @brief   UART implementation based on /dev/tty devices on host
 * @author  Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <poll.h>
#include <string.h>
#include <termios.h>

#include "thread.h"
#include "async_read.h"
#include "native_internal.h"
#include "periph/uart.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_STDIO_UART) && IS_ACTIVE(ENABLE_DEBUG)
#  error "stdio_uart can not be used when debugging the UART interface!"
#endif

/* Callback function and its argument */
static uart_isr_ctx_t uart_config[UART_NUMOF];

/* Filenames of /dev/tty */
static char *tty_device_filenames[UART_NUMOF];

/* File descriptors of /dev/tty, initialized to -1
 * Note: the syntax is a GNU extension, but LLVM supports it as well. */
static int tty_fds[UART_NUMOF] = { [0 ... UART_NUMOF - 1] = -1 };

/* When powering off the UART, we have to save the configuration. */
static struct termios _termios[UART_NUMOF];

void tty_uart_setup(uart_t uart, const char *filename)
{
    assert(uart < UART_NUMOF);

    tty_device_filenames[uart] = strndup(filename, PATH_MAX - 1);
}

static void io_signal_handler(int fd, void *arg)
{
    uart_t uart;
    (void)arg;

    for (uart = 0; uart < UART_NUMOF; uart++) {
        if (tty_fds[uart] == fd) {
            break;
        }
    }

    if (uart == UART_NUMOF) {
        DEBUG("[io_signal_handler] No matching UART for FD!\n");
        native_async_read_continue(fd);
        return;
    }

    int is_first = 1;

    while (1) {
        char c;
        int status = real_read(fd, &c, 1);

        if (status == 1) {
            if (is_first) {
                is_first = 0;
                DEBUG("[io_signal_handler] read char from serial port");
            }

            DEBUG(" %02x", (unsigned char) c);

            if (uart_config[uart].rx_cb != NULL) {
                uart_config[uart].rx_cb(uart_config[uart].arg, c);
            }
        }
        else {
            if (status == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
                DEBUG("[io_signal_handler] error: cannot read from serial port\n");

                uart_config[uart].rx_cb = NULL;
            }

            break;
        }
    }

    if (!is_first) {
        DEBUG("\n");
    }

    native_async_read_continue(fd);
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    if (tty_device_filenames[uart] == NULL) {
        DEBUG("[uart_init] Error: File name is NULL.\n");
        return UART_NODEV;
    }

    if (tty_fds[uart] >= 0) {
        DEBUG("[uart_init] Warning: UART already initialized, powering off.\n");
        uart_poweroff(uart);
    }

    memset(&_termios, 0, sizeof(_termios));

    _termios[uart].c_iflag = 0;
    _termios[uart].c_oflag = 0;
    _termios[uart].c_cflag = CS8 | CREAD | CLOCAL;
    _termios[uart].c_lflag = 0;

    speed_t speed;

    switch (baudrate) {
    case 0: speed = B0; break;
    case 50: speed = B50; break;
    case 75: speed = B75; break;
    case 110: speed = B110; break;
    case 134: speed = B134; break;
    case 150: speed = B150; break;
    case 200: speed = B200; break;
    case 300: speed = B300; break;
    case 600: speed = B600; break;
    case 1200: speed = B1200; break;
    case 1800: speed = B1800; break;
    case 2400: speed = B2400; break;
    case 4800: speed = B4800; break;
    case 9600: speed = B9600; break;
    case 19200: speed = B19200; break;
    case 38400: speed = B38400; break;
    case 57600: speed = B57600; break;
    case 115200: speed = B115200; break;
    case 230400: speed = B230400; break;
    case 460800: speed = B460800; break;
    case 500000: speed = B500000; break;
    case 576000: speed = B576000; break;
    case 921600: speed = B921600; break;
    case 1000000: speed = B1000000; break;
    case 1152000: speed = B1152000; break;
    case 1500000: speed = B1500000; break;
    case 2000000: speed = B2000000; break;
    case 2500000: speed = B2500000; break;
    case 3000000: speed = B3000000; break;
    case 3500000: speed = B3500000; break;
    case 4000000: speed = B4000000; break;
    default:
        DEBUG("[uart_init] Error: unsupported baudrate %" PRIu32 "\n", baudrate);
        return UART_NOBAUD;
    }

    cfsetospeed(&_termios[uart], speed);
    cfsetispeed(&_termios[uart], speed);

    tty_fds[uart] = real_open(tty_device_filenames[uart], O_RDWR | O_NONBLOCK);

    if (tty_fds[uart] < 0) {
        DEBUG("[uart_init] Error: could not open UART.\n");
        return UART_INTERR;
    }

    if (tcsetattr(tty_fds[uart], TCSANOW, &_termios[uart]) < 0) {
        DEBUG("[uart_init] Error: could not set terminal attributes\n");

        real_close(tty_fds[uart]);
        tty_fds[uart] = -1;

        return UART_INTERR;
    }

    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].arg = arg;

    native_async_read_setup();
    native_async_read_add_handler(tty_fds[uart], NULL, io_signal_handler);

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    DEBUG("[uart_write] writing to serial port ");

    for (size_t i = 0; i < len; i++) {
        DEBUG("%02x ", (unsigned char)data[i]);
    }
    for (size_t i = 0; i < len; i++) {
        DEBUG("%c", (char)data[i]);
    }

    DEBUG("\n");

    if (tty_fds[uart] < 0) {
        DEBUG("[uart_write] FD is closed %d!\n", tty_fds[uart]);
        return;
    }

    /* The file descriptor is non-blocking, so the write can be partial or
     * fail with EAGAIN when the TX buffer of the host is full. uart_write()
     * has to block until all data is written, so wait until there is room. */
    while (len > 0) {
        ssize_t ret = _native_write(tty_fds[uart], data, len);

        if (ret > 0) {
            data += ret;
            len -= ret;
            continue;
        }

        if (ret < 0 && errno != EAGAIN && errno != EINTR) {
            DEBUG("[uart_write] Error: write failed (errno %d)\n", errno);
            return;
        }

        struct pollfd pfd = { .fd = tty_fds[uart], .events = POLLOUT };

        _native_syscall_enter();
        real_poll(&pfd, 1, -1);
        _native_syscall_leave();
    }

    /* The data is only in the TX buffer of the host now, wait until it was
     * actually transmitted like a hardware UART would. */
    _native_syscall_enter();
    while ((tcdrain(tty_fds[uart]) < 0) && (errno == EINTR)) {}
    _native_syscall_leave();
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    if (tty_device_filenames[uart] == NULL) {
        DEBUG("[uart_poweron] Error: File name is NULL.\n");
        return;
    }

    if (tty_fds[uart] >= 0) {
        DEBUG("[uart_poweron] Warning: File Descriptor already open.\n");
        return;
    }

    if (cfgetospeed(&_termios[uart]) == B0) {
        DEBUG("[uart_poweron] Error: Initialize UART before power on!\n");
        return;
    }

    tty_fds[uart] = real_open(tty_device_filenames[uart], O_RDWR | O_NONBLOCK);

    if (tty_fds[uart] < 0) {
        DEBUG("[uart_poweron] Error: could not open UART.\n");
        return;
    }

    if (tcsetattr(tty_fds[uart], TCSANOW, &_termios[uart]) < 0) {
        DEBUG("[uart_poweron] Error: could not set terminal attributes.\n");

        real_close(tty_fds[uart]);
        tty_fds[uart] = -1;

        return;
    }

    native_async_read_add_handler(tty_fds[uart], NULL, io_signal_handler);
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    if (tty_fds[uart] >= 0) {
        native_async_read_remove_handler(tty_fds[uart]);
        real_close(tty_fds[uart]);
        tty_fds[uart] = -1;
    }
}
