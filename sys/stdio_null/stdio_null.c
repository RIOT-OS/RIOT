/*
 * SPDX-FileCopyrightText: 2019 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_stdio_null
 * @{
 *
 * @file
 * @brief       STDIO null driver
 *
 * This file provides a null driver for STDIO that does not depend on anything.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "stdio_base.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static ssize_t _write(const void* buffer, size_t len)
{
    (void) buffer;
    (void) len;

    return 0;
}

STDIO_PROVIDER(STDIO_NULL, NULL, NULL, _write)
