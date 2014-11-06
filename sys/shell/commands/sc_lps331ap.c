/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to poll the LPS331AP sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "lps331ap.h"

#ifdef MODULE_LPS331AP

#define RATE        LPS331AP_RATE_7HZ

static lps331ap_t lps331ap_dev;

void _get_lps331ap_init_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res;

    res = lps331ap_init(&lps331ap_dev, LPS331AP_I2C, LPS331AP_ADDR, RATE);

    if (res) {
        puts("Error initializing LPS331AP sensor.");
    }
    else {
        puts("Initialized LPS331AP sensor with default values");
    }
}

void _get_lps331ap_read_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int temp;
    int pres;

    if (!lps331ap_dev.address) {
        puts("Error: please call `lps331ap_init` first!");
        return;
    }

    temp = lps331ap_read_temp(&lps331ap_dev);
    pres = lps331ap_read_pres(&lps331ap_dev);

    if (temp < 0) {
        puts("Error reading temperature value from LPS331AP.");
        return;
    }
    else {
        int temp_abs = temp / 1000;
        temp -= (temp_abs * 1000);
        printf("LPS331AP: temperature:  %i.%03i °C\n", temp_abs, temp);
    }

    if (pres < 0) {
        puts("Error reading pressure value from LPS331AP.");
        return;
    }
    else {
        printf("LPS331AP: pressure: %i mBar\n", pres);
    }
}

#endif /* MODULE_LPS331AP */
