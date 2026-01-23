/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys
 * @{
 *
 * @file
 * @brief CDC ACM stdio implementation for USBUS CDC ACM
 *
 * This file implements a USB CDC ACM callback and read/write functions.
 *
 *
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <stdio.h>
#include <sys/types.h>

#include "log.h"
#include "isrpipe.h"
#include "stdio_base.h"

#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"

static usbus_cdcacm_device_t cdcacm;
static uint8_t _cdc_tx_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];

static ssize_t _write(const void* buffer, size_t len)
{
    const char *start = buffer;
    while (len) {
        size_t n = usbus_cdc_acm_submit(&cdcacm, buffer, len);
        usbus_cdc_acm_flush(&cdcacm);
        /* Use tsrb and flush */
        buffer = (char *)buffer + n;
        len -= n;
    }
    return (char *)buffer - start;
}

static void _cdc_acm_rx_pipe(usbus_cdcacm_device_t *cdcacm,
                             uint8_t *data, size_t len)
{
    (void)cdcacm;
    isrpipe_write(&stdin_isrpipe, data, len);
}

void usb_cdc_acm_stdio_init(usbus_t *usbus)
{
    usbus_cdc_acm_init(usbus, &cdcacm, _cdc_acm_rx_pipe, NULL,
                       _cdc_tx_buf_mem, sizeof(_cdc_tx_buf_mem));
}

STDIO_PROVIDER(STDIO_USBUS_CDC_ACM, NULL, NULL, _write)
