/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "ztimer.h"
#include "shell.h"
#include "periph/spi.h"
#include "schedstatistics.h"
#include "thread.h"

/**
 * @brief   Default port number used for the CS pin if unassigned
 */
#ifndef DEFAULT_SPI_CS_PORT
#define DEFAULT_SPI_CS_PORT 0
#endif

/**
 * @brief   Default port number used for the CS pin if unassigned
 */
#ifndef DEFAULT_SPI_CS_PIN
#define DEFAULT_SPI_CS_PIN 0
#endif

#ifndef ENABLE_BENCHMARK
#define ENABLE_BENCHMARK 0
#endif

#ifndef LOW_MEMORY
#define LOW_MEMORY 0
#endif

#define BUF_SIZE                (128U)

#if ENABLE_BENCHMARK
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

extern void sched_statistics_cb(kernel_pid_t active_thread, kernel_pid_t next_thread);
#endif

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

#if ENABLE_BENCHMARK
/*
 * @brief Trigger an update of the scheduler runtime statistics.
 *
 * Increases the number of context switches by one as a side effect
 */
static void _sched_statistics_trigger(void)
{
    sched_statistics_cb(thread_getpid(), thread_getpid());
}

static uint32_t _sched_us(void)
{
    _sched_statistics_trigger();
    return sched_pidlist[thread_getpid()].runtime_us;
}

static uint32_t _ztimer_diff_usec(uint32_t stop, uint32_t start)
{
    return stop - start;
}
#endif

static void print_bytes(const char *title, const uint8_t *data, size_t len)
{
    printf("%4s\n", title);
    for (size_t i = 0; i < len; i++) {
        printf("  %2" PRIuSIZE " ", i);
    }
    printf("\n ");
    for (size_t i = 0; i < len; i++) {
        printf(" 0x%02x", data[i]);
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

static int cmd_init(int argc, char **argv)
{
    int dev, mode, clk, port, pin, tmp;

    if (argc < 4) {
        printf("usage: %s <dev> <mode> <clk> [cs port] [cs pin]\n", argv[0]);
        puts("\tdev:");
        for (int i = 0; i < (int)SPI_NUMOF; i++) {
            printf("\t\t%i: SPI_DEV(%i)\n", i, i);
        }
        if (!LOW_MEMORY) {
            puts("\tmode:\n"
                 "\t\t0: POL:0, PHASE:0 - on first rising edge\n"
                 "\t\t1: POL:0, PHASE:1 - on second rising edge\n"
                 "\t\t2: POL:1, PHASE:0 - on first falling edge\n"
                 "\t\t3: POL:1, PHASE:1 - on second falling edge\n"
                 "\tclk:\n"
                 "\t\t0: 100 KHz\n"
                 "\t\t1: 400 KHz\n"
                 "\t\t2: 1 MHz\n"
                 "\t\t3: 5 MHz\n"
                 "\t\t4: 10 MHz\n"
                 "\tcs port:\n"
                 "\t\tPort of the CS pin, set to -1 for hardware chip select\n"
                 "\tcs pin:\n"
                 "\t\tPin used for chip select. If hardware chip select is enabled,\n"
                 "\t\tthis value specifies the internal HWCS line");
        }
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
    if (argc > 5) {
        pin = atoi(argv[5]);
    }
    else {
        pin = DEFAULT_SPI_CS_PIN;
    }

    if (argc > 4) {
        port = atoi(argv[4]);
    }
    else {
        port = DEFAULT_SPI_CS_PORT;
    }

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
    if (IS_ACTIVE(NDEBUG)) {
        puts("cannot test configuration without assert()ions enabled");
    }
    else {
        printf("Trying to initialize SPI_DEV(%i): mode: %i, clk: %i, cs_port: %i, cs_pin: %i\n",
               dev, mode, clk, port, pin);
        puts("(if below the program crashes with a failed assertion, then it means the"
             " configuration is not supported)");
        spi_acquire(spiconf.dev, spiconf.cs, spiconf.mode, spiconf.clk);
        spi_release(spiconf.dev);
        puts("Success.");
    }

    return 0;
}
SHELL_COMMAND(init, "Setup a particular SPI configuration", cmd_init);

static int cmd_transfer(int argc, char **argv)
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
    spi_acquire(spiconf.dev, spiconf.cs, spiconf.mode, spiconf.clk);

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
SHELL_COMMAND(send, "Transfer string to slave", cmd_transfer);

#if ENABLE_BENCHMARK
int cmd_bench(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t start, stop;
    uint32_t sched_start, sched_stop;
    uint32_t sched_diff_us;
    uint32_t sum = 0;
    uint32_t sched_sum = 0;
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

    puts("### Running some benchmarks, all values in [us] ###");
    puts("### Test\t\t\t\tTransfer time\tuser time\n");

    /* 1 - write 1000 times 1 byte */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        in = spi_transfer_byte(spiconf.dev, spiconf.cs, false, out);
        (void)in;
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 1 - write %i times %i byte:", BENCH_REDOS, 1);
    printf("\t\t\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 2 - write 1000 times 2 byte */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, NULL, BENCH_SMALL);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 2 - write %i times %i byte:", BENCH_REDOS, BENCH_SMALL);
    printf("\t\t\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 3 - write 1000 times 100 byte */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, NULL, BENCH_LARGE);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 3 - write %i times %i byte:", BENCH_REDOS, BENCH_LARGE);
    printf("\t\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 4 - write 1000 times 1 byte to register */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        in = spi_transfer_reg(spiconf.dev, spiconf.cs, BENCH_REGADDR, out);
        (void)in;
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 4 - write %i times %i byte to register:", BENCH_REDOS, 1);
    printf("\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 5 - write 1000 times 2 byte to register */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, NULL, BENCH_SMALL);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 5 - write %i times %i byte to register:", BENCH_REDOS, BENCH_SMALL);
    printf("\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 6 - write 1000 times 100 byte to register */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, NULL, BENCH_LARGE);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 6 - write %i times %i byte to register:", BENCH_REDOS, BENCH_LARGE);
    printf("\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 7 - read 1000 times 2 byte */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           NULL, bench_rbuf, BENCH_SMALL);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 7 - read %i times %i byte:", BENCH_REDOS, BENCH_SMALL);
    printf("\t\t\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 8 - read 1000 times 100 byte */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           NULL, bench_rbuf, BENCH_LARGE);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 8 - read %i times %i byte:", BENCH_REDOS, BENCH_LARGE);
    printf("\t\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 9 - read 1000 times 2 byte from register */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          NULL, bench_rbuf, BENCH_SMALL);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf(" 9 - read %i times %i byte from register:", BENCH_REDOS, BENCH_SMALL);
    printf("\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 10 - read 1000 times 100 byte from register */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          NULL, bench_rbuf, BENCH_LARGE);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf("10 - read %i times %i byte from register:", BENCH_REDOS, BENCH_LARGE);
    printf("\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 11 - transfer 1000 times 2 byte */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, bench_rbuf, BENCH_SMALL);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf("11 - transfer %i times %i byte:", BENCH_REDOS, BENCH_SMALL);
    printf("\t\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 12 - transfer 1000 times 100 byte */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_bytes(spiconf.dev, spiconf.cs, false,
                           bench_wbuf, bench_rbuf, BENCH_LARGE);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf("12 - transfer %i times %i byte:", BENCH_REDOS, BENCH_LARGE);
    printf("\t\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 13 - transfer 1000 times 2 byte from/to register */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, bench_rbuf, BENCH_SMALL);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf("13 - transfer %i times %i byte to register:", BENCH_REDOS, BENCH_SMALL);
    printf("\t%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 14 - transfer 1000 times 100 byte from/to register */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_transfer_regs(spiconf.dev, spiconf.cs, BENCH_REGADDR,
                          bench_wbuf, bench_rbuf, BENCH_LARGE);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf("14 - transfer %i times %i byte to register:", BENCH_REDOS, BENCH_LARGE);
    printf("%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    /* 15 - release & acquire the bus 1000 times */
    sched_start = _sched_us();
    start = ztimer_now(ZTIMER_USEC);
    for (int i = 0; i < BENCH_REDOS; i++) {
        spi_release(spiconf.dev);
        spi_acquire(spiconf.dev, spiconf.cs, spiconf.mode, spiconf.clk);
    }
    stop = ztimer_now(ZTIMER_USEC);
    sched_stop = _sched_us();
    sched_diff_us = _ztimer_diff_usec(sched_stop, sched_start);
    printf("15 - acquire/release %i times:\t\t", BENCH_REDOS);
    printf("%"PRIu32"\t%"PRIu32"\n", (stop - start), sched_diff_us);
    sum += (stop - start);
    sched_sum += sched_diff_us;

    ztimer_sleep(ZTIMER_SEC, 1);

    printf("-- - SUM:\t\t\t\t\t%"PRIu32"\t%"PRIu32"\n", sum, sched_sum);

    spi_release(spiconf.dev);
    puts("\n### All runs complete ###");

    return 0;
}
SHELL_COMMAND(bench, "Runs some benchmarks", cmd_bench);
#endif

#ifdef MODULE_PERIPH_SPI_RECONFIGURE
int cmd_spi_gpio(int argc, char **argv)
{
    int dev = -1;

    /* parse the given SPI device */
    if (argc > 1) {
        dev = atoi(argv[1]);
    }

    if (dev < 0 || dev >= (int)SPI_NUMOF) {
        puts("error: invalid SPI device specified");
        return 1;
    }

    gpio_t miso_pin = spi_pin_miso(dev);
    gpio_t mosi_pin = spi_pin_mosi(dev);

    printf("Command: spi_deinit_pins(%i)\n", dev);
    spi_deinit_pins(dev);

    gpio_init(miso_pin, GPIO_OUT);
    gpio_init(mosi_pin, GPIO_OUT);

    ztimer_sleep(ZTIMER_SEC, 1);

    printf("Command: gpio_set()\n");
    gpio_set(miso_pin);
    gpio_set(mosi_pin);

    ztimer_sleep(ZTIMER_SEC, 1);

    printf("Command: gpio_clear()\n");
    gpio_clear(miso_pin);
    gpio_clear(mosi_pin);

    ztimer_sleep(ZTIMER_SEC, 1);

    printf("Command: spi_init_pins(%i)\n", dev);
    spi_init_pins(dev);

    printf("Success: spi_%i re-init\n", dev);
    return 0;
}
SHELL_COMMAND(spi_gpio, "Re-configures MISO & MOSI pins to GPIO mode and back.", cmd_spi_gpio);
#endif

int main(void)
{
    puts("Manual SPI peripheral driver test (see README.md)");

    printf("There are %i SPI devices configured for your platform.\n",
           (int)SPI_NUMOF);

    /* reset local SPI configuration */
    spiconf.dev = SPI_UNDEF;

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
