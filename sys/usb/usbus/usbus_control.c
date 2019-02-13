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
 * @brief   USBUS control endpoint handling
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */
#include "usb/usbdev.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/fmt.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>
#define ENABLE_DEBUG    (0)
#include "debug.h"

static void _init(usbus_t *usbus, usbus_handler_t *handler);
static int _handler_ep0_event(usbus_t *usbus, usbus_handler_t *handler, uint16_t event, void *arg);
const usbus_handler_driver_t _ep0_driver = {
    .init = _init,
    .event_handler = _handler_ep0_event,
};

int usbus_ctrlslicer_update(usbus_t *usbus)
{
    usbus_controlslicer_t *bldr = &usbus->slicer;
    size_t end = bldr->start + usbus->in->len;
    if (bldr->cur > end && bldr->start < bldr->reqlen && bldr->transfered < bldr->reqlen) {
        bldr->start += usbus->in->len;
        bldr->cur = 0;
        bldr->len = 0;
        return 1;
    }
    return 0;
}

size_t usbus_ctrlslicer_put_bytes(usbus_t *usbus, const uint8_t *buf, size_t len)
{
    usbus_controlslicer_t *builder = &usbus->slicer;
    size_t end = builder->start + usbus->in->len;
    size_t byte_len = 0;    /* Length of the string to copy */

    /* Calculate start offset of the supplied bytes */
    size_t byte_offset = (builder->start > builder->cur) ? builder->start - builder->cur : 0;

    /* Check for string before or beyond window */
    if ((builder->cur >= end) || (byte_offset > len)) {
        builder->cur += len;
        return 0;
    }
    /* Check if string is over the end of the window */
    if ((builder->cur + len) >= end) {
        byte_len = end - (builder->cur + byte_offset);
    }
    else {
        byte_len = len - byte_offset;
    }
    size_t start_offset = builder->cur - builder->start + byte_offset;
    builder->cur += len;
    builder->len += byte_len;
    memcpy(usbus->in->buf + start_offset , buf + byte_offset, byte_len);
    return byte_len;
}

size_t usbus_ctrlslicer_put_char(usbus_t *usbus, char c)
{
    usbus_controlslicer_t *builder = &usbus->slicer;
    size_t end = builder->start + usbus->in->len;
    /* Only copy the char if it is within the window */
    if ((builder->start <=  builder->cur) && (builder->cur < end)) {
        uint8_t *pos = usbus->in->buf + builder->cur - builder->start;
        *pos = c;
        builder->cur++;
        builder->len++;
        return 1;
    }
    builder->cur++;
    return 0;
}

void usbus_ctrlslicer_ready(usbus_t *usbus)
{
    usbus_controlslicer_t *bldr = &usbus->slicer;
    size_t len = bldr->len;
    len = len < bldr->reqlen - bldr->start ? len : bldr->reqlen - bldr->start;
    bldr->transfered += len;
    usbdev_ep_ready(usbus->in, len);
}

static void _activate_endpoints(usbus_t *usbus)
{
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        for (usbus_endpoint_t *ep = iface->ep; ep; ep = ep->next) {
            if (ep->active) {
                static const usbopt_enable_t enable = USBOPT_ENABLE;
                usbdev_ep_set(ep->ep, USBOPT_EP_ENABLE, &enable,
                                    sizeof(usbopt_enable_t));
                DEBUG("activated endpoint %d, dir %s\n", ep->ep->num,
                      ep->ep->dir == USB_EP_DIR_OUT? "out" : "in");
            }
        }
        for (usbus_interface_alt_t *alt = iface->alts; alt; alt = alt->next) {
            for (usbus_endpoint_t *ep = alt->ep; ep; ep = ep->next) {
                if (ep->active) {
                    static const usbopt_enable_t enable = USBOPT_ENABLE;
                    usbdev_ep_set(ep->ep, USBOPT_EP_ENABLE, &enable,
                                        sizeof(usbopt_enable_t));
                    DEBUG("activated endpoint %d, dir %s\n", ep->ep->num,
                          ep->ep->dir == USB_EP_DIR_OUT? "out" : "in");
                }
            }
        }
    }
}

static size_t usbus_cpy_str_to_utf16(usbus_t *usbus, const char *str)
{
    size_t len = 0;
    while(*str) {
        usbus_ctrlslicer_put_char(usbus, *str);
        usbus_ctrlslicer_put_char(usbus, 0);
        len += 2; /* Two bytes added each iteration */
        str++;
    }
    return len;
}

static usbus_string_t *_get_descriptor(usbus_t *usbus, uint16_t idx)
{
    for (usbus_string_t * str = usbus->strings; str; str = str->next) {
        if (str->idx == idx) {
            return str;
        }
    }
    return NULL;
}

static int _req_status(usbus_t *usbus)
{
    uint8_t status[2];
    memset(status, 0, sizeof(status));
    usbus_ctrlslicer_put_bytes(usbus, status, sizeof(status));
    usbdev_ep_ready(usbus->in, sizeof(status));
    return 0;
}

static int _req_str(usbus_t *usbus, uint16_t idx)
{
    if (idx == 0) {
        usb_descriptor_string_t desc;
        desc.type = USB_TYPE_DESCRIPTOR_STRING;
        desc.length = sizeof(uint16_t)+sizeof(usb_descriptor_string_t);
        usbus_ctrlslicer_put_bytes(usbus, (uint8_t*)&desc, sizeof(desc));
        /* Only one language ID supported */
        uint16_t us = USB_CONFIG_DEFAULT_LANGID;
        usbus_ctrlslicer_put_bytes(usbus, (uint8_t*)&us, sizeof(uint16_t));
    }
    else {
        usb_descriptor_string_t desc;
        desc.type = USB_TYPE_DESCRIPTOR_STRING;
        usbus_string_t *str = _get_descriptor(usbus, idx);
        if (str) {
            desc.length = sizeof(usb_descriptor_string_t);
            desc.length += 2*strlen(str->str); /* USB strings are UTF-16 */
            usbus_ctrlslicer_put_bytes(usbus, (uint8_t*)&desc, sizeof(desc));
            usbus_cpy_str_to_utf16(usbus, str->str);
        }
    }
    return 1;
}

static int _req_dev(usbus_t *usbus)
{
    usb_descriptor_device_t desc;
    memset(&desc, 0, sizeof(usb_descriptor_device_t));
    desc.length = sizeof(usb_descriptor_device_t);
    desc.type = USB_TYPE_DESCRIPTOR_DEVICE;
    desc.bcd_usb = 0x0200;
    desc.max_packet_size = USBUS_EP0_SIZE;
    desc.vendor_id = USB_CONFIG_VID;
    desc.product_id = USB_CONFIG_PID;
    desc.manufacturer_idx = usbus->manuf.idx;
    desc.product_idx = usbus->product.idx;
    desc.num_configurations = 1;
    usbus_ctrlslicer_put_bytes(usbus, (uint8_t*)&desc, sizeof(usb_descriptor_device_t));
    return 1;
}

static int _req_config(usbus_t *usbus)
{
    usbus_hdrs_fmt_conf(usbus);
    return 1;
}

static int _req_dev_qualifier(usbus_t *usbus)
{
    usb_speed_t speed = USB_SPEED_LOW;
    usbus->dev->driver->get(usbus->dev, USBOPT_MAX_SPEED, &speed, sizeof(usb_speed_t));
    if (speed == USB_SPEED_HIGH) {
        /* TODO: implement device qualifier support (only required
         * for High speed) */
    }
    /* Signal stall to indicate unsupported (USB 2.0 spec 9.6.2 */
    static const usbopt_enable_t enable = USBOPT_ENABLE;
    usbdev_ep_set(usbus->in, USBOPT_EP_STALL, &enable, sizeof(usbopt_enable_t));

    return 0;
}

static int _req_descriptor(usbus_t *usbus, usb_setup_t *pkt)
{
    uint8_t type = pkt->value >> 8;
    uint8_t idx = (uint8_t)pkt->value;
    /* Decode descriptor type */
    switch (type) {
        case USB_TYPE_DESCRIPTOR_DEVICE:
            return _req_dev(usbus);
        case USB_TYPE_DESCRIPTOR_CONFIGURATION:
            return _req_config(usbus);
        case USB_TYPE_DESCRIPTOR_STRING:
            return _req_str(usbus, idx);
        case USB_TYPE_DESCRIPTOR_DEV_QUALIFIER:
            return _req_dev_qualifier(usbus);
        default:
            return 0;
    }
}

static int _recv_dev_setup(usbus_t *usbus, usb_setup_t *pkt)
{
    int res = 0;
    if (usb_setup_is_read(pkt)) {
        switch (pkt->request) {
            case USB_SETUP_REQ_GET_STATUS:
                res = _req_status(usbus);
                break;
            case USB_SETUP_REQ_GET_DESCRIPTOR:
                res = _req_descriptor(usbus, pkt);
                break;
            default:
                break;
        }
    }
    else{
        switch (pkt->request) {
            case USB_SETUP_REQ_SET_ADDRESS:
                usbus->addr = (uint8_t)pkt->value;
                break;
            case USB_SETUP_REQ_SET_CONFIGURATION:
                /* Nothing configuration dependent to do here, only one
                 * configuration supported */
                _activate_endpoints(usbus);
                break;
            default:
                break;
        }
        /* Signal zero-length packet */
        usbdev_ep_ready(usbus->in, 0);
    }
    return res;
}

static int _recv_interface_setup(usbus_t *usbus, usb_setup_t *pkt)
{
    uint16_t destination = pkt->index & 0x0f;
    /* Find interface handler */
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        if (destination == iface->idx) {
            return iface->handler->driver->event_handler(usbus, iface->handler, USBUS_MSG_TYPE_SETUP_RQ, pkt);
        }
    }
    return 0;
}

static void _recv_setup(usbus_t *usbus, usbus_ep0_handler_t *handler)
{
    usb_setup_t *pkt = &handler->setup;
    if (usb_setup_is_read(pkt)) {
        handler->setup_state = USBUS_SETUPRQ_INDATA;
    }
    else {
        if (pkt->length) {
            handler->setup_state = USBUS_SETUPRQ_OUTDATA;
        }
        else {
            handler->setup_state = USBUS_SETUPRQ_INACK;
            /* Signal zero-length packet as ACK */
            usbdev_ep_ready(usbus->in, 0);
        }
    }
    uint8_t destination = pkt->type & USB_SETUP_REQUEST_RECIPIENT_MASK;
    int res = 0;
    switch(destination) {
        case USB_SETUP_REQUEST_RECIPIENT_DEVICE:
            res = _recv_dev_setup(usbus, pkt);
            break;
        case USB_SETUP_REQUEST_RECIPIENT_INTERFACE:
            res = _recv_interface_setup(usbus, pkt);
            break;
        default:
            DEBUG("usbus: Unhandled setup request\n");
    }
    if (res) {
        usbus_ctrlslicer_ready(usbus);
    }
}

static void _usbus_config_ep0(usbus_t *usbus)
{
    static const usbopt_enable_t enable = USBOPT_ENABLE;
    usbdev_ep_set(usbus->in, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
    usbdev_ep_set(usbus->out, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
    usbdev_ep_ready(usbus->out, 0);
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    DEBUG("Initializing EP0\n");
    usbus_ep0_handler_t *ep0_handler = (usbus_ep0_handler_t*)handler;
    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);
    ep0_handler->setup_state = USBUS_SETUPRQ_READY;

    usbus->in = usbdev_new_ep(usbus->dev, USB_EP_TYPE_CONTROL, USB_EP_DIR_IN, USBUS_EP0_SIZE);
    usbus->out = usbdev_new_ep(usbus->dev, USB_EP_TYPE_CONTROL, USB_EP_DIR_OUT, USBUS_EP0_SIZE);

    usbdev_ep_init(usbus->in);
    usbdev_ep_init(usbus->out);
    _usbus_config_ep0(usbus);
    DEBUG("initialized EP0 at 0x%x (IN) and 0x%x (OUT)\n", usbus->in->num, usbus->out->num);
}

/* USB endpoint 0 callback */
static int _handler_ep0_event(usbus_t *usbus, usbus_handler_t *handler, uint16_t event, void *arg)
{
    usbus_ep0_handler_t *ep0_handler = (usbus_ep0_handler_t*)handler;
    usbdev_ep_t *ep = (usbdev_ep_t *)arg;
    switch (event) {
        case USBUS_MSG_TYPE_TR_COMPLETE:
            /* Configure address if we have received one and handled the zlp */
            if (ep0_handler->setup_state == USBUS_SETUPRQ_INACK && ep->dir == USB_EP_DIR_IN) {
                if (usbus->addr && usbus->state == USBUS_STATE_RESET) {
                    usbdev_set(usbus->dev, USBOPT_ADDRESS, &usbus->addr, sizeof(uint8_t));
                    /* Address configured */
                    usbus->state = USBUS_STATE_ADDR;
                    DEBUG("Setting addres %u\n", usbus->addr);
                }
                ep0_handler->setup_state = USBUS_SETUPRQ_READY;
            }
            else if (ep0_handler->setup_state == USBUS_SETUPRQ_OUTACK && ep->dir == USB_EP_DIR_OUT) {
                memset(&usbus->slicer, 0, sizeof(usbus_controlslicer_t));
                static const usbopt_enable_t disable = USBOPT_DISABLE;
                usbdev_ep_set(usbus->in, USBOPT_EP_READY, &disable, sizeof(usbopt_enable_t));
                ep0_handler->setup_state = USBUS_SETUPRQ_READY;
            }
            else if (ep0_handler->setup_state == USBUS_SETUPRQ_INDATA && ep->dir == USB_EP_DIR_IN) {
                if (usbus_ctrlslicer_update(usbus)) {
                    _recv_setup(usbus, ep0_handler);
                    ep0_handler->setup_state = USBUS_SETUPRQ_INDATA;
                }
                else {
                    /* Ready out ZLP */
                    ep0_handler->setup_state = USBUS_SETUPRQ_OUTACK;
                }
            }
            else if (ep0_handler->setup_state == USBUS_SETUPRQ_OUTDATA && ep->dir == USB_EP_DIR_OUT) {
                /* Ready in ZLP */
                ep0_handler->setup_state = USBUS_SETUPRQ_INACK;
                usbdev_ep_ready(usbus->in, 0);
            }
            else if (ep->dir == USB_EP_DIR_OUT) {
                memset(&usbus->slicer, 0, sizeof(usbus_controlslicer_t));
                memcpy(&ep0_handler->setup, usbus->out->buf, sizeof(usb_setup_t));
                usbus->slicer.reqlen = ep0_handler->setup.length;
                usbdev_ep_ready(usbus->out, 0);
                _recv_setup(usbus, ep0_handler);
            }
            break;
        case USBUS_MSG_TYPE_RESET:
            DEBUG("Reset, re-enabling EP0\n");
            ep0_handler->setup_state = USBUS_SETUPRQ_READY;
            _usbus_config_ep0(usbus);
            break;
        case USBUS_MSG_TYPE_TR_FAIL:
            break;
        case USBUS_MSG_TYPE_TR_STALL:
            {
                static const usbopt_enable_t disable = USBOPT_DISABLE;
                usbdev_ep_set(ep, USBOPT_EP_STALL, &disable, sizeof(usbopt_enable_t));
            }
            break;
        default:
            break;
    }
    return 0;
}

