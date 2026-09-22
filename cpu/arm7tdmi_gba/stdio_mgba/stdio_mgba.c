/*
 * SPDX-FileCopyrightText: 2026 Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_gba_stdio_mgba
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

#include <string.h>

#include "macros/utils.h"
#include "stdio_base.h"

#define MGBA_LOG_MAX_LINE (256)

/* Log levels we can pick */
#define MGBA_FATAL_ERROR  (0x100) /* This will terminate mGBA after the first print! */
#define MGBA_ERROR        (0x101)
#define MGBA_WARNING      (0x102)
#define MGBA_INFO         (0x103)
#define MGBA_DEBUG        (0x104)

/* We use MGBA_DEBUG as a default */
#ifndef MGBA_LOGLEVEL
#  define MGBA_LOGLEVEL MGBA_DEBUG
#endif

/* magic value to enable the debug log of mGBA */
#define MGBA_LOG_ENABLE_MAGIC (0xC0DE)

#define MGBA_REG_LOG_ENABLE   (uint32_t volatile *)(0x4FFF780)
#define MGBA_REG_LOG_BUFFER   (uint32_t volatile *)(0x4FFF600)
#define MGBA_REG_LOG_SEND     (uint32_t volatile *)(0x4FFF700)

void stdio_init(void)
{
    *MGBA_REG_LOG_ENABLE = MGBA_LOG_ENABLE_MAGIC;
}

ssize_t stdio_read(void *buffer, size_t count)
{
    (void)buffer;
    (void)count;
    return 0;
}

static inline void _send_msg(const void *buffer, size_t len)
{
    memcpy((void *)MGBA_REG_LOG_BUFFER, buffer, len);
    *MGBA_REG_LOG_SEND = MGBA_LOGLEVEL;
}

ssize_t stdio_write(const void *buffer, size_t len)
{
    if (len == 0) {
        return 0;
    }

    size_t remaining = len;

    /* we omit the trailing newline as the mgba logger already adds one for us */
    if (((char *)buffer)[len - 1] == '\n') {
        remaining--;
    }

    while (remaining > 0) {
        size_t chunk = MIN(remaining, MGBA_LOG_MAX_LINE);
        _send_msg(buffer, chunk);
        buffer += chunk;
        remaining -= chunk;
    }

    return len;
}
