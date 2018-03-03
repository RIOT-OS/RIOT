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

#include "xtimer.h"
#include "shell.h"
#include "periph/spi.h"

/**
 * @brief   Some parameters used for benchmarking
 */
#define BENCH_REDOS             (1000)
#define BENCH_SMALL             (2)
#define BENCH_LARGE             (100)
#define BENCH_PAYLOAD           ('b')
#define BENCH_REGADDR           (0x23)

#define BUF_SIZE                (512U)

/**
 * @brief   Benchmark buffers
 */
static uint8_t bench_wbuf[BENCH_LARGE];
static uint8_t bench_rbuf[BENCH_LARGE];

/**
 * @brief   Generic buffer used for receiving
 */
static uint8_t buf[BUF_SIZE];

static struct {
    spi_t dev;
    spi_mode_t mode;
    spi_clk_t clk;
    spi_cs_t cs;
} spiconf;

void print_bytes(char* title, uint8_t* data, size_t len)
{
    printf("%4s\n", title);
    for (size_t i = 0; i < len; i++) {
        printf("  %2i ", (int)i);
    }
    printf("\n ");
    for (size_t i = 0; i < len; i++) {
        printf(" 0x%02x", (int)data[i]);
    }
    printf("\n ");
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

int cmd_init(int argc, char **argv)
{
    int dev, mode, clk, port, pin, tmp;

    if (argc < 5) {
        printf("usage: %s <dev> <mode> <clk> <cs port> <cs pin>\n", argv[0]);
        puts("\tdev:");
        for (int i = 0; i < (int)SPI_NUMOF; i++) {
            printf("\t\t%i: SPI_DEV(%i)\n", i, i);
        }
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
        puts("\tcs pin:");
        puts("\t\tPin used for chip select. If hardware chip select is enabled,\n"
             "\t\tthis value specifies the internal HWCS line");
        return 1;
    }

    /* parse the given SPI device */
    dev = atoi(argv[1]);
    if (dev < 0 || dev >= (int)SPI_NUMOF) {
        puts("error: invalid SPI device specified");
        return 1;
    }
    spiconf.dev = SPI_DEV(dev);

    /* parse the SPI mode */
    mode = atoi(argv[2]);
    switch (mode) {
        case 0: spiconf.mode = SPI_MODE_0; break;
        case 1: spiconf.mode = SPI_MODE_1; break;
        case 2: spiconf.mode = SPI_MODE_2; break;
        case 3: spiconf.mode = SPI_MODE_3; break;
        default:
            puts("error: invalid SPI mode specified");
            return 1;
    }

    /* parse the targeted clock speed */
    clk = atoi(argv[3]);
    switch (clk) {
        case 0: spiconf.clk = SPI_CLK_100KHZ; break;
        case 1: spiconf.clk = SPI_CLK_400KHZ; break;
        case 2: spiconf.clk = SPI_CLK_1MHZ;   break;
        case 3: spiconf.clk = SPI_CLK_5MHZ;   break;
        case 4: spiconf.clk = SPI_CLK_10MHZ;  break;
        default:
            puts("error: invalid bus speed specified");
            return 1;
    }

    /* parse chip select port and pin */
    port = atoi(argv[4]);
    pin = atoi(argv[5]);
    if (pin < 0 || port < -1) {
        puts("error: invalid CS port/pin combination specified");
    }
    if (port == -1) {                    /* hardware chip select line */
        spiconf.cs = SPI_HWCS(pin);
    }
    else {
        spiconf.cs = (spi_cs_t)GPIO_PIN(port, pin);
    }

    /* test setup */
    tmp = spi_init_cs(spiconf.dev, spiconf.cs);
    if (tmp != SPI_OK) {
        puts("error: unable to initialize the given chip select line");
        return 1;
    }
    tmp = spi_acquire(spiconf.dev, spiconf.cs, spiconf.mode, spiconf.clk);
    if (tmp == SPI_NOMODE) {
        puts("error: given SPI mode is not supported");
        return 1;
    }
    else if (tmp == SPI_NOCLK) {
        puts("error: targeted clock speed is not supported");
        return 1;
    }
    else if (tmp != SPI_OK) {
        puts("error: unable to acquire bus with given parameters");
        return 1;
    }
    spi_release(spiconf.dev);

    printf("SPI_DEV(%i) initialized: mode: %i, clk: %i, cs_port: %i, cs_pin: %i\n",
           dev, mode, clk, port, pin);

    return 0;
}

int cmd_transfer(int argc, char **argv)
{
    size_t len;

    if (argc < 2) {
        printf("usage: %s <data>\n", argv[0]);
        return 1;
    }

    if (spiconf.dev == SPI_UNDEF) {
        puts("error: SPI is not initialized, please initialize bus first");
        return 1;
    }

    /* get bus access */
    if (spi_acquire(spiconf.dev, spiconf.cs,
                    spiconf.mode, spiconf.clk) != SPI_OK) {
        puts("error: unable to acquire the SPI bus");
        return 1;
    }

    /* transfer data */
    len = strlen(argv[1]);
    memset(buf, 0, sizeof(buf));
    spi_transfer_bytes(spiconf.dev, spiconf.cs, false, argv[1], buf, len);

    /* release the bus */
    spi_release(spiconf.dev);

    /* print results */
    print_bytes("Sent bytes", (uint8_t *)argv[1], len);
    print_bytes("Received bytes", buf, len);

    return 0;
}

int cmd_bench(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t start, stop;
    uint32_t sum = 0;
    uint8_t in;
    uint8_t out = (uint8_t)BENCH_PAYLOAD;

    if (spiconf.dev == SPI_UNDEF) {
        puts("error: SPI is not initialized, please initialize bus first");
        return 1;
    }

    /* prepare buffer */
    memset(bench_wbuf, BENCH_PAYLOAD, BENCH_LARGE);

    /* get access to the bus */
    if (spi_acquire(spiconf.dev, spiconf.cs,
                    spiconf.mode, spiconf.clk) != SPI_OK) {
        puts("error: unable to acquire the SPI bus");
        return 1;
    }

    puts("### Running some benchmarks, all values in [us] ###\n");

    /* 1 - write 1000 times 1 byte */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        in = spi_transfer_byte(spiconf.dev, spiconf.cs, false, out);
        (void)in;
    }
    stop = xtimer_now_usec();
    printf(" 1 - write %i times %i byte:", BENCH_REDOS, 1);
    printf("\t\t\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 2 - write 1000 times 2 byte */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, NULL, BENCH_SMALL);
    }
    stop = xtimer_now_usec();
    printf(" 2 - write %i times %i byte:", BENCH_REDOS, BENCH_SMALL);
    printf("\t\t\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 3 - write 1000 times 100 byte */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, NULL, BENCH_LARGE);
    }
    stop = xtimer_now_usec();
    printf(" 3 - write %i times %i byte:", BENCH_REDOS, BENCH_LARGE);
    printf("\t\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 4 - write 1000 times 1 byte to register */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        in = spi_transfer_reg(spiconf.dev, spiconf.cs, BENCH_REGADDR, out);
        (void)in;
    }
    stop = xtimer_now_usec();
    printf(" 4 - write %i times %i byte to register:", BENCH_REDOS, 1);
    printf("\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 5 - write 1000 times 2 byte to register */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, NULL, BENCH_SMALL);
    }
    stop = xtimer_now_usec();
    printf(" 5 - write %i times %i byte to register:", BENCH_REDOS, BENCH_SMALL);
    printf("\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 6 - write 1000 times 100 byte to register */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, NULL, BENCH_LARGE);
    }
    stop = xtimer_now_usec();
    printf(" 6 - write %i times %i byte to register:", BENCH_REDOS, BENCH_LARGE);
    printf("\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 7 - read 1000 times 2 byte */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           NULL, bench_rbuf, BENCH_SMALL);
    }
    stop = xtimer_now_usec();
    printf(" 7 - read %i times %i byte:", BENCH_REDOS, BENCH_SMALL);
    printf("\t\t\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 8 - read 1000 times 100 byte */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           NULL, bench_rbuf, BENCH_LARGE);
    }
    stop = xtimer_now_usec();
    printf(" 8 - read %i times %i byte:", BENCH_REDOS, BENCH_LARGE);
    printf("\t\t\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 9 - read 1000 times 2 byte from register */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          NULL, bench_rbuf, BENCH_SMALL);
    }
    stop = xtimer_now_usec();
    printf(" 9 - read %i times %i byte from register:", BENCH_REDOS, BENCH_SMALL);
    printf("\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 10 - read 1000 times 100 byte from register */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          NULL, bench_rbuf, BENCH_LARGE);
    }
    stop = xtimer_now_usec();
    printf("10 - read %i times %i byte from register:", BENCH_REDOS, BENCH_LARGE);
    printf("\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 11 - transfer 1000 times 2 byte */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, bench_rbuf, BENCH_SMALL);
    }
    stop = xtimer_now_usec();
    printf("11 - transfer %i times %i byte:", BENCH_REDOS, BENCH_SMALL);
    printf("\t\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 12 - transfer 1000 times 100 byte */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, bench_rbuf, BENCH_LARGE);
    }
    stop = xtimer_now_usec();
    printf("12 - transfer %i times %i byte:", BENCH_REDOS, BENCH_LARGE);
    printf("\t\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 13 - transfer 1000 times 2 byte from/to register */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, bench_rbuf, BENCH_SMALL);
    }
    stop = xtimer_now_usec();
    printf("13 - transfer %i times %i byte to register:", BENCH_REDOS, BENCH_SMALL);
    printf("\t%i\n", (int)(stop - start));
    sum += (stop - start);

    /* 14 - transfer 1000 times 100 byte from/to register */
    start = xtimer_now_usec();
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, bench_rbuf, BENCH_LARGE);
    }
    stop = xtimer_now_usec();
    printf("14 - transfer %i times %i byte to register:", BENCH_REDOS, BENCH_LARGE);
    printf("\t%i\n", (int)(stop - start));
    sum += (stop - start);

    printf("-- - SUM:\t\t\t\t\t%i\n", (int)sum);

    spi_release(spiconf.dev);
    puts("\n### All runs complete ###");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Setup a particular SPI configuration", cmd_init },
    { "send", "Transfer string to slave", cmd_transfer },
    { "bench", "Runs some benchmarks", cmd_bench },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Manual SPI peripheral driver test");
    puts("Refer to the README.md file for more information.\n");

    printf("There are %i SPI devices configured for your platform.\n",
           (int)SPI_NUMOF);

    /* reset local SPI configuration */
    spiconf.dev = SPI_UNDEF;

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
