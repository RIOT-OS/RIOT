/*
 * Copyright (C) 2019 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
