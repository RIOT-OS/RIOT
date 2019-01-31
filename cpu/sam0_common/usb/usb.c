/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sam0_common
 * @{
 * @file
 * @brief   USB interface functions for the sam0 class devices
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "cpu.h"
#include "periph/gpio.h"
#include "usb/usbdev.h"
#include "sam_usb.h"

#include "bitarithm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static sam0_common_usb_t *_usbdev;

/* Descriptor bank */
static UsbDeviceDescBank banks[2 * SAM_USB_NUM_EP];
static usbdev_ep_t endpoints[2 * SAM_USB_NUM_EP];

const usbdev_ep_driver_t driver_ep;
static void usbdev_ep_init(usbdev_ep_t *ep);
static int usbdev_ep_ready(usbdev_ep_t *ep, size_t len);


static usbdev_ep_t *usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                  usb_ep_dir_t dir, size_t buf_len);

static void _ep_address(usbdev_ep_t *ep);
static void _ep_size(usbdev_ep_t *ep);
static int _ep_unready(usbdev_ep_t *ep);

static inline unsigned _get_ep_num(unsigned num, usb_ep_dir_t dir)
{
    return 2 * num + (dir == USB_EP_DIR_OUT ? 0 : 1);
}

static inline unsigned _get_ep_num2(usbdev_ep_t *ep)
{
    return 2 * ep->num + (ep->dir == USB_EP_DIR_OUT ? 0 : 1);
}

static inline usbdev_ep_t *_get_ep(unsigned num, usb_ep_dir_t dir)
{
    return &endpoints[_get_ep_num(num, dir)];
}

static inline void _enable_irq(void)
{
    USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
}

static inline void _disable_irq(void)
{
    USB->DEVICE.INTENCLR.reg = USB_DEVICE_INTENCLR_EORST;
}

static void _enable_ep_irq(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];

    if (ep->dir == USB_EP_DIR_OUT) {
        ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0
                                 | USB_DEVICE_EPINTENSET_TRFAIL0
                                 | USB_DEVICE_EPINTENSET_STALL0;
        if (ep->num == 0) {
            ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;
        }
    }
    else {
        ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1
                                 | USB_DEVICE_EPINTENSET_TRFAIL1
                                 | USB_DEVICE_EPINTENSET_STALL1;
    }
}

static void _disable_ep_irq(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];

    if (ep->dir == USB_EP_DIR_OUT) {
        ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_TRCPT0 |
                                 USB_DEVICE_EPINTENCLR_STALL0 |
                                 USB_DEVICE_EPINTENCLR_TRFAIL0;
        if (ep->num == 0) {
            ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_RXSTP;
        }
    }
    else {
        ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_TRCPT1 |
                                 USB_DEVICE_EPINTENCLR_STALL1 |
                                 USB_DEVICE_EPINTENCLR_TRFAIL1;
    }
}

static bool _ep_out_flags_set(UsbDeviceEndpoint *ep_reg)
{
    return ep_reg->EPINTFLAG.reg  &
           ep_reg->EPINTENSET.reg &
           (USB_DEVICE_EPINTENSET_TRFAIL0 |
            USB_DEVICE_EPINTENSET_TRCPT0 |
            USB_DEVICE_EPINTENSET_RXSTP |
            USB_DEVICE_EPINTENSET_STALL0);
}

static bool _ep_in_flags_set(UsbDeviceEndpoint *ep_reg)
{
    return ep_reg->EPINTFLAG.reg &
           ep_reg->EPINTENSET.reg &
           (USB_DEVICE_EPINTENSET_TRFAIL1 |
            USB_DEVICE_EPINTENSET_TRCPT1 |
            USB_DEVICE_EPINTENSET_STALL1);
}

static bool usb_enable_syncing(void)
{
    if (USB->DEVICE.SYNCBUSY.reg & USB_SYNCBUSY_ENABLE) {
        return true;
    }
    return false;
}

static bool usb_swrst_syncing(void)
{
    if (USB->DEVICE.SYNCBUSY.reg & USB_SYNCBUSY_SWRST) {
        return true;
    }
    return false;
}

static usbdev_ep_t *usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                  usb_ep_dir_t dir, size_t buf_len)
{
    /* The IP supports all types for all endpoints */
    (void)dev;
    usbdev_ep_t *res = NULL;
    if (type == USB_EP_TYPE_CONTROL) {
        res = _get_ep(0, dir);
        res->num = 0;
    }
    else {
        for (unsigned idx = 1; idx < 8; idx++) {
            usbdev_ep_t *ep = _get_ep(idx, dir);
            if (ep->type == USB_EP_TYPE_NONE) {
                res = ep;
                res->num = idx;
                break;
            }
        }
    }
    if (res) {
        res->dir = dir;
        if (_usbdev->used + buf_len < SAM_USB_BUF_SPACE) {
            res->buf = _usbdev->buffer + _usbdev->used;
            _usbdev->used += buf_len;
            res->len = buf_len;
            _ep_address(res);
            _ep_size(res);
        }
        else {
            return NULL;
        }
        res->type = type;
        res->cb = NULL;
        res->driver = &driver_ep;
    }
    return res;
}

static inline void poweron(void)
{
#if defined(CPU_FAM_SAMD21)
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
    GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN |
                                   GCLK_CLKCTRL_GEN_GCLK0 |
                                   (GCLK_CLKCTRL_ID(USB_GCLK_ID)));
#elif defined(CPU_FAM_SAML21)
    MCLK->AHBMASK.reg |= (MCLK_AHBMASK_USB);
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN |
                                     GCLK_PCHCTRL_GEN_GCLK0;
#endif
}

static inline void poweroff(void)
{
#if defined(CPU_FAM_SAMD21)
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN |
                                   GCLK_CLKCTRL_GEN_GCLK0 |
                                   (GCLK_CLKCTRL_ID(USB_GCLK_ID)));
#elif defined(CPU_FAM_SAML21)
    MCLK->AHBMASK.reg |= (MCLK_AHBMASK_USB);
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN |
                                     GCLK_PCHCTRL_GEN_GCLK0;
#endif
}

void usbdev_init(usbdev_t *dev)
{
    /* Only one usb device on this board */
    _usbdev = (sam0_common_usb_t *)dev;
    _usbdev->used = 0;
    /* Set GPIO */
    gpio_init(GPIO_PIN(PA, 24), GPIO_IN);
    gpio_init(GPIO_PIN(PA, 25), GPIO_IN);
    gpio_init_mux(GPIO_PIN(PA, 24), GPIO_MUX_G);
    gpio_init_mux(GPIO_PIN(PA, 25), GPIO_MUX_G);
    poweron();
    /* Reset peripheral */
    USB->DEVICE.CTRLA.reg |= USB_CTRLA_SWRST;
    while (usb_swrst_syncing()) {}
    while (USB->DEVICE.CTRLA.bit.SWRST) {}

    /* Enable USB device */
    USB->DEVICE.DESCADD.reg = (uint32_t)banks;
    USB->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE | USB_CTRLA_RUNSTDBY;
    while (usb_enable_syncing()) {}
    /* Callibration values */
    USB->DEVICE.PADCAL.reg =
        USB_PADCAL_TRANSP((*(uint32_t *)USB_FUSES_TRANSP_ADDR >>
                           USB_FUSES_TRANSP_Pos)) |
        USB_PADCAL_TRANSN((*(uint32_t *)USB_FUSES_TRANSN_ADDR >>
                           USB_FUSES_TRANSN_Pos)) |
        USB_PADCAL_TRIM((*(uint32_t *)USB_FUSES_TRIM_ADDR >>
                         USB_FUSES_TRIM_Pos));

    USB->DEVICE.CTRLB.bit.SPDCONF = 0x0;
    _enable_irq();
    /* Interrupt configuration */
    NVIC_EnableIRQ(USB_IRQn);
}

void usb_attach(void)
{
    /* Datasheet is not clear whether device starts detached */
    USB->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_DETACH;
}

void usb_detach(void)
{
    /* Datasheet is not clear whether device starts detached */
    USB->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_DETACH;
}

void isr_usb(void)
{
    if (USB->DEVICE.EPINTSMRY.reg) {
        unsigned ep_num = bitarithm_lsb(USB->DEVICE.EPINTSMRY.reg);
        UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep_num];
        if (_ep_in_flags_set(ep_reg)) {
            usbdev_ep_t *ep = &endpoints[_get_ep_num(ep_num, USB_EP_DIR_IN)];
            _disable_ep_irq(ep);
            ep->cb(ep, USBDEV_EVENT_ESR);
        }
        else if (_ep_out_flags_set(ep_reg)) {
            usbdev_ep_t *ep = &endpoints[_get_ep_num(ep_num, USB_EP_DIR_OUT)];
            _disable_ep_irq(ep);
            ep->cb(ep, USBDEV_EVENT_ESR);
        }
        else {
            DEBUG("Unhandled ISR\n");
        }
    }
    else {
        _disable_irq();
        _usbdev->usbdev.cb(&_usbdev->usbdev, USBDEV_EVENT_ESR);
    }
    cortexm_isr_end();
}

int usbdev_get(usbdev_t *usbdev, usbopt_t opt, void *value, size_t max_len)
{
    sam0_common_usb_t *dev = (sam0_common_usb_t *) usbdev;

    (void)dev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case USBOPT_MAX_VERSION:
            assert(max_len == sizeof(usb_speed_t));
            *(usb_version_t *)value = USB_VERSION_20;
            res = sizeof(usb_speed_t);
            break;
        case USBOPT_MAX_SPEED:
            assert(max_len == sizeof(usb_speed_t));
            *(usb_speed_t *)value = USB_SPEED_FULL;
            res = sizeof(usb_speed_t);
            break;
        default:
            break;
    }
    return res;
}

int usbdev_set(usbdev_t *usbdev, usbopt_t opt,
               const void *value, size_t value_len)
{
    sam0_common_usb_t *dev = (sam0_common_usb_t *) usbdev;

    (void)dev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case USBOPT_ADDRESS:
            assert(value_len == sizeof(uint8_t));
            uint8_t addr = (*((uint8_t *)value));
            USB->DEVICE.DADD.bit.DADD = addr;
            if (addr) {
                USB->DEVICE.DADD.bit.ADDEN = 1;
            }
            else {
                USB->DEVICE.DADD.bit.ADDEN = 0;
            }
            break;
        case USBOPT_ATTACH:
            assert(value_len == sizeof(usbopt_enable_t));
            if (*((usbopt_enable_t *)value)) {
                usb_attach();
            }
            else {
                usb_detach();
            }
            res = sizeof(usbopt_enable_t);
            break;
        default:
            break;
    }
    return res;
}

static void _ep_disable(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];

    if (ep->dir == USB_EP_DIR_OUT) {
        ep_reg->EPCFG.bit.EPTYPE0 = 0;
    }
    else {
        ep_reg->EPCFG.bit.EPTYPE1 = 0;
    }
}

static void _ep_enable(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];
    uint8_t type = 0;

    switch (ep->type) {
        case USB_EP_TYPE_CONTROL:
            type = 0x01;
            break;
        case USB_EP_TYPE_ISOCHRONOUS:
            type = 0x02;
            break;
        case USB_EP_TYPE_BULK:
            type = 0x03;
            break;
        case USB_EP_TYPE_INTERRUPT:
            type = 0x04;
            break;
        case USB_EP_TYPE_NONE:
            /* Should never happen */
            assert(false);
    }
    if (ep->dir == USB_EP_DIR_OUT) {
        ep_reg->EPCFG.bit.EPTYPE0 = type;
    }
    else {
        ep_reg->EPCFG.bit.EPTYPE1 = type;
    }
}

void usbdev_esr(usbdev_t *dev)
{
    (void)dev;
    if (USB->DEVICE.INTFLAG.reg) {
        if (USB->DEVICE.INTFLAG.bit.EORST) {
            /* Clear flag */
            USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
            usbdev_ep_init(&endpoints[0]);
            _ep_enable(&endpoints[0]);
            usbdev_ep_init(&endpoints[1]);
            _ep_enable(&endpoints[1]);
            _usbdev->usbdev.cb(&_usbdev->usbdev, USBDEV_EVENT_RESET);
        }
        /* Re-enable the USB IRQ */
        _enable_irq();
    }
}


static void _ep_address(usbdev_ep_t *ep)
{
    UsbDeviceDescBank *bank = &banks[_get_ep_num(ep->num, ep->dir)];

    bank->ADDR.reg = (uint32_t)ep->buf;
}

void _ep_set_stall(usbdev_ep_t *ep, usbopt_enable_t enable)
{
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];

    if (ep->dir == USB_EP_DIR_IN) {
        if (enable) {
            ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ1;
        }
        else {
            ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSSET_STALLRQ1;
        }
    }
    else {
        if (enable) {
            ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ0;
        }
        else {
            ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSSET_STALLRQ0;
        }
    }
}

usbopt_enable_t _ep_get_stall(usbdev_ep_t *ep)
{
    usbopt_enable_t res;
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];

    if (ep->dir == USB_EP_DIR_IN) {
        res = ep_reg->EPSTATUSSET.bit.STALLRQ1 
            ? USBOPT_ENABLE 
            : USBOPT_DISABLE;
    }
    else {
        res = ep_reg->EPSTATUSSET.bit.STALLRQ0
            ? USBOPT_ENABLE
            : USBOPT_DISABLE;
    }
    return res;

}

static void _ep_size(usbdev_ep_t *ep)
{
    UsbDeviceDescBank *bank = &banks[_get_ep_num(ep->num, ep->dir)];
    unsigned val = 0x00;

    switch (ep->len) {
        case 8:
            val = 0x0;
            break;
        case 16:
            val = 0x1;
            break;
        case 32:
            val = 0x2;
            break;
        case 64:
            val = 0x3;
            break;
        case 128:
            val = 0x4;
            break;
        case 256:
            val = 0x5;
            break;
        case 512:
            val = 0x6;
            break;
        case 1023:
            val = 0x7;
            break;
        default:
            return;
    }
    bank->PCKSIZE.bit.SIZE = val;
}

static void usbdev_ep_init(usbdev_ep_t *ep)
{
    _enable_ep_irq(ep);
}

size_t _ep_get_available(usbdev_ep_t *ep)
{
    UsbDeviceDescBank *bank = &banks[_get_ep_num(ep->num, ep->dir)];

    return (size_t)bank->PCKSIZE.bit.BYTE_COUNT;

}

int usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
                  void *value, size_t max_len)
{
    (void)ep;
    (void)value;
    (void)max_len;
    int res = -ENOTSUP;
    assert(ep != NULL);
    switch (opt) {
        case USBOPT_EP_ENABLE:
        case USBOPT_EP_BUF_ADDR:
            break;
        case USBOPT_EP_STALL:
            *(usbopt_enable_t *)value = _ep_get_stall(ep);
            res = sizeof(usbopt_enable_t);
            break;
        case USBOPT_EP_AVAILABLE:
            *(size_t *)value = _ep_get_available(ep);
            res = sizeof(size_t);
            break;
        default:
            break;
    }
    return res;
}

int usbdev_ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                  const void *value, size_t value_len)
{
    (void)ep;
    (void)value;
    (void)value_len;
    int res = -ENOTSUP;
    assert(ep != NULL);
    switch (opt) {
        case USBOPT_EP_ENABLE:
            assert(value_len == sizeof(usbopt_enable_t));
            if (*((usbopt_enable_t *)value)) {
                usbdev_ep_init(ep);
                _ep_enable(ep);
            }
            else {
                _ep_disable(ep);
            }
            break;
        case USBOPT_EP_BUF_ADDR:
            assert(value_len == sizeof(uint8_t *));
            res = sizeof(char *);
            break;
        case USBOPT_EP_BUF_SIZE:
            assert(value_len == sizeof(size_t));
            break;
        case USBOPT_EP_STALL:
            assert(value_len == sizeof(usbopt_enable_t));
            _ep_set_stall(ep, *(usbopt_enable_t *)value);
            res = sizeof(usbopt_enable_t);
            break;
        case USBOPT_EP_READY:
            assert(value_len == sizeof(usbopt_enable_t));
            if (*((usbopt_enable_t *)value)) {
                _ep_unready(ep);
            }
            else {
                usbdev_ep_ready(ep, 0);
            }
            res = sizeof(usbopt_enable_t);
            break;
        default:
            break;
    }
    return res;
}

static int _ep_unready(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];

    if (ep->dir == USB_EP_DIR_IN) {
        ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSSET_BK1RDY;
    }
    else {
        ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSCLR_BK0RDY;
    }
    return 0;
}

static int usbdev_ep_ready(usbdev_ep_t *ep, size_t len)
{
    _ep_set_stall(ep, USBOPT_DISABLE);
    UsbDeviceDescBank *bank = &banks[_get_ep_num(ep->num, ep->dir)];
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];
    if (ep->dir == USB_EP_DIR_IN) {
        bank->PCKSIZE.bit.BYTE_COUNT = len;
        ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_BK1RDY;
    }
    else {
        ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_BK0RDY;
    }
    return 0;
}

void usbdev_ep_esr(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = &USB->DEVICE.DeviceEndpoint[ep->num];
    signed event = -1;

    if (ep->dir == USB_EP_DIR_OUT) {
        if (ep_reg->EPINTFLAG.bit.TRCPT0) {
            ep_reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
            event = USBDEV_EVENT_TR_COMPLETE;
        }
        else if (ep_reg->EPINTFLAG.bit.RXSTP) {
            ep_reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
            event = USBDEV_EVENT_TR_COMPLETE;
        }
        else if (ep_reg->EPINTFLAG.bit.TRFAIL0) {
            ep_reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL0;
            event = USBDEV_EVENT_TR_FAIL;
        }
        else if (ep_reg->EPINTFLAG.bit.STALL0) {
            ep_reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
            event = USBDEV_EVENT_TR_STALL;
        }
        else {
            DEBUG("Unhandled in %u: %x\n", ep->num, ep_reg->EPINTFLAG.reg);
        }
    }
    else {
        if (ep_reg->EPINTFLAG.bit.TRCPT1) {
            ep_reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
            event = USBDEV_EVENT_TR_COMPLETE;
        }
        else if (ep_reg->EPINTFLAG.bit.TRFAIL1) {
            ep_reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL1;
            event = USBDEV_EVENT_TR_FAIL;
        }
        else if (ep_reg->EPINTFLAG.bit.STALL1) {
            ep_reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
            event = USBDEV_EVENT_TR_STALL;
        }
        else {
            DEBUG("Unhandled out %u: %x\n", ep->num, ep_reg->EPINTFLAG.reg);
        }
    }
    if (event >= 0) {
        ep->cb(ep, event);
    }
    _enable_ep_irq(ep);
}

const usbdev_driver_t driver = {
    .init = usbdev_init,
    .new_ep = usbdev_new_ep,
    .get = usbdev_get,
    .set = usbdev_set,
    .esr = usbdev_esr,
};

const usbdev_ep_driver_t driver_ep = {
    .init = usbdev_ep_init,
    .get = usbdev_ep_get,
    .set = usbdev_ep_set,
    .esr = usbdev_ep_esr,
    .ready = usbdev_ep_ready,
};

