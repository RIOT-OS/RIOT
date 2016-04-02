/*
 * Copyright (C) 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     native_cpu
 * @{
 *
 * @file
 * @brief       UART implementation based on /dev/tty devices on host
 *
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include "thread.h"
#include "periph/uart.h"
#include "native_internal.h"
#include "async_read.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief callback function and its argument
 */
static uart_isr_ctx_t uart_config[UART_NUMOF];

/**
 * @brief filenames of /dev/tty
 */
static char *tty_device_filenames[UART_NUMOF];

/**
 * @brief file descriptors of /dev/tty
 */
static int tty_fds[UART_NUMOF];

void tty_uart_setup(uart_t uart, const char *filename)
{
    tty_device_filenames[uart] = strndup(filename, PATH_MAX - 1);
}

static void io_signal_handler(int fd)
{
    uart_t uart;

    for (uart = 0; uart < UART_NUMOF; uart++) {
        if (tty_fds[uart] == fd) {
            break;
        }
    }

    int is_first = 1;

    while (1) {
        char c;
        int status = real_read(fd, &c, 1);

        if (status == 1) {
            if (is_first) {
                is_first = 0;
                DEBUG("read char from serial port");
            }

            DEBUG(" %02x", (unsigned char) c);

            uart_config[uart].rx_cb(uart_config[uart].arg, c);
        } else {
            if (status == -1 && errno != EAGAIN) {
                DEBUG("error: cannot read from serial port\n");

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
        return -1;
    }

    struct termios termios;

    memset(&termios, 0, sizeof(termios));

    termios.c_iflag = 0;
    termios.c_oflag = 0;
    termios.c_cflag = CS8 | CREAD | CLOCAL;
    termios.c_lflag = 0;

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
    case 230400: speed = B230400 ; break;
    default:
        return -1;
        break;
    }

    cfsetospeed(&termios, speed);
    cfsetispeed(&termios, speed);

    tty_fds[uart] = real_open(tty_device_filenames[uart], O_RDWR | O_NONBLOCK);

    if (tty_fds[uart] < 0) {
        return -3;
    }

    tcsetattr(tty_fds[uart], TCSANOW, &termios);

    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].arg = arg;

    native_async_read_setup();
    native_async_read_add_handler(tty_fds[uart], io_signal_handler);

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    DEBUG("writing to serial port ");

#if ENABLE_DEBUG
    for (size_t i = 0; i < len; i++) {
        DEBUG("%02x ", (unsigned char) data[i]);
    }
    for (size_t i = 0; i < len; i++) {
        DEBUG("%c", (char) data[i]);
    }
#endif

    DEBUG("\n");

    _native_write(tty_fds[uart], data, len);
}

void uart_cleanup(void) {
    native_async_read_cleanup();

    for (uart_t uart = 0; uart < UART_NUMOF; uart++) {
        if (uart_config[uart].rx_cb != NULL) {
            real_close(tty_fds[uart]);
        }
    }
}

/** @} */
