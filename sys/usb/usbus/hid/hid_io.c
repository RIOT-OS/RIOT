/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usbus_hid_io
 * @{
 * @file
 *
 * @author  Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <string.h>

#include "isrpipe.h"
#include "isrpipe/read_timeout.h"

#include "usb/usbus.h"
#include "usb/usbus/hid.h"
#include "usb/usbus/hid_io.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static usbus_hid_device_t hid;
static uint8_t _hid_rx_buf_mem[CONFIG_USBUS_HID_INTERRUPT_EP_SIZE];
static isrpipe_t _hid_stdio_isrpipe = ISRPIPE_INIT(_hid_rx_buf_mem);
static usb_hid_io_cb_t _rx_cb;
static void *_rx_cb_arg;

int usb_hid_io_read(void *buffer, size_t len)
{
    assert(buffer);
    return isrpipe_read(&_hid_stdio_isrpipe, buffer, len);
}

int usb_hid_io_read_timeout(void *buffer, size_t len, uint32_t timeout)
{
    assert(buffer);
    return isrpipe_read_timeout(&_hid_stdio_isrpipe, buffer, len, timeout);
}

void usb_hid_io_write(const void *buffer, size_t len)
{
    assert(buffer);
    uint8_t *buffer_ep = hid.in_buf;
    uint16_t max_size = hid.ep_in->maxpacketsize;
    size_t offset = 0;

    while (len) {
        mutex_lock(&hid.in_lock);
        if (len > max_size) {
            memmove(buffer_ep + offset, (uint8_t *)buffer + offset, max_size);
            offset += max_size;
            hid.occupied = max_size;
            len -= max_size;
        }
        else {
            memmove(buffer_ep + offset, (uint8_t *)buffer + offset, len);
            offset += len;
            hid.occupied = len;
            len = 0;
        }
        usbus_event_post(hid.usbus, &hid.tx_ready);
    }
}

static void _hid_rx_pipe(usbus_hid_device_t *hid, uint8_t *data, size_t len)
{
    (void)hid;
    for (size_t i = 0; i < len; i++) {
        isrpipe_write_one(&_hid_stdio_isrpipe, data[i]);
    }

    if (_rx_cb) {
        _rx_cb(_rx_cb_arg);
    }
}

void usb_hid_io_init(usbus_t *usbus, const uint8_t *report_desc,
                     size_t report_desc_size)
{
    assert(usbus);
    assert(report_desc);
    usbus_hid_init(usbus, &hid, _hid_rx_pipe,  report_desc, report_desc_size);
}

void usb_hid_io_set_rx_cb(usb_hid_io_cb_t cb, void *arg)
{
    _rx_cb = cb;
    _rx_cb_arg = arg;
}
