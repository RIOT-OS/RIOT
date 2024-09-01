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

#define USB_H_USER_IS_RIOT_INTERNAL

#include "periph/usbdev.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/fmt.h"
#include "usb/usbus/control.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>

#define ENABLE_DEBUG 1
#include "debug.h"

static void _init(usbus_t *usbus, usbus_handler_t *handler);
static void _handler_ep0_event(usbus_t *usbus, usbus_handler_t *handler,
                               usbus_event_usb_t event);
static void _handler_ep0_transfer(usbus_t *usbus, usbus_handler_t *handler,
                                  usbdev_ep_t *ep, usbus_event_transfer_t event);

const usbus_handler_driver_t _ep0_driver = {
    .init = _init,
    .event_handler = _handler_ep0_event,
    .transfer_handler = _handler_ep0_transfer,
};

void usbus_control_init(usbus_t *usbus, usbus_control_handler_t *handler)
{
    handler->handler.driver = &_ep0_driver;

    /* Ensure that ep0 is the first handler */
    handler->handler.next = usbus->handlers;
    usbus->handlers = &handler->handler;
}

static void _activate_endpoints(usbus_t *usbus)
{
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        for (usbus_endpoint_t *ep = iface->ep; ep; ep = ep->next) {
            if (ep->active) {
                static const usbopt_enable_t enable = USBOPT_ENABLE;
                usbdev_ep_set(ep->ep, USBOPT_EP_ENABLE, &enable,
                              sizeof(usbopt_enable_t));
                DEBUG("usbus_control: activated endpoint %d, dir %s\n",
                      ep->ep->num,
                      ep->ep->dir == USB_EP_DIR_OUT ? "out" : "in");
            }
        }
        for (usbus_interface_alt_t *alt = iface->alts; alt; alt = alt->next) {
            for (usbus_endpoint_t *ep = alt->ep; ep; ep = ep->next) {
                if (ep->active) {
                    static const usbopt_enable_t enable = USBOPT_ENABLE;
                    usbdev_ep_set(ep->ep, USBOPT_EP_ENABLE, &enable,
                                  sizeof(usbopt_enable_t));
                    DEBUG("usbus_control: activated endpoint %d, dir %s\n",
                          ep->ep->num,
                          ep->ep->dir == USB_EP_DIR_OUT ? "out" : "in");
                }
            }
        }
    }
}

static size_t _cpy_str_to_utf16(usbus_t *usbus, const char *str)
{
    size_t len = 0;

    while (*str) {
        usbus_control_slicer_put_char(usbus, *str);
        usbus_control_slicer_put_char(usbus, 0);
        len += 2; /* Two bytes added each iteration */
        str++;
    }
    return len;
}

static usbus_string_t *_get_descriptor(usbus_t *usbus, uint16_t idx)
{
    for (usbus_string_t *str = usbus->strings; str; str = str->next) {
        if (str->idx == idx) {
            return str;
        }
    }
    return NULL;
}

static int _req_status(usbus_t *usbus)
{
    /* Signal self powered and remote wakeup status */
    uint16_t status = (CONFIG_USB_SELF_POWERED) ? 1 : 0 | usbus->wakeup_enabled << 1;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&status, sizeof(status));
    return sizeof(status);
}

static int _req_iface_status(usbus_t *usbus)
{
    uint16_t status = 0; /* always zero */
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&status, sizeof(status));
    return sizeof(status);
}

static int _req_endpoint_status(usbus_t *usbus, usbus_endpoint_t *ep)
{
    uint16_t status = ep->halted ? 1 : 0;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&status, sizeof(status));
    return sizeof(status);
}

static int _req_endpoint_feature(usbus_endpoint_t *ep, uint16_t feature, bool enable)
{
    switch (feature) {
        case USB_FEATURE_ENDPOINT_HALT:
            enable ? usbus_endpoint_halt(ep) : usbus_endpoint_clear_halt(ep);
            break;
        default:
            DEBUG("usbus: unknown endpoint feature request: %u\n", feature);
            return -1;
    }
    return 1;
}

static int _req_str(usbus_t *usbus, uint16_t idx)
{
    /* Return an error condition by default */
    int res = -1;

    /* Language ID must only be supported if there are string descriptors
     * available */
    if (usbus->strings) {
        if (idx == 0) {
            usb_descriptor_string_t desc;
            desc.type = USB_TYPE_DESCRIPTOR_STRING;
            desc.length = sizeof(uint16_t) + sizeof(usb_descriptor_string_t);
            usbus_control_slicer_put_bytes(usbus, (uint8_t *)&desc, sizeof(desc));
            /* Only one language ID supported */
            uint16_t us = CONFIG_USB_DEFAULT_LANGID;
            usbus_control_slicer_put_bytes(usbus, (uint8_t *)&us, sizeof(uint16_t));
            res = 1;
        }
        else {
            usb_descriptor_string_t desc;
            desc.type = USB_TYPE_DESCRIPTOR_STRING;
            usbus_string_t *str = _get_descriptor(usbus, idx);
            if (str) {
                desc.length = sizeof(usb_descriptor_string_t);
                desc.length += 2 * strlen(str->str); /* USB strings are UTF-16 */
                usbus_control_slicer_put_bytes(usbus, (uint8_t *)&desc,
                                               sizeof(desc));
                _cpy_str_to_utf16(usbus, str->str);
                res = 1;
            }
        }
    }
    return res;
}

static int _req_dev(usbus_t *usbus)
{
    return usbus_fmt_descriptor_dev(usbus);
}

static int _req_config(usbus_t *usbus)
{
    return usbus_fmt_descriptor_conf(usbus);
}

static int _req_dev_qualifier(usbus_t *usbus)
{
    usb_speed_t speed = USB_SPEED_LOW;

    usbus->dev->driver->get(usbus->dev, USBOPT_MAX_SPEED, &speed,
                            sizeof(usb_speed_t));
    if (speed == USB_SPEED_HIGH) {
        /* TODO: implement device qualifier support (only required
         * for High speed) */
    }
    /* Signal a stall condition */
    return -1;
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
            DEBUG("usbus: unknown descriptor request %u, signalling stall\n",
                  type);
            return -1;
    }
}

static int _req_dev_feature(usbus_t *usbus, uint16_t feature, bool enable)
{
    int res = -1;

    switch (feature) {
        case USB_FEATURE_DEVICE_REMOTE_WAKEUP:
            if (CONFIG_USB_REM_WAKEUP) {
                usbus->wakeup_enabled = enable;
                res = 1;
            }
            break;
        default:
            DEBUG("usbus: unknown device feature request: %u\n", feature);
            break;
    }
    return res;
}

static int _recv_dev_setup(usbus_t *usbus, usb_setup_t *pkt)
{
    int res = -1;

    if (usb_setup_is_read(pkt)) {
        switch (pkt->request) {
            case USB_SETUP_REQ_GET_STATUS:
                res = _req_status(usbus);
                break;
            case USB_SETUP_REQ_GET_DESCRIPTOR:
                res = _req_descriptor(usbus, pkt);
                break;
            default:
                DEBUG("usbus: Unknown read request %u\n", pkt->request);
                break;
        }
    }
    else {
        switch (pkt->request) {
            case USB_SETUP_REQ_SET_ADDRESS:
                DEBUG("usbus_control: Setting address\n");
                usbus->addr = (uint8_t)pkt->value;
                if (!USBDEV_CPU_SET_ADDR_AFTER_STATUS) {
                    usbdev_set(usbus->dev, USBOPT_ADDRESS, &usbus->addr,
                               sizeof(usbus->addr));
                }
                res = 1;
                break;
            case USB_SETUP_REQ_SET_CONFIGURATION:
                /* Nothing configuration dependent to do here, only one
                 * configuration supported */
                usbus->state = USBUS_STATE_CONFIGURED;
                _activate_endpoints(usbus);
                res = 1;
                break;
            case USB_SETUP_REQ_SET_FEATURE:
                res = _req_dev_feature(usbus, pkt->value, true);
                break;
            case USB_SETUP_REQ_CLEAR_FEATURE:
                res = _req_dev_feature(usbus, pkt->value, false);
                break;
            default:
                DEBUG("usbus: Unknown write request %u\n", pkt->request);
                break;
        }
    }
    return res;
}

static int _recv_interface_setup(usbus_t *usbus, usb_setup_t *pkt)
{
    usbus_control_handler_t *ep0_handler =
        (usbus_control_handler_t *)usbus->control;
    uint16_t destination = pkt->index & 0x0f;
    uint8_t type_mask = pkt->type & USB_SETUP_REQUEST_TYPE_MASK;

    /* Globally handle the iface get status request */
    if (pkt->request == USB_SETUP_REQ_GET_STATUS &&
        type_mask == USB_SETUP_REQUEST_TYPE_STANDARD) {
        return _req_iface_status(usbus);
    }

    /* Find interface handler */
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        if (destination == iface->idx &&
            iface->handler->driver->control_handler) {
            return iface->handler->driver->control_handler(usbus,
                                                           iface->handler,
                                                           ep0_handler->control_request_state,
                                                           pkt);
        }
    }
    return -1;
}

static int _recv_endpoint_setup(usbus_t *usbus, usb_setup_t *pkt)
{
    uint8_t destination = pkt->index & 0x0f;
    bool in = pkt->index & (1 << 7); /* Bit seven is 1 for IN, 0 for OUT */
    usbus_endpoint_t *ep = in ? &usbus->ep_in[destination] :
                                &usbus->ep_out[destination];

    switch (pkt->request) {
        case USB_SETUP_REQ_GET_STATUS:
            return _req_endpoint_status(usbus, ep);
        case USB_SETUP_REQ_SET_FEATURE:
            return _req_endpoint_feature(ep, pkt->value, true);
        case USB_SETUP_REQ_CLEAR_FEATURE:
            return _req_endpoint_feature(ep, pkt->value, false);
        default:
            DEBUG("usbus: Unknown endpoint request %u\n", pkt->request);
            return -1;
    }
}

static void _recv_setup(usbus_t *usbus, usbus_control_handler_t *handler)
{
    usb_setup_t *pkt = &handler->setup;

    DEBUG("usbus_control: Received setup %x %x @ %d\n", pkt->type,
          pkt->request, pkt->length);
    int res = 0;

    /* If write and length is more than expected */
    if (!(usb_setup_is_read(pkt)) && (handler->received_len > pkt->length)) {
       res = -1; /* Stall */
    }
    else {
        uint8_t destination = pkt->type & USB_SETUP_REQUEST_RECIPIENT_MASK;
        switch (destination) {
            case USB_SETUP_REQUEST_RECIPIENT_DEVICE:
                res = _recv_dev_setup(usbus, pkt);
                break;
            case USB_SETUP_REQUEST_RECIPIENT_INTERFACE:
                res = _recv_interface_setup(usbus, pkt);
                break;
            case USB_SETUP_REQUEST_RECIPIENT_ENDPOINT:
                res = _recv_endpoint_setup(usbus, pkt);
                break;
            default:
                DEBUG("usbus_control: Unhandled setup request\n");
        }
    }
    if (res < 0) {
        /* Signal stall to indicate unsupported (USB 2.0 spec 9.6.2) */
        usbdev_ep0_stall(usbus->dev);
        handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_READY;
    }
    else if (res) {
        if (usb_setup_is_read(pkt)) {
            handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_INDATA;
            usbus_control_slicer_ready(usbus);
        }
        else {
            /* Signal ready for new data in case there is more */
            if (handler->received_len < pkt->length) {
                handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_OUTDATA;
                usbdev_ep_xmit(handler->out, handler->out_buf, CONFIG_USBUS_EP0_SIZE);
            }
            else {
                handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_INACK;
                usbdev_ep_xmit(handler->in, handler->in_buf, 0);
            }
        }

    }
}

static void _usbus_config_ep0(usbus_control_handler_t *ep0_handler)
{
    DEBUG("usbus_control: Enabling EP0\n");
    static const usbopt_enable_t enable = USBOPT_ENABLE;
    usbdev_ep_init(ep0_handler->in);
    usbdev_ep_init(ep0_handler->out);
    usbdev_ep_set(ep0_handler->in, USBOPT_EP_ENABLE, &enable,
                  sizeof(usbopt_enable_t));
    usbdev_ep_set(ep0_handler->out, USBOPT_EP_ENABLE, &enable,
                  sizeof(usbopt_enable_t));
    usbdev_ep_xmit(ep0_handler->out, ep0_handler->out_buf, CONFIG_USBUS_EP0_SIZE);
}

uint8_t *usbus_control_get_out_data(usbus_t *usbus, size_t *len)
{
    usbus_control_handler_t *handler = (usbus_control_handler_t*)usbus->control;

    assert(len);
    assert(handler->control_request_state == USBUS_CONTROL_REQUEST_STATE_OUTDATA);

    usbdev_ep_get(handler->out, USBOPT_EP_AVAILABLE,
                  len, sizeof(size_t));
    return handler->out_buf;
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    DEBUG("usbus_control: Initializing EP0\n");
    usbus_control_handler_t *ep0_handler = (usbus_control_handler_t *)handler;
    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);
    ep0_handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_READY;

    ep0_handler->in = usbus_add_endpoint(usbus, NULL, USB_EP_TYPE_CONTROL,
                                         USB_EP_DIR_IN, CONFIG_USBUS_EP0_SIZE)->ep;
    ep0_handler->out = usbus_add_endpoint(usbus, NULL, USB_EP_TYPE_CONTROL,
                                          USB_EP_DIR_OUT, CONFIG_USBUS_EP0_SIZE)->ep;
}

static int _handle_tr_complete(usbus_t *usbus,
                               usbus_control_handler_t *ep0_handler,
                               usbdev_ep_t *ep)
{
    switch (ep0_handler->control_request_state) {
        case USBUS_CONTROL_REQUEST_STATE_INACK:
            if (ep->dir == USB_EP_DIR_IN) {
                if (usbus->addr && usbus->state == USBUS_STATE_RESET) {
                    if (USBDEV_CPU_SET_ADDR_AFTER_STATUS) {
                        usbdev_set(usbus->dev, USBOPT_ADDRESS, &usbus->addr,
                                   sizeof(usbus->addr));
                    }
                    /* Address configured */
                    usbus->state = USBUS_STATE_ADDR;
                }
                ep0_handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_READY;
                /* Ready for new control request */
                usbdev_ep_xmit(ep0_handler->out, ep0_handler->out_buf, CONFIG_USBUS_EP0_SIZE);
            }
            break;
        case USBUS_CONTROL_REQUEST_STATE_OUTACK:
            if (ep->dir == USB_EP_DIR_OUT) {
                ep0_handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_READY;
                /* Ready for new control request */
                usbdev_ep_xmit(ep0_handler->out, ep0_handler->out_buf, CONFIG_USBUS_EP0_SIZE);
            }
            break;
        case USBUS_CONTROL_REQUEST_STATE_INDATA:
            if (ep->dir == USB_EP_DIR_IN) {
                if (usbus_control_slicer_nextslice(usbus)) {
                    _recv_setup(usbus, ep0_handler);
                    ep0_handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_INDATA;
                }
                else {
                    /* Ready out ZLP */
                    usbdev_ep_xmit(ep0_handler->out, ep0_handler->out_buf, CONFIG_USBUS_EP0_SIZE);
                    ep0_handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_OUTACK;
                }
            }
            break;
        case USBUS_CONTROL_REQUEST_STATE_OUTDATA:
            if (ep->dir == USB_EP_DIR_OUT) {
                size_t len = 0;
                usbdev_ep_get(ep0_handler->out, USBOPT_EP_AVAILABLE,
                              &len, sizeof(size_t));
                ep0_handler->received_len += len;
                _recv_setup(usbus, ep0_handler);
            }
            else {
                DEBUG("usbus_control: Invalid state OUTDATA with IN request\n");
            }
            break;
        case USBUS_CONTROL_REQUEST_STATE_READY:
            if (ep->dir == USB_EP_DIR_OUT) {
                memset(&ep0_handler->slicer, 0, sizeof(usbus_control_slicer_t));
                memcpy(&ep0_handler->setup, ep0_handler->out_buf,
                       sizeof(usb_setup_t));
                ep0_handler->received_len = 0;
                ep0_handler->slicer.reqlen = ep0_handler->setup.length;
                _recv_setup(usbus, ep0_handler);
            }
            else {
                DEBUG("usbus_control: invalid state, READY with IN request\n");
            }
            break;
        default:
            DEBUG("usbus_control: Invalid state\n");
            assert(false);
            break;
    }
    return 0;
}

/* USB endpoint 0 callback */
static void _handler_ep0_event(usbus_t *usbus, usbus_handler_t *handler,
                              usbus_event_usb_t event)
{
    usbus_control_handler_t *ep0_handler = (usbus_control_handler_t *)handler;

    (void)usbus;
    switch (event) {
        case USBUS_EVENT_USB_RESET:
            DEBUG("usbus_control: Reset event triggered\n");
            usbus->wakeup_enabled = false;
            ep0_handler->control_request_state = USBUS_CONTROL_REQUEST_STATE_READY;
            _usbus_config_ep0(ep0_handler);
            break;
        default:
            break;
    }
}

static void _handler_ep0_transfer(usbus_t *usbus, usbus_handler_t *handler,
                                 usbdev_ep_t *ep, usbus_event_transfer_t event)
{
    usbus_control_handler_t *ep0_handler = (usbus_control_handler_t *)handler;

    switch (event) {
        case USBUS_EVENT_TRANSFER_COMPLETE:
            _handle_tr_complete(usbus, ep0_handler, ep);
            break;
        default:
            break;
    }
}
