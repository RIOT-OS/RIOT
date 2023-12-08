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
 * @brief       Test application for LIS2DE12 accelerometer driver
 *
 * @author      Léo <leo.cordier@etu.esisar.grenoble-inp.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "fmt.h"
#include "thread.h"
#include "ztimer.h"
#include "shell.h"

#include "lis2de12.h"
#include "lis2de12_params.h"
#include "lis2de12_registers.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* device specific */
#define NUM_FIFO_VALUES 32

/* allocate device descriptor */
static lis2de12_t dev;

void lis2de12_test_init(void) {

    /* init lis */
    if (lis2de12_init(&dev, &lis2de12_params[0]) == LIS2DE12_OK) {
        puts("lis2de12 [Initialized]");
    }
    else {
        puts("lis2de12 [Failed]");
    }

    /* change LIS settings */
    lis2de12_set_datarate(&dev, LIS2DE12_RATE_100HZ);
    lis2de12_set_scale(&dev, LIS2DE12_SCALE_2G);

    /* configure FIFO */
    lis2de12_fifo_t fifo_cfg = {
        .FIFO_mode = LIS2DE12_FIFO_MODE_STREAM,
    };

    lis2de12_set_fifo(&dev, &fifo_cfg);
}

static int shell_is2de12_read(int argc, char **argv)
{
    (void)argc;
    (void)argv;


    lis2de12_fifo_data_t data;

    for(int i = 1; i<= 10; i++){
        lis2de12_read(&dev, &data);

        /* Memory to print current data */
        char str_out[3][8];

        /* format data */
        for (unsigned j = 0; j < 3; ++j) {
            size_t len = fmt_s16_dfp(str_out[j], data.data[j], -3);
            str_out[j][len] = '\0';
        }

        printf("measure n°%d : X: %6s g  Y: %6s g Z: %6s g\n", i, str_out[0], str_out[1], str_out[2]);

       ztimer_sleep(ZTIMER_MSEC, 1000);
    }

   

    return 0;
}

static int shell_is2de12_set_rate(int argc, char **argv)
{
    unsigned rate = UINT_MAX;

    if (argc > 1) {
        rate = atoi(argv[1]);
    } else {
        printf("Current sampling rate: %u Hz\n", lis2de12_get_datarate(&dev));
    }

    if (rate > LIS2DE12_RATE_VERYHIGH) {
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

    lis2de12_set_datarate(&dev, rate);

    return 0;
}

static int shell_is2de12_set_scale(int argc, char **argv)
{
    unsigned scale = UINT_MAX;

    const uint8_t scales[] = {
        2, 4, 8, 16
    };

    if (argc > 1) {
        scale = atoi(argv[1]);
    } else {
        printf("current range: ± %ug\n", scales[lis2de12_get_scale(&dev)]);
    }

    if (scale > LIS2DE12_SCALE_16G) {
        printf("usage: %s <scale>\n", argv[0]);
        puts("where <scale> is:");
        for (unsigned i = 0; i < ARRAY_SIZE(scales); ++i) {
            printf("\t%u: ± %ug\n", i, scales[i]);
        }

        return -1;
    }

    lis2de12_set_scale(&dev, scale);

    return 0;
}

static int shell_is2de12_read_temp(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int16_t temp;
    lis2de12_read_temperature(&dev, &temp);

    printf("%d.%02d °C\n", temp / 100, temp % 100);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "read", "Read acceleration data", shell_is2de12_read },
    { "rate", "Get/Set sampline rate", shell_is2de12_set_rate },
    { "scale", "Get/Set measuring range", shell_is2de12_set_scale },
    { "temp", "Read temperature data", shell_is2de12_read_temp },
    { NULL, NULL, NULL },
};

int main(void)
{
    /* init lis */
    lis2de12_test_init();

    /* running shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
