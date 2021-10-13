/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for LIS2DH12 accelerometer driver
 *
 * @author      Jan Mohr <jan.mohr@ml-pa.com>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "xtimer.h"
#include "fmt.h"
#include "thread.h"
#include "shell.h"

#include "lis2dh12.h"
#include "lis2dh12_params.h"
#include "lis2dh12_registers.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* device specific */
#define NUM_FIFO_VALUES 32

/* allocate device descriptor */
static lis2dh12_t dev;

void lis2dh12_test_init(void) {

    if (IS_USED(MODULE_LIS2DH12_SPI)) {
        puts("using SPI mode, for I2C mode select the lis2dh12_i2c module");
    } else {
        puts("using I2C mode, for SPI mode select the lis2dh12_spi module");
    }

    /* init lis */
    if (lis2dh12_init(&dev, &lis2dh12_params[0]) == LIS2DH12_OK) {
        puts("lis2dh12 [Initialized]");
    }
    else {
        puts("lis2dh12 [Failed]");
    }

    /* change LIS settings */
    lis2dh12_set_resolution(&dev, LIS2DH12_POWER_LOW);
    lis2dh12_set_datarate(&dev, LIS2DH12_RATE_100HZ);
    lis2dh12_set_scale(&dev, LIS2DH12_SCALE_16G);

    /* configure FIFO */
    lis2dh12_fifo_t fifo_cfg = {
        .FIFO_mode = LIS2DH12_FIFO_MODE_STREAM,
    };

    lis2dh12_set_fifo(&dev, &fifo_cfg);
}

#ifdef MODULE_LIS2DH12_INT
void* lis2dh12_test_process(void* arg) {
    (void) arg;

    /* start processing */
    DEBUG("[Process]: start process\n");

    while (1) {

        /* wait for interrupt */
        int int1_src = lis2dh12_wait_event(&dev, LIS2DH12_INT1, false);

        if (int1_src <= 0) {
            printf("error: %d\n", int1_src);
            continue;
        }

        if (LIS2DH12_INT_SRC_1(int1_src) & LIS2DH12_INT_SRC_IA) {
            puts("event 1");
        }
        if (LIS2DH12_INT_SRC_2(int1_src) & LIS2DH12_INT_SRC_IA) {
            puts("event 2");
        }
        if (LIS2DH12_INT_SRC_CLICK(int1_src) & LIS2DH12_INT_SRC_IA) {
            puts("click event");
        }
    }

    return NULL;
}
#endif /* MODULE_LIS2DH12_INT */

static int shell_is2dh12_read(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    lis2dh12_fifo_data_t data;

    lis2dh12_read(&dev, &data);

    /* Memory to print current data */
    char str_out[3][8];

    /* format data */
    for (unsigned j = 0; j < 3; ++j) {
        size_t len = fmt_s16_dfp(str_out[j], data.data[j], -3);
        str_out[j][len] = '\0';
    }

    printf("X: %6s  Y: %6s  Z: %6s\n", str_out[0], str_out[1], str_out[2]);

    return 0;
}

static int shell_is2dh12_read_fifo(int argc, char **argv)
{
    uint8_t num = NUM_FIFO_VALUES;
    lis2dh12_fifo_data_t data[NUM_FIFO_VALUES];

    if (argc > 1) {
        num = atoi(argv[1]);
    }

    num = lis2dh12_read_fifo_data(&dev, data, num);

    /* print data */
    for (unsigned i = 0; i < num; ++i) {

        /* Memory to print current data */
        char str_out[3][8];

        /* format data */
        for (unsigned j = 0; j < 3; ++j) {
            size_t len = fmt_s16_dfp(str_out[j], data[i].data[j], -3);
            str_out[j][len] = '\0';
        }

        printf("[%2u] X: %6s  Y: %6s  Z: %6s\n", i, str_out[0], str_out[1], str_out[2]);
    }

    return 0;
}

static int shell_is2dh12_threshold(int argc, char **argv)
{
    uint8_t slot;
    uint32_t mg;
    uint32_t us = 0;
    uint8_t axis = LIS2DH12_INT_CFG_XHIE
                 | LIS2DH12_INT_CFG_YHIE
                 | LIS2DH12_INT_CFG_ZHIE;

    if (argc < 3) {
        printf("usage: %s <slot> <mg> [µs]\n", argv[0]);
        return -1;
    }

    slot = atoi(argv[1]);
    mg   = atoi(argv[2]);

    if (argc > 3) {
        us = atoi(argv[3]);
    }

    if (slot < 1 || slot > 2) {
        puts("event slot must be either 1 or 2");
        return -1;
    }

    lis2dh12_cfg_threshold_event(&dev, mg, us, axis, slot, LIS2DH12_INT1);

    return 0;
}

static int shell_is2dh12_click(int argc, char **argv)
{
    uint32_t mg;
    uint32_t us = 0;
    uint32_t us_delay = 0;
    uint32_t us_double = 0;
    uint8_t clicks = LIS2DH12_CLICK_X_SINGLE
                   | LIS2DH12_CLICK_Y_SINGLE
                   | LIS2DH12_CLICK_Z_SINGLE;

    if (argc < 2) {
        printf("usage: %s <mg> [µs] [dead time µs] [double click µs]\n", argv[0]);
        return -1;
    }

    mg = atoi(argv[1]);

    if (argc > 2) {
        us = atoi(argv[2]);
    }

    if (argc > 3) {
        us_delay = atoi(argv[3]);
    }

    if (argc > 4) {
        us_double = atoi(argv[4]);
        clicks |= clicks << 1;
    }

    lis2dh12_cfg_click_event(&dev, mg, us, us_delay, us_double, clicks, LIS2DH12_INT1);

    return 0;
}

static int shell_is2dh12_power(int argc, char **argv)
{
    bool on;

    if (argc > 1 && (!strcmp(argv[1], "on") || !strcmp(argv[1], "1"))) {
        on = true;
    } else if (argc > 1 && (!strcmp(argv[1], "off") || !strcmp(argv[1], "0"))) {
        on = false;
    } else {
        printf("usage: %s <on|off>\n", argv[0]);
        return -1;
    }

    if (on) {
        lis2dh12_poweron(&dev);
    } else {
        lis2dh12_poweroff(&dev);
    }

    return 0;
}

static int shell_is2dh12_set_resolution(int argc, char **argv)
{
    unsigned resolution = UINT_MAX;

    const char* resolutions[4] = {
        "off",
        "8-bit",
        "10-bit",
        "12-bit",
    };

    if (argc > 1) {
        resolution = atoi(argv[1]);
    } else {
        printf("current resolution: %s\n", resolutions[lis2dh12_get_resolution(&dev)]);
    }

    if (resolution > LIS2DH12_POWER_HIGH) {
        printf("usage: %s <mode>\n", argv[0]);
        puts("where <mode> is:");
        for (unsigned i = 0; i < ARRAY_SIZE(resolutions); ++i) {
            printf("\t%u: %s\n", i, resolutions[i]);
        }
        return -1;
    }

    lis2dh12_set_resolution(&dev, resolution);

    return 0;
}

static int shell_is2dh12_set_rate(int argc, char **argv)
{
    unsigned rate = UINT_MAX;

    if (argc > 1) {
        rate = atoi(argv[1]);
    } else {
        printf("Current sampling rate: %u Hz\n", lis2dh12_get_datarate(&dev));
    }

    if (rate > LIS2DH12_RATE_VERYHIGH) {
        printf("usage: %s <rate>\n", argv[0]);
        puts("where <rate> is:");
        puts("\t1: 1 Hz");
        puts("\t2: 10 Hz");
        puts("\t3: 25 Hz");
        puts("\t4: 50 Hz");
        puts("\t5: 100 Hz");
        puts("\t6: 200 Hz");
        puts("\t7: 400 Hz");
        puts("\t8: 1620 Hz");
        puts("\t9: 5376 Hz");
        return -1;
    }

    lis2dh12_set_datarate(&dev, rate);

    return 0;
}

static int shell_is2dh12_set_scale(int argc, char **argv)
{
    unsigned scale = UINT_MAX;

    const uint8_t scales[] = {
        2, 4, 8, 16
    };

    if (argc > 1) {
        scale = atoi(argv[1]);
    } else {
        printf("current range: ± %ug\n", scales[lis2dh12_get_scale(&dev)]);
    }

    if (scale > LIS2DH12_SCALE_16G) {
        printf("usage: %s <scale>\n", argv[0]);
        puts("where <scale> is:");
        for (unsigned i = 0; i < ARRAY_SIZE(scales); ++i) {
            printf("\t%u: ± %ug\n", i, scales[i]);
        }

        return -1;
    }

    lis2dh12_set_scale(&dev, scale);

    return 0;
}

static int shell_is2dh12_read_temp(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int16_t temp;
    lis2dh12_read_temperature(&dev, &temp);

    printf("%d.%02d °C\n", temp / 100, temp % 100);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "read", "Read acceleration data", shell_is2dh12_read },
    { "read_fifo", "Read acceleration data from fifo", shell_is2dh12_read_fifo },
    { "threshold", "Configure threshold event", shell_is2dh12_threshold },
    { "click", "Configure click event", shell_is2dh12_click },
    { "power", "Enable / Disable the sensor", shell_is2dh12_power },
    { "resolution", "Get/Set resolution", shell_is2dh12_set_resolution },
    { "rate", "Get/Set sampline rate", shell_is2dh12_set_rate },
    { "scale", "Get/Set measuring range", shell_is2dh12_set_scale },
    { "temp", "Read temperature data", shell_is2dh12_read_temp },
    { NULL, NULL, NULL },
};

int main(void)
{
    /* init lis */
    lis2dh12_test_init();

#ifdef MODULE_LIS2DH12_INT
    static char lis2dh12_process_stack[THREAD_STACKSIZE_MAIN];

    /* processing lis2dh12 acceleration data */
    thread_create(lis2dh12_process_stack, sizeof(lis2dh12_process_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  lis2dh12_test_process, NULL, "lis2dh12_process");
#endif /* MODULE_LIS2DH12_INT */

    /* running shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
