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
 * @brief   USBUS USB manager thread, handles driver interaction and
 *          EP0 management
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include "thread.h"
#include "byteorder.h"
#include "usb/usbdev.h"
#include "usb/message.h"
#include "usb/usbus.h"
#include "usb/usbus/hdrs.h"

#include "usb.h"
#include "cpu.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _USBUS_MSG_QUEUE_SIZE    (8)

void _event_cb(usbdev_t *usbdev, usbdev_event_t event);
void _event_ep0_cb(usbdev_ep_t *ep, usbdev_event_t event);
void _event_ep_cb(usbdev_ep_t *ep, usbdev_event_t event);
static void *_usbus_thread(void *args);

void usbus_init(usbus_t *usbus, usbdev_t *usbdev)
{
    /* TODO: Check if memset works/is necessary */
    memset(usbus, 0, sizeof(usbus));
    usbus->dev = usbdev;
}

void usbus_create(char *stack, int stacksize, char priority,
                   const char *name, usbus_t *usbus)
{
    int res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                        _usbus_thread, (void *)usbus, name);
    (void)res;
    assert(res > 0);
}

size_t usbus_put_bytes(usbus_t *usbus, const uint8_t *buf, size_t len)
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

size_t usbus_put_char(usbus_t *usbus, char c)
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

uint16_t usbus_add_string_descriptor(usbus_t *usbus, usbus_string_t *desc, const char *str)
{
    desc->next = usbus->strings;
    usbus->strings = desc;
    desc->idx = usbus->str_idx++;
    desc->str = str;
    DEBUG("usbus: Adding string descriptor number %u for: \"%s\"\n", desc->idx, str);
    return desc->idx;
}

void usbus_add_conf_descriptor(usbus_t *usbus, usbus_hdr_gen_t* hdr_gen)
{
    hdr_gen->next = usbus->hdr_gen;
    usbus->hdr_gen = hdr_gen;
}

void usbus_ep0_ready(usbus_t *usbus)
{
    usbus_controlslicer_t *bldr = &usbus->slicer;
    size_t len = bldr->len;
    len = len < bldr->reqlen - bldr->start ? len : bldr->reqlen - bldr->start;
    bldr->transfered += len;
    usbus->in->driver->ready(usbus->in, len);
}

usbus_interface_t *_ep_to_iface(usbus_t *usbus, usbdev_ep_t *ep)
{
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        for (usbus_endpoint_t *pep = iface->ep; pep; pep = pep->next) {
            if (pep->ep == ep) {
                return iface;
            }
        }
        for (usbus_interface_alt_t *alt = iface->alts; alt; alt = alt->next) {
            for (usbus_endpoint_t *pep = alt->ep; pep; pep = pep->next) {
                if (pep->ep == ep) {
                    return iface;
                }
            }
        }
    }
    return NULL;
}

uint16_t usbus_add_interface(usbus_t *usbus, usbus_interface_t *iface)
{
    iface->next = usbus->iface;
    usbus->iface = iface;
    return iface->idx;
}

void usbus_register_event_handler(usbus_t *usbus, usbus_handler_t *handler)
{
    handler->next = usbus->handler;
    usbus->handler = handler;
}

int usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface, usbus_endpoint_t* ep, usb_ep_type_t type, usb_ep_dir_t dir, size_t len)
{
    int res = -ENOMEM;
    usbdev_ep_t* usbdev_ep = usbus->dev->driver->new_ep(usbus->dev, type, dir, len);
    if (usbdev_ep) {
        ep->maxpacketsize = usbdev_ep->len;
        usbdev_ep->context = usbus;
        usbdev_ep->cb = _event_ep_cb;
        ep->ep = usbdev_ep;
        ep->next = iface->ep;
        iface->ep = ep;
        res = 0;
    }
    return res;
}

static void _usbus_config_ep0(usbus_t *usbus)
{
    static const usbopt_enable_t enable = USBOPT_ENABLE;
    usbus->in->driver->set(usbus->in, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
    usbus->out->driver->set(usbus->out, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
    usbus->out->driver->ready(usbus->out, 0);
}

static inline size_t usbus_pkt_maxlen(usbus_t *usbus, usb_setup_t *pkt)
{
    return pkt->length > usbus->in->len ? usbus->in->len : pkt->length;
}

static void _usbus_init_handlers(usbus_t *usbus)
{
    for(usbus_handler_t *handler = usbus->handler; handler;
            handler = handler->next) {
        handler->driver->init(usbus, handler);
    }
}

static void *_usbus_thread(void *args)
{
    usbus_t *usbus = (usbus_t*)args;

    usbdev_t *dev = usbus->dev;
    usbus->pid = sched_active_pid;
    usbus->addr = 0;
    usbus->strings = NULL;
    usbus->iface = NULL;
    usbus->str_idx = 1;
    usbus->setup_state = USBUS_SETUPRQ_READY;
    msg_t msg, msg_queue[_USBUS_MSG_QUEUE_SIZE];
    DEBUG("usbus: starting thread %i\n", sched_active_pid);
    /* setup the link-layer's message queue */
    msg_init_queue(msg_queue, _USBUS_MSG_QUEUE_SIZE);
    /* register the event callback with the device driver */
    dev->cb = _event_cb;
    /* initialize low-level driver */
    dev->driver->init(dev);
    dev->context = usbus;

    usbus->in = usbus->dev->driver->new_ep(usbus->dev, USB_EP_TYPE_CONTROL, USB_EP_DIR_IN, USBUS_EP0_SIZE);
    usbus->out = usbus->dev->driver->new_ep(usbus->dev, USB_EP_TYPE_CONTROL, USB_EP_DIR_OUT, USBUS_EP0_SIZE);
    usbus->in->cb = _event_ep0_cb;
    usbus->out->cb = _event_ep0_cb;
    usbus->in->context = usbus;
    usbus->out->context = usbus;

    usbus->in->driver->init(usbus->in);
    usbus->out->driver->init(usbus->out);
    _usbus_config_ep0(usbus);
    usbus_add_string_descriptor(usbus, &usbus->config, USB_CONFIG_CONFIGURATION_STR);
    usbus_add_string_descriptor(usbus, &usbus->product, USB_CONFIG_PRODUCT_STR);
    usbus_add_string_descriptor(usbus, &usbus->manuf, USB_CONFIG_MANUF_STR);

    usbus->state = USBUS_STATE_DISCONNECT;

    /* Initialize handlers */
    _usbus_init_handlers(usbus);

//#if(USBUS_AUTO_ATTACH)
    usbopt_enable_t enable = USBOPT_ENABLE;
    dev->driver->set(dev, USBOPT_ATTACH, &enable, sizeof(usbopt_enable_t));
//#endif

    while (1) {
        msg_receive(&msg);
        /* dispatch USBUS messages */
        switch (msg.type & 0xFF00) {
            case USBUS_MSG_TYPE_EVENT:
                dev->driver->esr(dev);
                break;
            case USBUS_MSG_TYPE_EP_EVENT:
                {
                    usbdev_ep_t *ep = (usbdev_ep_t*)msg.content.ptr;
                    ep->driver->esr(ep);
                }
              break;
            case USBUS_MSG_TYPE_HANDLER:
                {
                    usbus_handler_t *handler = (usbus_handler_t*)msg.content.ptr;
                    handler->driver->event_handler(usbus, handler, msg.type, handler);
                }
              break;
            default:
                DEBUG("usbus: unhandled event\n");
                break;
        }
    }
    return NULL;
}

/* USB event callback */
void _event_cb(usbdev_t *usbdev, usbdev_event_t event)
{
    usbus_t *usbus = (usbus_t *)usbdev->context;
    if (event == USBDEV_EVENT_ESR) {
        msg_t msg = { .type = USBUS_MSG_TYPE_EVENT,
                      .content = { .ptr = usbdev } };

        if (msg_send(&msg, usbus->pid) <= 0) {
            puts("usbus: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case USBDEV_EVENT_RESET:
                {
                usbus->state = USBUS_STATE_RESET;
                usbus->addr = 0;
                usbus->setup_state = USBUS_SETUPRQ_READY;
                usbus->dev->driver->set(usbus->dev, USBOPT_ADDRESS, &usbus->addr, sizeof(uint8_t));
                DEBUG("USB reset condition detected\n");
                }
                break;
            default:
                DEBUG("usbus: unhandled event\n");
                break;
        }
    }
}

/* USB generic endpoint callback */
void _event_ep_cb(usbdev_ep_t *ep, usbdev_event_t event)
{
    usbus_t *usbus = (usbus_t *)ep->context;
    if (event == USBDEV_EVENT_ESR) {
        msg_t msg = { .type = USBUS_MSG_TYPE_EP_EVENT,
                      .content = { .ptr = ep} };

        if (msg_send(&msg, usbus->pid) <= 0) {
            puts("usbus_ep: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
            case USBDEV_EVENT_TR_COMPLETE:
                {
                    usbus_interface_t *iface = _ep_to_iface(usbus, ep);
                    if (iface) {
                        iface->handler->driver->event_handler(usbus, iface->handler, USBUS_MSG_TYPE_TR_COMPLETE, ep);
                    }
                    else {
                    }
                    if (ep->dir == USB_EP_DIR_OUT)
                    {
                        ep->driver->ready(ep, 0);
                    }
                }
                break;
            case USBDEV_EVENT_TR_FAIL:
                break;
            case USBDEV_EVENT_TR_STALL:
                {
                    static const usbopt_enable_t disable = USBOPT_DISABLE;
                    ep->driver->set(ep, USBOPT_EP_STALL, &disable, sizeof(usbopt_enable_t));
                }
                break;
            default:
                puts("unhandled event");
                break;
        }
    }
}
