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
#include "graphics.h"
#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"

void stdio_init(void)
{
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
