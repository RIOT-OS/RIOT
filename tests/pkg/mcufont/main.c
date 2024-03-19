/*
 * Copyright (C) 2023 Koen Zandberg
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
 * @brief       Test application for the MCUFont package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "mcufont.h"
#include "disp_dev.h"
#include "byteorder.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_DISP_DEV
#ifdef LCD_SCREEN_WIDTH
#define DISPLAY_BUFFER_MAX_SIZE (LCD_SCREEN_WIDTH)
#else
#define DISPLAY_BUFFER_MAX_SIZE (320)
#endif
static uint16_t display_buffer[DISPLAY_BUFFER_MAX_SIZE] = { 0 };
#endif

#define PIXBUF_NUM_ROWS     15
#define PIXBUF_NUM_COLS     16
#define PIXBUF_ALPHA_THRESHOLD  80

#define MCUFONT_FIG_TEST_FONT mf_rlefont_DejaVuSans12bw
#define MCUFONT_DISP_TEST_FONT mf_rlefont_DejaVuSans12

extern const struct mf_rlefont_s MCUFONT_FIG_TEST_FONT;
extern const struct mf_rlefont_s MCUFONT_DISP_TEST_FONT;

static const char input[] = "Hello, RIOT!";
static uint8_t fig_buf[PIXBUF_NUM_ROWS][PIXBUF_NUM_COLS] = { 0 };

static disp_dev_reg_t *init_display(void)
{
    disp_dev_reg_t *disp_dev = NULL;
#ifdef MODULE_DISP_DEV
    /* Use the first screen */
    disp_dev = disp_dev_reg_find_screen(0);
    if (!disp_dev) {
        puts("No screen found!");
        return NULL;
    }
    disp_dev_backlight_on();

    /* Clear the screen */
    disp_dev_area_t area;
    for (uint16_t y = 0; y < disp_dev_height(disp_dev->dev); y ++) {
        area.x1 = 0;
        area.x2 = disp_dev_width(disp_dev->dev) - 1;
        area.y1 = area.y2 = y;
        disp_dev_map(disp_dev->dev, &area, display_buffer);
    }
#endif
    return disp_dev;
}

static void fig_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state)
{
    (void)state;
    DEBUG("[draw] x: %"PRIi16", y: %"PRIi16", count: %u, color: %u\n", x, y, count, alpha);
    if (y >= PIXBUF_NUM_ROWS || ((x + count) >= (PIXBUF_NUM_COLS * 8))) {
        return;
    }
    while (count--) {
        uint8_t bitpos = x % 8;
        uint8_t bytepos = x / 8;
        if (alpha > PIXBUF_ALPHA_THRESHOLD) {
            fig_buf[y][bytepos] |= (1 << (7 - bitpos));
        }
        x++;
    }
}

static uint16_t _alpha2rgb(uint8_t alpha)
{
    /* Assumes RGB 565 */
    return (alpha / 8) | ((alpha / 7) << 5) | ((alpha / 8) << 11);
}

static void disp_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *disp)
{
    DEBUG("[disp_draw] x: %"PRIi16", y: %"PRIi16", count: %u, color: %u\n", x, y, count, alpha);
    while (count--) {
        uint16_t color = htons(_alpha2rgb(alpha));
        disp_dev_area_t area = {
            .x1 = x,
            .y1 = y,
            .x2 = x,
            .y2 = y,
        };
        disp_dev_map(disp, &area, &color);
        x++;
    }
}

static void print_fig_buf(void)
{
    for (size_t y = 0; y < PIXBUF_NUM_ROWS; y++) {
        for (size_t x = 0; x < PIXBUF_NUM_COLS; x++) {
            for (int bitpos = 7; bitpos >= 0; bitpos--) {
                if (fig_buf[y][x] & (1 << bitpos)) {
                    printf("█");
                }
                else {
                    printf(" ");
                }
            }
        }
        puts("");
    }
}

static uint8_t char_callback(int16_t x0, int16_t y0, mf_char character, void *state)
{
    return mf_render_character(&MCUFONT_FIG_TEST_FONT.font, x0, y0, character, &fig_callback, state);
}

static uint8_t disp_char_callback(int16_t x0, int16_t y0, mf_char character, void *state)
{
    return mf_render_character(&MCUFONT_DISP_TEST_FONT.font, x0, y0, character, &disp_callback, state);
}

int main(void)
{
    puts("mcufont package test application");
    mf_render_aligned(
            &MCUFONT_FIG_TEST_FONT.font,
            0, 0,
            MF_ALIGN_LEFT,
            input, strlen(input),
            &char_callback, NULL);

    print_fig_buf();

    if (IS_ACTIVE(MODULE_DISP_DEV)) {
        disp_dev_reg_t *display = init_display();
        puts("mcufont package test application");
        int16_t width = mf_get_string_width(&MCUFONT_DISP_TEST_FONT.font, input, 0, true);
        mf_render_aligned(
                &MCUFONT_DISP_TEST_FONT.font,
                (disp_dev_width(display->dev) - width) / 2,
                (disp_dev_height(display->dev) - MCUFONT_DISP_TEST_FONT.font.line_height) / 2,
                MF_ALIGN_LEFT,
                input, strlen(input),
                &disp_char_callback, display->dev);
    }

    return 0;
}
