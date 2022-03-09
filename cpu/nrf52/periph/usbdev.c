/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52_nrfusb
 * @{
 * @file
 * @brief       USB interface functions
 *
 * @file
 * @brief       Low level USB interface functions for the nrf52840 class
 *              devices
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "architecture.h"
#include "cpu.h"
#include "nrfusb.h"
#include "periph/usbdev.h"
#include "usb.h"
#include "usb/descriptor.h"
#include "bitarithm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static nrfusb_t _usbdevs[NRF_USB_NUM_PERIPH];

static void _init(usbdev_t *usbdev);
static int _get(usbdev_t *usbdev, usbopt_t opt, void *value, size_t max_len);
static int _set(usbdev_t *usbdev, usbopt_t opt, const void *value, size_t value_len);
static usbdev_ep_t *_new_ep(usbdev_t *dev, usb_ep_type_t type, usb_ep_dir_t dir, size_t len);
static void _esr(usbdev_t *usbdev);
static void _ep_init(usbdev_ep_t *ep);
static int _ep_get(usbdev_ep_t *ep, usbopt_ep_t opt, void *value, size_t max_len);
static int _ep_set(usbdev_ep_t *ep, usbopt_ep_t opt, const void *value, size_t value_len);
static int _ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len);
static void _ep_esr(usbdev_ep_t *ep);

static const usbdev_driver_t _driver = {
    .init = _init,
    .new_ep = _new_ep,
    .get = _get,
    .set = _set,
    .esr = _esr,
    .ep_init = _ep_init,
    .ep_get = _ep_get,
    .ep_set = _ep_set,
    .ep_esr = _ep_esr,
    .xmit = _ep_xmit,
};

static inline usbdev_ep_t *_get_ep_in(nrfusb_t *usbdev, unsigned num)
{
    return &usbdev->ep_ins[num];
}

static inline usbdev_ep_t *_get_ep_out(nrfusb_t *usbdev, unsigned num)
{
    return &usbdev->ep_outs[num];
}

static inline usbdev_ep_t *_get_ep(nrfusb_t *usbdev,
                                   unsigned num, usb_ep_dir_t dir)
{
    return dir == USB_EP_DIR_IN ? _get_ep_in(usbdev, num)
                                : _get_ep_out(usbdev, num);
}
static inline void _enable_errata_199(void)
{
    /* Contains the workaround as described in nRF52840 Errata 199 */
    *(volatile uint32_t *)0x40027C1C = 0x00000082;
}

static inline void _disable_errata_199(void)
{
    /* Contains the workaround as described in nRF52840 Errata 199 */
    *(volatile uint32_t *)0x40027C1C = 0x00000000;
}

/* Contains the sequence as described in nRF52840 Errata 187 */
static inline void poweron(nrfusb_t *usbdev)
{
    /* Apply magic */
    *(volatile uint32_t *)0x4006EC00 = 0x00009375;
    *(volatile uint32_t *)0x4006ED14 = 0x00000003;
    *(volatile uint32_t *)0x4006EC00 = 0x00009375;

    /* Enable the peripheral */
    usbdev->device->ENABLE = USBD_ENABLE_ENABLE_Msk;
    /* Waiting for peripheral to enable, this should take a few μs */
    while (!(usbdev->device->EVENTCAUSE & USBD_EVENTCAUSE_READY_Msk)) {}

    usbdev->device->EVENTCAUSE &= ~USBD_EVENTCAUSE_READY_Msk;

    /* Apply more magic */
    *(volatile uint32_t *)0x4006EC00 = 0x00009375;
    *(volatile uint32_t *)0x4006ED14 = 0x00000000;
    *(volatile uint32_t *)0x4006EC00 = 0x00009375;

    /* Enable peripheral a second time */
    /* cppcheck-suppress redundantAssignment
     * (reason: re-enabled as per nordic instructions for this errata) */
    usbdev->device->ENABLE = USBD_ENABLE_ENABLE_Msk;
}

static void usb_attach(nrfusb_t *usbdev)
{
    DEBUG("nrfusb: Enabling pull-up\n");
    usbdev->device->USBPULLUP = 0x01;
}

static void usb_detach(nrfusb_t *usbdev)
{
    DEBUG("nrfusb: Disabling pull-up\n");
    usbdev->device->USBPULLUP = 0x00;
}

static void _copy_setup(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t*)ep->dev;
    usb_setup_t *setup = (usb_setup_t*)(intptr_t)usbdev->device->EPOUT[0].PTR;

    setup->type = usbdev->device->BMREQUESTTYPE;
    setup->request = usbdev->device->BREQUEST;
    setup->value = usbdev->device->WVALUEL | usbdev->device->WVALUEH << 8;
    setup->index = usbdev->device->WINDEXL | usbdev->device->WINDEXH << 8;
    setup->length = usbdev->device->WLENGTHL | usbdev->device->WLENGTHH << 8;
    usbdev->sstate = usb_setup_is_read(setup) ? NRFUSB_SETUP_READ
                                              : NRFUSB_SETUP_WRITE;
    if (setup->request == USB_SETUP_REQ_SET_ADDRESS) {
        DEBUG("nrfusb: set address call\n");
        usbdev->sstate = NRFUSB_SETUP_READY;
    }
}

static void _ep_enable(usbdev_ep_t *ep)
{
    DEBUG("Enabling endpoint %u dir %s\n", ep->num, ep->dir == USB_EP_DIR_OUT ? "OUT" : "IN");
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    if (ep->dir == USB_EP_DIR_OUT) {
        usbdev->device->EPOUTEN |= 1 << ep->num;
    }
    else {
        usbdev->device->EPINEN |= 1 << ep->num;
    }
}

static void _ep_disable(usbdev_ep_t *ep)
{
    /* TODO: validate size */
    DEBUG("disabling endpoint %u dir %s\n", ep->num, ep->dir == USB_EP_DIR_OUT ? "OUT" : "IN");
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    if (ep->dir == USB_EP_DIR_OUT) {
        usbdev->device->EPOUTEN &= ~(1 << ep->num);
    }
    else {
        usbdev->device->EPINEN &= ~(1 << ep->num);
    }
}

static void _ep_set_stall(usbdev_ep_t *ep, usbopt_enable_t enable)
{
    /* TODO: validate size */
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;
    uint32_t val = (ep->num & USBD_EPSTALL_EP_Msk) |
                   (ep->dir == USB_EP_DIR_IN ? USBD_EPSTALL_IO_Msk : 0) |
                   (enable ? USBD_EPSTALL_STALL_Msk : 0);

    usbdev->device->EPSTALL = val;
}

static usbopt_enable_t _ep_get_stall(usbdev_ep_t *ep)
{
    /* TODO: validate size */
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    if (ep->dir == USB_EP_DIR_OUT) {
        return usbdev->device->HALTED.EPOUT[ep->num] ? USBOPT_ENABLE
                                                     : USBOPT_DISABLE;
    }
    else {
        return usbdev->device->HALTED.EPIN[ep->num] ? USBOPT_ENABLE
                                                    : USBOPT_DISABLE;
    }
}

static size_t _ep_get_available(usbdev_ep_t *ep)
{
    /* TODO: validate size */
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    if (ep->dir == USB_EP_DIR_OUT) {
        return usbdev->device->SIZE.EPOUT[ep->num];
    }
    else {
        return usbdev->device->EPIN[ep->num].AMOUNT;
    }
}

static void _ep_dma_out(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    assert(ep->dir == USB_EP_DIR_OUT);
    _enable_errata_199();
    usbdev->device->TASKS_STARTEPOUT[ep->num] = 1;
    /* Block while waiting for dma to finish */
    while (!(usbdev->device->EVENTS_ENDEPOUT[ep->num])) {}

    usbdev->device->EVENTS_ENDEPOUT[ep->num] = 0;
    _disable_errata_199();
}

static void _ep_dma_in(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    assert(ep->dir == USB_EP_DIR_IN);
    _enable_errata_199();
    usbdev->device->TASKS_STARTEPIN[ep->num] = 1;
    /* Block while waiting for dma to finish */
    while (!(usbdev->device->EVENTS_ENDEPIN[ep->num])) {}

    usbdev->device->EVENTS_ENDEPIN[ep->num] = 0;
    _disable_errata_199();
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    assert(num < NRF_USB_NUM_PERIPH);
    return &_usbdevs[num].usbdev;
}

void usbdev_init_lowlevel(void)
{
    for (size_t i = 0; i < NRF_USB_NUM_PERIPH; i++) {
        _usbdevs[i].usbdev.driver = &_driver;
        _usbdevs[i].device = NRF_USBD;
    }
}

static void _init(usbdev_t *dev)
{
    DEBUG("nrfusb: initializing\n");
    /* Engineering revision version A is affected by errata 94, crash *
    * instead of pretending to have functional USB                   */
    assert(NRF_FICR->INFO.VARIANT != 0x41414141);
    nrfusb_t *usbdev = (nrfusb_t *)dev;

    poweron(usbdev);
    usbdev->sstate = NRFUSB_SETUP_READY;

    /* Enable a set of interrupts */
    usbdev->device->INTEN = USBD_INTEN_USBRESET_Msk |
                            USBD_INTEN_EPDATA_Msk |
                            USBD_INTEN_USBEVENT_Msk;
    NVIC_EnableIRQ(USBD_IRQn);
}

static int _get(usbdev_t *usbdev, usbopt_t opt, void *value, size_t max_len)
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
        DEBUG("unhandled get call: 0x%x\n", opt);
        break;
    }
    return res;
}

static int _set(usbdev_t *dev, usbopt_t opt,
                const void *value, size_t value_len)
{
    nrfusb_t *usbdev = (nrfusb_t *)dev;

    (void)value_len;
    int res = -ENOTSUP;

    switch (opt) {
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

static usbdev_ep_t *_new_ep(usbdev_t *dev,
                            usb_ep_type_t type,
                            usb_ep_dir_t dir,
                            size_t len)
{
    nrfusb_t *usbdev = (nrfusb_t*)dev;
    /* The IP supports all types for all endpoints */
    usbdev_ep_t *res = NULL;

    /* Always return endpoint 0 for control types */
    if (type == USB_EP_TYPE_CONTROL) {
        res = _get_ep(usbdev, 0, dir);
        res->num = 0;
    }
    else if (type == USB_EP_TYPE_INTERRUPT || type == USB_EP_TYPE_BULK) {
        /* Find the first unassigned ep with proper dir */
        for (unsigned idx = 1; idx < NRF_USB_NUM_EP && !res; idx++) {
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
        res->len = len;
        DEBUG("nrfusb: Allocated new ep (%d %s)\n", res->num, res->dir == USB_EP_DIR_OUT ? "OUT" : "IN");
    }
    return res;
}

static void _ep_enable_irq(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    if (ep->dir == USB_EP_DIR_IN) {
        if (ep->num == 0) {
            usbdev->device->INTENSET = USBD_INTENSET_EP0DATADONE_Msk;
        }
    }
    else {
        if (ep->num == 0) {
            usbdev->device->INTENSET = USBD_INTENSET_EP0SETUP_Msk;
        }
    }
}

static void _ep_disable_irq(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    if (ep->dir == USB_EP_DIR_IN) {
        if (ep->num == 0) {
            usbdev->device->INTENCLR = USBD_INTENCLR_EP0DATADONE_Msk;
        }
    }
    else {
        if (ep->num == 0) {
            usbdev->device->INTENCLR = USBD_INTENCLR_EP0SETUP_Msk;
        }
    }
}

static void _ep_init(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t*)ep->dev;

    if (ep->num == 0) {
        usbdev->device->EVENTS_EP0SETUP = 0;
    }
    if (ep->dir == USB_EP_DIR_OUT) {
        usbdev->device->EVENTS_ENDEPOUT[ep->num] = 0;
        usbdev->device->EPDATASTATUS = 1 << ep->num;
    }
    else {
        usbdev->device->EPDATASTATUS = 1 << ep->num;
    }
    _ep_enable_irq(ep);
}

static int _ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
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

static int _ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                   const void *value, size_t value_len)
{
    (void)value_len;
    int res = -ENOTSUP;

    switch (opt) {
    case USBOPT_EP_ENABLE:
        assert(value_len == sizeof(usbopt_enable_t));
        if (*((usbopt_enable_t *)value)) {
            _ep_enable(ep);
            _ep_init(ep);
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
        DEBUG("Unhandled set call: 0x%x\n", opt);
        break;
    }
    return res;
}

static int _ep0_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;
    /* Assert the alignment required for the buffers */
    assert(HAS_ALIGNMENT_OF(buf, USBDEV_CPU_DMA_ALIGNMENT));

    if (ep->dir == USB_EP_DIR_IN) {
        if (len == 0 && usbdev->sstate == NRFUSB_SETUP_WRITE) {
            usbdev->device->TASKS_EP0STATUS = 1;
            usbdev->usbdev.epcb(_get_ep_in(usbdev, 0), USBDEV_EVENT_ESR);
            usbdev->sstate = NRFUSB_SETUP_ACKIN;
        }
        else {
            usbdev->device->EPIN[0].PTR = (uint32_t)(intptr_t)buf;
            usbdev->device->EPIN[0].MAXCNT = (uint32_t)len;
            usbdev->device->TASKS_STARTEPIN[0] = 1;
        }
    }
    else {
        usbdev->device->EPOUT[0].PTR = (uint32_t)(intptr_t)buf;
        usbdev->device->EPOUT[0].MAXCNT = (uint32_t)len;
        /* USB_EP_DIR_OUT */
        if (usbdev->sstate == NRFUSB_SETUP_READ) {
            usbdev->device->TASKS_EP0STATUS = 1;
            usbdev->sstate = NRFUSB_SETUP_ACKOUT;
            usbdev->usbdev.epcb(_get_ep_out(usbdev, 0), USBDEV_EVENT_ESR);
        }
        else if (usbdev->sstate == NRFUSB_SETUP_WRITE) {
            usbdev->device->TASKS_EP0RCVOUT = 1;
        }
    }
    return len;
}

static int _ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;

    if (ep->num == 0) {
        /* Endpoint 0 requires special handling as per datasheet sec 6.35.9 */
        return _ep0_xmit(ep, buf, len);
    }
    if (ep->dir == USB_EP_DIR_IN) {
        usbdev->device->EPIN[ep->num].PTR = (uint32_t)(intptr_t)buf;
        usbdev->device->EPIN[ep->num].MAXCNT = (uint32_t)len;
        _ep_dma_in(ep);
    }
    else {
        /* Pre-Setup the EasyDMA settings */
        usbdev->device->EPOUT[ep->num].PTR = (uint32_t)(intptr_t)buf;
        usbdev->device->EPOUT[ep->num].MAXCNT = (uint32_t)(intptr_t)len;
        /* Write nonzero value to EPOUT to indicate ready */
        usbdev->device->SIZE.EPOUT[ep->num] = 1;
    }
    return len;
}

static void _esr(usbdev_t *dev)
{
    nrfusb_t *usbdev = (nrfusb_t *)dev;

    if (usbdev->device->EVENTS_USBRESET) {
        DEBUG("nrfusb: reset condition\n");
        usbdev->device->EVENTS_USBRESET = 0;
        usbdev->sstate = NRFUSB_SETUP_READY;
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESET);
        usbdev->device->INTENSET = USBD_INTENSET_USBRESET_Msk;
    }
    else if (usbdev->device->EVENTS_USBEVENT) {
        uint32_t events = usbdev->device->EVENTCAUSE;
        if (events & USBD_EVENTCAUSE_SUSPEND_Msk) {
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_SUSPEND);
        }
        if (events & USBD_EVENTCAUSE_RESUME_Msk) {
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESUME);
        }
        usbdev->device->EVENTS_USBEVENT = 0;
        /* Clear eventcause register */
        usbdev->device->EVENTCAUSE = 0x0f01;
        usbdev->device->INTENSET = USBD_INTENSET_USBEVENT_Msk;
    }
}

static signed _ep0_esr(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;
    signed event = -1;

    if (ep->dir == USB_EP_DIR_OUT) {
        if (usbdev->sstate == NRFUSB_SETUP_ACKOUT) {
            usbdev->sstate = NRFUSB_SETUP_READY;
            event = USBDEV_EVENT_TR_COMPLETE;
        }
        else if (usbdev->device->EVENTS_EP0SETUP) {
            usbdev->device->EVENTS_EP0SETUP = 0;
            event = USBDEV_EVENT_TR_COMPLETE;
            /* Copy setup request info to buffer */
            _copy_setup(ep);
            if ((uint8_t)usbdev->device->BREQUEST == 0x05) {
                event = 0;
            }
        }
        else if (usbdev->device->EVENTS_EP0DATADONE) {
            usbdev->device->EVENTS_EP0DATADONE = 0;
            _ep_dma_out(ep);
            event = USBDEV_EVENT_TR_COMPLETE;
        }
    }
    else {
        if (usbdev->sstate == NRFUSB_SETUP_ACKIN) {
            usbdev->sstate = NRFUSB_SETUP_READY;
            event = USBDEV_EVENT_TR_COMPLETE;
        }
        else if (usbdev->device->EVENTS_EP0DATADONE) {
            usbdev->device->EVENTS_EP0DATADONE = 0;
            event = USBDEV_EVENT_TR_COMPLETE;
        }
    }
    return event;
}

static void _ep_esr(usbdev_ep_t *ep)
{
    nrfusb_t *usbdev = (nrfusb_t *)ep->dev;
    signed event = -1;

    if (ep->num == 0) {
        event = _ep0_esr(ep);
    }
    else {
        if (ep->dir == USB_EP_DIR_IN) {
            if (usbdev->device->EPDATASTATUS & 1 << ep->num) {
                usbdev->device->EPDATASTATUS = 1 << (ep->num);
                usbdev->device->EVENTS_EPDATA = 0;
                usbdev->device->INTENSET = USBD_INTENSET_EPDATA_Msk;

                event = USBDEV_EVENT_TR_COMPLETE;
            }
        }
        else {
            if (usbdev->device->EPDATASTATUS & 1 << (ep->num + 16)) {
                /* start dma to transfer payload to memory */
                _ep_dma_out(ep);

                usbdev->device->EPDATASTATUS = 1 << (ep->num + 16);
                event = USBDEV_EVENT_TR_COMPLETE;
            }
        }
    }
    if (event) {
        ep->dev->epcb(ep, event);
    }
    _ep_enable_irq(ep);
}

void isr_usbd(void)
{
    /* Only one usb peripheral possible at the moment */
    nrfusb_t *usbdev = &_usbdevs[0];

    /* Generic USB peripheral events */
    if (usbdev->device->EVENTS_USBRESET &&
        (usbdev->device->INTEN & USBD_INTEN_USBRESET_Msk)) {
        usbdev->device->INTENCLR = USBD_INTENCLR_USBRESET_Msk;
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_ESR);
    }
    else if (usbdev->device->EVENTS_USBEVENT &&
             (usbdev->device->INTEN & USBD_INTEN_USBEVENT_Msk)) {
        usbdev->device->INTENCLR = USBD_INTENCLR_USBEVENT_Msk;
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_ESR);
    }
    else {
        /* Endpoint specific isr handling*/
        /* Endpoint 0 SETUP data received requests */
        if (usbdev->device->EVENTS_EP0SETUP &&
            (usbdev->device->INTEN & USBD_INTEN_EP0SETUP_Msk)) {
            usbdev->usbdev.epcb(_get_ep_out(usbdev, 0), USBDEV_EVENT_ESR);
            _ep_disable_irq(_get_ep_out(usbdev, 0));
        }
        if (usbdev->device->EVENTS_EP0DATADONE &&
            (usbdev->device->INTEN & USBD_INTEN_EP0DATADONE_Msk)) {
            if (usbdev->sstate == NRFUSB_SETUP_READ) {
                usbdev->usbdev.epcb(_get_ep_in(usbdev, 0), USBDEV_EVENT_ESR);
                _ep_disable_irq(_get_ep_in(usbdev, 0));
            }
            if (usbdev->sstate == NRFUSB_SETUP_WRITE) {
                usbdev->usbdev.epcb(_get_ep_out(usbdev, 0), USBDEV_EVENT_ESR);
                _ep_disable_irq(_get_ep_in(usbdev, 0));
            }
        }
        if (usbdev->device->EVENTS_EPDATA && usbdev->device->EPDATASTATUS) {
            usbdev->device->EVENTS_EPDATA = 0;
            uint32_t epdatastatus = usbdev->device->EPDATASTATUS;
            while (epdatastatus) {
                unsigned epnum = bitarithm_lsb(epdatastatus);
                if (epnum > 16) {
                    usbdev_ep_t *ep = _get_ep_out(usbdev, epnum - 16);
                    if (ep->type != USB_EP_TYPE_NONE) {
                        /* OUT type endpoint */
                        usbdev->usbdev.epcb(ep,
                                            USBDEV_EVENT_ESR);
                    }
                }
                else {
                    usbdev_ep_t *ep = _get_ep_in(usbdev, epnum);
                    if (ep->type != USB_EP_TYPE_NONE) {
                        usbdev->usbdev.epcb(ep,
                                            USBDEV_EVENT_ESR);
                    }
                }
                epdatastatus &= ~(1 << epnum);
            }
        }
    }
    cortexm_isr_end();
}
