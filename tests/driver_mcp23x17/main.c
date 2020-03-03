/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for Microchip MCP23x17 I2C I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * ## Overview
 *
 * This test appliation demonstrates the usage of the MCP23x17 driver interface
 * and can be used to test each MCP23x17 expander I/O pin with shell commands.
 *
 * The application bases on the test application for GPIO peripheral drivers
 * which is under following copyright:
 *
 *      Copyright (C) 2014,2017 Freie Universität Berlin
 *      @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * ## Usage
 *
 * To use the test application, compile it with one or more of the pseudomodules
 * `mcp23017` or `mcp23s17` to enable the driver for your expander modules.
 * Please check the default configuration parameters in
 * `$(RIOTBASE)/drivers/mcp23x17/include/mcp23x17_params.h` and adopt them
 * if necessary.
 *
 *      USEMODULE=mcp23017 make -C tests/driver_mcp23x17 BOARD=...
 *
 * @note When no pseudomodule is given, `mcp23017` is used by default.
 *
 * The usage of the MCP23x17 low-active push-pull interrupt signal is
 * recommended to be able to use external interrupts with the expander I/O
 * pins. Add module `periph_gpio_irq` for this purpose and define the
 * MCU interrupt pin by parameter `MCP23X17_PARAM_INT_PIN`, e.g.
 *
 *      CFLAGS="-DMCP23X17_PARAM_INT_PIN=\(GPIO\(0,6\)\)" \
 *      USEMODULE="mcp23017 periph_gpio_irq" make -C tests/driver_mcp23x17 BOARD=...
 */

#include <stdio.h>
#include <stdlib.h>

#include "mcp23x17.h"
#include "mcp23x17_params.h"

#include "irq.h"
#include "shell.h"
#include "benchmark.h"

#if MODULE_EXTEND_GPIO
#include "extend/gpio.h"
#include "gpio_ext_conf.h"
#endif

#define BENCH_RUNS_DEFAULT      (100UL * 100)

/* Number of configured MCP23x17 I/O expander devices */
#define MCP23X17_NUM    (sizeof(mcp23x17_params) / sizeof(mcp23x17_params[0]))

/* MCP23x17 devices allocation */
mcp23x17_t mcp23x17_dev[MCP23X17_NUM];

#if MODULE_EXTEND_GPIO
/* MCP23x17 GPIO extension driver definition */
const gpio_ext_driver_t mcp23x17_extend_gpio_driver = {
    .init = (gpio_ext_init_t)mcp23x17_gpio_init,
    .init_int = (gpio_ext_init_int_t)mcp23x17_gpio_init_int,
    .irq_enable = (gpio_ext_irq_enable_t)mcp23x17_gpio_irq_enable,
    .irq_disable = (gpio_ext_irq_disable_t)mcp23x17_gpio_irq_disable,
    .read = (gpio_ext_read_t)mcp23x17_gpio_read,
    .set = (gpio_ext_set_t)mcp23x17_gpio_set,
    .clear = (gpio_ext_clear_t)mcp23x17_gpio_clear,
    .toggle = (gpio_ext_toggle_t)mcp23x17_gpio_toggle,
    .write = (gpio_ext_write_t)mcp23x17_gpio_write,
};
#endif /* MODULE_EXTEND_GPIO */

#ifdef MODULE_PERIPH_GPIO_IRQ
static void cb(void *arg)
{
    printf("INT: external interrupt from pin %i\n", (int)arg);
}
#endif

static int init_pin(int argc, char **argv, gpio_mode_t mode)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int dev = atoi(argv[1]);
    int pin = atoi(argv[2]);

#if MODULE_EXTEND_GPIO
    if (gpio_init(GPIO_EXT_PIN(dev, pin), mode) < 0) {
        printf("error: init GPIO_EXT_PIN(%i, %i)) failed\n", dev, pin);
#else
    if (mcp23x17_gpio_init(&mcp23x17_dev[dev], pin, mode) < 0) {
        printf("error: init MCP23x17 pin (dev %i, pin %i)) failed\n", dev, pin);
#endif
        return 1;
    }

    return 0;
}

static int init_out(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OUT);
}

static int init_in(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN);
}

static int init_in_pu(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_IN_PU);
}

static int init_od(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OD);
}

static int init_od_pu(int argc, char **argv)
{
    return init_pin(argc, argv, GPIO_OD_PU);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static int init_int(int argc, char **argv)
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

    int dev = atoi(argv[1]);
    int pin = atoi(argv[2]);

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

#if MODULE_EXTEND_GPIO
    if (gpio_init_int(GPIO_EXT_PIN(dev, pin), mode, flank, cb, (void *)pin) < 0) {
        printf("error: init_int GPIO_EXT_PIN(%i, %i)) failed\n", dev, pin);
#else
    if (mcp23x17_gpio_init_int(&mcp23x17_dev[dev], pin,
                               mode, flank, cb, (void *)pin) < 0) {
        printf("error: init_int MCP23x17 pin (dev %i, pin %i)) failed\n", dev, pin);

#endif
        return 1;
    }

    return 0;
}

static int init_int_pu(int argc, char **argv)
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

    int dev = atoi(argv[1]);
    int pin = atoi(argv[2]);

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

#if MODULE_EXTEND_GPIO
    if (gpio_init_int(GPIO_EXT_PIN(dev, pin), mode, flank, cb, (void *)pin) < 0) {
        printf("error: init_int GPIO_EXT_PIN(%i, %i)) failed\n", dev, pin);
#else
    if (mcp23x17_gpio_init_int(&mcp23x17_dev[dev], pin,
                               mode, flank, cb, (void *)pin) < 0) {
        printf("error: init_int MCP23x17 pin (dev %i, pin %i)) failed\n", dev, pin);

#endif
        return 1;
    }

    return 0;
}

static int enable_int(int argc, char **argv)
{
    int status;

    if (argc < 4) {
        printf("usage: %s <dev> <pin> <status>\n", argv[0]);
        puts("\tstatus:\n"
             "\t0: disable\n"
             "\t1: enable\n");
        return 1;
    }

    int dev = atoi(argv[1]);
    int pin = atoi(argv[2]);

    status = atoi(argv[3]);

    switch (status) {
#if MODULE_EXTEND_GPIO
        case 0:
            puts("disabling GPIO interrupt");
            gpio_irq_disable(GPIO_EXT_PIN(dev, pin));
            break;
        case 1:
            puts("enabling GPIO interrupt");
            gpio_irq_enable(GPIO_EXT_PIN(dev, pin));
            break;
#else /* MODULE_EXTEND_GPIO */
        case 0:
            puts("disabling GPIO interrupt");
            mcp23x17_gpio_irq_disable(&mcp23x17_dev[dev], pin);
            break;
        case 1:
            puts("enabling GPIO interrupt");
            mcp23x17_gpio_irq_enable(&mcp23x17_dev[dev], pin);
            break;
#endif /* MODULE_EXTEND_GPIO */
        default:
            puts("error: invalid status");
            return 1;
    }

    return 0;
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

static int read(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

    int dev = atoi(argv[1]);
    int pin = atoi(argv[2]);

#if MODULE_EXTEND_GPIO
    if (gpio_read(GPIO_EXT_PIN(dev, pin))) {
        printf("GPIO_EXT_PIN(%i, %i)) is HIGH\n", dev, pin);
    }
    else {
        printf("GPIO_EXT_PIN(%i, %i)) is LOW\n", dev, pin);
    }
#else
    if (mcp23x17_gpio_read(&mcp23x17_dev[dev], pin)) {
        printf("MCP23x17 pin (dev %i, pin %i)) is HIGH\n", dev, pin);
    }
    else {
        printf("MCP23x17 pin (dev %i, pin %i)) is LOW\n", dev, pin);
    }
#endif

    return 0;
}

static int set(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

#if MODULE_EXTEND_GPIO
    gpio_set(GPIO_EXT_PIN(atoi(argv[1]), atoi(argv[2])));
#else
    mcp23x17_gpio_set(&mcp23x17_dev[atoi(argv[1])], atoi(argv[2]));
#endif

    return 0;
}

static int clear(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

#if MODULE_EXTEND_GPIO
    gpio_clear(GPIO_EXT_PIN(atoi(argv[1]), atoi(argv[2])));
#else
    mcp23x17_gpio_clear(&mcp23x17_dev[atoi(argv[1])], atoi(argv[2]));
#endif

    return 0;
}

static int toggle(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin>\n", argv[0]);
        return 1;
    }

#if MODULE_EXTEND_GPIO
    gpio_toggle(GPIO_EXT_PIN(atoi(argv[1]), atoi(argv[2])));
#else
    mcp23x17_gpio_toggle(&mcp23x17_dev[atoi(argv[1])], atoi(argv[2]));
#endif

    return 0;
}

static int bench(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <pin> [# of runs]\n", argv[0]);
        return 1;
    }

#if MODULE_EXTEND_GPIO
    gpio_t pin = GPIO_EXT_PIN(atoi(argv[1]), atoi(argv[2]));
#else
    gpio_t pin = atoi(argv[2]);
#endif


    unsigned long runs = BENCH_RUNS_DEFAULT;
    if (argc > 3) {
        runs = (unsigned long)atol(argv[3]);
    }

    puts("\nGPIO driver run-time performance benchmark\n");
#if MODULE_EXTEND_GPIO
    BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
    gpio_init(pi, GPIO_OUT);
    BENCHMARK_FUNC("gpio_set", runs, gpio_set(pin));
    BENCHMARK_FUNC("gpio_clear", runs, gpio_clear(pin));
    BENCHMARK_FUNC("gpio_toggle", runs, gpio_toggle(pin));
    gpio_init(pin, GPIO_IN);
    BENCHMARK_FUNC("gpio_read", runs, (void)gpio_read(pin));
    gpio_init(pin, GPIO_OUT);
    BENCHMARK_FUNC("gpio_write", runs, gpio_write(pin, 1));
#else
    mcp23x17_t* dev = &mcp23x17_dev[atoi(argv[1])];
    BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
    mcp23x17_gpio_init(dev, pin, GPIO_OUT);
    BENCHMARK_FUNC("gpio_set", runs, mcp23x17_gpio_set(dev, pin));
    BENCHMARK_FUNC("gpio_clear", runs, mcp23x17_gpio_clear(dev, pin));
    BENCHMARK_FUNC("gpio_toggle", runs, mcp23x17_gpio_toggle(dev, pin));
    mcp23x17_gpio_init(dev, pin, GPIO_IN);
    BENCHMARK_FUNC("gpio_read", runs, (void)mcp23x17_gpio_read(dev, pin));
    mcp23x17_gpio_init(dev, pin, GPIO_OUT);
    BENCHMARK_FUNC("gpio_write", runs, mcp23x17_gpio_write(dev, pin, 1));
#endif
    puts("\n --- DONE ---");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init_in", "init as input w/o pull-up", init_in },
    { "init_in_pu", "init as input with pull-up", init_in_pu },
    { "init_out", "init as output (push-pull mode)", init_out },
    { "init_od", "init as output (open-drain w/o pull-up)", init_od },
    { "init_od_pu", "init as output (open-drain with pull-up)", init_od_pu },
#ifdef MODULE_PERIPH_GPIO_IRQ
    { "init_int", "init as external INT w/o pull-up", init_int },
    { "init_int_pu", "init as external INT with pull-up", init_int_pu },
    { "enable_int", "enable or disable gpio interrupt", enable_int },
#endif
    { "read", "read pin status", read },
    { "set", "set pin to HIGH", set },
    { "clear", "set pin to LOW", clear },
    { "toggle", "toggle pin", toggle },
    { "bench", "run a set of predefined benchmarks", bench },
    { NULL, NULL, NULL }
};

int main(void)
{
#if MODULE_GPIO_EXTEND
    /*
     * number of GPIO extension list entries has correspond to the
     * number of configured MCP23x17 devices
     */
    assert(MCP23X17_NUM == sizeof(gpio_ext_list[]) / sizeof(gpio_ext_list[0]));
#endif

    puts("MCP23x17 I/O expander GPIO peripheral driver test\n");
    puts("Initializing MCP23x17");

    /* initialize configured MCP23x17 devices */
    for (unsigned i = 0; i < MCP23X17_NUM; i++) {
        if (mcp23x17_init(&mcp23x17_dev[i], &mcp23x17_params[i]) != MCP23X17_OK) {
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
