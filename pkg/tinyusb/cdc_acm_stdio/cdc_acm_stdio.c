/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pkg_tinyusb
 * @{
 *
 * @file
 * @brief CDC ACM stdio implementation for tinyUSB CDC ACM
 *
 * This file implements a USB CDC ACM callback and read/write functions.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <stdio.h>
#include <sys/types.h>

#include "tusb.h"
#include "tinyusb.h"

static mutex_t data_lock = MUTEX_INIT_LOCKED;

void stdio_init(void)
{
}

#if IS_USED(MODULE_STDIO_AVAILABLE)
int stdio_available(void)
{
    return tud_cdc_available();
}
#endif

ssize_t stdio_read(void* buffer, size_t len)
{
    mutex_lock(&data_lock);
    return tud_cdc_read(buffer, len);
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    const char *start = buffer;

    while (tud_cdc_connected() && len) {
        size_t n = tud_cdc_write(buffer, len);
        buffer = (char *)buffer + n;
        len -= n;
    };

    tud_cdc_write_flush();

    return (char *)buffer - start;
}

void tud_cdc_rx_cb(uint8_t itf)
{
    (void)itf;

    mutex_unlock(&data_lock);
}
