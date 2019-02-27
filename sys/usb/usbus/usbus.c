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
 * @brief   USBUS USB manager thread, handles USB interaction
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include "thread.h"
#include "byteorder.h"
#include "usb/usbdev.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/fmt.h"

#include "usb.h"
#include "cpu.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _USBUS_MSG_QUEUE_SIZE    (16)
extern const usbus_handler_driver_t _ep0_driver;

/* Forward declaration of the generic USBUS event callback */
static void _event_cb(usbdev_t *usbdev, usbdev_event_t event);
/* Forward declaration of the endpoint USBUS event callback */
static void _event_ep_cb(usbdev_ep_t *ep, usbdev_event_t event);

static void *_usbus_thread(void *args);

void usbus_init(usbus_t *usbus, usbdev_t *usbdev)
{
    memset(usbus, 0, sizeof(usbus_t));
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

usbus_handler_t *_ep_to_handler(usbus_t *usbus, usbdev_ep_t *ep)
{
    if (ep->num == 0) {
        return usbus->handlers;
    }
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        for (usbus_endpoint_t *pep = iface->ep; pep; pep = pep->next) {
            if (pep->ep == ep) {
                return iface->handler;
            }
        }
        for (usbus_interface_alt_t *alt = iface->alts; alt; alt = alt->next) {
            for (usbus_endpoint_t *pep = alt->ep; pep; pep = pep->next) {
                if (pep->ep == ep) {
                    return iface->handler;
                }
            }
        }
    }
    return NULL;
}

uint16_t usbus_add_interface(usbus_t *usbus, usbus_interface_t *iface)
{
    /* While it is possible to us clist.h here, this results in less flash
     * usages. Furthermore, the O(1) append is not really necessary as this is
     * only used at init */
    uint16_t idx = 0;
    usbus_interface_t *last = usbus->iface;
    if (last) {
        idx++;
        while (last->next) {
            last = last->next;
            idx++;
        }
        last->next = iface;
    }
    else {
        usbus->iface = iface;
    }
    iface->idx = idx;
    return idx;
}

void usbus_register_event_handler(usbus_t *usbus, usbus_handler_t *handler)
{
    /* See note above for reasons against clist.h */
    usbus_handler_t *last = usbus->handlers;
    if (last) {
        while (last->next) {
            last = last->next;
        }
        last->next = handler;
    }
    else {
        usbus->handlers = handler;
    }
}

int usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface, usbus_endpoint_t* ep, usb_ep_type_t type, usb_ep_dir_t dir, size_t len)
{
    int res = -ENOMEM;
    usbdev_ep_t* usbdev_ep = usbdev_new_ep(usbus->dev, type, dir, len);
    if (usbdev_ep) {
        ep->maxpacketsize = usbdev_ep->len;
        ep->ep = usbdev_ep;
        ep->next = iface->ep;
        iface->ep = ep;
        res = 0;
    }
    return res;
}

static inline size_t usbus_pkt_maxlen(usbus_t *usbus, usb_setup_t *pkt)
{
    return pkt->length > usbus->in->len ? usbus->in->len : pkt->length;
}

static void _usbus_init_handlers(usbus_t *usbus)
{
    for (usbus_handler_t *handler = usbus->handlers;
            handler; handler = handler->next) {
        handler->driver->init(usbus, handler);
    }
}

static int ep0_init(usbus_t *usbus, usbus_ep0_handler_t *handler)
{
    handler->handler.driver = &_ep0_driver;

    /* Ensure that ep0 is the first handler */
    handler->handler.next = usbus->handlers;
    usbus->handlers = &handler->handler;
    return 0;
}

static void *_usbus_thread(void *args)
{
    usbus_t *usbus = (usbus_t*)args;
    usbus_ep0_handler_t ep0_handler;

    ep0_init(usbus, &ep0_handler);

    usbdev_t *dev = usbus->dev;
    usbus->pid = sched_active_pid;
    usbus->addr = 0;
    usbus->iface = NULL;
    usbus->str_idx = 1;
    msg_t msg, msg_queue[_USBUS_MSG_QUEUE_SIZE];
    DEBUG("usbus: starting thread %i\n", sched_active_pid);
    /* setup the link-layer's message queue */
    msg_init_queue(msg_queue, _USBUS_MSG_QUEUE_SIZE);
    /* register the event callback with the device driver */
    dev->cb = _event_cb;
    dev->epcb = _event_ep_cb;
    /* initialize low-level driver */
    usbdev_init(dev);
    dev->context = usbus;

    usbus_add_string_descriptor(usbus, &usbus->config, USB_CONFIG_CONFIGURATION_STR);
    usbus_add_string_descriptor(usbus, &usbus->product, USB_CONFIG_PRODUCT_STR);
    usbus_add_string_descriptor(usbus, &usbus->manuf, USB_CONFIG_MANUF_STR);

    usbus->state = USBUS_STATE_DISCONNECT;

    /* Initialize handlers */
    _usbus_init_handlers(usbus);

#if(USBUS_AUTO_ATTACH)
    usbopt_enable_t enable = USBOPT_ENABLE;
    usbdev_set(dev, USBOPT_ATTACH, &enable, sizeof(usbopt_enable_t));
#endif

    while (1) {
        msg_receive(&msg);
        /* dispatch USBUS messages */
        switch (msg.type & 0xFF00) {
            case USBUS_MSG_TYPE_EVENT:
                usbdev_esr(dev);
                break;
            case USBUS_MSG_TYPE_EP_EVENT:
                {
                    usbdev_ep_t *ep = (usbdev_ep_t*)msg.content.ptr;
                    usbdev_ep_esr(ep);
                }
              break;
            case USBUS_MSG_TYPE_HANDLER:
                {
                    usbus_handler_t *handler = (usbus_handler_t*)msg.content.ptr;
                    handler->driver->event_handler(usbus, handler, msg.type, handler);
                }
              break;
            default:
                DEBUG("usbus: unhandled event %x\n", msg.type);
                break;
        }
    }
    return NULL;
}

/* USB event callback */
static void _event_cb(usbdev_t *usbdev, usbdev_event_t event)
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
                usbdev_set(usbus->dev, USBOPT_ADDRESS, &usbus->addr, sizeof(uint8_t));

                for (usbus_handler_t *handler = usbus->handlers; handler; handler = handler->next) {
                    if (handler->flags & USBUS_HANDLER_FLAG_RESET) {
                        handler->driver->event_handler(usbus, handler,
                                USBUS_MSG_TYPE_RESET, NULL);
                    }
                }
                DEBUG("USB reset condition detected\n");
                }
                break;
            default:
                DEBUG("usbus: unhandled event %x\n", event);
                break;
        }
    }
}

/* USB generic endpoint callback */
static void _event_ep_cb(usbdev_ep_t *ep, usbdev_event_t event)
{
    usbus_t *usbus = (usbus_t *)ep->dev->context;
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
                    usbus_handler_t *handler = _ep_to_handler(usbus, ep);
                    if (handler) {
                        handler->driver->event_handler(usbus, handler, USBUS_MSG_TYPE_TR_COMPLETE, ep);
                    }
                }
                break;
            case USBDEV_EVENT_TR_FAIL:
                {
                    usbus_handler_t *handler = _ep_to_handler(usbus, ep);
                    if (handler && (usbus_handler_isset_flag(handler, USBUS_HANDLER_FLAG_TR_FAIL))) {
                        handler->driver->event_handler(usbus, handler, USBUS_MSG_TYPE_TR_FAIL, ep);
                    }
                }
                break;
            case USBDEV_EVENT_TR_STALL:
                {
                    usbus_handler_t *handler = _ep_to_handler(usbus, ep);
                    if (handler && (usbus_handler_isset_flag(handler, USBUS_HANDLER_FLAG_TR_STALL))) {
                        handler->driver->event_handler(usbus, handler, USBUS_MSG_TYPE_TR_STALL, ep);
                    }
                    static const usbopt_enable_t disable = USBOPT_DISABLE;
                    usbdev_ep_set(ep, USBOPT_EP_STALL, &disable, sizeof(usbopt_enable_t));
                }
                break;
            default:
                DEBUG("unhandled event: %x\n", event);
                break;
        }
    }
}
