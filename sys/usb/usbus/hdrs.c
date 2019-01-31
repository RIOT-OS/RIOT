/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usb_usbus_hdrs
 * @{
 * @file
 * @brief   USBUS header formatting functions
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <string.h>
#include <stdio.h>
#include "usb/usbus/hdrs.h"
#include "usb/message.h"

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
    switch(ep->ep->type) {
        case USB_EP_TYPE_NONE:
        case USB_EP_TYPE_CONTROL:
            return 0x00;
        case USB_EP_TYPE_ISOCHRONOUS:
            return 0x01;
        case USB_EP_TYPE_BULK:
            return 0x02;
        case USB_EP_TYPE_INTERRUPT:
            return 0x03;
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

size_t _hdr_gen_size(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    size_t len = 0;
    for (; hdr; hdr = hdr->next) {
        len += hdr->hdr_len(usbus, hdr->arg);
    }
    return len;
}

size_t _ep_size(usbus_t *usbus, usbus_endpoint_t *ep)
{
    size_t len = 0;
    for (; ep; ep = ep->next) {
        len += sizeof(usb_descriptor_endpoint_t);
        len += _hdr_gen_size(usbus, ep->hdr_gen);
    }
    return len;
}

size_t _alt_size(usbus_t *usbus, usbus_interface_alt_t *alt)
{
    size_t len = 0;
    for (; alt; alt = alt->next) {
        len += sizeof(usb_descriptor_interface_t);
        len += _hdr_gen_size(usbus, alt->hdr_gen);
        len += _ep_size(usbus, alt->ep);
    }
    return len;
}

size_t usbus_hdrs_config_size(usbus_t *usbus)
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

size_t usbus_hdrs_fmt_hdrs(usbus_t *usbus)
{
    return usbus_hdrs_fmt_additional(usbus, usbus->hdr_gen);
}

size_t usbus_hdrs_fmt_additional(usbus_t *usbus, usbus_hdr_gen_t *hdr)
{
    size_t len = 0;
    for (; hdr; hdr = hdr->next) {
        len += hdr->gen_hdr(usbus, hdr->arg);
    }
    return len;
}

size_t usbus_hdrs_fmt_iface_alts(usbus_t *usbus, usbus_interface_t *iface)
{
    size_t len = 0;
    uint8_t alts = 1;
    for (usbus_interface_alt_t *alt = iface->alts;
            alt;
            alt = alt->next) {
        usb_descriptor_interface_t usb_iface;
        memset(&usb_iface, 0 , sizeof(usb_descriptor_interface_t));
        usb_iface.length = sizeof(usb_descriptor_interface_t);
        usb_iface.type = USB_TYPE_DESCRIPTOR_INTERFACE;
        usb_iface.interface_num = iface->idx;
        usb_iface.alternate_setting = alts++;
        usb_iface.class = iface->class;
        usb_iface.subclass = iface->subclass;
        usb_iface.protocol = iface->protocol;
        usb_iface.num_endpoints = _num_endpoints_alt(alt);
        usbus_put_bytes(usbus, (uint8_t*)&usb_iface, sizeof(usb_descriptor_interface_t));
        len += usbus_hdrs_fmt_additional(usbus, alt->hdr_gen);
        len += usbus_hdrs_fmt_endpoints(usbus, alt->ep);
    }
    return len;
}

size_t usbus_hdrs_fmt_ifaces(usbus_t *usbus)
{
    size_t len = 0;
    for (usbus_interface_t *iface = usbus->iface;
            iface;
            iface = iface->next) {
        usb_descriptor_interface_t usb_iface;
        memset(&usb_iface, 0 , sizeof(usb_descriptor_interface_t));
        usb_iface.length = sizeof(usb_descriptor_interface_t);
        usb_iface.type = USB_TYPE_DESCRIPTOR_INTERFACE;
        usb_iface.interface_num = iface->idx;
        usb_iface.alternate_setting = 0;
        usb_iface.class = iface->class;
        usb_iface.subclass = iface->subclass;
        usb_iface.protocol = iface->protocol;
        usb_iface.num_endpoints = _num_endpoints(iface);
        if (iface->descr) {
            usb_iface.idx = iface->descr->idx;
        }
        else {
            usb_iface.idx = 0;
        }
        usbus_put_bytes(usbus, (uint8_t*)&usb_iface, sizeof(usb_descriptor_interface_t));
        len += sizeof(usb_descriptor_interface_t);
        len += usbus_hdrs_fmt_additional(usbus, iface->hdr_gen);
        len += usbus_hdrs_fmt_endpoints(usbus, iface->ep);
        len += usbus_hdrs_fmt_iface_alts(usbus, iface);
    }
    return len;
}

size_t usbus_hdrs_fmt_endpoints(usbus_t *usbus, usbus_endpoint_t *ep)
{
    size_t len = 0;
    for (; ep; ep = ep->next) {
        usb_descriptor_endpoint_t usb_ep;
        memset(&usb_ep, 0 , sizeof(usb_descriptor_endpoint_t));
        usb_ep.length = sizeof(usb_descriptor_endpoint_t);
        usb_ep.type = USB_TYPE_DESCRIPTOR_ENDPOINT;
        usb_ep.address = ep->ep->num;
        if (ep->ep->dir == USB_EP_DIR_IN) {
            usb_ep.address |= 0x80;
        }
        usb_ep.attributes = _type_to_attribute(ep);
        usb_ep.max_packet_size = ep->maxpacketsize;
        usb_ep.interval = ep->interval;
        usbus_put_bytes(usbus, (uint8_t*)&usb_ep, sizeof(usb_descriptor_endpoint_t));
        usbus_hdrs_fmt_additional(usbus, ep->hdr_gen);
        len += usb_ep.length;
    }
    return len;
}

size_t usbus_hdrs_fmt_conf(usbus_t *usbus)
{
    size_t len = 0;
    usb_descriptor_configuration_t conf;
    memset(&conf, 0 ,sizeof(usb_descriptor_configuration_t));
    conf.length = sizeof(usb_descriptor_configuration_t);
    conf.type = USB_TYPE_DESCRIPTOR_CONFIGURATION;
    conf.total_length = sizeof(usb_descriptor_configuration_t);
    conf.val = 1;
    conf.attributes = USB_CONF_ATTR_RESERVED;
    if (USB_CONFIG_SELF_POWERED) {
        conf.attributes |= USB_CONF_ATTR_SELF_POWERED;
    }
    /* Todo: upper bound */
    conf.max_power = USB_CONFIG_MAX_POWER/2;
    conf.num_interfaces = _num_ifaces(usbus);
    len += sizeof(usb_descriptor_configuration_t);
    conf.total_length = usbus_hdrs_config_size(usbus);
    conf.idx = usbus->config.idx;
    usbus_put_bytes(usbus, (uint8_t*)&conf, sizeof(conf));
    len += usbus_hdrs_fmt_hdrs(usbus);
    len += usbus_hdrs_fmt_ifaces(usbus);
    return len;
}

