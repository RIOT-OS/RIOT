/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usb_usbus
 * @{
 * @file
 * @brief   USBUS multipart control message handling
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <string.h>
#include "periph/usbdev.h"
#include "usb/usbus.h"
#include "usb/usbus/control.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int usbus_control_slicer_nextslice(usbus_t *usbus)
{
    usbus_control_handler_t *ep0 = (usbus_control_handler_t *)usbus->control;
    usbus_control_slicer_t *bldr = &ep0->slicer;
    size_t end = bldr->start + ep0->in->len;

    if (bldr->cur > end && bldr->start < bldr->reqlen &&
        bldr->transferred < bldr->reqlen) {
        bldr->start += ep0->in->len;
        bldr->cur = 0;
        bldr->len = 0;
        return 1;
    }
    return 0;
}

size_t usbus_control_slicer_put_bytes(usbus_t *usbus, const uint8_t *buf,
                                      size_t len)
{
    usbus_control_handler_t *ep0 = (usbus_control_handler_t *)usbus->control;
    usbus_control_slicer_t *bldr = &ep0->slicer;
    size_t end = bldr->start + ep0->in->len;
    size_t byte_len = 0;    /* Length of the string to copy */

    /* Calculate start offset of the supplied bytes */
    size_t byte_offset =
        (bldr->start > bldr->cur) ? bldr->start - bldr->cur : 0;

    /* Check for string before or beyond window */
    if ((bldr->cur >= end) || (byte_offset > len)) {
        bldr->cur += len;
        return 0;
    }
    /* Check if string is over the end of the window */
    if ((bldr->cur + len) >= end) {
        byte_len = end - (bldr->cur + byte_offset);
    }
    else {
        byte_len = len - byte_offset;
    }
    size_t start_offset = bldr->cur - bldr->start + byte_offset;
    bldr->cur += len;
    bldr->len += byte_len;
    memcpy(ep0->in->buf + start_offset, buf + byte_offset, byte_len);
    return byte_len;
}

size_t usbus_control_slicer_put_char(usbus_t *usbus, char c)
{
    usbus_control_handler_t *ep0 = (usbus_control_handler_t *)usbus->control;
    usbus_control_slicer_t *bldr = &ep0->slicer;
    size_t end = bldr->start + ep0->in->len;

    /* Only copy the char if it is within the window */
    if ((bldr->start <=  bldr->cur) && (bldr->cur < end)) {
        uint8_t *pos = ep0->in->buf + bldr->cur - bldr->start;
        *pos = c;
        bldr->cur++;
        bldr->len++;
        return 1;
    }
    bldr->cur++;
    return 0;
}

void usbus_control_slicer_ready(usbus_t *usbus)
{
    usbus_control_handler_t *ep0 = (usbus_control_handler_t *)usbus->control;
    usbus_control_slicer_t *bldr = &ep0->slicer;
    size_t len = bldr->len;

    len = len < bldr->reqlen - bldr->start ? len : bldr->reqlen - bldr->start;
    bldr->transferred += len;
    usbdev_ep_ready(ep0->in, len);
}
