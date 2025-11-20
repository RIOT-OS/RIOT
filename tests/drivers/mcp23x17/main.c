/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @brief       Test application for Microchip MCP23x17 I2C I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * ## Overview
 *
 * This test application demonstrates the usage of the MCP23x17 driver
 * interface and can be used to test each MCP23x17 expander I/O pin with shell
 * commands.
 *
 * The application bases on the test application for GPIO peripheral drivers
 * which is under following copyright:
 *
 * Copyright (C) 2014,2017 Freie Universität Berlin
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * ## Compilation
 *
 * Used MCP23x17 I/O expander or interface variants have to be defined by
 * the variable `DRIVER` using the corresponding pseudomodules `mcp23017`,
 * `mcp23s17`, `mcp23017_i2c` and/or `mcp23017_spi`, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * DRIVER='mcp23x17_i2c mcp23x17_spi' BOARD=... make -C tests/driver/mcp23x17 flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note If `DRIVER` is not defined, `mcp23x17_i2c` is used by default.
 *
 * The default hardware configuration parameters are defined in
 * `$(RIOTBASE)/drivers/mcp23x17/include/mcp23x17_params.h`. They can
 * be overridden either by placing a `mcp23x17_params.h` file with changed
 * configuration parameters in the directory of the test application or by
 * defining them in the `CFLAGS` variable in the make command, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-DMCP23X17_PARAM_RESET_PIN=GPIO_PIN\(0,7\)" \
 * USEMODULE=mcp23x17_reset \
 * BOARD=... make -C tests/driver/mcp23x17 flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To use external interrupts for the MCP23x17 I/O expander pins, the MCP23x17
 * low-active push/pull interrupt signal has to be enabled. For this purpose
 * add the module `mcp23x17_irq` and define the GPIO pin to which the
 * combined `INTA`/`INTB` interrupt signal is connected, e.g. by overriding the
 * default parameter `MCP23X17_PARAM_I2C_INT` for the default I2C device or
 * `MCP23X17_PARAM_SPI_INT` for the default SPI device.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-DMCP23X17_PARAM_I2C_INT=GPIO_PIN\(0,6\)" \
 * USEMODULE=mcp23x17_irq \
 * BOARD=... make -C tests/driver/mcp23x17 flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Using module `mcp23x17_irq` uses by default an event thread with medium
 * priority. An event thread with high priority can be used by enabling
 * the module `mcp23x17_irq_highest`.
 *
 * @note Since interrupts are handled in the context of a separate event thread,
 * enabling interrupts requires more RAM.
 *
 * ## Usage
 *
 * The test allows to use commands as known from GPIO test application for
 * MCP23x17 I/O expanders:

 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * > help
 * Command              Description
 * ---------------------------------------
 * init_out             init as output (push-pull mode)
 * init_in              init as input w/o pull resistor
 * init_in_pu           init as input with pull-up
 * init_od              init as output (open-drain without pull resistor)
 * init_od_pu           init as output (open-drain with pull-up)
 * init_int             init as external INT w/o pull resistor
 * enable_int           enable gpio interrupt
 * disable_int          disable gpio interrupt
 * read                 read pin status
 * set                  set pin to HIGH
 * clear                set pin to LOW
 * toggle               toggle pin
 * bench                run a set of predefined benchmarks
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The number of the first MCP23x17 I/O expander port used by the test
 * application is defined by the macro `MCP23X17_PORT_0`, which is 16 by
 * default. This value can be overridden during compilation, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-DMCP23X17_PORT_0=8" \
 * BOARD=... make -C tests/driver_mcp23x17 flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Using the port number defined by `MCP23X17_PORT_0` and following port
 * numbers, you can apply the command to the MCP23x17 expander ports. For
 * example, the following command initializes I/O pin 7 of the first MCP23x17
 * expander:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * init_out 16 7
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Commands with port numbers less than `MCP23X17_PORT_0` refer to GPIO
 * peripheral ports. Thus, both the I/O pins of the MCP23x17 expanders as well
 * as the GPIO peripheral pins of the MCU can be addressed by all commands.
 *
 * Thus it is possible to connect a GPIO pin of the MCU with MCP23x17
 * I/O expander pins for testing.
 */

#include <stdio.h>
#include <stdlib.h>

#include "mcp23x17.h"
#include "mcp23x17_params.h"

#include "irq.h"
#include "shell.h"
#include "benchmark.h"

#define BENCH_RUNS_DEFAULT  (100UL * 100)

/* Number of configured MCP23x17 I/O expander devices */
#define MCP23X17_NUM        ARRAY_SIZE(mcp23x17_params)

/* Port number of the first MCP23x17 I/O expander device */
#ifndef MCP23X17_PORT_0
#define MCP23X17_PORT_0     (16)
#endif

/* Maps a port number to MCP23x17 device address */
#define MCP23X17_DEV(port)  (&mcp23x17_dev[po - MCP23X17_PORT_0])

/* MCP23x17 devices allocation */
mcp23x17_t mcp23x17_dev[MCP23X17_NUM];

#if IS_USED(MODULE_MCP23X17_IRQ)
static void gpio_cb(void *arg)
{
    printf("INT: external interrupt from GPIO pin %i\n", (int)arg);
}

static void mcp23x17_cb(void *arg)
{
    printf("INT: external interrupt from MCP23x17 pin %i\n", (int)arg);
}
#endif

static int _init_pin(int argc, char **argv, gpio_mode_t mode)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    if (po < MCP23X17_PORT_0) {
        gpio_init(GPIO_PIN(po, pi), mode);
    }
    else if (mcp23x17_gpio_init(MCP23X17_DEV(po), pi, mode) < 0) {
        printf("error: init MCP23x17 pin (dev %i, pin %i) failed\n", po, pi);
        return 1;
    }

    return 0;
}

static int _init_out(int argc, char **argv)
{
    return _init_pin(argc, argv, GPIO_OUT);
}

static int _init_in(int argc, char **argv)
{
    return _init_pin(argc, argv, GPIO_IN);
}

static int _init_in_pu(int argc, char **argv)
{
    return _init_pin(argc, argv, GPIO_IN_PU);
}

static int _init_od(int argc, char **argv)
{
    return _init_pin(argc, argv, GPIO_OD);
}

static int _init_od_pu(int argc, char **argv)
{
    return _init_pin(argc, argv, GPIO_OD_PU);
}

#if IS_USED(MODULE_MCP23X17_IRQ)
static int _init_int(int argc, char **argv)
{
    gpio_mode_t mode = GPIO_IN;
    gpio_flank_t flank;
    int fl;

    if (argc < 4) {
        printf("usage: %s <dev> <pin> <flank>\n", argv[0]);
        puts("\tflank:\n"
             "\t0: falling\n"
             "\t1: rising\n"
             "\t2: both\n");
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    fl = atoi(argv[3]);
    switch (fl) {
        case 0:
            flank = GPIO_FALLING;
            break;
        case 1:
            flank = GPIO_RISING;
            break;
        case 2:
            flank = GPIO_BOTH;
            break;
        default:
            puts("error: invalid value for active flank");
            return 1;
    }

    if (po < MCP23X17_PORT_0) {
        gpio_init_int(GPIO_PIN(po, pi), mode, flank, gpio_cb, (void *)pi);
    }
    else if (mcp23x17_gpio_init_int(MCP23X17_DEV(po), pi,
                                    mode, flank, mcp23x17_cb, (void *)pi) < 0) {
        printf("error: init_int MCP23x17 pin (dev %i, pin %i) failed\n", po,
               pi);

        return 1;
    }

    return 0;
}

static int _init_int_pu(int argc, char **argv)
{
    gpio_mode_t mode = GPIO_IN_PU;
    gpio_flank_t flank;
    int fl;

    if (argc < 4) {
        printf("usage: %s <dev> <pin> <flank>\n", argv[0]);
        puts("\tflank:\n"
             "\t0: falling\n"
             "\t1: rising\n"
             "\t2: both\n");
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    fl = atoi(argv[3]);
    switch (fl) {
        case 0:
            flank = GPIO_FALLING;
            break;
        case 1:
            flank = GPIO_RISING;
            break;
        case 2:
            flank = GPIO_BOTH;
            break;
        default:
            puts("error: invalid value for active flank");
            return 1;
    }

    if (po < MCP23X17_PORT_0) {
        gpio_init_int(GPIO_PIN(po, pi), mode, flank, gpio_cb, (void *)pi);
    }
    else if (mcp23x17_gpio_init_int(MCP23X17_DEV(po), pi,
                                    mode, flank, mcp23x17_cb, (void *)pi) < 0) {
        printf("error: init_int MCP23x17 pin (dev %i, pin %i) failed\n",
               po, pi);
        return 1;
    }

    return 0;
}

static int _enable_int(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    if (po < MCP23X17_PORT_0) {
        gpio_irq_enable(GPIO_PIN(po, pi));
    }
    else {
        mcp23x17_gpio_irq_enable(MCP23X17_DEV(po), pi);
    }
    return 0;
}

static int _disable_int(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    if (po < MCP23X17_PORT_0) {
        gpio_irq_disable(GPIO_PIN(po, pi));
    }
    else {
        mcp23x17_gpio_irq_disable(MCP23X17_DEV(po), pi);
    }
    return 0;
}
#endif /* MODULE_MCP23X17_IRQ */

static int _read(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    if (po < MCP23X17_PORT_0) {
        if (gpio_read(GPIO_PIN(po, pi))) {
            printf("GPIO pin (port %i, pin %i) is HIGH\n", po, pi);
        }
        else {
            printf("GPIO pin (port %i, pin %i) is LOW\n", po, pi);
        }
    }
    else {
        if (mcp23x17_gpio_read(MCP23X17_DEV(po), pi)) {
            printf("MCP23x17 pin (dev %i, pin %i) is HIGH\n", po, pi);
        }
        else {
            printf("MCP23x17 pin (dev %i, pin %i) is LOW\n", po, pi);
        }
    }
    return 0;
}

static int _set(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    if (po < MCP23X17_PORT_0) {
        gpio_set(GPIO_PIN(po, pi));
    }
    else {
        mcp23x17_gpio_set(MCP23X17_DEV(po), pi);
    }

    return 0;
}

static int _clear(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    if (po < MCP23X17_PORT_0) {
        gpio_clear(GPIO_PIN(po, pi));
    }
    else {
        mcp23x17_gpio_clear(MCP23X17_DEV(po), pi);
    }

    return 0;
}

static int _toggle(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int po = atoi(argv[1]);
    int pi = atoi(argv[2]);

    if (po < MCP23X17_PORT_0) {
        gpio_toggle(GPIO_PIN(po, pi));
    }
    else {
        mcp23x17_gpio_toggle(MCP23X17_DEV(po), pi);
    }

    return 0;
}

static int _bench(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin> [# of runs]\n", argv[0]);
        return 1;
    }

    int pi = atoi(argv[2]);
    int po = atoi(argv[1]);

    unsigned long runs = BENCH_RUNS_DEFAULT;
    if (argc > 3) {
        runs = (unsigned long)atol(argv[3]);
    }

    if (po < MCP23X17_PORT_0) {
        puts("\nGPIO driver run-time performance benchmark\n");
        BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
        gpio_init(GPIO_PIN(po, pi), GPIO_OUT);
        BENCHMARK_FUNC("gpio_set", runs, gpio_set(GPIO_PIN(po, pi)));
        BENCHMARK_FUNC("gpio_clear", runs, gpio_clear(GPIO_PIN(po, pi)));
        BENCHMARK_FUNC("gpio_toggle", runs, gpio_toggle(GPIO_PIN(po, pi)));
        gpio_init(GPIO_PIN(po, pi), GPIO_IN);
        BENCHMARK_FUNC("gpio_read", runs, (void)gpio_read(GPIO_PIN(po, pi)));
        gpio_init(GPIO_PIN(po, pi), GPIO_OUT);
        BENCHMARK_FUNC("gpio_write", runs, gpio_write(GPIO_PIN(po, pi), 1));
    }
    else {
        puts("\nGPIO driver run-time performance benchmark\n");
        mcp23x17_t* dev = MCP23X17_DEV(po);
        BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
        mcp23x17_gpio_init(dev, pi, GPIO_OUT);
        BENCHMARK_FUNC("gpio_set", runs, mcp23x17_gpio_set(dev, pi));
        BENCHMARK_FUNC("gpio_clear", runs, mcp23x17_gpio_clear(dev, pi));
        BENCHMARK_FUNC("gpio_toggle", runs, mcp23x17_gpio_toggle(dev, pi));
        mcp23x17_gpio_init(dev, pi, GPIO_IN);
        BENCHMARK_FUNC("gpio_read", runs, (void)mcp23x17_gpio_read(dev, pi));
        mcp23x17_gpio_init(dev, pi, GPIO_OUT);
        BENCHMARK_FUNC("gpio_write", runs, mcp23x17_gpio_write(dev, pi, 1));
    }
    puts("\n --- DONE ---");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_in", "init as input w/o pull-up", _init_in },
    { "init_in_pu", "init as input with pull-up", _init_in_pu },
    { "init_out", "init as output (push-pull mode)", _init_out },
    { "init_od", "init as output (open-drain w/o pull-up)", _init_od },
    { "init_od_pu", "init as output (open-drain with pull-up)", _init_od_pu },
#if IS_USED(MODULE_MCP23X17_IRQ)
    { "init_int", "init as external INT w/o pull-up", _init_int },
    { "init_int_pu", "init as external INT with pull-up", _init_int_pu },
    { "enable_int", "enable gpio interrupt", _enable_int },
    { "disable_int", "disable gpio interrupt", _disable_int },
#endif
    { "read", "read pin status", _read },
    { "set", "set pin to HIGH", _set },
    { "clear", "set pin to LOW", _clear },
    { "toggle", "toggle pin", _toggle },
    { "bench", "run a set of predefined benchmarks", _bench },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MCP23x17 I/O expander GPIO peripheral driver test\n");
    puts("Initializing MCP23x17");

    /* initialize configured MCP23x17 devices */
    for (unsigned i = 0; i < MCP23X17_NUM; i++) {
        if (mcp23x17_init(&mcp23x17_dev[i],
                          &mcp23x17_params[i]) != MCP23X17_OK) {
            puts("[Failed]");
            return 1;
        }
    }
    puts("[OK]\n");

    puts("In this test, pins are specified by integer device and pin numbers.\n"
         "Use device x and pin 0...15 in all commands to access them.\n\n"
         "NOTE: make sure the values you use exist! The\n"
         "      behavior for not existing device/pins is not defined!");

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
