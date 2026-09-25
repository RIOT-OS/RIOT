/*
 * SPDX-FileCopyrightText: 2026 Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_gba_stdio_fb
 * @{
 *
 * @file
 * @brief       STDIO via mGBA debug registers
 *
 * This file provides a driver for STDIO, implementing stdout only.
 *
 * @author      Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 *
 * @}
 */

#include "stdio_base.h"
#include "periph_gba.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define MGBA_LOG_MAX_LINE 256
#define MGBA_ERROR 0x101
#define MGBA_WARNING 0x102
#define MGBA_INFO 0x103
#define MGBA_DEBUG 0x104
#define REG_LOG_ENABLE  (uint32_t volatile *)(0x4FFF780)
#define REG_LOG_BUFFER (uint32_t volatile *)(0x4FFF600)
#define REG_LOG_SEND (uint32_t volatile *)(0x4FFF700)


void stdio_init(void)
{
    /* nop */
}

ssize_t stdio_read(void *buffer, size_t count)
{
    (void)buffer;
    (void)count;
    return 0;
}

static inline void _send_msg(const void *buffer, size_t len)
{
    *REG_LOG_ENABLE = 0xC0DE;
    memcpy((void *)REG_LOG_BUFFER, buffer, len);
    *REG_LOG_SEND = 0x104;
}

ssize_t stdio_write(const void *buffer, size_t len)
{
    size_t trailing_newline = ((char *)buffer)[len - 1] == '\n' ? 1 : 0;

    /* not printing a single newline */
    if (trailing_newline && len == 1) {
        return len;
    }

    size_t offset = 0;
    while (len - trailing_newline - offset > MGBA_LOG_MAX_LINE) {
        _send_msg(buffer + offset, MGBA_LOG_MAX_LINE);
        offset += MGBA_LOG_MAX_LINE;
    }

    _send_msg(buffer + offset, len - trailing_newline - offset);

    return len;
}
