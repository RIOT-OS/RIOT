/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usb_usbus_fmt
 * @{
 * @file
 * @brief   USBUS protocol message formatting functions
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <string.h>
#include <stdio.h>
#include "usb/descriptor.h"
#include "usb/usbus/fmt.h"
#include "usb/usbus/control.h"

static size_t _num_ifaces(usbus_t *usbus)
{
    size_t num = 0;

    for (usbus_interface_t *iface = usbus->iface;
         iface;
         iface = iface->next) {
        num++;
    }
    return num;
}

static size_t _num_endpoints(usbus_interface_t *iface)
{
    size_t num = 0;

    for (usbus_endpoint_t *ep = iface->ep;
         ep; ep = ep->next) {
        num++;
    }
    return num;
}

static uint8_t _type_to_attribute(usbus_endpoint_t *ep)
{
    switch (ep->ep->type) {
        case USB_EP_TYPE_CONTROL:
            return 0x00;
        case USB_EP_TYPE_ISOCHRONOUS:
            return 0x01;
        case USB_EP_TYPE_BULK:
            return 0x02;
        case USB_EP_TYPE_INTERRUPT:
            return 0x03;
        default:
            assert(false);
            break;
    }
    return 0x00;
}

static size_t _num_endpoints_alt(usbus_interface_alt_t *alt)
{
    size_t num = 0;

    for (usbus_endpoint_t *ep = alt->ep;
         ep; ep = ep->next) {
        num++;
    }
    return num;
}

static inline size_t call_get_header_len(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    return hdr->funcs->len_type == USBUS_HDR_LEN_FIXED ?
           hdr->funcs->len.fixed_len :
           hdr->funcs->len.get_header_len(usbus, hdr->arg);
}

static size_t _hdr_gen_size(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    size_t len = 0;

    for (; hdr; hdr = hdr->next) {
        len += call_get_header_len(usbus, hdr);
    }
    return len;
}

static size_t _ep_size(usbus_t *usbus, usbus_endpoint_t *ep)
{
    size_t len = 0;

    for (; ep; ep = ep->next) {
        len += sizeof(usb_descriptor_endpoint_t);
        len += _hdr_gen_size(usbus, ep->hdr_gen);
    }
    return len;
}

static size_t _alt_size(usbus_t *usbus, usbus_interface_alt_t *alt)
{
    size_t len = 0;

    for (; alt; alt = alt->next) {
        len += sizeof(usb_descriptor_interface_t);
        len += _hdr_gen_size(usbus, alt->hdr_gen);
        len += _ep_size(usbus, alt->ep);
    }
    return len;
}

static size_t _hdrs_config_size(usbus_t *usbus)
{
    size_t len = sizeof(usb_descriptor_configuration_t);

    len += _hdr_gen_size(usbus, usbus->hdr_gen);
    for (usbus_interface_t *iface = usbus->iface;
         iface;
         iface = iface->next) {
        len += sizeof(usb_descriptor_interface_t);
        len += _hdr_gen_size(usbus, iface->hdr_gen);
        len += _ep_size(usbus, iface->ep);
        len += _alt_size(usbus, iface->alts);
    }
    return len;
}

static inline size_t _get_pre_header(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    return (hdr->funcs->fmt_pre_descriptor != NULL)
        ? hdr->funcs->fmt_pre_descriptor(usbus, hdr->arg)
        : 0;
}

static inline size_t _get_additional_header(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    return (hdr->funcs->get_header != NULL)
        ? hdr->funcs->get_header(usbus, hdr->arg)
        : 0;
}

static size_t _hdrs_fmt_pre(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    size_t len = 0;

    for (; hdr; hdr = hdr->next) {
        len += _get_pre_header(usbus, hdr);
    }
    return len;
}

static size_t _hdrs_fmt_additional(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    size_t len = 0;

    for (; hdr; hdr = hdr->next) {
        len += _get_additional_header(usbus, hdr);
    }
    return len;
}

static size_t _hdrs_fmt_hdrs(usbus_t *usbus)
{
    return _hdrs_fmt_additional(usbus, usbus->hdr_gen);
}

static size_t _hdrs_fmt_endpoints(usbus_t *usbus, usbus_endpoint_t *ep)
{
    size_t len = 0;

    while (ep) {
        _hdrs_fmt_pre(usbus, ep->hdr_gen);
        usb_descriptor_endpoint_t usb_ep;
        memset(&usb_ep, 0, sizeof(usb_descriptor_endpoint_t));
        usb_ep.length = sizeof(usb_descriptor_endpoint_t);
        usb_ep.type = USB_TYPE_DESCRIPTOR_ENDPOINT;
        usb_ep.address = ep->ep->num;
        if (ep->ep->dir == USB_EP_DIR_IN) {
            usb_ep.address |= 0x80;
        }
        usb_ep.attributes = _type_to_attribute(ep);
        usb_ep.max_packet_size = ep->maxpacketsize;
        usb_ep.interval = ep->interval;
        usbus_control_slicer_put_bytes(usbus, (uint8_t *)&usb_ep,
                                       sizeof(usb_descriptor_endpoint_t));
        _hdrs_fmt_additional(usbus, ep->hdr_gen);
        len += usb_ep.length;
        /* iterate to next endpoint */
        ep = ep->next;
    }
    return len;
}

static void _hdrs_fmt_iface(usbus_interface_t *iface,
                            usb_descriptor_interface_t *usb_iface)
{
    memset(usb_iface, 0, sizeof(usb_descriptor_interface_t));
    usb_iface->length = sizeof(usb_descriptor_interface_t);
    usb_iface->type = USB_TYPE_DESCRIPTOR_INTERFACE;
    usb_iface->interface_num = iface->idx;
    usb_iface->class = iface->class;
    usb_iface->subclass = iface->subclass;
    usb_iface->protocol = iface->protocol;
}

static size_t _hdrs_fmt_iface_alts(usbus_t *usbus, usbus_interface_t *iface)
{
    size_t len = 0;
    uint8_t alts = 1;

    for (usbus_interface_alt_t *alt = iface->alts;
         alt;
         alt = alt->next) {
        usb_descriptor_interface_t usb_iface;
        _hdrs_fmt_iface(iface, &usb_iface);
        usb_iface.alternate_setting = alts++;
        usb_iface.num_endpoints = _num_endpoints_alt(alt);
        usbus_control_slicer_put_bytes(usbus, (uint8_t *)&usb_iface,
                                       sizeof(usb_descriptor_interface_t));
        len += _hdrs_fmt_additional(usbus, alt->hdr_gen);
        len += _hdrs_fmt_endpoints(usbus, alt->ep);
    }
    return len;
}

static size_t _hdrs_fmt_ifaces(usbus_t *usbus)
{
    size_t len = 0;

    for (usbus_interface_t *iface = usbus->iface;
         iface;
         iface = iface->next) {
        len += _hdrs_fmt_pre(usbus, iface->hdr_gen);
        usb_descriptor_interface_t usb_iface;
        _hdrs_fmt_iface(iface, &usb_iface);
        usb_iface.num_endpoints = _num_endpoints(iface);
        if (iface->descr) {
            usb_iface.idx = iface->descr->idx;
        }
        else {
            usb_iface.idx = 0;
        }
        usbus_control_slicer_put_bytes(usbus, (uint8_t *)&usb_iface,
                                       sizeof(usb_descriptor_interface_t));
        len += sizeof(usb_descriptor_interface_t);
        len += _hdrs_fmt_additional(usbus, iface->hdr_gen);
        len += _hdrs_fmt_endpoints(usbus, iface->ep);
        len += _hdrs_fmt_iface_alts(usbus, iface);
    }
    return len;
}

size_t usbus_fmt_hdr_conf(usbus_t *usbus)
{
    size_t len = 0;
    usb_descriptor_configuration_t conf;

    memset(&conf, 0, sizeof(usb_descriptor_configuration_t));
    conf.length = sizeof(usb_descriptor_configuration_t);
    conf.type = USB_TYPE_DESCRIPTOR_CONFIGURATION;
    conf.total_length = sizeof(usb_descriptor_configuration_t);
    conf.val = 1;
    conf.attributes = USB_CONF_ATTR_RESERVED;
    if (USB_CONFIG_SELF_POWERED) {
        conf.attributes |= USB_CONF_ATTR_SELF_POWERED;
    }
    /* TODO: upper bound */
    /* USB max power is reported in increments of 2 mA */
    conf.max_power = USB_CONFIG_MAX_POWER / 2;
    conf.num_interfaces = _num_ifaces(usbus);
    len += sizeof(usb_descriptor_configuration_t);
    conf.total_length = _hdrs_config_size(usbus);
    conf.idx = usbus->config.idx;
    usbus_control_slicer_put_bytes(usbus, (uint8_t *)&conf, sizeof(conf));
    len += _hdrs_fmt_hdrs(usbus);
    len += _hdrs_fmt_ifaces(usbus);
    return len;
}

size_t usbus_fmt_hdr_dev(usbus_t *usbus)
{
    usb_descriptor_device_t desc;

    memset(&desc, 0, sizeof(usb_descriptor_device_t));
    desc.length = sizeof(usb_descriptor_device_t);
    desc.type = USB_TYPE_DESCRIPTOR_DEVICE;
    desc.bcd_usb = USB_CONFIG_SPEC_BCDVERSION;
    desc.max_packet_size = USBUS_EP0_SIZE;
    desc.vendor_id = USB_CONFIG_VID;
    desc.product_id = USB_CONFIG_PID;
    desc.manufacturer_idx = usbus->manuf.idx;
    desc.product_idx = usbus->product.idx;
    /* USBUS supports only a single config at the moment */
    desc.num_configurations = 1;
    usbus_control_slicer_put_bytes(usbus, (uint8_t *)&desc,
                                   sizeof(usb_descriptor_device_t));
    return sizeof(usb_descriptor_device_t);
}
