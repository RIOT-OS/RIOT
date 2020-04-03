/*
 * Copyright (C) 2019 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
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

#if MODULE_VFS
#include "vfs.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

void stdio_init(void)
{
#if MODULE_VFS
    vfs_bind_stdio();
#endif
}

ssize_t stdio_read(void* buffer, size_t count)
{
    (void) buffer;
    (void) count;

    return 0;
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    (void) buffer;
    (void) len;

    return 0;
}
