/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "net/netdev.h"

int netdev_init_pass(netdev_t *dev)
{
    return dev->lower->driver->init(dev->lower);
}

void netdev_isr_pass(netdev_t *dev)
{
    dev->lower->driver->isr(dev->lower);
}

int netdev_send_pass(netdev_t *dev, const iolist_t *iolist)
{
    return dev->lower->driver->send(dev->lower, iolist);
}

int netdev_recv_pass(netdev_t *dev, void *buf, size_t len, void *info)
{
    return dev->lower->driver->recv(dev->lower, buf, len, info);
}

int netdev_get_pass(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    return dev->lower->driver->get(dev->lower, opt, value, max_len);
}

int netdev_set_pass(netdev_t *dev, netopt_t opt, const void *value, size_t value_len)
{
    return dev->lower->driver->set(dev->lower, opt, value, value_len);
}

void netdev_event_cb_pass(netdev_t *dev, netdev_event_t event)
{
    netdev_t *cur_dev = dev->context;

    cur_dev->event_callback(cur_dev, event);
}
