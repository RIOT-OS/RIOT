/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Dummy STDIO implementation that does nothing to preserve energy
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "stdio_base.h"

void stdio_init(void)
{
    /* do nothing */
}

ssize_t stdio_read(void* buffer, size_t count)
{
    (void)buffer;
    (void)count;
    return 0;
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    (void)buffer;
    return (ssize_t)len;
}
