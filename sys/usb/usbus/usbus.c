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

#define USB_H_USER_IS_RIOT_INTERNAL

#include "kernel_defines.h"
#include "bitarithm.h"
#include "event.h"
#include "thread.h"
#include "thread_flags.h"
#include "periph/usbdev.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/fmt.h"
#include "usb/usbus/control.h"

#include "usb.h"
#include "cpu.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define ENABLE_DEBUG             0
#include "debug.h"

#define _USBUS_MSG_QUEUE_SIZE    (16)

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

uint16_t usbus_add_string_descriptor(usbus_t *usbus, usbus_string_t *desc,
                                     const char *str)
{
    desc->next = usbus->strings;
    usbus->strings = desc;
    desc->idx = usbus->str_idx++;
    desc->str = str;
    DEBUG("usbus: Adding string descriptor number %u for: \"%s\"\n", desc->idx,
          str);
    return desc->idx;
}

void usbus_add_conf_descriptor(usbus_t *usbus, usbus_descr_gen_t *descr_gen)
{
    descr_gen->next = usbus->descr_gen;
    usbus->descr_gen = descr_gen;
}

static usbus_handler_t *_ep_to_handler(usbus_t *usbus, usbdev_ep_t *ep)
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
    usbus_interface_t **last = &usbus->iface;
    while (*last) {
        last = &(*last)->next;
        idx++;
    }
    iface->idx = idx;
    *last = iface;
    return idx;
}

void usbus_register_event_handler(usbus_t *usbus, usbus_handler_t *handler)
{
    /* See note above for reasons against clist.h */
    usbus_handler_t **last = &usbus->handlers;
    while (*last) {
        last = &(*last)->next;
    }
    *last = handler;
}

usbus_endpoint_t *usbus_interface_find_endpoint(usbus_interface_t *interface,
                                                usb_ep_type_t type,
                                                usb_ep_dir_t dir)
{
    for (usbus_endpoint_t *uep = interface->ep; uep; uep = uep->next) {
        if (uep->ep->type == type && uep->ep->dir == dir) {
            return uep;
        }
    }
    return NULL;
}

usbus_endpoint_t *usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface,
                                     usb_ep_type_t type, usb_ep_dir_t dir,
                                     size_t len)
{
    usbus_endpoint_t *ep = NULL;
    usbdev_ep_t *usbdev_ep = usbdev_new_ep(usbus->dev, type, dir, len);

    if (usbdev_ep) {
        ep = dir == USB_EP_DIR_IN ? &usbus->ep_in[usbdev_ep->num]
                                  : &usbus->ep_out[usbdev_ep->num];
        ep->maxpacketsize = usbdev_ep->len;
        ep->ep = usbdev_ep;
        if (iface) {
            ep->next = iface->ep;
            iface->ep = ep;
        }
    }
    return ep;
}

static inline uint32_t _get_ep_bitflag(usbdev_ep_t *ep)
{
    /* Endpoint activity bit flag, lower USBDEV_NUM_ENDPOINTS bits are
     * useb as OUT endpoint flags, upper bit are IN endpoints */
    return 1 << ((ep->dir == USB_EP_DIR_IN ? USBDEV_NUM_ENDPOINTS
                                           : 0x00) + ep->num);
}

static void _set_ep_event(usbus_t *usbus, usbdev_ep_t *ep)
{
    if (irq_is_in()) {
        usbus->ep_events |= _get_ep_bitflag(ep);
    }
    else {
        unsigned state = irq_disable();
        usbus->ep_events |= _get_ep_bitflag(ep);
        irq_restore(state);
    }

    thread_flags_set(thread_get(usbus->pid), USBUS_THREAD_FLAG_USBDEV_EP);
}

static uint32_t _get_and_reset_ep_events(usbus_t *usbus)
{
    unsigned state = irq_disable();
    uint32_t res = usbus->ep_events;

    usbus->ep_events = 0;
    irq_restore(state);
    return res;
}

static void _signal_handlers(usbus_t *usbus, uint16_t flag,
                             uint16_t msg)
{
    for (usbus_handler_t *handler = usbus->handlers;
         handler; handler = handler->next) {
        if (handler->flags & flag) {
            handler->driver->event_handler(usbus, handler, msg);
        }
    }
}

static void _usbus_init_handlers(usbus_t *usbus)
{
    for (usbus_handler_t *handler = usbus->handlers;
         handler; handler = handler->next) {
        handler->driver->init(usbus, handler);
    }
}

static void *_usbus_thread(void *args)
{
    usbus_t *usbus = (usbus_t *)args;
    usbus_control_handler_t ep0_handler;

    event_queue_init(&usbus->queue);

    usbus->control = &ep0_handler.handler;

    usbus_control_init(usbus, &ep0_handler);

    usbdev_t *dev = usbus->dev;
    usbus->pid = thread_getpid();
    usbus->addr = 0;
    usbus->iface = NULL;
    usbus->str_idx = 1;
    DEBUG("usbus: starting thread %i\n", thread_getpid());
    /* setup the link-layer's message queue */
    /* register the event callback with the device driver */
    dev->cb = _event_cb;
    dev->epcb = _event_ep_cb;
    /* initialize low-level driver */
    dev->context = usbus;
    usbdev_init(dev);

    usbus_add_string_descriptor(usbus, &usbus->config,
                                CONFIG_USB_CONFIGURATION_STR);
    usbus_add_string_descriptor(usbus, &usbus->product, CONFIG_USB_PRODUCT_STR);
    usbus_add_string_descriptor(usbus, &usbus->manuf, CONFIG_USB_MANUF_STR);

    usbus->state = USBUS_STATE_DISCONNECT;

    /* Initialize handlers */
    _usbus_init_handlers(usbus);

    if (IS_ACTIVE(CONFIG_USBUS_AUTO_ATTACH)) {
        static const usbopt_enable_t _enable = USBOPT_ENABLE;
        usbdev_set(dev, USBOPT_ATTACH, &_enable,
                   sizeof(usbopt_enable_t));
    }

    while (1) {
        thread_flags_t flags = thread_flags_wait_any(
            USBUS_THREAD_FLAG_USBDEV |
            USBUS_THREAD_FLAG_USBDEV_EP |
            THREAD_FLAG_EVENT
            );
        if (flags & USBUS_THREAD_FLAG_USBDEV) {
            usbdev_esr(dev);
        }
        if (flags & USBUS_THREAD_FLAG_USBDEV_EP) {
            uint32_t events = _get_and_reset_ep_events(usbus);
            while (events) {
                unsigned num = bitarithm_lsb(events);
                events &= ~(1 << num);
                if (num < USBDEV_NUM_ENDPOINTS) {
                    /* OUT endpoint */
                    usbdev_ep_esr(usbus->ep_out[num].ep);
                }
                else {
                    /* IN endpoint */
                    usbdev_ep_esr(usbus->ep_in[num - USBDEV_NUM_ENDPOINTS].ep);
                }
            }

        }
        if (flags & THREAD_FLAG_EVENT) {
            event_t *event = event_get(&usbus->queue);
            if (event) {
                event->handler(event);
            }
        }

    }
    return NULL;
}

/* USB event callback */
static void _event_cb(usbdev_t *usbdev, usbdev_event_t event)
{
    usbus_t *usbus = (usbus_t *)usbdev->context;

    if (event == USBDEV_EVENT_ESR) {
        thread_flags_set(thread_get(usbus->pid), USBUS_THREAD_FLAG_USBDEV);
    }
    else {
        usbus_event_usb_t msg;
        uint16_t flag;
        switch (event) {
            case USBDEV_EVENT_RESET:
                usbus->state = USBUS_STATE_RESET;
                usbus->addr = 0;
                usbdev_set(usbus->dev, USBOPT_ADDRESS, &usbus->addr,
                           sizeof(uint8_t));
                flag = USBUS_HANDLER_FLAG_RESET;
                msg = USBUS_EVENT_USB_RESET;
                break;
            case USBDEV_EVENT_SUSPEND:
                DEBUG("usbus: USB suspend detected\n");
                usbus->pstate = usbus->state;
                usbus->state = USBUS_STATE_SUSPEND;
                flag = USBUS_HANDLER_FLAG_SUSPEND;
                msg = USBUS_EVENT_USB_SUSPEND;
                break;
            case USBDEV_EVENT_RESUME:
                DEBUG("usbus: USB resume detected\n");
                usbus->state = usbus->pstate;
                flag = USBUS_HANDLER_FLAG_RESUME;
                msg = USBUS_EVENT_USB_RESUME;
                break;
            default:
                DEBUG("usbus: unhandled event %x\n", event);
                return;
        }
        _signal_handlers(usbus, flag, msg);
    }
}


/* USB generic endpoint callback */
static void _event_ep_cb(usbdev_ep_t *ep, usbdev_event_t event)
{
    usbus_t *usbus = (usbus_t *)ep->dev->context;

    if (event == USBDEV_EVENT_ESR) {
        _set_ep_event(usbus, ep);
    }
    else {
        usbus_handler_t *handler = _ep_to_handler(usbus, ep);
        if (handler) {
            switch (event) {
                case USBDEV_EVENT_TR_COMPLETE:
                    handler->driver->transfer_handler(usbus, handler, ep,
                                                      USBUS_EVENT_TRANSFER_COMPLETE);
                    break;
                case USBDEV_EVENT_TR_FAIL:
                    if (usbus_handler_isset_flag(handler,
                                                 USBUS_HANDLER_FLAG_TR_FAIL)) {
                        handler->driver->transfer_handler(usbus, handler, ep,
                                                          USBUS_EVENT_TRANSFER_FAIL);
                    }
                    break;
                case USBDEV_EVENT_TR_STALL:
                    if (usbus_handler_isset_flag(handler,
                                                 USBUS_HANDLER_FLAG_TR_STALL)) {
                        handler->driver->transfer_handler(usbus, handler, ep,
                                                          USBUS_EVENT_TRANSFER_STALL);
                        static const usbopt_enable_t disable = USBOPT_DISABLE;
                        usbdev_ep_set(ep, USBOPT_EP_STALL, &disable,
                                      sizeof(usbopt_enable_t));
                    }
                    break;
                default:
                    DEBUG("unhandled event: %x\n", event);
                    break;
            }
        }
    }
}
