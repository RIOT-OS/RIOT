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

#include "atomic_utils.h"
#include "kernel_defines.h"
#include "bitarithm.h"
#include "event.h"
#include "fmt.h"
#include "luid.h"
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
    int res = thread_create(stack, stacksize, priority, 0,
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

static inline usbus_endpoint_t *_usbus_ep_from_usbdev(usbus_t *usbus, usbdev_ep_t* ep)
{
    return ep->dir == USB_EP_DIR_IN ? &usbus->ep_in[ep->num]
                                    : &usbus->ep_out[ep->num];
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

void usbus_add_interface_alt(usbus_interface_t *iface,
                             usbus_interface_alt_t *alt)
{
    usbus_interface_alt_t **last = &iface->alts;
    while (*last) {
        last = &(*last)->next;
    }
    *last = alt;
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

size_t usbus_max_bulk_endpoint_size(usbus_t *usbus)
{
    usb_speed_t speed;
    int res = usbdev_get(usbus->dev, USBOPT_ENUMERATED_SPEED, &speed,
                               sizeof(speed));
    if (res == -ENOTSUP) {
        res = usbdev_get(usbus->dev, USBOPT_MAX_SPEED, &speed,
                               sizeof(speed));
    }

    if (res < 0) {
        return 0; /* Misbehaving usbdev device not implementing any speed indication */
    }

    switch (speed) {
        case USB_SPEED_HIGH:
            return USB_ENDPOINT_BULK_HS_MAX_SIZE;
        default:
            return USB_ENDPOINT_BULK_FS_MAX_SIZE;
    }
}

size_t usbus_max_interrupt_endpoint_size(usbus_t *usbus)
{
    usb_speed_t speed;
    int res = usbdev_get(usbus->dev, USBOPT_ENUMERATED_SPEED, &speed,
                               sizeof(speed));
    if (res == -ENOTSUP) {
        res = usbdev_get(usbus->dev, USBOPT_MAX_SPEED, &speed,
                               sizeof(speed));
    }

    if (res < 0) {
        assert(false); /* Misbehaving usbdev device not implementing mandatory USBOPTS */
    }

    switch (speed) {
        case USB_SPEED_HIGH:
            return USB_ENDPOINT_INTERRUPT_HS_MAX_SIZE;
        default:
            return USB_ENDPOINT_INTERRUPT_FS_MAX_SIZE;
    }
}

usbus_endpoint_t *usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface,
                                     usb_ep_type_t type, usb_ep_dir_t dir,
                                     size_t len)
{
    usbus_endpoint_t *ep = NULL;
    usbdev_ep_t *usbdev_ep = usbdev_new_ep(usbus->dev, type, dir, len);

    if (usbdev_ep) {
        ep = _usbus_ep_from_usbdev(usbus, usbdev_ep);
        ep->maxpacketsize = len;
        ep->ep = usbdev_ep;
        if (iface) {
            ep->next = iface->ep;
            iface->ep = ep;
        }
    }
    return ep;
}

static inline uint8_t _get_ep_bitnum(usbdev_ep_t *ep)
{
    /* Endpoint activity bit flag, lower USBDEV_NUM_ENDPOINTS bits are
     * useb as OUT endpoint flags, upper bit are IN endpoints */
    return (ep->dir == USB_EP_DIR_IN ? USBDEV_NUM_ENDPOINTS
                                     : 0x00) + ep->num;
}

static void _set_ep_event(usbus_t *usbus, usbdev_ep_t *ep)
{
    atomic_bit_u32_t bitflag = atomic_bit_u32(&usbus->ep_events,
                                              _get_ep_bitnum(ep));
    atomic_set_bit_u32(bitflag);

    thread_flags_set(thread_get(usbus->pid), USBUS_THREAD_FLAG_USBDEV_EP);
}

static inline uint32_t _get_and_reset_ep_events(usbus_t *usbus)
{
    return atomic_fetch_and_u32(&usbus->ep_events, 0);
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

#ifdef MODULE_USBUS_URB
static void _usbus_transfer_urb_submit(usbus_endpoint_t *usbus_ep,
                                       usbus_urb_t *urb)
{
    /* Maximum between the urb length and the endpoint maximum size */
    size_t len = urb->len > usbus_ep->maxpacketsize ?
                 usbus_ep->maxpacketsize :
                 urb->len;
    usbdev_ep_xmit(usbus_ep->ep, urb->buf, len);
    urb->buf += len;
    urb->len -= len;
}

void usbus_urb_submit(usbus_t *usbus, usbus_endpoint_t *endpoint, usbus_urb_t *urb)
{
    (void)usbus;

    if (clist_find(&endpoint->urb_list, &urb->list)) {
        return;
    }

    if (endpoint->ep->dir == USB_EP_DIR_IN &&
            ((urb->len % endpoint->maxpacketsize) == 0) &&
            usbus_urb_isset_flag(urb, USBUS_URB_FLAG_AUTO_ZLP)) {
        /* If it is an IN endpoint, the urb length is a whole number of
         * transfers and the ZLP is requested, then set flag that it needs the
         * ZLP
         */
        urb->flags |= USBUS_URB_FLAG_NEEDS_ZLP;
    }
    clist_rpush(&endpoint->urb_list, &urb->list);
    /* Initiate transfer immediately if the list is empty */
    if (clist_exactly_one(&endpoint->urb_list)) {
        _usbus_transfer_urb_submit(endpoint, urb);
    }
}

int usbus_urb_cancel(usbus_t *usbus, usbus_endpoint_t *endpoint, usbus_urb_t *urb)
{
    (void)usbus;
    usbus_urb_t *active_urb = (usbus_urb_t*)clist_lpeek(&endpoint->urb_list);
    if (active_urb == urb) {
        usbus_urb_set_flag(active_urb, USBUS_URB_FLAG_CANCELLED);
        usbus_urb_remove_flag(active_urb, USBUS_URB_FLAG_NEEDS_ZLP);
        return 0;
    }
    if (clist_remove(&endpoint->urb_list, &urb->list)) {
        return 1;
    }
    return -1; /* URB not found */
}

static bool _urb_transfer_complete(usbus_t *usbus, usbdev_ep_t *ep,
                                   usbus_handler_t *handler)
{
    usbus_endpoint_t *usbus_ep = _usbus_ep_from_usbdev(usbus, ep);
    if (clist_is_empty(&usbus_ep->urb_list)) {
        return false;
    }

    /* Ongoing transfer */
    usbus_urb_t *active_urb = (usbus_urb_t*)clist_lpeek(&usbus_ep->urb_list);

    size_t len = usbus_ep->maxpacketsize;
    if (ep->dir == USB_EP_DIR_OUT) {
        usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
        active_urb->transferred += len;
    }

    if ((active_urb->len == 0) || (len < usbus_ep->maxpacketsize) ||
        (usbus_urb_isset_flag(active_urb, USBUS_URB_FLAG_CANCELLED))) {
        /* Only set for IN endpoints */
        if (usbus_urb_isset_flag(active_urb, USBUS_URB_FLAG_NEEDS_ZLP)) {
            usbus_urb_remove_flag(active_urb, USBUS_URB_FLAG_NEEDS_ZLP);
            _usbus_transfer_urb_submit(usbus_ep, active_urb);
        }
        else {
            /* transfer of URB complete */
            clist_lpop(&usbus_ep->urb_list);

            /* Schedule next URB first, then notify the handler */
            usbus_urb_t *next_urb = (usbus_urb_t*)clist_lpeek(&usbus_ep->urb_list);
            if (next_urb) {
                _usbus_transfer_urb_submit(usbus_ep, next_urb);
            }

            DEBUG("Done with the transfer, available: %" PRIuSIZE ", len: %" PRIuSIZE "\n",
                  active_urb->transferred, active_urb->len);
            handler->driver->transfer_handler(usbus, handler, ep,
                                              USBUS_EVENT_TRANSFER_COMPLETE);
        }
    }
    else {
        _usbus_transfer_urb_submit(usbus_ep, active_urb);
    }

    return true;
}
#else
static bool _urb_transfer_complete(usbus_t *usbus, usbdev_ep_t *ep,
                                   usbus_handler_t *handler)
{
    (void)usbus;
    (void)ep;
    (void)handler;
    return false;
}
#endif /* MODULE_USBUS_URB */

static void _usbus_transfer_complete(usbus_t *usbus, usbdev_ep_t *ep, usbus_handler_t *handler)
{
    if (_urb_transfer_complete(usbus, ep, handler)) {
        return;
    }

    /* Raw usbdev transfers by the handler */
    handler->driver->transfer_handler(usbus, handler, ep, USBUS_EVENT_TRANSFER_COMPLETE);
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

#ifdef CONFIG_USB_SERIAL_STR
    usbus_add_string_descriptor(usbus, &usbus->serial, CONFIG_USB_SERIAL_STR);
#else
    static_assert(CONFIG_USB_SERIAL_BYTE_LENGTH <= UINT8_MAX/4,
                  "USB serial byte length must be at most 63 due to protocol "
                  "limitations");
    uint8_t luid_buf[CONFIG_USB_SERIAL_BYTE_LENGTH];
    luid_base(luid_buf, sizeof(luid_buf));
    fmt_bytes_hex(usbus->serial_str, luid_buf, sizeof(luid_buf));
    usbus_add_string_descriptor(usbus, &usbus->serial, usbus->serial_str);
#endif

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

void usbus_endpoint_halt(usbus_endpoint_t *ep)
{
    assert(ep->ep->num != 0); /* Not valid for endpoint 0 */
    DEBUG("Endpoint %u halted\n", ep->ep->num);
    ep->halted = 1;
    usbdev_ep_stall(ep->ep, true);
}

void usbus_endpoint_clear_halt(usbus_endpoint_t *ep)
{
    assert(ep->ep->num != 0); /* Not valid for endpoint 0 */
    DEBUG("Endpoint %u unhalted\n", ep->ep->num);
    ep->halted = 0;
    usbdev_ep_stall(ep->ep, false);
}

/**
 * @brief Reset the halted status on USB reset condition
 */
static void _usbus_endpoint_reset_halt(usbus_t *usbus)
{
    /* Clear halted state. No need to notify usbdev, USB reset already resets those */
    for (size_t i = 0; i < USBDEV_NUM_ENDPOINTS; i++) {
        usbus->ep_out[i].halted = 0;
        usbus->ep_in[i].halted = 0;
    }
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
                _usbus_endpoint_reset_halt(usbus);
                flag = USBUS_HANDLER_FLAG_RESET;
                msg = USBUS_EVENT_USB_RESET;
                DEBUG("usbus: USB reset detected\n");
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
                    _usbus_transfer_complete(usbus, ep, handler);
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
