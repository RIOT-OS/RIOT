/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the Guestgood GP8xxx I2C DACs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "ztimer.h"
#include "shell.h"

#include "gp8xxx.h"
#include "gp8xxx_params.h"

static gp8xxx_t gp8xxx_devs[ARRAY_SIZE(gp8xxx_params)];

static int _info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (unsigned i = 0; i < ARRAY_SIZE(gp8xxx_devs); i++) {
        const gp8xxx_t *dev = &gp8xxx_devs[i];
        const gp8xxx_info_t *info = dev->params.info;

        printf("GP8xxx #%d\n", i);

        if (info->type == GP8XXX_INFO_TYPE_VDAC) {
            puts("Type: VDAC");
        }
        else if (info->type == GP8XXX_INFO_TYPE_IDAC) {
            puts("Type: IDAC");
        }

        printf("Channels: %d\n", info->channels);

        if (info->range == GP8XXX_INFO_RANGE_2V5_VCC) {
            puts("Range: 0 - 2.5 V");
        }
        else if (info->range == GP8XXX_INFO_RANGE_5V_10V) {
            puts("Range: 5 - 10 V");
        }
        else if (info->range == GP8XXX_INFO_RANGE_4MA_20MA) {
            puts("Range: 4 - 20 mA");
        }

        if (info->type == GP8XXX_INFO_TYPE_VDAC) {
            printf("Voltage range: 0 - %d mV\n", dev->range);
        }
        else if (info->type == GP8XXX_INFO_TYPE_IDAC) {
            printf("Current range: 0 - %d uA\n", dev->range);
        }

        printf("Resolution: 0 - %d\n", info->resolution);

#if GP8XXX_HAS_IDAC
        if (info->type == GP8XXX_INFO_TYPE_IDAC) {
            printf("Calibration: %d @ 4 mA, %d @ 20 mA\n", dev->calibration_4ma,
                   dev->calibration_20ma);
        }
#endif

        puts("");
    }

    return 0;
}

#if GP8XXX_HAS_IDAC
static int _calibrate(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: %s <dev> <dac value @ 4 mA> <dac value @ 20 mA>\n", argv[0]);
        return 1;
    }

    unsigned dev = strtoul(argv[1], NULL, 10);
    unsigned calibration_4ma = strtoul(argv[2], NULL, 10);
    unsigned calibration_20ma = strtoul(argv[3], NULL, 10);

    if (dev >= ARRAY_SIZE(gp8xxx_params)) {
        puts("Invalid device.");
        return 1;
    }

    if (gp8xxx_devs[dev].params.info->type != GP8XXX_INFO_TYPE_IDAC) {
        puts("Device is not an IDAC.");
        return 1;
    }

    if (calibration_4ma != 0 && calibration_20ma != 0 && calibration_4ma >= calibration_20ma) {
        puts("Invalid calibration values.");
        return 1;
    }

    gp8xxx_set_current_calibration(&gp8xxx_devs[dev], calibration_4ma, calibration_20ma);

    return 0;
}

static int _set_current(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: %s <dev> <channel> <current (uA)>\n", argv[0]);
        return 1;
    }

    unsigned dev = strtoul(argv[1], NULL, 10);
    unsigned channel = strtoul(argv[2], NULL, 10);
    unsigned current = strtoul(argv[3], NULL, 10);

    if (dev >= ARRAY_SIZE(gp8xxx_devs)) {
        puts("Invalid device.");
        return 1;
    }

    if (gp8xxx_devs[dev].params.info->type != GP8XXX_INFO_TYPE_IDAC) {
        puts("Device is not an IDAC.");
        return 1;
    }

    if (channel != GP8XXX_CHANNEL_ALL && channel >= gp8xxx_devs[dev].params.info->channels) {
        puts("Invalid channel.");
        return 1;
    }

    int res = gp8xxx_set_current(&gp8xxx_devs[dev], channel, current);

    if (res != 0) {
        printf("Failed to set current (%d).\n", res);
        return 1;
    }

    return 0;
}
#endif /* GP8XXX_HAS_IDAC */

static int _set_dac(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: %s <dev> <channel> <value>\n", argv[0]);
        return 1;
    }

    unsigned dev = strtoul(argv[1], NULL, 10);
    unsigned channel = strtoul(argv[2], NULL, 10);
    unsigned value = strtoul(argv[3], NULL, 10);

    if (dev >= ARRAY_SIZE(gp8xxx_devs)) {
        puts("Invalid device.");
        return 1;
    }

    if (channel != GP8XXX_CHANNEL_ALL && channel >= gp8xxx_devs[dev].params.info->channels) {
        puts("Invalid channel.");
        return 1;
    }

    if (value > gp8xxx_devs[dev].params.info->resolution) {
        puts("Value out of range.");
        return 1;
    }

    int res = gp8xxx_set_dac(&gp8xxx_devs[dev], channel, value);

    if (res != 0) {
        printf("Failed to set DAC (%d).\n", res);
        return 1;
    }

    return 0;
}

#if GP8XXX_HAS_VDAC
static int _set_voltage(int argc, char **argv)
{
    if (argc != 4) {
        printf("Usage: %s <dev> <channel> <voltage (mV)>\n", argv[0]);
        return 1;
    }

    unsigned dev = strtoul(argv[1], NULL, 10);
    unsigned channel = strtoul(argv[2], NULL, 10);
    unsigned voltage = strtoul(argv[3], NULL, 10);

    if (dev >= ARRAY_SIZE(gp8xxx_devs)) {
        puts("Invalid device.");
        return 1;
    }

    if (gp8xxx_devs[dev].params.info->type != GP8XXX_INFO_TYPE_VDAC) {
        puts("Device is not an VDAC.");
        return 1;
    }

    if (channel != GP8XXX_CHANNEL_ALL && channel >= gp8xxx_devs[dev].params.info->channels) {
        puts("Invalid channel.");
        return 1;
    }

    int res = gp8xxx_set_voltage(&gp8xxx_devs[dev], channel, voltage);

    if (res != 0) {
        printf("Failed to set voltage (%d).\n", res);
        return 1;
    }

    return 0;
}

static int _set_range(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s <dev> <range>\n", argv[0]);
        return 1;
    }

    unsigned dev = strtoul(argv[1], NULL, 10);
    unsigned range = strtoul(argv[2], NULL, 10);

    if (dev >= ARRAY_SIZE(gp8xxx_devs)) {
        puts("Invalid device.");
        return 1;
    }

    if (gp8xxx_devs[dev].params.info->type != GP8XXX_INFO_TYPE_VDAC) {
        puts("Device is not an VDAC.");
        return 1;
    }

    if (range != GP8XXX_OUTPUT_RANGE_5V && range != GP8XXX_OUTPUT_RANGE_10V) {
        puts("Invalid range.");
        return 1;
    }

    int res = gp8xxx_set_voltage_range(&gp8xxx_devs[dev], range);

    if (res != 0) {
        printf("Failed to set range (%d).\n", res);
        return 1;
    }

    return 0;
}

#endif /* GP8XXX_HAS_VDAC */

static int _sweep(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <dev>\n", argv[0]);
        return 1;
    }

    unsigned dev = strtoul(argv[1], NULL, 10);

    if (dev >= ARRAY_SIZE(gp8xxx_devs)) {
        puts("Invalid device.");
        return 1;
    }

    uint16_t resolution = gp8xxx_devs[dev].params.info->resolution;

    printf("Sweeping channel(s) from 0 - %d.\n", resolution);

    for (uint16_t i = 0; i < resolution; i++) {
        if (gp8xxx_set_dac(&gp8xxx_devs[dev], GP8XXX_CHANNEL_ALL, i) != 0) {
            printf("Sweep failed at value %d.\n", i);
            return 1;
        }

        ztimer_sleep(ZTIMER_MSEC, 1);
    }

    puts("Sweep completed.");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "info", "Print DAC info.", _info },
#if GP8XXX_HAS_IDAC
    { "calibrate", "Calibrate 4 mA and 20 mA values.", _calibrate },
    { "set_current", "Set current (in uA).", _set_current },
#endif /* GP8XXX_HAS_IDAC */
    { "set_dac", "Set DAC value (raw).", _set_dac },
#if GP8XXX_HAS_VDAC
    { "set_voltage", "Set voltage (in mV).", _set_voltage },
    { "set_range", "Set voltage range.", _set_range },
#endif /* GP8XXX_HAS_VDAC */
    { "sweep", "Sweep DAC range.", _sweep },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* initialize the DACs */
    puts("Initializing DACs... ");

    for (unsigned i = 0; i < ARRAY_SIZE(gp8xxx_params); i++) {
        if (gp8xxx_init(&gp8xxx_devs[i], &gp8xxx_params[i]) == 0) {
            printf("GP8xxx #%d [OK]\n", i);
        }
        else {
            printf("GP8xxx #%d [FAILED]\n", i);
        }
    }

    /* run shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
