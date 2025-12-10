/*
 * SPDX-FileCopyrightText: 2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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
