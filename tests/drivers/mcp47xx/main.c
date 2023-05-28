/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for Microchip MCP47xx DAC with I2C interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * This test application demonstrates the usage of the MCP47xx driver.
 * It can be used to test each MCP47xx DAC channel with shell commands.
 *
 * Functions `init`, `set`, `poweron`, `poweroff` demonstrate the usage of
 * the driver API for one channel of one device using the driver API.
 * The `setall` function shows the iteration over all channels of all
 * devices for setting a value.
 */

#include <stdio.h>
#include <stdlib.h>

#include "mcp47xx.h"
#include "mcp47xx_params.h"

#include "irq.h"
#include "shell.h"
#include "benchmark.h"

#define BENCH_RUNS_DEFAULT      (100UL * 100)

/* Number of configured MCP47xx I/O expander devices */
#define MCP47XX_NUM    (sizeof(mcp47xx_params) / sizeof(mcp47xx_params[0]))

/* MCP47xx devices allocation */
mcp47xx_t mcp47xx_dev[MCP47XX_NUM];

static int init(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <channel>\n", argv[0]);
        return 1;
    }

    int dev = atoi(argv[1]);
    int chn = atoi(argv[2]);

    if (mcp47xx_dac_init(&mcp47xx_dev[dev], chn) < 0) {
        printf("error: init MCP47xx pin (dev %i, chn %i) failed\n", dev, chn);
        return 1;
    }

    return 0;
}

static int set(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s <dev> <channel> <value>\n", argv[0]);
        return 1;
    }

    mcp47xx_dac_set(&mcp47xx_dev[atoi(argv[1])], atoi(argv[2]),
                                                 atoi(argv[3]));
    return 0;
}

static int setall(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <value>\n", argv[0]);
        return 1;
    }

    uint16_t value = atoi(argv[1]);

    for (unsigned dev = 0; dev < MCP47XX_NUM; dev++) {
        uint8_t num = mcp47xx_dac_channels(&mcp47xx_dev[dev]);

        for (uint8_t chn = 0; chn < num; chn++) {
            mcp47xx_dac_set(&mcp47xx_dev[dev], chn, value);
        }
    }

    return 0;
}

static int poweron(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <channel>\n", argv[0]);
        return 1;
    }

    mcp47xx_dac_poweron(&mcp47xx_dev[atoi(argv[1])], atoi(argv[2]));

    return 0;
}

static int poweroff(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <channel>\n", argv[0]);
        return 1;
    }

    mcp47xx_dac_poweroff(&mcp47xx_dev[atoi(argv[1])], atoi(argv[2]));

    return 0;
}

static int channels(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <dev>\n", argv[0]);
        return 1;
    }

    uint8_t chn = mcp47xx_dac_channels(&mcp47xx_dev[atoi(argv[1])]);

    printf("channel_num: %u\n", chn);
    return 0;
}

static int saw(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s <dev> <channel> <number>\n", argv[0]);
        return 1;
    }

    int dev = atoi(argv[1]);
    int chn = atoi(argv[2]);
    unsigned num = atoi(argv[3]);

    for (unsigned i = 0; i < num; i++) {
        for (uint16_t j = 0; j < UINT16_MAX; j ++) {
            mcp47xx_dac_set(&mcp47xx_dev[dev], chn, j);
        }
    }

    return 0;
}

static int bench(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <channel> [# of runs]\n", argv[0]);
        return 1;
    }

    unsigned long runs = BENCH_RUNS_DEFAULT;
    if (argc > 3) {
        runs = (unsigned long)atol(argv[3]);
    }

    puts("\nDAC driver run-time performance benchmark\n");

    int dev = atoi(argv[1]);
    int chn = atoi(argv[2]);
    BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
    mcp47xx_dac_init(&mcp47xx_dev[dev], chn);
    BENCHMARK_FUNC("dac_set", runs, mcp47xx_dac_set(&mcp47xx_dev[dev], chn, 0));

    puts("\n --- DONE ---");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "init one channel of a device", init },
    { "set", "set the output of one channel of a device", set },
    { "setall", "set the outputs of all channels of all devices", setall },
    { "poweron", "power on the output of one channel of a device", poweron },
    { "poweroff", "power off the output of one channel of a device", poweroff },
    { "channels", "number of channels of a device", channels },
    { "saw", "generate a saw signal on one channel of a device", saw },
    { "bench", "run a set of predefined benchmarks", bench },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MCP47xx DAC peripheral driver test\n");
    puts("Initializing MCP47xx");

    /* initialize configured MCP47xx devices */
    for (unsigned i = 0; i < MCP47XX_NUM; i++) {
        if (mcp47xx_init(&mcp47xx_dev[i], &mcp47xx_params[i]) != MCP47XX_OK) {
            puts("[Failed]");
            return 1;
        }
    }
    puts("[OK]\n");

    puts("In this test, DAC lines are specified by device and channel numbers.\n"
         "NOTE: make sure the values exist! The\n"
         "      behavior for not existing devices/channels is not defined!");

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
