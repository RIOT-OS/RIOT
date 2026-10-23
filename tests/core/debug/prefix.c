/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test debug.h with prefix
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 */

#include <inttypes.h>

#include "tests.h"

#define ENABLE_DEBUG 1
#define LOG_UNIT "prefix"
#include "debug.h"

void prefix(void)
{
    volatile uint8_t value = 42;
    (void)value;

    DEBUG_PUTS("debug puts");
    DEBUG("debug printf number '%d'", value);
    DEBUG_CONT(" ... continued\n");
}
