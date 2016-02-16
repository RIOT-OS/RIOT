/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "net/netdev2_test.h"

static int _send(netdev2_t *netdev, const struct iovec *vector, int count);
static int _recv(netdev2_t *netdev, char *buf, int len, void *info);
static int _init(netdev2_t *dev);
static void _isr(netdev2_t *dev);
static int _get(netdev2_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(netdev2_t *dev, netopt_t opt, void *value, size_t value_len);

static const netdev2_driver_t _driver = {
    .send   = _send,
    .recv   = _recv,
    .init   = _init,
    .isr    = _isr,
    .get    = _get,
    .set    = _set,
};

void netdev2_test_setup(netdev2_test_t *dev, void *state)
{
    netdev2_t *netdev = (netdev2_t *)dev;

    netdev->driver = &_driver;
    dev->state = state;
    mutex_init(&dev->mutex);
    netdev2_test_reset(dev);
}

void netdev2_test_reset(netdev2_test_t *dev)
{
    mutex_lock(&dev->mutex);
    dev->send_cb = NULL;
    dev->recv_cb = NULL;
    dev->init_cb = NULL;
    dev->isr_cb = NULL;
    memset(dev->get_cbs, 0, sizeof(dev->get_cbs));
    memset(dev->set_cbs, 0, sizeof(dev->set_cbs));
    mutex_unlock(&dev->mutex);
}

static int _send(netdev2_t *netdev, const struct iovec *vector, int count)
{
    netdev2_test_t *dev = (netdev2_test_t *)netdev;
    int res = count;    /* assume everything would be fine */

    mutex_lock(&dev->mutex);
    if (dev->send_cb != NULL) {
        res = dev->send_cb(netdev, vector, count);
    }
    mutex_unlock(&dev->mutex);
    return res;
}

static int _recv(netdev2_t *netdev, char *buf, int len, void *info)
{
    netdev2_test_t *dev = (netdev2_test_t *)netdev;
    int res = (buf == NULL) ? 0 : len;  /* assume everything would be fine */

    mutex_lock(&dev->mutex);
    if (dev->recv_cb != NULL) {
        /* could fire context change and call _recv so we need to unlock */
        mutex_unlock(&dev->mutex);
        res = dev->recv_cb(netdev, buf, len, info);
    }
    else {
        mutex_unlock(&dev->mutex);
    }
    return res;
}

static int _init(netdev2_t *netdev)
{
    netdev2_test_t *dev = (netdev2_test_t *)netdev;
    int res = 0;        /* assume everything would be fine */

    mutex_lock(&dev->mutex);
    if (dev->init_cb != NULL) {
        res = dev->init_cb(netdev);
    }
    mutex_unlock(&dev->mutex);
    return res;
}

static void _isr(netdev2_t *netdev)
{
    netdev2_test_t *dev = (netdev2_test_t *)netdev;

    mutex_lock(&dev->mutex);
    if (dev->isr_cb != NULL) {
        mutex_unlock(&dev->mutex);
        dev->isr_cb(netdev);
    }
    else {
        mutex_unlock(&dev->mutex);
    }
}

static int _get(netdev2_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    netdev2_test_t *dev = (netdev2_test_t *)netdev;
    int res = -ENOTSUP;     /* option assumed to be not supported */

    mutex_lock(&dev->mutex);
    if (dev->get_cbs[opt] != NULL) {
        res = dev->get_cbs[opt](netdev, value, max_len);
    }
    mutex_unlock(&dev->mutex);
    return res;
}

static int _set(netdev2_t *netdev, netopt_t opt, void *value, size_t value_len)
{
    netdev2_test_t *dev = (netdev2_test_t *)netdev;
    int res = -ENOTSUP;     /* option assumed to be not supported */

    mutex_lock(&dev->mutex);
    if (dev->set_cbs[opt] != NULL) {
        res = dev->set_cbs[opt](netdev, value, value_len);
    }
    mutex_unlock(&dev->mutex);
    return res;
}


/** @} */
