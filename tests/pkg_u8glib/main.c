/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Test application for the U8glib package.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#define TEST_U8GLIB_OUTPUT_STDOUT 1
#define TEST_U8GLIB_OUTPUT_I2C 2
#define TEST_U8GLIB_OUTPUT_SPI 3

#ifndef TEST_U8GLIB_OUTPUT
#error "TEST_U8GLIB_OUTPUT not defined"
#endif
#ifndef TEST_U8GLIB_DISPLAY
#error "TEST_U8GLIB_DISPLAY not defined"
#endif

#if TEST_U8GLIB_OUTPUT == TEST_U8GLIB_OUTPUT_I2C
#ifndef TEST_U8GLIB_I2C
#error "TEST_U8GLIB_I2C not defined"
#endif
#ifndef TEST_U8GLIB_ADDR
#error "TEST_U8GLIB_ADDR not defined"
#endif
#ifndef TEST_U8GLIB_PIN_RESET
#error "TEST_U8GLIB_PIN_RESET not defined"
#endif
#endif

#if TEST_U8GLIB_OUTPUT == TEST_U8GLIB_OUTPUT_SPI
#ifndef TEST_U8GLIB_SPI
#error "TEST_U8GLIB_SPI not defined"
#endif
#ifndef TEST_U8GLIB_PIN_CSN
#error "TEST_U8GLIB_PIN_CSN not defined"
#endif
#ifndef TEST_U8GLIB_PIN_A0
#error "TEST_U8GLIB_PIN_A0 not defined"
#endif
#ifndef TEST_U8GLIB_PIN_RESET
#error "TEST_U8GLIB_PIN_RESET not defined"
#endif
#endif

#include <stdio.h>

#include "xtimer.h"
#include "u8g.h"

int main(void)
{
    uint32_t screen = 0;
    u8g_t u8g;

    /* initialize to stdout */
#if TEST_U8GLIB_OUTPUT == TEST_U8GLIB_OUTPUT_STDOUT
    if (u8g_init_stdout(&u8g) != 0) {
        puts("Failed to initialize U8glib to stdout.");
        return -1;
    }
#endif

    /* initialize to I2C */
#if TEST_U8GLIB_OUTPUT == TEST_U8GLIB_OUTPUT_I2C
    u8g_i2c_t dev;

    if (u8g_init_i2c(&dev, TEST_U8GLIB_I2C, TEST_U8GLIB_ADDR, TEST_U8GLIB_PIN_RESET, &u8g, &TEST_U8GLIB_DISPLAY)) {
        puts("Failed to initialize U8glib to I2C device.");
        return -1;
    }
#endif

    /* initialize to SPI */
#if TEST_U8GLIB_OUTPUT == TEST_U8GLIB_OUTPUT_SPI
    u8g_spi_t dev;

    if (u8g_init_spi(&dev, TEST_U8GLIB_SPI, TEST_U8GLIB_PIN_CSN, TEST_U8GLIB_PIN_A0, TEST_U8GLIB_PIN_RESET, &u8g, &TEST_U8GLIB_DISPLAY)) {
        puts("Failed to initialize U8glib to SPI device.");
        return -1;
    }
#endif

    /* draw a text */
    puts("Drawing on screen.");

    while (true) {
        u8g_FirstPage(&u8g);

        do {
            u8g_SetColorIndex(&u8g, 1);
            u8g_SetFont(&u8g, u8g_font_10x20);

            if (screen == 0) {
                u8g_DrawStr(&u8g, 0, 20, "THIS");
            } else if (screen == 1) {
                u8g_DrawStr(&u8g, 0, 20, "IS");
            } else if (screen == 2) {
                u8g_DrawStr(&u8g, 0, 20, "RIOT-OS");
                u8g_DrawHLine(&u8g, 0, 21, 70);
            }
        } while (u8g_NextPage(&u8g));

        /* show next in next iteration */
        screen = (screen + 1) % 3;

        /* sleep a little */
        xtimer_sleep(1);
    }

    return 0;
}
