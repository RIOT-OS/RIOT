/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2023 Hugues Larrive
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "ps.h"
#include "periph/spi.h"

#define DEFAULT_SPI_CS_PORT 1
#define DEFAULT_SPI_CS_PIN 2

/**
 * @brief   Some parameters used for benchmarking
 */
#define BENCH_REDOS             (1000)
#define BENCH_SMALL             (2)
#define BENCH_LARGE             (100)
#define BENCH_PAYLOAD           ('b')
#define BENCH_REGADDR           (0x23)

/**
 * @brief   Benchmark buffers
 */
static uint8_t bench_wbuf[BENCH_LARGE];
static uint8_t bench_rbuf[BENCH_LARGE];

static struct {
    spi_t dev;
    spi_mode_t mode;
    spi_clk_t clk;
    spi_cs_t cs;
} spiconf;

int cmd_init(void)
{
    spiconf.dev = SPI_DEV(0);
    spiconf.mode = SPI_MODE_0;
    spiconf.clk = SPI_CLK_1MHZ;
    spiconf.cs = (spi_cs_t)GPIO_PIN(DEFAULT_SPI_CS_PORT, DEFAULT_SPI_CS_PIN);
    spi_init_cs(spiconf.dev, spiconf.cs);
    return 0;
}

int cmd_bench(void)
{
    uint8_t in;
    uint8_t out = (uint8_t)BENCH_PAYLOAD;

    if (spiconf.dev == SPI_UNDEF) {
        puts("error: SPI is not initialized, please initialize bus first");
        return 1;
    }

    /* prepare buffer */
    memset(bench_wbuf, BENCH_PAYLOAD, BENCH_LARGE);

    /* get access to the bus */
    spi_acquire(spiconf.dev, spiconf.cs, spiconf.mode, spiconf.clk);

    puts("### Running some benchmarks ###");

    /* 1 - write 1000 times 1 byte */
    for (int i = 0; i < BENCH_REDOS; i++) {
        in = spi_transfer_byte(spiconf.dev, spiconf.cs, false, out);
        (void)in;
    }
    printf(" 1 - write %i times %i byte\n", BENCH_REDOS, 1);

    /* 2 - write 1000 times 2 byte */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, NULL, BENCH_SMALL);
    }
    printf(" 2 - write %i times %i byte\n", BENCH_REDOS, BENCH_SMALL);

    /* 3 - write 1000 times 100 byte */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, NULL, BENCH_LARGE);
    }
    printf(" 3 - write %i times %i byte\n", BENCH_REDOS, BENCH_LARGE);

    /* 4 - write 1000 times 1 byte to register */
    for (int i = 0; i < BENCH_REDOS; i++) {
        in = spi_transfer_reg(spiconf.dev, spiconf.cs, BENCH_REGADDR, out);
        (void)in;
    }
    printf(" 4 - write %i times %i byte to register\n", BENCH_REDOS, 1);

    /* 5 - write 1000 times 2 byte to register */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, NULL, BENCH_SMALL);
    }
    printf(" 5 - write %i times %i byte to register\n", BENCH_REDOS, BENCH_SMALL);

    /* 6 - write 1000 times 100 byte to register */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, NULL, BENCH_LARGE);
    }
    printf(" 6 - write %i times %i byte to register\n", BENCH_REDOS, BENCH_LARGE);

    /* 7 - read 1000 times 2 byte */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           NULL, bench_rbuf, BENCH_SMALL);
    }
    printf(" 7 - read %i times %i byte\n", BENCH_REDOS, BENCH_SMALL);

    /* 8 - read 1000 times 100 byte */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           NULL, bench_rbuf, BENCH_LARGE);
    }
    printf(" 8 - read %i times %i byte\n", BENCH_REDOS, BENCH_LARGE);

    /* 9 - read 1000 times 2 byte from register */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          NULL, bench_rbuf, BENCH_SMALL);
    }
    printf(" 9 - read %i times %i byte from register\n", BENCH_REDOS, BENCH_SMALL);

    /* 10 - read 1000 times 100 byte from register */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          NULL, bench_rbuf, BENCH_LARGE);
    }
    printf("10 - read %i times %i byte from register\n", BENCH_REDOS, BENCH_LARGE);

    /* 11 - transfer 1000 times 2 byte */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, bench_rbuf, BENCH_SMALL);
    }
    printf("11 - transfer %i times %i byte\n", BENCH_REDOS, BENCH_SMALL);

    /* 12 - transfer 1000 times 100 byte */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, bench_rbuf, BENCH_LARGE);
    }
    printf("12 - transfer %i times %i byte\n", BENCH_REDOS, BENCH_LARGE);

    /* 13 - transfer 1000 times 2 byte from/to register */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, bench_rbuf, BENCH_SMALL);
    }
    printf("13 - transfer %i times %i byte to register\n", BENCH_REDOS, BENCH_SMALL);

    /* 14 - transfer 1000 times 100 byte from/to register */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, bench_rbuf, BENCH_LARGE);
    }
    printf("14 - transfer %i times %i byte to register\n", BENCH_REDOS, BENCH_LARGE);

    /* 15 - release & acquire the bus 1000 times */
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_release(spiconf.dev);
        spi_acquire(spiconf.dev, spiconf.cs, spiconf.mode, spiconf.clk);
    }
    printf("15 - acquire/release %i times\n", BENCH_REDOS);

    spi_release(spiconf.dev);
    puts("### All runs complete ###\n");

    return 0;
}

int main(void)
{
    puts("SPI peripheral driver test");

    printf("There are %i SPI devices configured for your platform.\n",
           (int)SPI_NUMOF);

    /* reset local SPI configuration */
    spiconf.dev = SPI_UNDEF;

    cmd_init();

    while (1) {
        cmd_bench();
    }

    return 0;
}
