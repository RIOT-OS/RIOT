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

#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"

#if MODULE_VFS
#include "vfs.h"
#endif

#ifdef MODULE_USB_BOARD_RESET
#include "usb_board_reset_internal.h"
#endif

static usbus_cdcacm_device_t cdcacm;
static uint8_t _cdc_tx_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
static uint8_t _cdc_rx_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
static isrpipe_t _cdc_stdio_isrpipe = ISRPIPE_INIT(_cdc_rx_buf_mem);

void stdio_init(void)
{
    /* Initialize this side of the CDC ACM pipe */
#if MODULE_VFS
    vfs_bind_stdio();
#endif
}

#if IS_USED(MODULE_STDIO_AVAILABLE)
int stdio_available(void)
{
    return tsrb_avail(&_cdc_stdio_isrpipe.tsrb);
}
#endif

ssize_t stdio_read(void* buffer, size_t len)
{
    (void)buffer;
    (void)len;
    return isrpipe_read(&_cdc_stdio_isrpipe, buffer, len);
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    const char *start = buffer;
    do {
        size_t n = usbus_cdc_acm_submit(&cdcacm, buffer, len);
        usbus_cdc_acm_flush(&cdcacm);
        /* Use tsrb and flush */
        buffer = (char *)buffer + n;
        len -= n;
    } while (len);
    return (char *)buffer - start;
}

static void _cdc_acm_rx_pipe(usbus_cdcacm_device_t *cdcacm,
                             uint8_t *data, size_t len)
{
    (void)cdcacm;
    for (size_t i = 0; i < len; i++) {
        isrpipe_write_one(&_cdc_stdio_isrpipe, data[i]);
    }
}

void usb_cdc_acm_stdio_init(usbus_t *usbus)
{
    usbus_cdc_acm_init(usbus, &cdcacm, _cdc_acm_rx_pipe, NULL,
                       _cdc_tx_buf_mem, sizeof(_cdc_tx_buf_mem));
#ifdef MODULE_USB_BOARD_RESET
    usbus_cdc_acm_set_coding_cb(&cdcacm, usb_board_reset_coding_cb);
#endif
}
