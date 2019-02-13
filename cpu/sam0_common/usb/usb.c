/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_sam0_common_usb
 * @{
 * @file
 * @brief   Low level USB interface functions for the sam0 class devices
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

/**
 * Be careful with enabling debug here. As with all timing critical systems it
 * is able to interfere with USB functionality and you might see different
 * errors than debug disabled
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

static sam0_common_usb_t *devs[USB_INST_NUM];

/* Forward declaration for the usb device driver */
const usbdev_driver_t driver;

static void _usbdev_ep_init(usbdev_ep_t *ep);
static int _usbdev_ep_ready(usbdev_ep_t *ep, size_t len);
static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                  usb_ep_dir_t dir, size_t buf_len);

static int _bank_set_size(usbdev_ep_t *ep);
static int _ep_unready(usbdev_ep_t *ep);

static inline unsigned _ep_num(unsigned num, usb_ep_dir_t dir)
{
    return 2 * num + (dir == USB_EP_DIR_OUT ? 0 : 1);
}

static inline UsbDeviceDescBank* _bank_from_ep(usbdev_ep_t *ep)
{
    sam0_common_usb_t *usbdev = (sam0_common_usb_t *)ep->dev;
    return &usbdev->banks[_ep_num(ep->num, ep->dir)];
}

static inline usbdev_ep_t *_get_ep(sam0_common_usb_t *dev,
                                   unsigned num, usb_ep_dir_t dir)
{
    return &dev->endpoints[_ep_num(num, dir)];
}

static inline UsbDeviceEndpoint *_ep_reg_from_ep(usbdev_ep_t *ep)
{
    sam0_common_usb_t *usbdev = (sam0_common_usb_t *)ep->dev;
    return &usbdev->device->DeviceEndpoint[ep->num];
}

static inline void _enable_irq(sam0_common_usb_t *dev)
{
    dev->device->INTENSET.reg = USB_DEVICE_INTENSET_EORST;
}

static inline void _disable_irq(sam0_common_usb_t *dev)
{
    dev->device->INTENCLR.reg = USB_DEVICE_INTENCLR_EORST;
}

static void _enable_ep_irq_out(sam0_common_usb_t *usbdev,
                               UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0 |
                             USB_DEVICE_EPINTENSET_STALL0 |
                             USB_DEVICE_EPINTENSET_TRFAIL0;
    if (ep_reg == &usbdev->device->DeviceEndpoint[0]) {
        ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;
    }
}

static void _enable_ep_irq_in(UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1 |
                             USB_DEVICE_EPINTENSET_STALL1 |
                             USB_DEVICE_EPINTENSET_TRFAIL1;
}

/* Enable the endpoint specific interrupts */
static void _enable_ep_irq(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);

    if (ep->dir == USB_EP_DIR_OUT) {
        _enable_ep_irq_out((sam0_common_usb_t*)ep->dev, ep_reg);
    }
    else {
        _enable_ep_irq_in(ep_reg);
    }
}

static void _disable_ep_irq_out(sam0_common_usb_t *usbdev,
                                UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_TRCPT0 |
                             USB_DEVICE_EPINTENCLR_STALL0 |
                             USB_DEVICE_EPINTENCLR_TRFAIL0;
    if (ep_reg == &usbdev->device->DeviceEndpoint[0]) {
        ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_RXSTP;
    }
}

static void _disable_ep_irq_in(UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_TRCPT1 |
                             USB_DEVICE_EPINTENCLR_STALL1 |
                             USB_DEVICE_EPINTENCLR_TRFAIL1;
}

static void _bank_set_address(usbdev_ep_t *ep)
{
    UsbDeviceDescBank *bank = _bank_from_ep(ep);

    bank->ADDR.reg = (uint32_t)ep->buf;
}

static int _bank_set_size(usbdev_ep_t *ep)
{
    UsbDeviceDescBank *bank = _bank_from_ep(ep);
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
            return -1;
    }
    bank->PCKSIZE.bit.SIZE = val;
    return val;
}

/**
 * Checks if one of the endpoints interrupts flags for the OUT direction are
 * set
 */
static bool _ep_out_flags_set(UsbDeviceEndpoint *ep_reg)
{
    return ep_reg->EPINTFLAG.reg  &
           ep_reg->EPINTENSET.reg &
           (USB_DEVICE_EPINTENSET_TRFAIL0 |
            USB_DEVICE_EPINTENSET_TRCPT0 |
            USB_DEVICE_EPINTENSET_RXSTP |
            USB_DEVICE_EPINTENSET_STALL0);
}

/**
 * Checks if one of the endpoints interrupts flags for the IN direction are
 * set
 */
static bool _ep_in_flags_set(UsbDeviceEndpoint *ep_reg)
{
    return ep_reg->EPINTFLAG.reg &
           ep_reg->EPINTENSET.reg &
           (USB_DEVICE_EPINTENSET_TRFAIL1 |
            USB_DEVICE_EPINTENSET_TRCPT1 |
            USB_DEVICE_EPINTENSET_STALL1);
}

static void _set_address(sam0_common_usb_t *dev, uint8_t addr)
{
    dev->device->DADD.bit.DADD = addr;
    /* Only enable the address if it is nonzero */
    dev->device->DADD.bit.ADDEN = addr ? 1 : 0;
}

static bool _syncbusy_enable(sam0_common_usb_t *dev)
{
    return dev->device->SYNCBUSY.bit.ENABLE;
}

static bool _syncbusy_swrst(sam0_common_usb_t *dev)
{
    return dev->device->SYNCBUSY.bit.SWRST;
}

static inline void _poweron(void)
{
#if defined(CPU_FAM_SAMD21)
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
    GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN |
                                   GCLK_CLKCTRL_GEN_GCLK0 |
                                   (GCLK_CLKCTRL_ID(USB_GCLK_ID)));
#elif defined(CPU_FAM_SAML21)
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_USB;
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN |
                                     GCLK_PCHCTRL_GEN_GCLK0;
#endif
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                  usb_ep_dir_t dir, size_t buf_len)
{
    sam0_common_usb_t *usbdev = (sam0_common_usb_t*)dev;
    /* The IP supports all types for all endpoints */
    usbdev_ep_t *res = NULL;

    /* Always return endpoint 0 for control types */
    if (type == USB_EP_TYPE_CONTROL) {
        res = _get_ep(usbdev, 0, dir);
        res->num = 0;
    }
    else {
        /* Find the first unassigned ep with proper dir */
        for (unsigned idx = 1; idx < SAM_USB_NUM_EP && !res; idx++) {
            usbdev_ep_t *ep = _get_ep(usbdev, idx, dir);
            if (ep->type == USB_EP_TYPE_NONE) {
                res = ep;
                res->num = idx;
            }
        }
    }
    if (res) {
        res->dev = dev;
        res->dir = dir;
        if (usbdev->used + buf_len < SAM_USB_BUF_SPACE) {
            res->buf = usbdev->buffer + usbdev->used;
            res->len = buf_len;
            if (_bank_set_size(res) < 0) {
                return NULL;
            }
            usbdev->used += buf_len;
            _bank_set_address(res);
            res->type = type;
            res->dev = dev;
        }
    }
    return res;
}

void sam_usbdev_setup(sam0_common_usb_t *usbdev)
{
    usbdev->usbdev.driver = &driver;
    usbdev->device = &USB->DEVICE;
    devs[0] = usbdev;
}

static void _usbdev_init(usbdev_t *dev)
{
    /* Only one usb device on this board */
    sam0_common_usb_t * usbdev = (sam0_common_usb_t *)dev;
    usbdev->used = 0;
    /* Set GPIO */
    gpio_init(GPIO_PIN(PA, 24), GPIO_IN);
    gpio_init(GPIO_PIN(PA, 25), GPIO_IN);
    gpio_init_mux(GPIO_PIN(PA, 24), GPIO_MUX_G);
    gpio_init_mux(GPIO_PIN(PA, 25), GPIO_MUX_G);
    _poweron();

    /* Reset peripheral */
    usbdev->device->CTRLA.reg |= USB_CTRLA_SWRST;
    while (_syncbusy_swrst(usbdev)) {}

    /* Enable USB device */
    usbdev->device->DESCADD.reg = (uint32_t)&usbdev->banks;
    usbdev->device->CTRLA.reg |= USB_CTRLA_ENABLE | USB_CTRLA_RUNSTDBY;
    while (_syncbusy_enable(usbdev)) {}

    /* Callibration values */
    usbdev->device->PADCAL.reg =
        USB_PADCAL_TRANSP((*(uint32_t *)USB_FUSES_TRANSP_ADDR >>
                           USB_FUSES_TRANSP_Pos)) |
        USB_PADCAL_TRANSN((*(uint32_t *)USB_FUSES_TRANSN_ADDR >>
                           USB_FUSES_TRANSN_Pos)) |
        USB_PADCAL_TRIM((*(uint32_t *)USB_FUSES_TRIM_ADDR >>
                         USB_FUSES_TRIM_Pos));

    usbdev->device->CTRLB.bit.SPDCONF = USB_DEVICE_CTRLB_SPDCONF_FS;
    _enable_irq(usbdev);
    /* Interrupt configuration */
    NVIC_EnableIRQ(USB_IRQn);
}

static void usb_attach(sam0_common_usb_t *dev)
{
    dev->device->CTRLB.reg &= ~USB_DEVICE_CTRLB_DETACH;
}

static void usb_detach(sam0_common_usb_t *dev)
{
    dev->device->CTRLB.reg |= USB_DEVICE_CTRLB_DETACH;
}

/*
 * sam0 USB interrupt service routine
 */
void isr_usb(void)
{
    /* TODO: make a bit more elegant for multi-periph support */
    sam0_common_usb_t *dev = devs[0];

    if (dev->device->EPINTSMRY.reg) {
        /* Endpoint specific interrupt */
        unsigned ep_num = bitarithm_lsb(dev->device->EPINTSMRY.reg);
        UsbDeviceEndpoint *ep_reg = &dev->device->DeviceEndpoint[ep_num];
        if (_ep_in_flags_set(ep_reg)) {
            usbdev_ep_t *ep = _get_ep(dev, ep_num, USB_EP_DIR_IN);
            _disable_ep_irq_in(ep_reg);
            dev->usbdev.epcb(ep, USBDEV_EVENT_ESR);
        }
        else if (_ep_out_flags_set(ep_reg)) {
            usbdev_ep_t *ep = _get_ep(dev, ep_num, USB_EP_DIR_OUT);
            _disable_ep_irq_out(dev, ep_reg);
            dev->usbdev.epcb(ep, USBDEV_EVENT_ESR);
        }
        else {
            DEBUG("Unhandled EP interrupt\n");
        }
    }
    else {
        /* Device interrupt */
        _disable_irq(dev);
        dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_ESR);
    }
    cortexm_isr_end();
}

static int _usbdev_get(usbdev_t *usbdev, usbopt_t opt, void *value, size_t max_len)
{
    (void)usbdev;
    (void)max_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_MAX_VERSION:
            assert(max_len == sizeof(usb_version_t));
            *(usb_version_t *)value = USB_VERSION_20;
            res = sizeof(usb_version_t);
            break;
        case USBOPT_MAX_SPEED:
            assert(max_len == sizeof(usb_speed_t));
            *(usb_speed_t *)value = USB_SPEED_FULL;
            res = sizeof(usb_speed_t);
            break;
        default:
            DEBUG("Unhandled get call: 0x%x\n", opt);
            break;
    }
    return res;
}

static int _usbdev_set(usbdev_t *dev, usbopt_t opt,
               const void *value, size_t value_len)
{
    sam0_common_usb_t *usbdev = (sam0_common_usb_t*)dev;
    (void)value_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_ADDRESS:
            assert(value_len == sizeof(uint8_t));
            uint8_t addr = (*((uint8_t *)value));
            _set_address(usbdev, addr);
            break;
        case USBOPT_ATTACH:
            assert(value_len == sizeof(usbopt_enable_t));
            if (*((usbopt_enable_t *)value)) {
                usb_attach(usbdev);
            }
            else {
                usb_detach(usbdev);
            }
            res = sizeof(usbopt_enable_t);
            break;
        default:
            DEBUG("Unhandled set call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _ep_disable(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);

    if (ep->dir == USB_EP_DIR_OUT) {
        ep_reg->EPCFG.bit.EPTYPE0 = 0;
    }
    else {
        ep_reg->EPCFG.bit.EPTYPE1 = 0;
    }
}

static void _ep_enable(usbdev_ep_t *ep)
{
    DEBUG("Enabling endpoint %d, dir %s\n", ep->num, ep->dir == USB_EP_DIR_OUT ?
                                                   "OUT" : "IN");
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);
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
            /* Must never happen */
            assert(false);
    }
    if (ep->dir == USB_EP_DIR_OUT) {
        ep_reg->EPCFG.bit.EPTYPE0 = type;
    }
    else {
        ep_reg->EPCFG.bit.EPTYPE1 = type;
    }
}

static void _usbdev_esr(usbdev_t *dev)
{
    sam0_common_usb_t *usbdev = (sam0_common_usb_t*)dev;

    if (usbdev->device->INTFLAG.reg) {
        if (usbdev->device->INTFLAG.bit.EORST) {
            /* Clear flag */
            usbdev->device->INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESET);
        }
        /* Re-enable the USB IRQ */
        _enable_irq(usbdev);
    }
}

static inline void _enable_ep_stall_out(UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ0;
}

static inline void _enable_ep_stall_in(UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ1;
}

static inline void _disable_ep_stall_out(UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_STALLRQ0;
}

static inline void _disable_ep_stall_in(UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_STALLRQ1;
}

static void _ep_set_stall(usbdev_ep_t *ep, usbopt_enable_t enable)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);

    if (ep->dir == USB_EP_DIR_IN) {
        enable ? _enable_ep_stall_in(ep_reg)
               : _disable_ep_stall_in(ep_reg);
    }
    else {
        enable ? _enable_ep_stall_out(ep_reg)
               : _disable_ep_stall_out(ep_reg);
    }
}

usbopt_enable_t _ep_get_stall(usbdev_ep_t *ep)
{
    usbopt_enable_t res;
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);

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

static void _usbdev_ep_init(usbdev_ep_t *ep)
{
    _enable_ep_irq(ep);
}

static size_t _ep_get_available(usbdev_ep_t *ep)
{
    return _bank_from_ep(ep)->PCKSIZE.bit.BYTE_COUNT;
}

static int _usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
                  void *value, size_t max_len)
{
    (void)max_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_EP_STALL:
            assert(max_len == sizeof(usbopt_enable_t));
            *(usbopt_enable_t *)value = _ep_get_stall(ep);
            res = sizeof(usbopt_enable_t);
            break;
        case USBOPT_EP_AVAILABLE:
            assert(max_len == sizeof(size_t));
            *(size_t *)value = _ep_get_available(ep);
            res = sizeof(size_t);
            break;
        default:
            DEBUG("Unhandled get call: 0x%x\n", opt);
            break;
    }
    return res;
}

static int _usbdev_ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                  const void *value, size_t value_len)
{
    (void)value_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_EP_ENABLE:
            assert(value_len == sizeof(usbopt_enable_t));
            if (*((usbopt_enable_t *)value)) {
                _ep_enable(ep);
                _usbdev_ep_init(ep);
            }
            else {
                _ep_disable(ep);
            }
            res = sizeof(usbopt_enable_t);
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
                _usbdev_ep_ready(ep, 0);
            }
            res = sizeof(usbopt_enable_t);
            break;
        default:
            DEBUG("Unhandled set call: 0x%x\n", opt);
            break;
    }
    return res;
}

static int _ep_unready(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);

    if (ep->dir == USB_EP_DIR_IN) {
        ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_BK1RDY;
    }
    else {
        ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_BK0RDY;
    }
    return 0;
}

static int _usbdev_ep_ready(usbdev_ep_t *ep, size_t len)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);

    if (ep->dir == USB_EP_DIR_IN) {
        _disable_ep_stall_in(ep_reg);
        _bank_from_ep(ep)->PCKSIZE.bit.BYTE_COUNT = len;
        ep_reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_BK1RDY;
    }
    else {
        _disable_ep_stall_out(ep_reg);
        ep_reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_BK0RDY;
    }
    return 0;
}

/**
 * Endpoint event handler
 *
 * Calls the endpoint callback to report the event to the USB stack
 */
static void _usbdev_ep_esr(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);
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
            DEBUG("Unhandled event: EP IN %u: 0x%x\n", ep->num,
                                                       ep_reg->EPINTFLAG.reg);
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
            DEBUG("Unhandled event: EP OUT %u: 0x%x\n", ep->num,
                                                        ep_reg->EPINTFLAG.reg);
        }
    }
    if (event >= 0) {
        ep->dev->epcb(ep, event);
    }
    ep->dir == USB_EP_DIR_OUT ? _enable_ep_irq_out((sam0_common_usb_t*)ep->dev,
                                                   ep_reg)
                              : _enable_ep_irq_in(ep_reg);
}

const usbdev_driver_t driver = {
    .init = _usbdev_init,
    .new_ep = _usbdev_new_ep,
    .get = _usbdev_get,
    .set = _usbdev_set,
    .esr = _usbdev_esr,
    .ep_init = _usbdev_ep_init,
    .ep_get = _usbdev_ep_get,
    .ep_set = _usbdev_ep_set,
    .ep_esr = _usbdev_ep_esr,
    .ready = _usbdev_ep_ready,
};
