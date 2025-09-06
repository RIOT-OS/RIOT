/*
 * SPDX-FileCopyrightText: 2023 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_gba_stdio_fb
 * @{
 *
 * @file
 * @brief       STDIO framebuffer driver
 *
 * This file provides a framebuffer driver for STDIO, implementing stdout only.
 *
 * @author      Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * @}
 */

#include "stdio_base.h"
#include "periph_gba.h"

#include <string.h>

static const unsigned char systemFont[] =
{
#include "font_terminal.h"
};

#define BLACK 0x0000
#define WHITE 0x7FFF

static void clearScreen(void)
{
    /* width * height * two byte per pixel */
    memset(GBA_VRAM, 0x00, GBA_SCREEN_WIDTH * GBA_SCREEN_HEIGHT * 2);
}

static void drawPixel(unsigned short x, unsigned short y, unsigned short color)
{
    GBA_VRAM[x + y * GBA_SCREEN_WIDTH] = color;
}

static void drawChar(unsigned char c, int x, int y, unsigned short fgColour,
                     unsigned short bgColour)
{
    unsigned char mask;
    const unsigned char *font = systemFont + (c * FONT_HEIGHT);
    unsigned short colourToDraw;

    for (int h = 0; h < FONT_HEIGHT; h++) {
        mask = 0x01;

        /* This loop draws 8 pixels at most  / 1 byte per line */
        for (int w = 0; w < FONT_WIDTH; w++) {
            if ((*font) & mask) {
                colourToDraw = fgColour;
            }
            else {
                colourToDraw = bgColour;
            }

            drawPixel(x + w, y + h, colourToDraw);

            mask <<= 1;
        }
        font++;
    }
}

void stdio_init(void)
{
    /* setup bitmap mode */
    GBA_DISPCNT = GBA_DISPCNT_BGMODE_3 | GBA_DISPCNT_SDBG_2;
    clearScreen();
}

ssize_t stdio_read(void *buffer, size_t count)
{
    (void)buffer;
    (void)count;
    return 0;
}

ssize_t stdio_write(const void *buffer, size_t len)
{
    static unsigned short row = 0;
    static unsigned short cursor = 0;

    for (size_t i = 0; i < len; i++) {
        if (cursor >= (GBA_SCREEN_WIDTH / FONT_WIDTH)) {
            cursor = 0;
            row++;
        }
        if (row >= (GBA_SCREEN_HEIGHT / FONT_HEIGHT)) {
            clearScreen();
            row = 0;
        }
        char c = ((char *)buffer)[i];
        if (c == '\n') {
            row++;
            cursor = 0;
            continue;
        }

        drawChar(c, cursor * FONT_WIDTH, row * FONT_HEIGHT, WHITE, BLACK);
        cursor++;
    }
    return len;
}
