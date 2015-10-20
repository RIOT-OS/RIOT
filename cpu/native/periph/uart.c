/**
 * Beaglebone Black UART implementation
 *
 * Copyright (C) 2015 Kubos Corporation
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <termios.h>

#include "cpu.h"
#include "thread.h"

#include "native_internal.h"
#include "board_internal.h"
#include "periph_conf.h"
#include "periph/uart.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#if UART_NUMOF && defined(MODULE_NATIVE_UART)

typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
    int fd;
    struct termios orig_cfg;
} uart_conf_t;

static uart_conf_t uart_config[UART_NUMOF];

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void handle_uart_in(uart_t uart)
{
    char buf[42];
    int nread;

    DEBUG("handle_uart_in\n");

    nread = _native_read(uart_config[uart].fd, buf, sizeof(buf));
    if (nread == -1) {
        err(EXIT_FAILURE, "handle_uart_in(): read()");
    }
    else if (nread == 0) {
        /* end of file / socket closed */
        /*if (_native_uart_conn != 0) {
            if (_native_null_out_file != -1) {
                if (real_dup2(_native_null_out_file, STDOUT_FILENO) == -1) {
                    err(EXIT_FAILURE, "handle_uart_in: dup2(STDOUT_FILENO)");
                }
            }
            if (real_dup2(_native_null_in_pipe[0], STDIN_FILENO) == -1) {
                err(EXIT_FAILURE, "handle_uart_in: dup2(STDIN_FILENO)");
            }
            _native_uart_conn = 0;
            warnx("closed stdio");
        }
        else {
            errx(EXIT_FAILURE, "handle_uart_in: unhandled situation!");
        }*/
    }
    for (int pos = 0; pos < nread; pos++) {
        uart_config[uart].rx_cb(uart_config[uart].arg, buf[pos]);
    }
    //uart0_notify_thread();

    thread_yield();
}

void _native_handle_uart_input(void)
{
    unsigned int i = 0;
    for (; i < UART_NUMOF; i++) {
        if (FD_ISSET(uart_config[i].fd, &_native_rfds)) {
            handle_uart_in(i);
        }
        else {
            DEBUG("_native_handle_uart_input - nothing to do\n");
        }
    }
}

#ifdef MODULE_UART0
extern int _native_uart_sock;
#endif

int _native_set_uart_fds(void)
{
    int nfds = 0;
    unsigned int i = 0;

#ifdef MODULE_UART0
    FD_SET(STDIN_FILENO, &_native_rfds);
    nfds = STDIN_FILENO;

    if (_native_uart_sock != -1) {
        FD_SET(_native_uart_sock, &_native_rfds);
        nfds = MAX(_native_uart_sock, nfds);
    }
#endif

#ifdef MODULE_NATIVE_UART
    for (; i < UART_NUMOF; i++) {
        if (uart_config[i].fd != -1) {
            FD_SET(uart_config[i].fd, &_native_rfds);
            nfds = MAX(uart_config[i].fd, nfds);
        }
    }
#endif

    return nfds;
}

void _native_reset_termios(void)
{
    unsigned int i = 0;
    for (; i < UART_NUMOF; i++) {
        if (uart_config[i].fd == -1) {
            continue;
        }

        tcsetattr(uart_config[i].fd, TCSANOW, &(uart_config[i].orig_cfg));
        close(uart_config[i].fd);
    }
}

void _native_init_uarts(void)
{
    unsigned int i = 0;
    for (; i < UART_NUMOF; i++) {
        uart_config[i].fd = -1;
    }

    atexit(_native_reset_termios);
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    if (uart_init_blocking(uart, baudrate) < 0) {
        return -1;
    }

    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].tx_cb = tx_cb;
    uart_config[uart].arg = arg;
    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    char *path;
    struct termios new_cfg;
    int fd;

    uart_config[uart].fd = -1;

    switch (uart) {
        case 0: path = UART_0_PATH; break;
        case 1: path = UART_1_PATH; break;
        case 2: path = UART_2_PATH; break;
        case 3: path = UART_3_PATH; break;
        case 4: path = UART_4_PATH; break;
        default:
            return -1;
    }

    DEBUG("Init UART%d at %s, baud %d\n", uart, path, baudrate);
    _native_init_uart(uart, path);

    fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        perror(path);
        return -1;
    }

    uart_config[uart].fd = fd;

    tcgetattr(fd, &(uart_config[uart].orig_cfg));
    memcpy(&new_cfg, &(uart_config[uart].orig_cfg), sizeof(new_cfg));
    cfmakeraw(&new_cfg);

    new_cfg.c_cflag |= baudrate | CS8 | CLOCAL | CREAD;
    new_cfg.c_cflag &= ~(CRTSCTS | CSTOPB | PARENB | PARODD);
    new_cfg.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    new_cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG |
                         IEXTEN);

    new_cfg.c_iflag &= ~(INLCR | IGNCR | ICRNL | IGNBRK | INPCK | ISTRIP |
                         IXON | IXOFF | IXANY | IUCLC | PARMRK);

    new_cfg.c_cc[VMIN] = 0;
    new_cfg.c_cc[VTIME] = 0;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &new_cfg);

    // TODO tcsetattr(fd, TCSANOW, &oldti) to reset the termio when the process closes?
    // TODO close(fd) ?
    return 0;
}

#endif // UART_NUMOF
