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
 * @brief   USBUS EP0 handling
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */
#include "usb/usbdev.h"
#include "usb/message.h"
#include "usb/usbus.h"
#include "usb/usbus/hdrs.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>
#define ENABLE_DEBUG    (0)
#include "debug.h"

int usbus_update_builder(usbus_t *usbus)
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

static void _activate_endpoints(usbus_t *usbus)
{
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        for (usbus_endpoint_t *ep = iface->ep; ep; ep = ep->next) {
            if (ep->active) {
                static const usbopt_enable_t enable = USBOPT_ENABLE;
                ep->ep->driver->set(ep->ep, USBOPT_EP_ENABLE, &enable,
                                    sizeof(usbopt_enable_t));
                DEBUG("activated endpoint %d, dir %s\n", ep->ep->num,
                      ep->ep->dir == USB_EP_DIR_OUT? "out" : "in");
            }
        }
        for (usbus_interface_alt_t *alt = iface->alts; alt; alt = alt->next) {
            for (usbus_endpoint_t *ep = alt->ep; ep; ep = ep->next) {
                if (ep->active) {
                    static const usbopt_enable_t enable = USBOPT_ENABLE;
                    ep->ep->driver->set(ep->ep, USBOPT_EP_ENABLE, &enable,
                                        sizeof(usbopt_enable_t));
                    DEBUG("activated endpoint %d, dir %s\n", ep->ep->num,
                          ep->ep->dir == USB_EP_DIR_OUT? "out" : "in");
                }
            }
        }
    }
}

static size_t usbus_cpy_str(usbus_t *usbus, const char *str)
{
    size_t len = 0;
    while(*str) {
        usbus_put_char(usbus, *str);
        usbus_put_char(usbus, 0);
        len += 2;
        str++;
    }
    return len;
}

usbus_string_t *_get_descriptor(usbus_t *usbus, uint16_t idx)
{
    for (usbus_string_t * str = usbus->strings; str; str = str->next) {
        if (str->idx == idx) {
            return str;
        }
    }
    return NULL;
}

void _req_status(usbus_t *usbus)
{
    uint8_t status[2];
    memset(status, 0, 2);
    usbus_put_bytes(usbus, status, sizeof(status));
    usbus->in->driver->ready(usbus->in, 2);
}

void _req_str(usbus_t *usbus, uint16_t idx)
{
    if (idx == 0) {
        usb_descriptor_string_t desc;
        desc.type = USB_TYPE_DESCRIPTOR_STRING;
        desc.length = sizeof(uint16_t)+sizeof(usb_descriptor_string_t);
        usbus_put_bytes(usbus, (uint8_t*)&desc, sizeof(desc));
        /* Only one language ID supported */
        uint16_t us = USB_CONFIG_DEFAULT_LANGID;
        usbus_put_bytes(usbus, (uint8_t*)&us, sizeof(uint16_t));
        usbus_ep0_ready(usbus);
    }
    else {
        usb_descriptor_string_t desc;
        desc.type = USB_TYPE_DESCRIPTOR_STRING;
        usbus_string_t *str = _get_descriptor(usbus, idx);
        if (str) {
            desc.length = sizeof(usb_descriptor_string_t);
            desc.length += 2*strlen(str->str);
            usbus_put_bytes(usbus, (uint8_t*)&desc, sizeof(desc));
            usbus_cpy_str(usbus, str->str);
            usbus_ep0_ready(usbus);
        }
        else {
            usbus_ep0_ready(usbus);
        }
    }
}

static void _req_dev(usbus_t *usbus)
{
    usb_descriptor_device_t desc;
    memset(&desc, 0, sizeof(usb_descriptor_device_t));
    desc.length = sizeof(usb_descriptor_device_t);
    desc.type = USB_TYPE_DESCRIPTOR_DEVICE;
    desc.bcd_usb = 0x0110;
    desc.max_packet_size = USBUS_EP0_SIZE;
    desc.vendor_id = USB_CONFIG_VID;
    desc.product_id = USB_CONFIG_PID;
    desc.manufacturer_idx = usbus->manuf.idx;
    desc.product_idx = usbus->product.idx;
    desc.num_configurations = 1;
    usbus_put_bytes(usbus, (uint8_t*)&desc, sizeof(usb_descriptor_device_t));
    usbus_ep0_ready(usbus);
}

static void _req_config(usbus_t *usbus)
{
    usbus_hdrs_fmt_conf(usbus);
    usbus_ep0_ready(usbus);
}

static void _req_dev_qualifier(usbus_t *usbus)
{
    usb_speed_t speed = USB_SPEED_LOW;
    usbus->dev->driver->get(usbus->dev, USBOPT_MAX_SPEED, &speed, sizeof(usb_speed_t));
    if (speed == USB_SPEED_HIGH) {
        /* TODO: implement device qualifier support */
    }
    /* Signal stall to indicate unsupported (USB 2.0 spec 9.6.2 */
    static const usbopt_enable_t enable = USBOPT_ENABLE;

    usbus->in->driver->set(usbus->in, USBOPT_EP_STALL, &enable, sizeof(usbopt_enable_t));
}

static void _req_descriptor(usbus_t *usbus, usb_setup_t *pkt)
{
    uint8_t type = pkt->value >> 8;
    uint8_t idx = (uint8_t)pkt->value;
    switch (type) {
        case 0x1:
            _req_dev(usbus);
            break;
        case 0x2:
            _req_config(usbus);
            break;
        case 0x03:
            _req_str(usbus, idx);
            break;
        case 0x06:
            _req_dev_qualifier(usbus);
            break;
        default:
            break;
    }
}

void recv_dev_setup(usbus_t *usbus, usbdev_ep_t *ep, usb_setup_t *pkt)
{
    (void)ep;
    if (pkt->type & 0x80) {
        switch (pkt->request) {
            case 0x00:
                _req_status(usbus);
                break;
            case 0x06:
                _req_descriptor(usbus, pkt);
                break;
            default:
                break;
        }
    }
    else{
        switch (pkt->request) {
            case 0x05:
                usbus->addr = (uint8_t)pkt->value;
                break;
            case 0x09:
                _activate_endpoints(usbus);
                break;
            default:
                break;
        }
        /* Signal zlp */
        usbus->in->driver->ready(usbus->in, 0);
    }
}

void recv_interface_setup(usbus_t *usbus, usbdev_ep_t *ep, usb_setup_t *pkt)
{
    uint16_t destination = pkt->index & 0x0f;
    (void)ep;
    /* Find interface handler */
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        if (destination == iface->idx) {
            iface->handler->driver->event_handler(usbus, iface->handler, USBUS_MSG_TYPE_SETUP_RQ, pkt);
        }
    }
}

void recv_setup(usbus_t *usbus, usbdev_ep_t *ep)
{
    (void)ep;
    usb_setup_t *pkt = &usbus->setup;
    if (pkt->type & 0x80) {
        usbus->setup_state = USBUS_SETUPRQ_INDATA;
    }
    else {
        if (pkt->length) {
            usbus->setup_state = USBUS_SETUPRQ_OUTDATA;
        }
        else {
            usbus->setup_state = USBUS_SETUPRQ_INACK;
            usbus->in->driver->ready(usbus->in, 0);
        }
    }
    uint8_t destination = pkt->type & USB_SETUP_REQUEST_RECIPIENT_MASK;
    switch(destination) {
        case USB_SETUP_REQUEST_RECIPIENT_DEVICE:
            recv_dev_setup(usbus, ep, pkt);
            break;
        case USB_SETUP_REQUEST_RECIPIENT_INTERFACE:
            recv_interface_setup(usbus, ep, pkt);
            break;
        default:
            DEBUG("usbus: Unhandled setup request\n");
    }
}


/* USB endpoint 0 callback */
void _event_ep0_cb(usbdev_ep_t *ep, usbdev_event_t event)
{
    usbus_t *usbus = (usbus_t *)ep->context;
    if (event == USBDEV_EVENT_ESR) {
        msg_t msg = { .type = USBUS_MSG_TYPE_EP_EVENT,
                      .content = { .ptr = ep} };

        DEBUG("usbus_ep: pid: %u\n", usbus->pid);
        if (msg_send(&msg, usbus->pid) <= 0) {
            puts("usbus_ep0: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case USBDEV_EVENT_TR_COMPLETE:
                /* Configure address if we have received one and handled the zlp */
                if (usbus->setup_state == USBUS_SETUPRQ_INACK && ep->dir == USB_EP_DIR_IN) {
                    if (usbus->addr && usbus->state == USBUS_STATE_RESET) {
                        usbus->dev->driver->set(usbus->dev, USBOPT_ADDRESS, &usbus->addr, sizeof(uint8_t));
                        /* Address configured */
                        usbus->state = USBUS_STATE_ADDR;
                        DEBUG("Setting addres %u\n", usbus->addr);
                    }
                    usbus->setup_state = USBUS_SETUPRQ_READY;
                }
                else if (usbus->setup_state == USBUS_SETUPRQ_OUTACK && ep->dir == USB_EP_DIR_OUT) {
                    memset(&usbus->slicer, 0, sizeof(usbus_controlslicer_t));
                    static const usbopt_enable_t disable = USBOPT_DISABLE;
                    usbus->in->driver->set(usbus->in, USBOPT_EP_READY, &disable, sizeof(usbopt_enable_t));
                    usbus->setup_state = USBUS_SETUPRQ_READY;
                }
                else if (usbus->setup_state == USBUS_SETUPRQ_INDATA && ep->dir == USB_EP_DIR_IN) {
                    if (usbus_update_builder(usbus)) {
                        recv_setup(usbus, ep);
                        usbus->setup_state = USBUS_SETUPRQ_INDATA;
                    }
                    else {
                        /* Ready out ZLP */
                        usbus->setup_state = USBUS_SETUPRQ_OUTACK;
                    }
                }
                else if (usbus->setup_state == USBUS_SETUPRQ_OUTDATA && ep->dir == USB_EP_DIR_OUT) {
                    /* Ready in ZLP */
                    usbus->setup_state = USBUS_SETUPRQ_INACK;
                    usbus->in->driver->ready(usbus->in, 0);
                }
                else if (ep->dir == USB_EP_DIR_OUT) {
                    memset(&usbus->slicer, 0, sizeof(usbus_controlslicer_t));
                    memcpy(&usbus->setup, usbus->out->buf, sizeof(usb_setup_t));
                    usbus->slicer.reqlen = usbus->setup.length;
                    usbus->out->driver->ready(usbus->out, 0);
                    recv_setup(usbus, ep);
                }
                break;
            case USBDEV_EVENT_TR_FAIL:
                if (ep->dir == USB_EP_DIR_OUT) {
                }
                else {
                }
                break;
            case USBDEV_EVENT_TR_STALL:
                {
                    static const usbopt_enable_t disable = USBOPT_DISABLE;
                    ep->driver->set(ep, USBOPT_EP_STALL, &disable, sizeof(usbopt_enable_t));
                }
                break;
            default:
                break;
        }
    }
}

