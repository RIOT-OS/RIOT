/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test debug.h with prefix and thread and function name
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 */

#include <inttypes.h>

#include "tests.h"

/* this would typically be set application-wide via Kconfig or make */
#define CONFIG_DEBUG_SHOW_THREAD 1
#define CONFIG_DEBUG_SHOW_FUNC 1

#define ENABLE_DEBUG 1
#define DEBUG_UNIT "prefix"
#include "debug.h"

void prefix_thread_func(void)
{
    volatile uint8_t value = 42;
    (void)value;

    DEBUG_PUTS("debug puts");
    DEBUG("debug printf number '%d'", value);
    DEBUG_CONT(" ... continued\n");
}
