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

#include "sam_usb.h"
#include "thread.h"
#include "xtimer.h"
#include "byteorder.h"
#include "usb/usbdev.h"
#include "usb/message.h"
#include "usb/usbus.h"
#include "usb/usbus/hdrs.h"

#include "usb.h"
#include "cpu.h"

#include <string.h>
#include <errno.h>
#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _USBUS_MSG_QUEUE_SIZE    (8)
#define USBUS_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define USBUS_PRIO                (THREAD_PRIORITY_MAIN - 6)
#define USBUS_TNAME               "usbus"

#define USBUS_MAX_SIZE            64

static usbus_t _usbus;
extern const usbdev_driver_t driver;
static sam0_common_usb_t usbdev;
static char _stack[USBUS_STACKSIZE];

void _event_cb(usbdev_t *usbdev, usbdev_event_t event);
void _event_ep0_cb(usbdev_ep_t *ep, usbdev_event_t event);
void _event_ep_cb(usbdev_ep_t *ep, usbdev_event_t event);
static void *_usbus_thread(void *args);

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

usbus_t *usbus_get_ctx(void)
{
    return &_usbus;
}

void usbus_init(void)
{
    usbdev.usbdev.driver = &driver;
    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO,
                   USBUS_TNAME, &usbdev.usbdev );
}

void usbus_create(char *stack, int stacksize, char priority,
                   const char *name, usbdev_t *usbdev)
{
    usbus_t *usbus = &_usbus;
    usbus->dev = usbdev;
    int res = thread_create(stack, stacksize, priority, THREAD_CREATE_STACKTEST,
                        _usbus_thread, (void *)usbus, name);
    (void)res;
    assert(res > 0);
}

size_t usbus_put_bytes(usbus_t *usbus, const uint8_t *buf, size_t len)
{
    usbus_controlbuilder_t *builder = &usbus->builder;
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
    //printf("%u+%u/%u@%u\n",byte_len, byte_offset, len, start_offset);
    memcpy(usbus->in->buf + start_offset , buf + byte_offset, byte_len);
    return byte_len;
}

size_t usbus_put_char(usbus_t *usbus, char c)
{
    usbus_controlbuilder_t *builder = &usbus->builder;
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
    mutex_lock(&usbus->lock);
    desc->next = usbus->strings;
    usbus->strings = desc;
    desc->idx = usbus->str_idx++;
    desc->str = str;
    mutex_unlock(&usbus->lock);
    DEBUG("usbus: Adding string descriptor number %u for: \"%s\"\n", desc->idx, str);
    return desc->idx;
}

void usbus_add_conf_descriptor(usbus_t *usbus, usbus_hdr_gen_t* hdr_gen)
{
    mutex_lock(&usbus->lock);
    hdr_gen->next = usbus->hdr_gen;
    usbus->hdr_gen = hdr_gen;
    mutex_unlock(&usbus->lock);
}

void usbus_ep0_ready(usbus_t *usbus)
{
    usbus_controlbuilder_t *bldr = &usbus->builder;
    size_t len = bldr->len;
    len = len < bldr->reqlen - bldr->start ? len : bldr->reqlen - bldr->start;
    bldr->transfered += len;
    //printf("rdy %u, tx: %u\n", len, bldr->transfered);
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

int usbus_update_builder(usbus_t *usbus)
{
    usbus_controlbuilder_t *bldr = &usbus->builder;
    size_t end = bldr->start + usbus->in->len;
    if (bldr->cur > end && bldr->start < bldr->reqlen && bldr->transfered < bldr->reqlen) {
        bldr->start += usbus->in->len;
        bldr->cur = 0;
        bldr->len = 0;
        //printf("pkt cur %u, start: %u, end %u, max %u\n", bldr->cur, bldr->start, end, bldr->reqlen);
        return 1;
    }
    return 0;
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

uint16_t usbus_add_interface(usbus_t *usbus, usbus_interface_t *iface)
{
    mutex_lock(&usbus->lock);
    iface->next = usbus->iface;
    usbus->iface = iface;
    mutex_unlock(&usbus->lock);
    return iface->idx;
}

void usbus_register_event_handler(usbus_t *usbus, usbus_handler_t *handler)
{
    mutex_lock(&usbus->lock);
    handler->next = usbus->handler;
    usbus->handler = handler;
    mutex_unlock(&usbus->lock);
    handler->driver->init(usbus, handler);
}

int usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface, usbus_endpoint_t* ep, usb_ep_type_t type, usb_ep_dir_t dir, size_t len)
{
    int res = -ENOMEM;
    mutex_lock(&usbus->lock);
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
    mutex_unlock(&usbus->lock);
    return res;
}

static void _activate_endpoints(usbus_t *usbus)
{
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        for (usbus_endpoint_t *ep = iface->ep; ep; ep = ep->next) {
            if (ep->active) {
                static const usbopt_enable_t enable = USBOPT_ENABLE;
                ep->ep->driver->set(ep->ep, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
                printf("activated endpoint %d, dir %s\n", ep->ep->num, ep->ep->dir == USB_EP_DIR_OUT? "out" : "in");
            }
        }
        for (usbus_interface_alt_t *alt = iface->alts; alt; alt = alt->next) {
            for (usbus_endpoint_t *ep = alt->ep; ep; ep = ep->next) {
                if (ep->active) {
                    static const usbopt_enable_t enable = USBOPT_ENABLE;
                    ep->ep->driver->set(ep->ep, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
                    printf("activated endpoint %d, dir %s\n", ep->ep->num, ep->ep->dir == USB_EP_DIR_OUT? "out" : "in");
                }
            }
        }
    }
}

static void _usbus_config_ep0(usbus_t *usbus)
{
    static const usbopt_enable_t enable = USBOPT_ENABLE;
    usbus->in->driver->set(usbus->in, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
    usbus->out->driver->set(usbus->out, USBOPT_EP_ENABLE, &enable, sizeof(usbopt_enable_t));
    usbus->out->driver->ready(usbus->out, 0);
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
        mutex_lock(&usbus->lock);
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
        mutex_unlock(&usbus->lock);
    }
}

void _print_setup(usb_setup_t *pkt)
{
    printf("usbus: setup t:0x%.2x r:0x%x, v:0x%x l:%u\n", pkt->type, pkt->request, pkt->value, pkt->length);
}

static void _req_dev(usbus_t *usbus)
{
    usb_descriptor_device_t desc;
    memset(&desc, 0, sizeof(usb_descriptor_device_t));
    desc.length = sizeof(usb_descriptor_device_t);
    desc.type = USB_TYPE_DESCRIPTOR_DEVICE;
    desc.bcd_usb = 0x0110;
    desc.max_packet_size = USBUS_MAX_SIZE;
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
    mutex_lock(&usbus->lock);
    usbus_hdrs_fmt_conf(usbus);
    mutex_unlock(&usbus->lock);
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
    mutex_lock(&usbus->lock);
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        if (destination == iface->idx) {
            iface->handler->driver->event_handler(usbus, iface->handler, USBUS_MSG_TYPE_SETUP_RQ, pkt);
        }
    }
    mutex_unlock(&usbus->lock);
}


static inline size_t usbus_pkt_maxlen(usbus_t *usbus, usb_setup_t *pkt)
{
    return pkt->length > usbus->in->len ? usbus->in->len : pkt->length;
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

static void *_usbus_thread(void *args)
{
    usbus_t *usbus = (usbus_t*)args;

    mutex_lock(&usbus->lock);
    usbdev_t *dev = usbus->dev;
    usbus->pid = sched_active_pid;
    usbus->addr = 0;
    usbus->strings = NULL;
    usbus->iface = NULL;
    usbus->str_idx = 1;
    usbus->setup_state = USBUS_SETUPRQ_READY;
    mutex_init(&usbus->lock);
    msg_t msg, msg_queue[_USBUS_MSG_QUEUE_SIZE];
    DEBUG("usbus: starting thread %i\n", sched_active_pid);
    /* setup the link-layer's message queue */
    msg_init_queue(msg_queue, _USBUS_MSG_QUEUE_SIZE);
    /* register the event callback with the device driver */
    dev->cb = _event_cb;
    /* initialize low-level driver */
    dev->driver->init(dev);
    dev->context = usbus;

    usbus->in = usbus->dev->driver->new_ep(usbus->dev, USB_EP_TYPE_CONTROL, USB_EP_DIR_IN, USBUS_MAX_SIZE);
    usbus->out = usbus->dev->driver->new_ep(usbus->dev, USB_EP_TYPE_CONTROL, USB_EP_DIR_OUT, USBUS_MAX_SIZE);
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
    mutex_unlock(&usbus->lock);

    /* TODO: signal ready from application */
    xtimer_sleep(1);
    usbopt_enable_t enable = USBOPT_ENABLE;
    dev->driver->set(dev, USBOPT_ATTACH, &enable, sizeof(usbopt_enable_t));

    while (1) {
        msg_receive(&msg);
        /* dispatch netdev, MAC and gnrc_netapi messages */
        switch (msg.type) {
            case USBUS_MSG_TYPE_EVENT:
                dev->driver->esr(dev);
                break;
            case USBUS_MSG_TYPE_EP_EVENT:
                {
                    usbdev_ep_t *ep = (usbdev_ep_t*)msg.content.ptr;
                    ep->driver->esr(ep);
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
                puts("Reset");
                }
                break;
            default:
                DEBUG("usbus: unhandled event\n");
                break;
        }
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
                    memset(&usbus->builder, 0, sizeof(usbus_controlbuilder_t));
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
                    memset(&usbus->builder, 0, sizeof(usbus_controlbuilder_t));
                    memcpy(&usbus->setup, usbus->out->buf, sizeof(usb_setup_t));
                    usbus->builder.reqlen = usbus->setup.length;
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
