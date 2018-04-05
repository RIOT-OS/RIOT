/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Test application for the Ucglib package.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#define TEST_OUTPUT_DUMMY 1
#define TEST_OUTPUT_SDL 2
#define TEST_OUTPUT_SPI 3

#ifndef TEST_OUTPUT
#error "TEST_OUTPUT not defined"
#endif

#if TEST_OUTPUT == TEST_OUTPUT_SPI
#ifndef TEST_SPI
#error "TEST_SPI not defined"
#endif
#ifndef TEST_DISPLAY
#error "TEST_DISPLAY not defined"
#endif
#ifndef TEST_DISPLAY_EXT
#error "TEST_DISPLAY_EXT not defined"
#endif
#ifndef TEST_PIN_CS
#error "TEST_PIN_CS not defined"
#endif
#ifndef TEST_PIN_CD
#error "TEST_PIN_CD not defined"
#endif
#ifndef TEST_PIN_RESET
#error "TEST_PIN_RESET not defined"
#endif
#endif

#include <stdio.h>

#include "periph/gpio.h"
#if TEST_OUTPUT == TEST_OUTPUT_SPI
#include "periph/spi.h"
#endif

#include "xtimer.h"
#include "ucg.h"

#include "logo.h"

#if TEST_OUTPUT == TEST_OUTPUT_SPI
/**
 * @brief   RIOT-OS pin maping of Ucglib pin numbers to RIOT-OS GPIO pins.
 * @note    To minimize the overhead, you can implement an alternative for
 *          ucg_com_riotos_hw_spi.
 */
static gpio_t pins[] = {
    [UCG_PIN_CS] = TEST_PIN_CS,
    [UCG_PIN_CD] = TEST_PIN_CD,
    [UCG_PIN_RST] = TEST_PIN_RESET
};

/**
 * @brief   Bit mapping to indicate which pins are set.
 */
static uint32_t pins_enabled = (
    (1 << UCG_PIN_CS) +
    (1 << UCG_PIN_CD) +
    (1 << UCG_PIN_RST)
    );
#endif

int main(void)
{
    uint32_t screen = 0;
    ucg_t ucg;

#if TEST_OUTPUT == TEST_OUTPUT_DUMMY
    /* initialize dummy output */
    puts("Initializing dummy output.");

    ucg_Init(&ucg, ucg_dev_dummy_cb, ucg_ext_none, NULL);
#endif

#if TEST_OUTPUT == TEST_OUTPUT_SDL
    /* initialize to virtual SDL (native only) */
    puts("Initializing to SDL.");

    ucg_Init(&ucg, ucg_sdl_dev_cb, ucg_ext_none, NULL);
#endif

#if TEST_OUTPUT == TEST_OUTPUT_SPI
    /* initialize to SPI */
    puts("Initializing to SPI.");

    ucg_SetPins(&ucg, pins, pins_enabled);
    ucg_SetDevice(&ucg, SPI_DEV(TEST_SPI));

    ucg_Init(&ucg, TEST_DISPLAY, TEST_DISPLAY_EXT, ucg_com_riotos_hw_spi);
#endif

    /* initialize the display */
    puts("Initializing display.");

    ucg_SetFontMode(&ucg, UCG_FONT_MODE_TRANSPARENT);
    ucg_SetFont(&ucg, ucg_font_helvB12_tf);

    /* start drawing in a loop */
    puts("Drawing on screen.");

    while (1) {
        ucg_ClearScreen(&ucg);

        switch (screen) {
            case 0:
                ucg_SetColor(&ucg, 0, 189, 32, 43);
                ucg_DrawString(&ucg, 0, 20, 0, "THIS");
                break;
            case 1:
                ucg_SetColor(&ucg, 0, 63, 167, 136);
                ucg_DrawString(&ucg, 0, 20, 0, "IS");
                break;
            case 2:
                for (int y = 0; y < 48; y++) {
                    for (int x = 0; x < 96; x++) {
                        uint32_t offset = (x + (y * 96)) * 3;

                        ucg_SetColor(&ucg, 0, logo[offset + 0], logo[offset + 1], logo[offset + 2]);
                        ucg_DrawPixel(&ucg, x, y);
                    }
                }
                break;
        }

#if TEST_OUTPUT == TEST_OUTPUT_SDL
        /* handle SDL events */
        ucg_sdl_get_key();
#endif

        /* show screen in next iteration */
        screen = (screen + 1) % 3;

        /* sleep a little */
        xtimer_sleep(1);
    }

    return 0;
}
