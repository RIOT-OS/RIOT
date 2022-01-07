/*
 * Copyright (C) 2021 Inria
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
 * @brief       Test application for the LCD-TFT display controller
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "timex.h"
#include "ztimer.h"

#include "board.h"
#include "periph_cpu.h"

#include "riot_logo.h"

#define RECT_WIDTH      200
#define RECT_HEIGHT     100
#define PICTURE_WIDTH   128
#define PICTURE_HEIGHT  69

int main(void)
{
    ltdc_init();

    ltdc_fill(0, RECT_WIDTH, 0, RECT_HEIGHT, 0xf800);  /* top left red rectangle */
    ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);
    ltdc_clear();

    ltdc_fill(  /* middle green rectangle */
        (LCD_SCREEN_WIDTH - RECT_WIDTH) >> 1, (LCD_SCREEN_WIDTH + RECT_WIDTH) >> 1,
        (LCD_SCREEN_HEIGHT - RECT_HEIGHT) >> 1, (LCD_SCREEN_HEIGHT + RECT_HEIGHT) >> 1,
        0x07e0
    );
    ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);
    ltdc_clear();

    ltdc_fill(  /* bottom right blue rectangle */
        LCD_SCREEN_WIDTH - RECT_WIDTH, LCD_SCREEN_WIDTH - 1,
        LCD_SCREEN_HEIGHT - RECT_HEIGHT, LCD_SCREEN_HEIGHT - 1,
        0x001f
    );
    ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);
    ltdc_clear();

    /* Display the RIOT logo in all 4 screen corners */
    ltdc_map(
        0, PICTURE_WIDTH - 1, 0, PICTURE_HEIGHT - 1,
        (const uint16_t *)picture
    );
    ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);
    ltdc_clear();

    ltdc_map(
        LCD_SCREEN_WIDTH - PICTURE_WIDTH, LCD_SCREEN_WIDTH - 1, 0, PICTURE_HEIGHT - 1,
        (const uint16_t *)picture
    );
    ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);
    ltdc_clear();

    ltdc_map(
        LCD_SCREEN_WIDTH - PICTURE_WIDTH, LCD_SCREEN_WIDTH - 1,
        LCD_SCREEN_HEIGHT - PICTURE_HEIGHT, LCD_SCREEN_HEIGHT - 1,
        (const uint16_t *)picture
    );
    ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);
    ltdc_clear();

    ltdc_map(
        0, PICTURE_WIDTH - 1, LCD_SCREEN_HEIGHT - PICTURE_HEIGHT, LCD_SCREEN_HEIGHT - 1,
        (const uint16_t *)picture
    );
    ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);
    ltdc_clear();

    /* Display the RIOT logo in the center of the screen */
    ltdc_map(
        ((LCD_SCREEN_WIDTH - PICTURE_WIDTH) >> 1), ((LCD_SCREEN_WIDTH + PICTURE_WIDTH) >> 1) - 1,
        ((LCD_SCREEN_HEIGHT - PICTURE_HEIGHT) >> 1), ((LCD_SCREEN_HEIGHT + PICTURE_HEIGHT) >> 1) - 1,
        (const uint16_t *)picture
    );

    return 0;
}
