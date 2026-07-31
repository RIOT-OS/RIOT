/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test debug.h when disabled
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 */

#include <inttypes.h>

#define ENABLE_DEBUG 0
#include "debug.h"

void disabled(void)
{
    volatile uint8_t value = 42;
    (void)value;

    DEBUG_PUTS("debug puts");
    DEBUG("debug printf number '%d'", value);
    DEBUG_CONT(" ... continued\n");
}
