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

#define USB_H_USER_IS_RIOT_INTERNAL

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "architecture.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "periph/gpio.h"
#include "periph/usbdev.h"
#include "pm_layered.h"
#include "sam_usb.h"

#include "bitarithm.h"

/**
 * Be careful with enabling debug here. As with all timing critical systems it
 * is able to interfere with USB functionality and you might see different
 * errors than debug disabled
 */
#define ENABLE_DEBUG 0
#include "debug.h"

/* List of instantiated USB peripherals */
static sam0_common_usb_t _usbdevs[USB_INST_NUM];

/* Forward declaration for the usb device driver */
const usbdev_driver_t driver;

static void _usbdev_ep_init(usbdev_ep_t *ep);
static int _usbdev_ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len);
static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t len);

static int _bank_set_size(usbdev_ep_t *ep);

static inline unsigned _ep_num(unsigned num, usb_ep_dir_t dir)
{
    return 2 * num + (dir == USB_EP_DIR_OUT ? 0 : 1);
}

static inline UsbDeviceDescBank *_bank_from_ep(usbdev_ep_t *ep)
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

    return &usbdev->config->device->DeviceEndpoint[ep->num];
}

static inline void _enable_irq(sam0_common_usb_t *dev)
{
    if (dev->suspended) {
        dev->config->device->INTENSET.reg = USB_DEVICE_INTENSET_WAKEUP |
                                            USB_DEVICE_INTENCLR_EORST;
    }
    else {
        dev->config->device->INTENSET.reg = USB_DEVICE_INTENSET_SUSPEND |
                                            USB_DEVICE_INTENSET_EORST;
    }
}

static inline void _disable_irq(sam0_common_usb_t *dev)
{
    dev->config->device->INTENCLR.reg = USB_DEVICE_INTENCLR_EORST |
                                        USB_DEVICE_INTENCLR_SUSPEND |
                                        USB_DEVICE_INTENCLR_WAKEUP;
}

static void _enable_ep_irq_out(sam0_common_usb_t *usbdev,
                               UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0 |
                             USB_DEVICE_EPINTENSET_STALL0;
    if (ep_reg == &usbdev->config->device->DeviceEndpoint[0]) {
        ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;
    }
}

static void _enable_ep_irq_in(UsbDeviceEndpoint *ep_reg)
{
    ep_reg->EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1 |
                             USB_DEVICE_EPINTENSET_STALL1;
}

/* Enable the endpoint specific interrupts */
static void _enable_ep_irq(usbdev_ep_t *ep)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);

    if (ep->dir == USB_EP_DIR_OUT) {
        _enable_ep_irq_out((sam0_common_usb_t *)ep->dev, ep_reg);
    }
    else {
        _enable_ep_irq_in(ep_reg);
    }
}

static void _disable_ep_irq_out(sam0_common_usb_t *usbdev,
                                UsbDeviceEndpoint *ep_reg)
{
    DEBUG("Disabling OUT irq\n");
    ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_TRCPT0 |
                             USB_DEVICE_EPINTENCLR_STALL0;
    if (ep_reg == &usbdev->config->device->DeviceEndpoint[0]) {
        ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_RXSTP;
    }
}

static void _disable_ep_irq_in(UsbDeviceEndpoint *ep_reg)
{
    DEBUG("Disabling IN irq\n");
    ep_reg->EPINTENCLR.reg = USB_DEVICE_EPINTENCLR_TRCPT1 |
                             USB_DEVICE_EPINTENCLR_STALL1;
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
    dev->config->device->DADD.bit.DADD = addr;
    /* Only enable the address if it is nonzero */
    dev->config->device->DADD.bit.ADDEN = addr ? 1 : 0;
}

static bool _syncbusy_enable(sam0_common_usb_t *dev)
{
    return dev->config->device->SYNCBUSY.bit.ENABLE;
}

static bool _syncbusy_swrst(sam0_common_usb_t *dev)
{
    return dev->config->device->SYNCBUSY.bit.SWRST;
}

static inline void _poweron(sam0_common_usb_t *dev)
{
    sam0_gclk_enable(dev->config->gclk_src);

#if defined(MCLK)
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_USB;
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_USB;
#else
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    PM->APBBMASK.reg |= PM_APBBMASK_USB;
#endif

#if defined(CPU_COMMON_SAMD21)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                        | GCLK_CLKCTRL_GEN(dev->config->gclk_src)
                        | GCLK_CLKCTRL_ID(USB_GCLK_ID);
#else
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                                     | GCLK_PCHCTRL_GEN(dev->config->gclk_src);
#endif
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t size)
{
    sam0_common_usb_t *usbdev = (sam0_common_usb_t *)dev;
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
        res->type = type;
        res->len = size;
        _bank_set_size(res);
    }
    return res;
}

static void _block_pm(void)
{
#if defined(CPU_COMMON_SAMD21)
    pm_block(SAMD21_PM_IDLE_1);
#endif
}

static void _unblock_pm(void)
{
#if defined(CPU_COMMON_SAMD21)
    pm_unblock(SAMD21_PM_IDLE_1);
#endif
}

static void _setup(sam0_common_usb_t *usbdev,
                   const sam0_common_usb_config_t *config)
{
    usbdev->usbdev.driver = &driver;
    usbdev->config = config;
}

void usbdev_init_lowlevel(void)
{
    for (size_t i = 0; i < USB_INST_NUM; i++) {
        _setup(&_usbdevs[i], &sam_usbdev_config[i]);
    }
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    assert(num < USB_INST_NUM);
    return &_usbdevs[num].usbdev;
}

static void _usbdev_init(usbdev_t *dev)
{
    DEBUG("Initializing sam0 usb peripheral\n");
    /* Only one usb device on this board */
    sam0_common_usb_t *usbdev = (sam0_common_usb_t *)dev;

    /* Set GPIO */
    gpio_init(usbdev->config->dp, GPIO_IN);
    gpio_init(usbdev->config->dm, GPIO_IN);
    gpio_init_mux(usbdev->config->dm, usbdev->config->d_mux);
    gpio_init_mux(usbdev->config->dp, usbdev->config->d_mux);
    _poweron(usbdev);

    /* Reset peripheral */
    usbdev->config->device->CTRLA.reg |= USB_CTRLA_SWRST;
    while (_syncbusy_swrst(usbdev)) {}

    /* Enable USB device */
    usbdev->config->device->DESCADD.reg = (uint32_t)&usbdev->banks;
    usbdev->config->device->CTRLA.reg |= USB_CTRLA_ENABLE;
    while (_syncbusy_enable(usbdev)) {}

    /* Calibration values */
    usbdev->config->device->PADCAL.reg =
        USB_PADCAL_TRANSP((*(uint32_t *)USB_FUSES_TRANSP_ADDR >>
                           USB_FUSES_TRANSP_Pos)) |
        USB_PADCAL_TRANSN((*(uint32_t *)USB_FUSES_TRANSN_ADDR >>
                           USB_FUSES_TRANSN_Pos)) |
        USB_PADCAL_TRIM((*(uint32_t *)USB_FUSES_TRIM_ADDR >>
                         USB_FUSES_TRIM_Pos));

    usbdev->config->device->CTRLB.bit.SPDCONF = USB_DEVICE_CTRLB_SPDCONF_FS;
    _enable_irq(usbdev);

    _block_pm();
    usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_HOST_CONNECT);
    /* Interrupt configuration */
#ifdef CPU_COMMON_SAMD5X
    NVIC_EnableIRQ(USB_0_IRQn);
    NVIC_EnableIRQ(USB_1_IRQn);
    NVIC_EnableIRQ(USB_2_IRQn);
    NVIC_EnableIRQ(USB_3_IRQn);
#else
    NVIC_EnableIRQ(USB_IRQn);
#endif
}

static void usb_attach(sam0_common_usb_t *dev)
{
    DEBUG("sam_usb: Attaching to host\n");
    dev->config->device->CTRLB.reg &= ~USB_DEVICE_CTRLB_DETACH;
}

static void usb_detach(sam0_common_usb_t *dev)
{
    DEBUG("sam_usb: detaching to host\n");
    dev->config->device->CTRLB.reg |= USB_DEVICE_CTRLB_DETACH;
}

static int _usbdev_get(usbdev_t *usbdev, usbopt_t opt,
                       void *value, size_t max_len)
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
    sam0_common_usb_t *usbdev = (sam0_common_usb_t *)dev;

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
    DEBUG("sam_usb: Enabling endpoint %d, dir %s\n", ep->num,
          ep->dir == USB_EP_DIR_OUT ? "OUT" : "IN");
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
    default:
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

/*
 * sam0 USB interrupt service routine
 */
void isr_usb(void)
{
    /* TODO: make a bit more elegant for multi-periph support */
    sam0_common_usb_t *dev = &_usbdevs[0];

    if (dev->config->device->EPINTSMRY.reg) {
        /* Endpoint specific interrupt */
        unsigned ep_num = bitarithm_lsb(dev->config->device->EPINTSMRY.reg);
        UsbDeviceEndpoint *ep_reg =
            &dev->config->device->DeviceEndpoint[ep_num];
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
            DEBUG("sam_usb: Unhandled EP interrupt\n");
        }
    }
    else {
        /* Device interrupt */
        _disable_irq(dev);
        dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_ESR);
    }
    cortexm_isr_end();
}

void isr_usb0(void)
{
    isr_usb();
}

void isr_usb1(void)
{
    isr_usb();
}
void isr_usb2(void)
{
    isr_usb();
}
void isr_usb3(void)
{
    isr_usb();
}

static void _usbdev_esr(usbdev_t *dev)
{
    sam0_common_usb_t *usbdev = (sam0_common_usb_t *)dev;

    if (usbdev->config->device->INTFLAG.reg) {
        if (usbdev->config->device->INTFLAG.bit.EORST) {
            /* Clear flag */
            usbdev->config->device->INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESET);
            usbdev->config->device->INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
        }
        else if (usbdev->config->device->INTFLAG.bit.SUSPEND &&
                 !usbdev->suspended) {
            usbdev->config->device->INTFLAG.reg = USB_DEVICE_INTFLAG_WAKEUP |
                                                  USB_DEVICE_INTFLAG_SUSPEND;
            usbdev->suspended = true;
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_SUSPEND);
            /* Low power modes are available while suspended */
            _unblock_pm();
        }
        else if (usbdev->config->device->INTFLAG.bit.WAKEUP &&
                 usbdev->suspended) {
            usbdev->config->device->INTFLAG.reg = USB_DEVICE_INTFLAG_WAKEUP |
                                                  USB_DEVICE_INTFLAG_SUSPEND;
            usbdev->suspended = false;
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESUME);
            /* Device wakeup detected, blocking low power modes */
            _block_pm();
        }
        else {
            DEBUG("sam_usb: Unhandled interrupt\n");
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
        DEBUG("sam_usb: Unhandled get call: 0x%x\n", opt);
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
    default:
        DEBUG("sam_usb: Unhandled set call: 0x%x\n", opt);
        break;
    }
    return res;
}

static int _usbdev_ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    UsbDeviceEndpoint *ep_reg = _ep_reg_from_ep(ep);
    /* Assert the alignment required for the buffers */
    assert(HAS_ALIGNMENT_OF(buf, USBDEV_CPU_DMA_ALIGNMENT));

    _bank_from_ep(ep)->ADDR.reg = (uint32_t)(intptr_t)buf;
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
            DEBUG("Unhandled event: EP OUT %u: 0x%x\n", ep->num,
                  ep_reg->EPINTFLAG.reg);
        }
    }
    else {
        if (ep_reg->EPINTFLAG.bit.TRCPT1) {
            DEBUG("sam_usb: Transfer IN complete\n");
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
            DEBUG("Unhandled event: EP IN %u: 0x%x\n", ep->num,
                  ep_reg->EPINTFLAG.reg);
        }
    }
    if (event >= 0) {
        ep->dev->epcb(ep, event);
    }
    ep->dir == USB_EP_DIR_OUT ? _enable_ep_irq_out((sam0_common_usb_t *)ep->dev,
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
    .xmit = _usbdev_ep_xmit,
};
