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

#define USB_H_USER_IS_RIOT_INTERNAL

#include <assert.h>
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

static size_t _gen_descriptor_size(usbus_t *usbus, usbus_descr_gen_t *descr)
{
    size_t len = 0;

    for (; descr; descr = descr->next) {
        len += (descr->funcs->len_type == USBUS_DESCR_LEN_FIXED)
            ? descr->funcs->len.fixed_len
            : descr->funcs->len.get_descriptor_len(usbus, descr->arg);
    }
    return len;
}

static size_t _ep_size(usbus_t *usbus, usbus_endpoint_t *ep)
{
    size_t len = 0;

    for (; ep; ep = ep->next) {
        len += sizeof(usb_descriptor_endpoint_t);
        len += _gen_descriptor_size(usbus, ep->descr_gen);
    }
    return len;
}

static size_t _alt_size(usbus_t *usbus, usbus_interface_alt_t *alt)
{
    size_t len = 0;

    for (; alt; alt = alt->next) {
        len += sizeof(usb_descriptor_interface_t);
        len += _gen_descriptor_size(usbus, alt->descr_gen);
        len += _ep_size(usbus, alt->ep);
    }
    return len;
}

static size_t _gen_config_descriptor_size(usbus_t *usbus)
{
    size_t len = sizeof(usb_descriptor_configuration_t);

    len += _gen_descriptor_size(usbus, usbus->descr_gen);
    for (usbus_interface_t *iface = usbus->iface;
         iface;
         iface = iface->next) {
        len += sizeof(usb_descriptor_interface_t);
        len += _gen_descriptor_size(usbus, iface->descr_gen);
        len += _ep_size(usbus, iface->ep);
        len += _alt_size(usbus, iface->alts);
    }
    return len;
}

static size_t _fmt_descriptors_pre(usbus_t *usbus, usbus_descr_gen_t *descr)
{
    size_t len = 0;

    for (; descr; descr = descr->next) {
        len += (descr->funcs->fmt_pre_descriptor != NULL)
            ? descr->funcs->fmt_pre_descriptor(usbus, descr->arg)
            : 0;
    }
    return len;
}

static size_t _fmt_descriptors_post(usbus_t *usbus, usbus_descr_gen_t *descr)
{
    size_t len = 0;

    for (; descr; descr = descr->next) {
        len += (descr->funcs->fmt_post_descriptor!= NULL)
            ? descr->funcs->fmt_post_descriptor(usbus, descr->arg)
            : 0;
    }
    return len;
}

static size_t _fmt_descriptors_endpoints(usbus_t *usbus, usbus_endpoint_t *ep)
{
    size_t len = 0;

    while (ep) {
        _fmt_descriptors_pre(usbus, ep->descr_gen);
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
        _fmt_descriptors_post(usbus, ep->descr_gen);
        len += usb_ep.length;
        /* iterate to next endpoint */
        ep = ep->next;
    }
    return len;
}

static void _fmt_descriptor_iface(usbus_interface_t *iface,
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

static size_t _fmt_descriptors_iface_alts(usbus_t *usbus,
                                          usbus_interface_t *iface)
{
    size_t len = 0;
    uint8_t alts = 1;

    for (usbus_interface_alt_t *alt = iface->alts;
         alt;
         alt = alt->next) {
        usb_descriptor_interface_t usb_iface;
        _fmt_descriptor_iface(iface, &usb_iface);
        len += sizeof(usb_descriptor_interface_t);
        usb_iface.alternate_setting = alts++;
        usb_iface.num_endpoints = _num_endpoints_alt(alt);
        usbus_control_slicer_put_bytes(usbus, (uint8_t *)&usb_iface,
                                       sizeof(usb_descriptor_interface_t));
        len += _fmt_descriptors_post(usbus, alt->descr_gen);
        len += _fmt_descriptors_endpoints(usbus, alt->ep);
    }
    return len;
}

static size_t _fmt_descriptors_ifaces(usbus_t *usbus)
{
    size_t len = 0;

    for (usbus_interface_t *iface = usbus->iface;
         iface;
         iface = iface->next) {
        len += _fmt_descriptors_pre(usbus, iface->descr_gen);
        usb_descriptor_interface_t usb_iface;
        _fmt_descriptor_iface(iface, &usb_iface);
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
        len += _fmt_descriptors_post(usbus, iface->descr_gen);
        len += _fmt_descriptors_endpoints(usbus, iface->ep);
        len += _fmt_descriptors_iface_alts(usbus, iface);
    }
    return len;
}

size_t usbus_fmt_descriptor_conf(usbus_t *usbus)
{
    size_t len = 0;
    usb_descriptor_configuration_t conf;

    memset(&conf, 0, sizeof(usb_descriptor_configuration_t));
    conf.length = sizeof(usb_descriptor_configuration_t);
    conf.type = USB_TYPE_DESCRIPTOR_CONFIGURATION;
    conf.total_length = sizeof(usb_descriptor_configuration_t);
    conf.val = 1;
    conf.attributes = USB_CONF_ATTR_RESERVED;
    if (CONFIG_USB_SELF_POWERED) {
        conf.attributes |= USB_CONF_ATTR_SELF_POWERED;
    }
    /* TODO: upper bound */
    /* USB max power is reported in increments of 2 mA */
    conf.max_power = CONFIG_USB_MAX_POWER / 2;
    conf.num_interfaces = _num_ifaces(usbus);
    len += sizeof(usb_descriptor_configuration_t);
    conf.total_length = _gen_config_descriptor_size(usbus);
    conf.idx = usbus->config.idx;
    usbus_control_slicer_put_bytes(usbus, (uint8_t *)&conf, sizeof(conf));
    len += _fmt_descriptors_post(usbus, usbus->descr_gen);
    len += _fmt_descriptors_ifaces(usbus);
    assert(len == conf.total_length);
    return len;
}

size_t usbus_fmt_descriptor_dev(usbus_t *usbus)
{
    usb_descriptor_device_t desc;

    memset(&desc, 0, sizeof(usb_descriptor_device_t));
    desc.length = sizeof(usb_descriptor_device_t);
    desc.type = USB_TYPE_DESCRIPTOR_DEVICE;
    desc.bcd_usb = CONFIG_USB_SPEC_BCDVERSION;
    desc.max_packet_size = CONFIG_USBUS_EP0_SIZE;
    desc.vendor_id = CONFIG_USB_VID;
    desc.product_id = CONFIG_USB_PID;
    desc.manufacturer_idx = usbus->manuf.idx;
    desc.product_idx = usbus->product.idx;
    /* USBUS supports only a single config at the moment */
    desc.num_configurations = 1;
    usbus_control_slicer_put_bytes(usbus, (uint8_t *)&desc,
                                   sizeof(usb_descriptor_device_t));
    return sizeof(usb_descriptor_device_t);
}
