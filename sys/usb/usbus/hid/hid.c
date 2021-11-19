/*
 * Copyright (C) 2020 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usbus_hid
 * @{
 * @file
 *
 * @author  Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <string.h>

#include "usb/usbus.h"
#include "usb/usbus/control.h"
#include "usb/usbus/hid.h"
#include "tsrb.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _init(usbus_t *usbus, usbus_handler_t *handler);
static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                           usbus_event_usb_t event);
static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup);
static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event);

static void _handle_tx_ready(event_t *ev);

static const usbus_handler_driver_t hid_driver = {
    .init = _init,
    .event_handler = _event_handler,
    .control_handler = _control_handler,
    .transfer_handler = _transfer_handler
};

static size_t _gen_hid_descriptor(usbus_t *usbus, void *arg);

static const usbus_descr_gen_funcs_t _hid_descriptor = {
    .fmt_post_descriptor = _gen_hid_descriptor,
    .len = {
        .fixed_len = sizeof(usb_desc_hid_t)
    },
    .len_type = USBUS_DESCR_LEN_FIXED
};

static size_t _gen_hid_descriptor(usbus_t *usbus, void *arg)
{
    usbus_hid_device_t *hid_dev = arg;
    usb_desc_hid_t hid_desc;

    hid_desc.length = sizeof(usb_desc_hid_t);
    hid_desc.desc_type = USB_HID_DESCR_HID;
    hid_desc.bcd_hid = USB_HID_VERSION_BCD;
    hid_desc.country_code = USB_HID_COUNTRY_CODE_NOTSUPPORTED;
    hid_desc.num_descrs = 0x01;
    hid_desc.report_type = USB_HID_DESCR_REPORT;
    hid_desc.report_length = hid_dev->report_desc_size;

    usbus_control_slicer_put_bytes(usbus, (uint8_t *)&hid_desc,
                                   sizeof(hid_desc));
    return sizeof(usb_desc_hid_t);
}

static void _handle_tx_ready(event_t *ev)
{
    usbus_hid_device_t *hid = container_of(ev, usbus_hid_device_t, tx_ready);

    usbdev_ep_xmit(hid->ep_in->ep, hid->in_buf, hid->occupied);
}

void usbus_hid_init(usbus_t *usbus, usbus_hid_device_t *hid, usbus_hid_cb_t cb,
                    const uint8_t *report_desc, size_t report_desc_size)
{
    memset(hid, 0, sizeof(usbus_hid_device_t));
    hid->usbus = usbus;
    mutex_init(&hid->in_lock);
    hid->handler_ctrl.driver = &hid_driver;
    hid->report_desc = report_desc;
    hid->report_desc_size = report_desc_size;
    hid->cb = cb;

    DEBUG("hid_init: %d %d \n", report_desc_size, report_desc[0]);
    usbus_register_event_handler(usbus, &hid->handler_ctrl);
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    DEBUG("USB_HID: initialization\n");
    usbus_hid_device_t *hid = (usbus_hid_device_t *)handler;

    hid->tx_ready.handler = _handle_tx_ready;

    hid->hid_descr.next = NULL;
    hid->hid_descr.funcs = &_hid_descriptor;
    hid->hid_descr.arg = hid;

    /*
       Configure Interface as USB_HID interface, choosing NONE for subclass and
       protocol in order to represent a generic I/O device
     */
    hid->iface.class = USB_CLASS_HID;
    hid->iface.subclass = USB_HID_SUBCLASS_NONE;
    hid->iface.protocol = USB_HID_PROTOCOL_NONE;
    hid->iface.descr_gen = &hid->hid_descr;
    hid->iface.handler = handler;

    /* IN endpoint to send data to host */
    hid->ep_in = usbus_add_endpoint(usbus, &hid->iface,
                                    USB_EP_TYPE_INTERRUPT,
                                    USB_EP_DIR_IN,
                                    CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);

    /* interrupt endpoint polling rate in ms */
    hid->ep_in->interval = 0x05;

    usbus_enable_endpoint(hid->ep_in);

    /* OUT endpoint to receive data from host */
    hid->ep_out = usbus_add_endpoint(usbus, &hid->iface,
                                     USB_EP_TYPE_INTERRUPT, USB_EP_DIR_OUT,
                                     CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);

    /* interrupt endpoint polling rate in ms */
    hid->ep_out->interval = 0x05;

    usbus_enable_endpoint(hid->ep_out);

    usbus_add_interface(usbus, &hid->iface);

    /* Wait for data from HOST */
    usbdev_ep_xmit(hid->ep_out->ep, hid->out_buf,
                   CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
}

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                           usbus_event_usb_t event)
{
    (void)usbus;
    (void)handler;

    switch (event) {
    default:
        DEBUG("USB HID unhandled event: 0x%x\n", event);
        break;
    }
}

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup)
{
    usbus_hid_device_t *hid = (usbus_hid_device_t *)handler;

    DEBUG("USB_HID: request: %d type: %d value: %d length: %d state: %d \n",
          setup->request, setup->type, setup->value >> 8, setup->length, state);

    /* Requests defined in USB HID 1.11 spec section 7 */
    switch (setup->request) {
    case USB_SETUP_REQ_GET_DESCRIPTOR: {
        uint8_t desc_type = setup->value >> 8;
        if (desc_type == USB_HID_DESCR_REPORT) {
            usbus_control_slicer_put_bytes(usbus, hid->report_desc,
                                           hid->report_desc_size);
        }
        else if (desc_type == USB_HID_DESCR_HID) {
            _gen_hid_descriptor(usbus, hid);
        }
        break;
    }
    case USB_HID_REQUEST_GET_REPORT:
        break;
    case USB_HID_REQUEST_GET_IDLE:
        break;
    case USB_HID_REQUEST_GET_PROTOCOL:
        break;
    case USB_HID_REQUEST_SET_REPORT:
        if ((state == USBUS_CONTROL_REQUEST_STATE_OUTDATA)) {
            size_t size = 0;
            uint8_t *data = usbus_control_get_out_data(usbus, &size);
            if (size > 0) {
                hid->cb(hid, data, size);
            }
        }
        break;
    case USB_HID_REQUEST_SET_IDLE:
        /* Wait for data from HOST */
        usbdev_ep_xmit(hid->ep_out->ep, hid->out_buf,
                       CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
        break;
    case USB_HID_REQUEST_SET_PROTOCOL:
        break;
    default:
        DEBUG("USB_HID: unknown request %d \n", setup->request);
        return -1;
    }
    return 1;
}

static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event)
{
    (void)usbus;
    (void)event;
    DEBUG("USB_HID: transfer_handler\n");

    usbus_hid_device_t *hid = (usbus_hid_device_t *)handler;

    if ((ep->dir == USB_EP_DIR_IN) && (ep->type == USB_EP_TYPE_INTERRUPT)) {
        mutex_unlock(&hid->in_lock);
        hid->occupied = 0;
    }
    else if ((ep->dir == USB_EP_DIR_OUT) &&
             (ep->type == USB_EP_TYPE_INTERRUPT)) {
        size_t len;
        usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
        if (len > 0) {
            hid->cb(hid, hid->out_buf, len);
        }
        usbdev_ep_xmit(ep, hid->out_buf, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
    }
}
