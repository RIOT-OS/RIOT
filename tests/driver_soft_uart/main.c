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
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "shell.h"
#include "thread.h"
#include "msg.h"
#include "ringbuffer.h"
#include "soft_uart.h"
#include "soft_uart_params.h"

#ifndef SHELL_BUFSIZE
#define SHELL_BUFSIZE       (128U)
#endif
#ifndef UART_BUFSIZE
#define UART_BUFSIZE        (128U)
#endif

#define PRINTER_PRIO        (THREAD_PRIORITY_MAIN - 1)
#define PRINTER_TYPE        (0xabcd)

typedef struct {
    char rx_mem[UART_BUFSIZE];
    ringbuffer_t rx_buf;
} uart_ctx_t;

static uart_ctx_t ctx[SOFT_UART_NUMOF];

static kernel_pid_t printer_pid;
static char printer_stack[THREAD_STACKSIZE_MAIN];

#ifdef MODULE_SOFT_UART_MODECFG
static uart_data_bits_t data_bits_lut[] = { UART_DATA_BITS_5, UART_DATA_BITS_6,
                                            UART_DATA_BITS_7, UART_DATA_BITS_8 };
static int data_bits_lut_len = ARRAY_SIZE(data_bits_lut);

static uart_stop_bits_t stop_bits_lut[] = { UART_STOP_BITS_1, UART_STOP_BITS_2 };
static int stop_bits_lut_len = ARRAY_SIZE(stop_bits_lut);
#endif

static int parse_dev(char *arg)
{
    unsigned dev = atoi(arg);
    if (dev >= SOFT_UART_NUMOF) {
        printf("Error: Invalid UART_DEV device specified (%u).\n", dev);
        return -1;
    }

    return dev;
}

static void rx_cb(void *arg, uint8_t data)
{
    uart_t dev = (uart_t)arg;

    ringbuffer_add_one(&(ctx[dev].rx_buf), data);
    if (data == '\n' || ringbuffer_full(&(ctx[dev].rx_buf))) {
        msg_t msg;
        msg.content.value = (uint32_t)dev;
        msg_send(&msg, printer_pid);
    }
}

static void *printer(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    while (1) {
        msg_receive(&msg);
        uart_t dev = (uart_t)msg.content.value;
        int c;

        printf("Success: UART_DEV(%i) RX: \n[", dev);
        do {
            c = ringbuffer_get_one(&(ctx[dev].rx_buf));
            if (c == '\n') {
                puts("]\\n");
            }
            else if (c >= ' ' && c <= '~') {
                printf("%c", c);
            }
            else {
                printf("\\x%02x", (unsigned char)c);
            }
        } while (c >= 0 && c != '\n');
        puts("");
    }

    /* this should never be reached */
    return NULL;
}

static void sleep_test(int num, uart_t uart)
{
    printf("UARD_DEV(%i): test uart_poweron() and uart_poweroff()  ->  ", num);
    soft_uart_poweroff(uart);
    soft_uart_poweron(uart);
    puts("[OK]");
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
    baud = strtol(argv[2], NULL, 0);

    /* initialize UART */
    res = soft_uart_init(dev, baud, rx_cb, (void *)dev);
    if (res == UART_NOBAUD) {
        printf("Error: Given baudrate (%u) not possible\n", (unsigned int)baud);
        return 1;
    }
    else if (res != UART_OK) {
        puts("Error: Unable to initialize UART device");
        return 1;
    }
    printf("Success: Initialized UART_DEV(%i) at BAUD %"PRIu32"\n", dev, baud);

    /* also test if poweron() and poweroff() work (or at least don't break
     * anything) */
    sleep_test(dev, UART_DEV(dev));

    return 0;
}

#ifdef MODULE_SOFT_UART_MODECFG
static int cmd_mode(int argc, char **argv)
{
    int dev, data_bits_arg, stop_bits_arg;
    uart_data_bits_t data_bits;
    uart_parity_t  parity;
    uart_stop_bits_t  stop_bits;

    if (argc < 5) {
        printf("usage: %s <dev> <data bits> <parity> <stop bits>\n", argv[0]);
        return 1;
    }

    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }

    data_bits_arg = atoi(argv[2]) - 5;
    if (data_bits_arg >= 0 && data_bits_arg < data_bits_lut_len) {
        data_bits = data_bits_lut[data_bits_arg];
    }
    else {
        printf("Error: Invalid number of data_bits (%i).\n", data_bits_arg + 5);
        return 1;
    }

    argv[3][0] &= ~0x20;
    switch (argv[3][0]) {
        case 'N':
            parity = UART_PARITY_NONE;
            break;
        case 'E':
            parity = UART_PARITY_EVEN;
            break;
        case 'O':
            parity = UART_PARITY_ODD;
            break;
        case 'M':
            parity = UART_PARITY_MARK;
            break;
        case 'S':
            parity = UART_PARITY_SPACE;
            break;
        default:
            printf("Error: Invalid parity (%c).\n", argv[3][0]);
            return 1;
    }

    stop_bits_arg = atoi(argv[4]) - 1;
    if (stop_bits_arg >= 0 && stop_bits_arg < stop_bits_lut_len) {
        stop_bits = stop_bits_lut[stop_bits_arg];
    }
    else {
        printf("Error: Invalid number of stop bits (%i).\n", stop_bits_arg + 1);
        return 1;
    }

    if (soft_uart_mode(dev, data_bits, parity, stop_bits) != UART_OK) {
        puts("Error: Unable to apply UART settings");
        return 1;
    }
    printf("Success: Successfully applied UART_DEV(%i) settings\n", dev);

    return 0;
}
#endif /* MODULE_PERIPH_UART_MODECFG */

static int cmd_send(int argc, char **argv)
{
    int dev;
    uint8_t endline = (uint8_t)'\n';

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
    soft_uart_write(dev, (uint8_t *)argv[2], strlen(argv[2]));
    soft_uart_write(dev, &endline, 1);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize a UART device with a given baudrate", cmd_init },
#ifdef MODULE_SOFT_UART_MODECFG
    { "mode", "Setup data bits, stop bits and parity for a given UART device", cmd_mode },
#endif
    { "send", "Send a string through given UART device", cmd_send },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nManual UART driver test application");
    puts("===================================");
    puts("When receiving data on one of the software UART interfaces, this\n"
         "data will be outputted via STDIO.\n"
         "Unfortunately full-duplex operation is not possible, so you can't\n"
         "simply connect RX and TX in loopback mode, but have to connect an\n"
         "external UART interface (e.g. USB-UART adapter).\n"
         "Then you can send data on that interface and you should see the data\n"
         "being printed to STDOUT\n\n"
         "NOTE: all strings need to be '\\n' terminated!\n");

    puts("\nUART INFO:");
    printf("Available devices:               %i\n", SOFT_UART_NUMOF);

    /* initialize ringbuffers */
    for (unsigned i = 0; i < SOFT_UART_NUMOF; i++) {
        ringbuffer_init(&(ctx[i].rx_buf), ctx[i].rx_mem, UART_BUFSIZE);
    }

    /* start the printer thread */
    printer_pid = thread_create(printer_stack, sizeof(printer_stack),
                                PRINTER_PRIO, 0, printer, NULL, "printer");

    /* run the shell */
    char line_buf[SHELL_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_BUFSIZE);
    return 0;
}
