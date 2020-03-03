/*
 * Copyright (C) 2018 Gunar Schorcht
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
 * This test appliation demonstrates the usage of the MCP47xx driver
 * and can be used to test each MCP47xx DAC channel with shell commands.
 *
 * Functions `init`, `set`, `poweron`, `poweroff` demonstrate the usage of
 * the driver API for one channel of one device using the driver API directly
 * or through the DAC extension API. The `setall` function shows the iteration
 * over all channels of all devices for setting a value.
 */

#include <stdio.h>
#include <stdlib.h>

#include "mcp47xx.h"
#include "mcp47xx_params.h"

#include "irq.h"
#include "shell.h"
#include "benchmark.h"

#if MODULE_EXTEND_DAC
#include "extend/dac.h"
#include "dac_ext_conf.h"
#endif

#define BENCH_RUNS_DEFAULT      (100UL * 100)

/* Number of configured MCP47xx I/O expander devices */
#define MCP47XX_NUM    (sizeof(mcp47xx_params) / sizeof(mcp47xx_params[0]))

/* MCP47xx devices allocation */
mcp47xx_t mcp47xx_dev[MCP47XX_NUM];

#if MODULE_EXTEND_DAC
/* MCP47xx DAC extension driver definition */
const dac_ext_driver_t mcp47xx_extend_dac_driver = {
    .init = (dac_ext_init_t)mcp47xx_dac_init,
    .set = (dac_ext_set_t)mcp47xx_dac_set,
    .poweron = (dac_ext_poweron_t)mcp47xx_dac_poweron,
    .poweroff = (dac_ext_poweroff_t)mcp47xx_dac_poweroff,
    .channels = (dac_ext_channels_t)mcp47xx_dac_channels
};
#endif /* MODULE_EXTEND_DAC */

static int init(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <channel>\n", argv[0]);
        return 1;
    }

    int dev = atoi(argv[1]);
    int chn = atoi(argv[2]);

#if MODULE_EXTEND_DAC
    if (dac_init(DAC_EXT_LINE(dev, chn)) < 0) {
        printf("error: init DAC_EXT_LINE(%i, %02i) failed\n", dev, chn);
#else
    if (mcp47xx_dac_init(&mcp47xx_dev[dev], chn) < 0) {
        printf("error: init MCP47xx pin (dev %i, chn %i) failed\n", dev, chn);
#endif
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

#if MODULE_EXTEND_DAC
    dac_set(DAC_EXT_LINE(atoi(argv[1]), atoi(argv[2])), atoi(argv[3]));
#else
    mcp47xx_dac_set(&mcp47xx_dev[atoi(argv[1])], atoi(argv[2]),
                                                 atoi(argv[3]));
#endif

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
        #if MODULE_EXTEND_DAC
        uint8_t num = dac_channels(DAC_EXT_LINE(dev, 0));
        #else
        uint8_t num = mcp47xx_dac_channels(&mcp47xx_dev[dev]);
        #endif

        for (dac_t chn = 0; chn < num; chn++) {
            #if MODULE_EXTEND_DAC
            dac_set(DAC_EXT_LINE(dev, chn), value);
            #else
            mcp47xx_dac_set(&mcp47xx_dev[dev], chn, value);
            #endif
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

#if MODULE_EXTEND_DAC
    dac_poweron(DAC_EXT_LINE(atoi(argv[1]), atoi(argv[2])));
#else
    mcp47xx_dac_poweron(&mcp47xx_dev[atoi(argv[1])], atoi(argv[2]));
#endif

    return 0;
}

static int poweroff(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <dev> <channel>\n", argv[0]);
        return 1;
    }

#if MODULE_EXTEND_DAC
    dac_poweroff(DAC_EXT_LINE(atoi(argv[1]), atoi(argv[2])));
#else
    mcp47xx_dac_poweroff(&mcp47xx_dev[atoi(argv[1])], atoi(argv[2]));
#endif

    return 0;
}

static int channels(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <dev>\n", argv[0]);
        return 1;
    }

#if MODULE_EXTEND_DAC
    uint8_t chn = dac_channels(DAC_EXT_LINE(atoi(argv[1]), 0));
#else
    uint8_t chn = mcp47xx_dac_channels(&mcp47xx_dev[atoi(argv[1])]);
#endif
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
        for (uint32_t j = 0; j < UINT16_MAX+1; j += 16) {
#if MODULE_EXTEND_DAC
            dac_set(DAC_EXT_LINE(dev, chn), j);
#else
            mcp47xx_dac_set(&mcp47xx_dev[dev], chn, j);
#endif
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
#if MODULE_EXTEND_DAC
    dac_t line = DAC_EXT_LINE(atoi(argv[1]), atoi(argv[2]));
    BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
    dac_init(line);
    BENCHMARK_FUNC("dac_set", runs, dac_set(line, 0));
#else
    int dev = atoi(argv[1]);
    int chn = atoi(argv[2]);
    BENCHMARK_FUNC("nop loop", runs, __asm__ volatile("nop"));
    mcp47xx_dac_init(&mcp47xx_dev[dev], chn);
    BENCHMARK_FUNC("dac_set", runs, mcp47xx_dac_set(&mcp47xx_dev[dev], chn, 0));
#endif
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
#if MODULE_DAC_EXTEND
    /*
     * number of DAC extension list entries has correspond to the
     * number of configured MCP47xx devices
     */
    assert(MCP47XX_NUM == sizeof(dac_ext_list[]) / sizeof(dac_ext_list[0]));
#endif

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
