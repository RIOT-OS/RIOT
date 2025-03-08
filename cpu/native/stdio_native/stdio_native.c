/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include "kernel_defines.h"
#include "native_internal.h"

#include "stdio_base.h"

void stdio_init(void)
{
}

ssize_t stdio_read(void* buffer, size_t max_len)
{
    return real_read(STDIN_FILENO, buffer, max_len);
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    return real_write(STDOUT_FILENO, buffer, len);
}
