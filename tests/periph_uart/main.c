/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Manual test application for UART peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "shell.h"
#include "thread.h"
#include "msg.h"
#include "ringbuffer.h"
#include "periph/uart.h"

#define SHELL_BUFSIZE       (128U)
#define UART_BUFSIZE        (128U)

#define PRINTER_PRIO        (THREAD_PRIORITY_MAIN - 1)
#define PRINTER_TYPE        (0xabcd)

#ifndef STDIO
#define STDIO               (UART_UNDEF)
#endif

typedef struct {
    char rx_mem[UART_BUFSIZE];
    char tx_mem[UART_BUFSIZE];
    ringbuffer_t rx_buf;
    ringbuffer_t tx_buf;
} uart_ctx_t;

static uart_ctx_t ctx[UART_NUMOF];

static kernel_pid_t printer_pid;
static char printer_stack[THREAD_STACKSIZE_MAIN];

static int parse_dev(char *arg)
{
    int dev = atoi(arg);
    if (dev == STDIO) {
        printf("Error: The selected UART_DEV(%i) is used for the shell!\n", dev);
        return -2;
    }
    if (dev < 0 || dev >= UART_NUMOF) {
        printf("Error: Invalid UART_DEV device specified (%i).\n", dev);
        return -1;
    }
    return dev;
}

static void rx_cb(void *arg, char data)
{
    int dev = (int)arg;

    ringbuffer_add_one(&(ctx[dev].rx_buf), data);
    if (data == '\n') {
        msg_t msg;
        msg.content.value = (uint32_t)dev;
        msg_send(&msg, printer_pid);
    }
}

static int tx_cb(void *arg)
{
    int dev = (int)arg;
    int c = ringbuffer_get_one(&(ctx[dev].tx_buf));
    if (c == -1) {
        return 0;
    }
    uart_write(UART_DEV(dev), (char)c);
    return 1;
}

static void *printer(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    while (1) {
        msg_receive(&msg);
        int dev = (int)msg.content.value;
        char c;

        printf("UART_DEV(%i) RX: ", dev);
        do {
            c = (int)ringbuffer_get_one(&(ctx[dev].rx_buf));
            if (c == '\n') {
                puts("");
            }
            else if (c >= ' ' && c <= '~') {
                printf("%c", c);
            }
            else {
                printf("0x%02x", (unsigned char)c);
            }
        } while (c != '\n');
    }

    /* this should never be reached */
    return NULL;
}

static int cmd_init(int argc, char **argv)
{
    int dev, res;
    uint32_t baud;

    if (argc < 3) {
        printf("usage: %s <dev> <baudrate>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }
    baud = (uint32_t)atoi(argv[2]);

    /* initialize UART */
    res = uart_init(UART_DEV(dev), baud, rx_cb, tx_cb, (void *)dev);
    if (res == -1) {
        printf("Error: Given baudrate (%u) not possible\n", (unsigned int)baud);
        return 1;
    }
    else if (res < -1) {
        puts("Error: Unable to initialize UART device\n");
        return 1;
    }
    printf("Successfully initialized UART_DEV(%i)\n", dev);
    return 0;
}

static int cmd_send(int argc, char **argv)
{
    int dev;
    char *s;

    if (argc < 3) {
        printf("usage: %s <dev> <data (string)>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }

    printf("UART_DEV(%i) TX: %s\n", dev, argv[2]);
    s = argv[2];
    while (*s) {
        ringbuffer_add_one(&(ctx[dev].tx_buf), *s);
        ++s;
    }
    ringbuffer_add_one(&(ctx[dev].tx_buf), '\n');
    uart_tx_begin(UART_DEV(dev));
    return 0;
}

static int cmd_send_blocking(int argc, char **argv)
{
    int dev;
    char *s;

    if (argc < 3) {
        printf("usage: %s <dev> <data (string)>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }

    printf("UART_DEV(%i) TX: %s\n", dev, argv[2]);
    s = argv[2];
    while (*s) {
        uart_write_blocking(UART_DEV(dev), *s);
        ++s;
    }
    uart_write_blocking(UART_DEV(dev), '\n');
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize a UART device with a given baudrate", cmd_init },
    { "send", "Send a string (non-blocking)", cmd_send },
    { "send_blocking", "Send a string (blocking)", cmd_send_blocking },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;

    puts("\nManual UART driver test application");
    puts("===================================");
    puts("This application is intended for testing additional UART\n"
         "interfaces, that might be defined for a board. The 'primary' UART\n"
         "interface is tested implicitly, as it is running the shell...\n");

    puts("UART INFO:");
    printf("Available devices:               %i\n", UART_NUMOF);
    printf("UART used for STDIO (the shell): UART_DEV(%i)\n\n", STDIO);

    /* initialize ringbuffers */
    for (int i = 0; i < UART_NUMOF; i++) {
        ringbuffer_init(&(ctx[i].rx_buf), ctx[i].rx_mem, UART_BUFSIZE);
        ringbuffer_init(&(ctx[i].tx_buf), ctx[i].tx_mem, UART_BUFSIZE);
    }

    /* start the printer thread */
    printer_pid = thread_create(printer_stack, sizeof(printer_stack),
                                PRINTER_PRIO, 0, printer, NULL, "printer");

    /* run the shell */
    shell_init(&shell, shell_commands, SHELL_BUFSIZE, getchar, putchar);
    shell_run(&shell);
    return 0;
}
