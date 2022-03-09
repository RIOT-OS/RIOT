/*
 * Copyright (C) 2020 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Test application for EFM32 specific drivers
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "coretemp.h"

static void test_coretemp(void)
{
    puts("Testing internal EFM32 temperature driver.");

    /* initialize the sensor */
    int result = coretemp_init();

    if (result == 0) {
        puts("Driver initialization OK.");
    }
    else {
        printf("Driver initialization failed: %d.", result);
        return;
    }

    /* read temperature */
    int16_t temperature = coretemp_read();

    printf("Temperature: %d.%02d C\n", temperature / 100, temperature % 100);
}

int main(void)
{
    test_coretemp();

    return 0;
}
