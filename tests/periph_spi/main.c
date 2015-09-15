/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Application for testing low-level SPI driver implementations
 *
 * This implementation covers both, master and slave configurations.
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
#include "periph/spi.h"
#include "periph/gpio.h"

enum {
    READ = 0,
    WRITE,
    INIT
} rw;

static int spi_dev = -1;
static gpio_t spi_cs = -1;
static int spi_mode = -1;
static int spi_speed = -1;
static int spi_master = -1;     /* 0 for slave, 1 for master, -1 for not initialized */

static char buffer[256];       /* temporary buffer */
static char rx_buffer[256];    /* global receive buffer */
static int rx_counter = 0;

static volatile int state;
static char* mem = "Hello Master! abcdefghijklmnopqrstuvwxyz 0123456789 "
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int parse_spi_dev(int argc, char **argv)
{
    /* reset default values */
    int port, pin;
    spi_dev = SPI_0;
    spi_mode = SPI_CONF_FIRST_RISING;
    spi_speed = SPI_SPEED_1MHZ;

    if (argc < 4) {
        printf("usage: %s <dev> <cs port> <cs pin> [mode [speed]]\n", argv[0]);
        puts("        DEV is the SPI device to use:");
        for (int i = 0; i < SPI_NUMOF; i++) {
            printf("             %i - SPI_%i\n", i, i);
        }
        puts("        cs port:  port to use as the chip select line");
        puts("        cs pin:   pin to use on th given port as cs line");
        puts("        mode: must be one of the following options (* marks "
                      "default value):");
        puts("            *0 - POL:0, PHASE:0 - ON FIRST RISING EDGE");
        puts("             1 - POL:0, PHASE:1 - ON SECOND RISING EDGE");
        puts("             2 - POL:1, PHASE:0 - ON FIRST FALLING EDGE");
        puts("             3 - POL:1, PHASE:1 - on second falling edge");
        puts("        speed: must be one of the following options (only used "
                      "in master mode):");
        puts("             0 - 100 KHz");
        puts("             1 - 400 KHz");
        puts("            *2 - 1 MHz");
        puts("             3 - 5 MHz");
        puts("             4 - 10 MHz\n");
        return -4;
    }
    spi_dev = atoi(argv[1]);
    if (spi_dev < 0 || spi_dev >= SPI_NUMOF) {
        puts("error: invalid DEV value given");
        return -1;
    }
    port = atoi(argv[2]);
    pin = atoi(argv[3]);
    spi_cs = GPIO_PIN(port,pin);
    if (argc >= 5) {
        spi_mode = argv[4][0] - '0';
        if (spi_mode < 0 || spi_mode > 3) {
            puts("error: invalid MODE value given");
            return -2;
        }
    }
    if (argc >= 6) {
        spi_speed = argv[5][0] - '0';
        if (spi_speed < 0 || spi_speed > 4) {
            puts("error: invalid SPEED value given");
            return -3;
        }
    }
    return 0;
}

void print_bytes(char* title, char* chars, int length)
{
    printf("%4s", title);
    for (int i = 0; i < length; i++) {
        printf("  %2i ", i);
    }
    printf("\n    ");
    for (int i = 0; i < length; i++) {
        printf(" 0x%02x", (int)chars[i]);
    }
    printf("\n    ");
    for (int i = 0; i < length; i++) {
        if (chars[i] < ' ' || chars[i] > '~') {
            printf("  ?? ");
        }
        else {
            printf("   %c ", chars[i]);
        }
    }
    printf("\n\n");
}

void slave_on_cs(void *arg)
{
    (void)arg;

    LED_RED_ON;
    spi_transmission_begin(spi_dev, 'F');
    state = 0;
    rw = INIT;
    LED_RED_OFF;
}

char slave_on_data(char data)
{
    rx_buffer[rx_counter] = data;
    rx_counter++;
    if (rx_counter >= 256) {
        rx_counter = 0;
    }

    switch (rw) {
        case READ:
            return mem[state++];
        case WRITE:
            mem[state++] = data;
            return 'o';
        case INIT:
            if (data == ' ') {
                rw = READ;
                return mem[state++];
            } else if (data & 0x80) {
                rw = WRITE;
                state = (data & 0x7f);
                return 'W';
            } else {
                rw = READ;
                state = data;
                return mem[state++];
            }
    }
    return 'e';
}

int cmd_init_master(int argc, char **argv)
{
    int res;
    spi_master = -1;
    if (parse_spi_dev(argc, argv) < 0) {
        return 1;
    }
    spi_acquire(spi_dev);
    res = spi_init_master(spi_dev, spi_mode, spi_speed);
    spi_release(spi_dev);
    if (res < 0) {
        printf("spi_init_master: error initializing SPI_%i device (code %i)\n",
                spi_dev, res);
        return 1;
    }
    res = gpio_init(spi_cs, GPIO_DIR_OUT, GPIO_PULLUP);
    if (res < 0){
        printf("gpio_init: error initializing GPIO_%ld as CS line (code %i)\n",
                (long)spi_cs, res);
        return 1;
    }
    gpio_set(spi_cs);
    spi_master = 1;
    printf("SPI_%i successfully initialized as master, cs: GPIO_%ld, mode: %i, speed: %i\n",
            spi_dev, (long)spi_cs, spi_mode, spi_speed);
    return 0;
}

int cmd_init_slave(int argc, char **argv)
{
    int res;
    spi_master = -1;
    if (parse_spi_dev(argc, argv) < 0) {
        return 1;
    }
    spi_acquire(spi_dev);
    res = spi_init_slave(spi_dev, spi_mode, slave_on_data);
    spi_release(spi_dev);
    if (res < 0) {
        printf("spi_init_slave: error initializing SPI_%i device (code: %i)\n",
                spi_dev, res);
        return 1;
    }
    res = gpio_init_int(spi_cs, GPIO_NOPULL, GPIO_FALLING, slave_on_cs, 0);
    if (res < 0){
        printf("gpio_init_int: error initializing GPIO_%ld as CS line (code %i)\n",
                (long)spi_cs, res);
        return 1;
    }
    spi_master = 0;
    printf("SPI_%i successfully initialized as slave, cs: GPIO_%ld, mode: %i\n",
            spi_dev, (long)spi_cs, spi_mode);
    return 0;
}

int cmd_transfer(int argc, char **argv)
{
    int res;
    char *hello = "Hello";

    if (spi_master != 1) {
        puts("error: node is not initialized as master, please do so first");
        return 1;
    }

    if (argc < 2) {
        puts("No data to transfer given, will transfer 'Hello' to device");
    }
    else {
        hello = argv[1];
    }

    /* do the actual data transfer */
    spi_acquire(spi_dev);
    gpio_clear(spi_cs);
    res = spi_transfer_bytes(spi_dev, hello, buffer, strlen(hello));
    gpio_set(spi_cs);
    spi_release(spi_dev);

    /* look at the results */
    if (res < 0) {
        printf("error: unable to transfer data to slave (code: %i)\n", res);
        return 1;
    }
    else {
        printf("Transfered %i bytes:\n", res);
        print_bytes("MOSI", hello, res);
        print_bytes("MISO", buffer, res);
        return 0;
    }
}

int cmd_print(int argc, char **argv)
{
    if (spi_master != 0) {
        puts("error: node is not initialized as slave");
        return 1;
    }
    else {
        printf("Received %i bytes:\n", rx_counter);
        print_bytes("MOSI", rx_buffer, rx_counter);
    }
    rx_counter = 0;
    memset(&rx_buffer, 0, 256);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_master", "Initialize node as SPI master", cmd_init_master },
    { "init_slave", "Initialize node as SPI slave", cmd_init_slave },
    { "send", "Transfer string to slave (only in master mode)", cmd_transfer },
    { "print_rx", "Print the received string (only in slave mode)", cmd_print },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nRIOT low-level SPI driver test");
    puts("This application enables you to test a platforms SPI driver implementation.");
    puts("Enter 'help' to get started\n");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
