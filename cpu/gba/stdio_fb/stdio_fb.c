/*
 * Copyright (C) 2023 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "periph_cpu.h"
#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static const unsigned char systemFont[] =
    {
#include "font_terminal.h"
};

#define BLUE 0x7C00
#define RED 0x001F
#define GREEN 0x03E0
#define BLACK 0x0000
#define WHITE 0xFFFF

// unsigned short *videoctl_base = GBA_DISPCNT;
// unsigned short *framebuffer = (unsigned short *)FRAMEBUFFER;

void clearScreen(void)
{
    memset(GBA_VRAM, 0x00, 240 * 160 * 2);
}

void drawPixel(unsigned short x, unsigned short y, unsigned short color)
{
    GBA_VRAM[x + y * 240] = color;
}

static void drawChar(unsigned char c, int x, int y, unsigned short fgColour, unsigned short bgColour)
{
    unsigned char mask;
    const unsigned char *font = systemFont + (c * FONT_HEIGHT);
    unsigned short colourToDraw;

    for (int h = 0; h < FONT_HEIGHT; h++)
    {
        mask = 0x01;

        // This loop draws up to 1 byte (8 bit) per line; so 8 pixels at most
        for (int w = 0; w < FONT_WIDTH; w++)
        {
            if ((*font) & mask)
            {
                colourToDraw = fgColour;
            }
            else
            {
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
    GBA_DISPCNT = GBA_DISPCNT_BGMODE_3 | GBA_DISPCNT_SDBG_2;
    clearScreen();
}

ssize_t stdio_read(void *buffer, size_t count)
{
    static int first_run = 1;
#define HELP "help\n"
    (void)buffer;
    (void)count;
    if (first_run)
    {
        first_run = 0;
        memcpy(buffer, HELP, sizeof(HELP));
        return sizeof(HELP);
    }
    while (1)
        ;
    return 0;
}

ssize_t stdio_write(const void *buffer, size_t len)
{
    static unsigned short row = 0;
    static unsigned short cursor = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (cursor >= (240 / FONT_WIDTH))
        {
            cursor = 0;
            row++;
        }
        if (row >= (160 / FONT_HEIGHT))
        {
            clearScreen();
            row = 0;
        }
        char c = ((char *)buffer)[i];
        if (c == '\n')
        {
            row++;
            cursor = 0;
            continue;
        }

        drawChar(c, cursor * FONT_WIDTH, row * FONT_HEIGHT, WHITE, BLACK);
        cursor++;
    }
    return len;
}
