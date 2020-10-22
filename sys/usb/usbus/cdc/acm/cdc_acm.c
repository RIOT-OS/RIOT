/*
 * Copyright (C) 2018 Dylan Laduranty
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usbus_cdc_acm
 * @{
 * @file
 *
 * @author  Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <assert.h>
#include <string.h>

#include "tsrb.h"
#include "usb/descriptor.h"
#include "usb/cdc.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"
#include "usb/usbus/control.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _init(usbus_t *usbus, usbus_handler_t *handler);
static void _event_handler(usbus_t *usbus, usbus_handler_t *handler, usbus_event_usb_t event);
static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup);
static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                             usbdev_ep_t *ep, usbus_event_transfer_t event);

static void _handle_flush(event_t *ev);

static const usbus_handler_driver_t cdc_driver = {
    .init = _init,
    .event_handler = _event_handler,
    .control_handler = _control_handler,
    .transfer_handler = _transfer_handler,
};

static size_t _gen_full_acm_descriptor(usbus_t *usbus, void *arg);
static size_t _gen_assoc_descriptor(usbus_t *usbus, void *arg);

/* Descriptors */
static const usbus_descr_gen_funcs_t _cdcacm_descriptor = {
    .fmt_post_descriptor = _gen_full_acm_descriptor,
    .fmt_pre_descriptor = _gen_assoc_descriptor,
    .len = {
        .fixed_len = sizeof(usb_descriptor_interface_association_t) +
                     sizeof(usb_desc_cdc_t) +
                     sizeof(usb_desc_acm_t) +
                     sizeof(usb_desc_union_t) +
                     sizeof(usb_desc_call_mngt_t),
    },
    .len_type = USBUS_DESCR_LEN_FIXED,
};

static size_t _gen_assoc_descriptor(usbus_t *usbus, void *arg)
{
    usbus_cdcacm_device_t *cdcacm = arg;
    usb_descriptor_interface_association_t iad;
    iad.length = sizeof(usb_descriptor_interface_association_t);
    iad.type = USB_TYPE_DESCRIPTOR_INTERFACE_ASSOC;
    iad.first_interface = cdcacm->iface_ctrl.idx;
    iad.interface_count = 2; /* Management and data interface */
    iad.class = USB_CLASS_CDC_CONTROL;
    iad.subclass = USB_CDC_SUBCLASS_ACM;
    iad.protocol = 0;
    iad.idx = 0;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&iad, sizeof(iad));
    return sizeof(iad);
}

static size_t _gen_mngt_descriptor(usbus_t *usbus, usbus_cdcacm_device_t *cdcacm)
{
    usb_desc_call_mngt_t mngt;
    /* functional call management descriptor */
    mngt.length = sizeof(usb_desc_call_mngt_t);
    mngt.type = USB_TYPE_DESCRIPTOR_CDC;
    mngt.subtype = USB_CDC_DESCR_SUBTYPE_CALL_MGMT;
    mngt.capabalities = 0;
    mngt.data_if = cdcacm->iface_data.idx;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&mngt, sizeof(mngt));
    return sizeof(usb_desc_call_mngt_t);
}

static size_t _gen_union_descriptor(usbus_t *usbus,
                                    usbus_cdcacm_device_t *cdcacm)
{
    usb_desc_union_t uni;
    /* functional union descriptor */
    uni.length = sizeof(usb_desc_union_t);
    uni.type = USB_TYPE_DESCRIPTOR_CDC;
    uni.subtype = USB_CDC_DESCR_SUBTYPE_UNION;
    uni.master_if = cdcacm->iface_ctrl.idx;
    uni.slave_if = cdcacm->iface_data.idx;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&uni, sizeof(uni));
    return sizeof(usb_desc_union_t);
}

static size_t _gen_acm_descriptor(usbus_t *usbus)
{
    usb_desc_acm_t acm;
    /* functional cdc acm descriptor */
    acm.length = sizeof(usb_desc_acm_t);
    acm.type = USB_TYPE_DESCRIPTOR_CDC;
    acm.subtype = USB_CDC_DESCR_SUBTYPE_ACM;
    /* Support for Set/Get_Line_coding, Control_State, and Serial_State notif */
    acm.capabalities = 0x02;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&acm, sizeof(acm));
    return sizeof(usb_desc_acm_t);
}

static size_t _gen_cdc_descriptor(usbus_t *usbus)
{
    usb_desc_cdc_t cdc;
    /* functional cdc descriptor */
    cdc.length = sizeof(usb_desc_cdc_t);
    cdc.bcd_cdc = USB_CDC_VERSION_BCD;
    cdc.type = USB_TYPE_DESCRIPTOR_CDC;
    cdc.subtype = USB_CDC_DESCR_SUBTYPE_FUNCTIONAL;
    usbus_control_slicer_put_bytes(usbus, (uint8_t*)&cdc, sizeof(cdc));
    return sizeof(usb_desc_cdc_t);
}

static size_t _gen_full_acm_descriptor(usbus_t *usbus, void *arg)
{
    usbus_cdcacm_device_t *cdcacm = (usbus_cdcacm_device_t*)arg;
    size_t total_len = 0;
    total_len += _gen_cdc_descriptor(usbus);
    total_len += _gen_acm_descriptor(usbus);
    total_len += _gen_union_descriptor(usbus, cdcacm);
    total_len += _gen_mngt_descriptor(usbus, cdcacm);
    return total_len;
}

/* Submit (ACM interface in) */
size_t usbus_cdc_acm_submit(usbus_cdcacm_device_t *cdcacm, const uint8_t *buf, size_t len)
{
    size_t n;
    unsigned old;
    if (cdcacm->state != USBUS_CDC_ACM_LINE_STATE_DISCONNECTED) {
        old = irq_disable();
        n = tsrb_add(&cdcacm->tsrb, buf, len);
        irq_restore(old);
        return n;
    }
    /* stuff as much data as possible into tsrb, discarding the oldest */
    old = irq_disable();
    n = tsrb_free(&cdcacm->tsrb);
    if (len > n) {
        n += tsrb_drop(&cdcacm->tsrb, len - n);
        buf += len - n;
    } else {
        n = len;
    }
    tsrb_add(&cdcacm->tsrb, buf, n);
    /* behave as if everything has been written correctly */
    irq_restore(old);
    return len;
}

void usbus_cdc_acm_set_coding_cb(usbus_cdcacm_device_t *cdcacm,
                                 usbus_cdcacm_coding_cb_t coding_cb)
{
    unsigned old = irq_disable();
    cdcacm->coding_cb = coding_cb;
    irq_restore(old);
}

/* flush event */
void usbus_cdc_acm_flush(usbus_cdcacm_device_t *cdcacm)
{
    if (cdcacm->usbus) {
        usbus_event_post(cdcacm->usbus, &cdcacm->flush);
    }
}

void usbus_cdc_acm_init(usbus_t *usbus, usbus_cdcacm_device_t *cdcacm,
                        usbus_cdcacm_cb_t cb, usbus_cdcacm_coding_cb_t coding_cb,
                        uint8_t *buf, size_t len)
{
    memset(cdcacm, 0, sizeof(usbus_cdcacm_device_t));
    cdcacm->usbus = usbus;
    tsrb_init(&cdcacm->tsrb, buf, len);
    cdcacm->handler_ctrl.driver = &cdc_driver;
    cdcacm->cb = cb;
    cdcacm->coding_cb = coding_cb;
    cdcacm->state = USBUS_CDC_ACM_LINE_STATE_DISCONNECTED;
    usbus_register_event_handler(usbus, &cdcacm->handler_ctrl);
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    DEBUG("CDC_ACM: initialization\n");
    usbus_cdcacm_device_t *cdcacm = (usbus_cdcacm_device_t*)handler;

    cdcacm->flush.handler = _handle_flush;

    cdcacm->cdcacm_descr.next = NULL;
    cdcacm->cdcacm_descr.funcs = &_cdcacm_descriptor;
    cdcacm->cdcacm_descr.arg = cdcacm;

    /* Configure Interface 0 as control interface */
    cdcacm->iface_ctrl.class = USB_CLASS_CDC_CONTROL ;
    cdcacm->iface_ctrl.subclass = USB_CDC_SUBCLASS_ACM;
    cdcacm->iface_ctrl.protocol = USB_CDC_PROTOCOL_NONE;
    cdcacm->iface_ctrl.descr_gen = &cdcacm->cdcacm_descr;
    cdcacm->iface_ctrl.handler = handler;
    /* Configure second interface to handle data endpoint */
    cdcacm->iface_data.class = USB_CLASS_CDC_DATA ;
    cdcacm->iface_data.subclass = USB_CDC_SUBCLASS_NONE;
    cdcacm->iface_data.protocol = USB_CDC_PROTOCOL_NONE;
    cdcacm->iface_data.descr_gen = NULL;
    cdcacm->iface_data.handler = handler;

    /* Create required endpoints */
    usbus_endpoint_t *ep = usbus_add_endpoint(usbus, &cdcacm->iface_ctrl,
                                              USB_EP_TYPE_INTERRUPT,
                                              USB_EP_DIR_IN, 8);
    ep->interval = 255; /* Max interval */
    usbus_enable_endpoint(ep);
    ep = usbus_add_endpoint(usbus, &cdcacm->iface_data,
                            USB_EP_TYPE_BULK, USB_EP_DIR_IN,
                            CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE);
    ep->interval = 0; /* Interval is not used with bulk endpoints */
    usbus_enable_endpoint(ep);
    /* Store the endpoint reference to activate it
     * when DTE present is signalled by the host */
    ep = usbus_add_endpoint(usbus, &cdcacm->iface_data,
                            USB_EP_TYPE_BULK, USB_EP_DIR_OUT,
                            CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE);
    ep->interval = 0; /* Interval is not used with bulk endpoints */
    usbus_enable_endpoint(ep);

    /* Add interfaces to the stack */
    usbus_add_interface(usbus, &cdcacm->iface_ctrl);
    usbus_add_interface(usbus, &cdcacm->iface_data);

    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);
}

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup)
{
    (void)state;
    usbus_cdcacm_device_t *cdcacm = (usbus_cdcacm_device_t*)handler;
    switch(setup->request) {
        case USB_CDC_MGNT_REQUEST_SET_LINE_CODING:
            if ((state == USBUS_CONTROL_REQUEST_STATE_OUTDATA) &&
                    (setup->length == sizeof(usb_req_cdcacm_coding_t))) {
                size_t len = 0;
                usb_req_cdcacm_coding_t *coding =
                    (usb_req_cdcacm_coding_t*)usbus_control_get_out_data(usbus,
                                                                         &len);

                if (len != sizeof(usb_req_cdcacm_coding_t)) {
                    DEBUG("CDCACM: incorrect length of LINE_CODING set request"
                          ", expected: %u, got: %u",
                          sizeof(usb_req_cdcacm_coding_t), len);
                    return -1;
                }
                if (cdcacm->coding_cb) {
                    DEBUG("Setting line coding to baud rate %" PRIu32 ", "
                          "%u data bits, parity value %u, stop bit value %u\n",
                          coding->baud, coding->databits, coding->parity,
                          coding->format);
                    if (cdcacm->coding_cb(cdcacm, coding->baud,
                                          coding->databits, coding->parity,
                                          coding->format) < 0) {
                        return -1;
                    }
                }
                memcpy(&cdcacm->coding, coding,
                       sizeof(usb_req_cdcacm_coding_t));
            }
            break;
        case USB_CDC_MGNT_REQUEST_GET_LINE_CODING:
            usbus_control_slicer_put_bytes(usbus, (uint8_t*)&cdcacm->coding,
                               sizeof(usb_req_cdcacm_coding_t));
            break;
        case USB_CDC_MGNT_REQUEST_SET_CONTROL_LINE_STATE:
            if (setup->value & USB_CDC_ACM_CONTROL_LINE_DTE) {
                DEBUG("CDC ACM: DTE enabled on interface %u\n", setup->index);
                cdcacm->state = USBUS_CDC_ACM_LINE_STATE_DTE;
                usbus_endpoint_t *data_out = usbus_interface_find_endpoint(
                        &cdcacm->iface_data, USB_EP_TYPE_BULK, USB_EP_DIR_OUT);
                assert(data_out);
                usbdev_ep_ready(data_out->ep, 0);
                usbus_cdc_acm_flush(cdcacm);
            }
            else {
                cdcacm->state = USBUS_CDC_ACM_LINE_STATE_DISCONNECTED;
                DEBUG("CDC ACM: DTE disabled on interface %u\n", setup->index);
            }
            break;
        default:
            DEBUG("unhandled USB setup request:0x%x\n", setup->request);
            return -1;
    }
    return 1;
}

static void _handle_in(usbus_cdcacm_device_t *cdcacm,
                       usbdev_ep_t *ep)
{
    if ((cdcacm->usbus->state != USBUS_STATE_CONFIGURED) ||
        (cdcacm->state != USBUS_CDC_ACM_LINE_STATE_DTE)) {
        return;
    }
    /* copy at most CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE chars from input into ep->buf */
    unsigned old = irq_disable();
    while (!tsrb_empty(&cdcacm->tsrb)) {
        int c = tsrb_get_one(&cdcacm->tsrb);
        ep->buf[cdcacm->occupied++] = (uint8_t)c;
        if (cdcacm->occupied >= CONFIG_USBUS_CDC_ACM_BULK_EP_SIZE) {
            break;
        }
    }
    irq_restore(old);
    usbdev_ep_ready(ep, cdcacm->occupied);
}

static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                             usbdev_ep_t *ep, usbus_event_transfer_t event)
{
    (void)usbus;
    (void)event; /* Only receives TR_COMPLETE events */
    usbus_cdcacm_device_t *cdcacm = (usbus_cdcacm_device_t*)handler;
    if ((ep->dir == USB_EP_DIR_OUT) && (ep->type == USB_EP_TYPE_BULK)) {
        size_t len;
        /* Retrieve incoming data */
        usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
        if (len > 0) {
            cdcacm->cb(cdcacm, ep->buf, len);
        }
        usbdev_ep_ready(ep, 0);
    }
    if ((ep->dir == USB_EP_DIR_IN) && (ep->type == USB_EP_TYPE_BULK)) {
        cdcacm->occupied = 0;
        if (!tsrb_empty(&cdcacm->tsrb)) {
            return _handle_in(cdcacm, ep);
        }
    }
}

static void _handle_flush(event_t *ev)
{
    usbus_cdcacm_device_t *cdcacm = container_of(ev, usbus_cdcacm_device_t,
                                                 flush);
    if (cdcacm->occupied == 0) {
        _handle_in(cdcacm, cdcacm->iface_data.ep->next->ep);
    }
}

static void _handle_reset(usbus_handler_t *handler)
{
    usbus_cdcacm_device_t *cdcacm = (usbus_cdcacm_device_t *)handler;
    DEBUG("CDC ACM: Reset notification received\n");

    cdcacm->state = USBUS_CDC_ACM_LINE_STATE_DISCONNECTED;
}

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler, usbus_event_usb_t event)
{
    (void)usbus;
    switch(event) {
        case USBUS_EVENT_USB_RESET:
            _handle_reset(handler);
            break;

        default:
            DEBUG("Unhandled event :0x%x\n", event);
            break;
    }
}
