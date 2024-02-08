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
#include "stdio_base.h"

#include "tusb.h"
#include "tinyusb.h"

static ssize_t _write(const void* buffer, size_t len)
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

#ifdef MODULE_STDIN
void tud_cdc_rx_cb(uint8_t itf)
{
    (void)itf;

    uint8_t buffer[64];
    unsigned res = tud_cdc_read(buffer, sizeof(buffer));
    isrpipe_write(&stdin_isrpipe, buffer, res);
}
#endif

STDIO_PROVIDER(STDIO_TINYUSB_CDC_ACM, NULL, NULL, _write)
