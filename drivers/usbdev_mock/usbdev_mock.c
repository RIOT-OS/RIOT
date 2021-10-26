/*
 * Copyright (C) 2019 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "periph/usbdev.h"
#include "test_utils/expect.h"
#include "usbdev_mock.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static usbdev_mock_t _usbdev_mock;
static uint8_t _in_buf[256];    /* "host" in */
static uint8_t _out_buf[64];    /* "host" out */

static const usbdev_driver_t testdriver;

static usbdev_mock_t *_ep2dev(usbdev_ep_t *ep)
{
    return (usbdev_mock_t *)ep->dev;
}

void usbdev_init_lowlevel(void)
{}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    (void)num;
    return &_usbdev_mock.usbdev;
}

void usbdev_mock_setup(usbdev_mock_esr_cb_t esr_cb,
                       usbdev_mock_ep_esr_cb_t ep_esr_cb,
                       usbdev_mock_ready_cb_t ready_cb)
{
    memset(&_usbdev_mock, 0, sizeof(_usbdev_mock));
    _usbdev_mock.usbdev.driver = &testdriver;

    _usbdev_mock.esr_cb = esr_cb;
    _usbdev_mock.ep_esr_cb = ep_esr_cb;
    _usbdev_mock.ready_cb = ready_cb;
}

static void _init(usbdev_t *usbdev)
{
    usbdev_mock_t *dev = (usbdev_mock_t *)usbdev;

    /* Throw first event to start the test sequence */
    dev->usbdev.cb(usbdev, USBDEV_EVENT_ESR);
}

usbdev_ep_t *_new_ep(usbdev_t *dev, usb_ep_type_t type, usb_ep_dir_t dir,
                     size_t buf_len)
{
    usbdev_mock_t *testdev = (usbdev_mock_t *)dev;
    usbdev_mock_ep_t *res = NULL;

    if (type == USB_EP_TYPE_CONTROL) {
        if (dir == USB_EP_DIR_OUT) {

            res = &testdev->out[0];
            res->ep.num = 0;
            res->buf = _out_buf;
        }
        else {
            res = &testdev->in[0];
            res->ep.num = 0;
            res->buf = _in_buf;
        }
    }
    if (res) {
        res->state = EP_STATE_READY;
        res->available = 0;
        res->ep.len = buf_len;
        res->ep.dir = dir;
        res->ep.type = type;
        res->ep.dev = dev;
        return &res->ep;
    }
    return NULL;
}

int _get(usbdev_t *usbdev, usbopt_t opt,
         void *value, size_t max_len)
{
    (void)usbdev;
    (void)opt;
    (void)value;
    (void)max_len;
    DEBUG("[mock]: Unhandled get call: 0x%x\n", opt);
    return -ENOTSUP;
}

int _set(usbdev_t *usbdev, usbopt_t opt,
         const void *value, size_t value_len)
{
    usbdev_mock_t *testdev = (usbdev_mock_t *)usbdev;

    (void)value_len;
    int res = -ENOTSUP;

    switch (opt) {
    case USBOPT_ADDRESS:
        testdev->config_addr = *(uint8_t *)value;
        res = sizeof(uint8_t);
        break;
    case USBOPT_ATTACH:
        expect(value_len == sizeof(usbopt_enable_t));
        res = sizeof(usbopt_enable_t);
        break;
    default:
        DEBUG("[mock]: Unhandled set call: 0x%x\n", opt);
        break;
    }
    return res;
}

static void _esr(usbdev_t *dev)
{
    usbdev_mock_t *usbdev_mock = (usbdev_mock_t *)dev;

    usbdev_mock->esr_cb(usbdev_mock);
}

void _ep_init(usbdev_ep_t *ep)
{
    (void)ep;
}

int _ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
            void *value, size_t max_len)
{
    usbdev_mock_ep_t *testep = (usbdev_mock_ep_t *)ep;

    (void)max_len;
    switch (opt) {
    case USBOPT_EP_AVAILABLE:
        *((size_t *)value) = testep->available;
        return sizeof(size_t);
    default:
        DEBUG("[mock]: Unhandled endpoint get call: 0x%x\n", opt);
        break;
    }
    return -ENOTSUP;
}

int _ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
            const void *value, size_t value_len)
{
    (void)ep;
    (void)value;
    (void)value_len;
    int res = -ENOTSUP;

    switch (opt) {
    case USBOPT_EP_ENABLE:
        res = sizeof(usbopt_enable_t);
        break;
    case USBOPT_EP_STALL:
        res = sizeof(usbopt_enable_t);
        break;
    default:
        DEBUG("[mock]: Unhandled ep set call: 0x%x\n", opt);
        break;
    }
    return res;
}

void _ep_esr(usbdev_ep_t *ep)
{
    usbdev_mock_t *dev = _ep2dev(ep);

    DEBUG("[mock]: ESR EP %u, dir %s\n",
          ep->num, ep->dir == USB_EP_DIR_OUT ? "out" : "in");
    usbdev_mock_ep_t *mock_ep = (usbdev_mock_ep_t *)ep;

    if (mock_ep->ep.dir == USB_EP_DIR_OUT) {
        memcpy(mock_ep->target_buf, mock_ep->buf, mock_ep->available);
    }
    if (mock_ep->state == EP_STATE_DATA_AVAILABLE) {
        dev->ep_esr_cb(dev, mock_ep);
        mock_ep->state = EP_STATE_READY;
    }
}

int _xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    DEBUG("[mock]: Readying EP %u, dir %s, len %u\n",
          ep->num, ep->dir == USB_EP_DIR_OUT ? "out" : "in", (unsigned)len);
    if (ep->num == 0) {
        usbdev_mock_t *usbdev_mock = _ep2dev(ep);
        usbdev_mock_ep_t *mock_ep = (usbdev_mock_ep_t *)ep;
        mock_ep->target_buf = buf;
        if (ep->dir == USB_EP_DIR_IN) {
            memcpy(mock_ep->buf + mock_ep->available, mock_ep->target_buf, len);
            mock_ep->available = len;
        }
        mock_ep->state = EP_STATE_DATA_AVAILABLE;

        usbdev_mock->ready_cb(usbdev_mock, (usbdev_mock_ep_t *)ep, len);
    }
    return 0;
}

static const usbdev_driver_t testdriver = {
    .init = _init,
    .new_ep = _new_ep,
    .get = _get,
    .set = _set,
    .esr = _esr,
    .ep_init = _ep_init,
    .ep_get = _ep_get,
    .ep_set = _ep_set,
    .ep_esr = _ep_esr,
    .xmit = _xmit,
};
