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

#include "shell.h"
#include "periph/spi.h"

#define LINE_LIMIT              (10U)

#define BUF_SIZE                (256U)

typedef struct {
    spi_t dev;
    spi_mode_t mode;
    spi_clk_t clk;
    spi_cs_t cs;
} spi_line_t;

static spi_line_t config[LINE_LIMIT];

static uint8_t buf[BUF_SIZE];


static void print_config_help(const char *cmd)
{
    printf("usage: %s <config> <dev> <mode> <clk> <cs port> <cs pin>\n", cmd);
    puts("\tdev:");
    for (int i = 0; i < (int)SPI_NUMOF; i++) {
        printf("\t\t%i: SPI_DEV(%i)\n", i, i);
    }
    puts("\tconfig:");
    puts("\t\tConfiguration channel to save this to");
    puts("\tmode:");
    puts("\t\t0: POL:0, PHASE:0 - on first rising edge");
    puts("\t\t1: POL:0, PHASE:1 - on second rising edge");
    puts("\t\t2: POL:1, PHASE:0 - on first falling edge");
    puts("\t\t3: POL:1, PHASE:1 - on second falling edge");
    puts("\tclk:");
    puts("\t\t0: 100 KHz");
    puts("\t\t1: 400 KHz");
    puts("\t\t2: 1 MHz");
    puts("\t\t3: 5 MHz");
    puts("\t\t4: 10 MHz");
    puts("\tcs port:");
    puts("\t\tPort of the CS pin, set to -1 for hardware chip select");
    puts("\t\tcs pin:");
    puts("\t\tPin used for chip select. If hardware chip select is enabled,\n"
         "\t\tthis value specifies the internal HWCS line");
}

void print_bytes(char* title, uint8_t* data, size_t len)
{
    printf("%4s", title);
    for (size_t i = 0; i < len; i++) {
        printf("  %2i ", (int)i);
    }
    printf("\n    ");
    for (size_t i = 0; i < len; i++) {
        printf(" 0x%02x", (int)data[i]);
    }
    printf("\n    ");
    for (size_t i = 0; i < len; i++) {
        if (data[i] < ' ' || data[i] > '~') {
            printf("  ?? ");
        }
        else {
            printf("   %c ", (char)data[i]);
        }
    }
    printf("\n\n");
}

int cmd_config(int argc, char **argv)
{
    int line, tmp, ump;

    if (argc < 7) {
        print_config_help(argv[0]);
        return 1;
    }

    line = atoi(argv[1]);
    if (line < 0 || line >= LINE_LIMIT) {
        puts("error: invalid configuration channel");
        return 1;
    }

    tmp = atoi(argv[3]);                /* parse SPI mode */
    switch (tmp) {
        case 0: config[line].mode = SPI_MODE_0; break;
        case 1: config[line].mode = SPI_MODE_1; break;
        case 2: config[line].mode = SPI_MODE_2; break;
        case 3: config[line].mode = SPI_MODE_3; break;
        default:
            puts("error: invalid SPI mode specified");
            return 1;
    }

    tmp = atoi(argv[4]);                /* parse bus clock speed */
    switch (tmp) {
        case 0: config[line].clk = SPI_CLK_100KHZ; break;
        case 1: config[line].clk = SPI_CLK_400KHZ; break;
        case 2: config[line].clk = SPI_CLK_1MHZ;   break;
        case 3: config[line].clk = SPI_CLK_5MHZ;   break;
        case 4: config[line].clk = SPI_CLK_10MHZ;  break;
        default:
            puts("error: invalid bus speed specified");
            return 1;
    }

    tmp = atoi(argv[5]);
    ump = atoi(argv[6]);
    if (ump < 0 || tmp < -1) {
        puts("error: invalid port/pin combination specified");
    }
    if (tmp == -1) {                    /* hardware chip select line */
        config[line].cs = SPI_HWCS(ump);
    }
    else {
        config[line].cs = (spi_cs_t)GPIO_PIN(tmp, ump);
    }

    tmp = atoi(argv[2]);                /* parse SPI dev */
    if (tmp < 0 || tmp >= SPI_NUMOF) {
        puts("error: invalid SPI device specified");
        return 1;
    }
    config[line].dev = SPI_DEV(tmp);

    if (spi_init_pins(config[line].dev, config[line].cs) < 0) {
        puts("error: unable to initialize the given SPI device");
        config[line].dev = SPI_UNDEF;
        return 1;
    }

    return 0;
}

int cmd_transfer(int argc, char **argv)
{
    int line;
    size_t len;

    if (argc < 2) {
        printf("usage: %s <config> <data>\n", argv[0]);
    }

    line = atoi(argv[1]);
    if (line < 0 || line >= LINE_LIMIT) {
        puts("error: invalid configuration channel specified");
        return 1;
    }
    if (config[line].dev == SPI_UNDEF) {
        puts("error: the given configuration channel is not initialized");
        return 1;
    }

    /* get bus access */
    if (spi_acquire(config[line].dev, config[line].mode,
                    config[line].clk, config[line].cs) < 0) {
        puts("error: unable to acquire the bus with the given configuration");
    }

    /* transfer data */
    len = strlen(argv[2]);
    spi_transfer_bytes(config[line].dev, config[line].cs, false,
                       (uint8_t *)argv[2], buf, len);

    /* release the bus */
    spi_release(config[line].dev);

    /* print results */
    print_bytes("Send bytes", (uint8_t *)argv[2], len);
    print_bytes("Received bytes", buf, len);

    return 0;
}


static const shell_command_t shell_commands[] = {
    { "config", "Setup a particular SPI configuration", cmd_config },
    { "send", "Transfer string to slave (only in master mode)", cmd_transfer },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* un-initialize the configuration lines */
    for (size_t line = 0; line < LINE_LIMIT; line++) {
        config[line].dev = SPI_UNDEF;
    }

    puts("\nRIOT low-level SPI driver test");
    puts("This application enables you to test a platforms SPI driver \n"
         "implementation. Type 'help' to show the available commands.\n");

    printf("There are %i SPI devices configured for your platform:\n",
           (int)SPI_NUMOF);

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
