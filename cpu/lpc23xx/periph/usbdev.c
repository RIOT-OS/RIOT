/*
 * SPDX-FileCopyrightText: 2026 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_usbdev
 * @{
 * @file
 * @brief       LPC23xx USB device driver (device mode)
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "bitarithm.h"
#include "cpu.h"
#include "periph/usbdev.h"
#include "VIC.h"

#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_INT_EP_SLOW          (1U << 2)
#define DEV_INT_STATUS           (1U << 3)
#define DEV_INT_CCEMPTY          (1U << 4)
#define DEV_INT_CDFULL           (1U << 5)
#define DEV_INT_EP_REALIZED      (1U << 8)
#define DEV_INT_MASK             (DEV_INT_EP_SLOW | DEV_INT_STATUS)

#define CMD_PHASE_COMMAND       (0x0500U)
/* UM10211 section 13-12 and its examples are correct; table 296 swaps these. */
#define CMD_PHASE_WRITE         (0x0100U)
#define CMD_PHASE_READ          (0x0200U)
#define CMD_SET_ADDRESS         (0xd0U)
#define CMD_CONFIGURE           (0xd8U)
#define CMD_SET_MODE            (0xf3U)
#define CMD_DEVICE_STATUS       (0xfeU)
#define CMD_EP_STATUS           (0x40U)
#define CMD_CLEAR_BUFFER        (0xf2U)
#define CMD_VALIDATE_BUFFER     (0xfaU)

#define DEV_ADDRESS_ENABLE      (1U << 7)
#define DEV_STATUS_CONNECT      (1U << 0)
#define DEV_STATUS_CONNECT_CH   (1U << 1)
#define DEV_STATUS_SUSPEND      (1U << 2)
#define DEV_STATUS_SUSPEND_CH   (1U << 3)
#define DEV_STATUS_RESET        (1U << 4)
#define MODE_ALWAYS_CLOCK       (1U << 0)

#define EP_SELECT_FULL          (1U << 0)
#define EP_SELECT_STALL         (1U << 1)
#define EP_SELECT_SETUP         (1U << 2)
#define EP_STATUS_STALL         (1U << 0)
#define EP_STATUS_DISABLED      (1U << 5)
#define EP_STATUS_COND_STALL    (1U << 7)
#define BUFFER_OVERWRITTEN      (1U << 0)

#define CTRL_READ               (1U << 0)
#define CTRL_WRITE              (1U << 1)
#define CTRL_EP_SHIFT           (2U)
#define RX_LENGTH_MASK          (0x3ffU)
#define RX_DATA_VALID           (1U << 10)
#define RX_PACKET_READY         (1U << 11)
#define USB_CLOCKS              ((1U << 1) | (1U << 4))
#define USB_PORTSEL_CLOCK       (1U << 3)
#define USB_INT_ENABLE          (1U << 31)
#define DEV_INT_ALL             (0x3ffU)
#define VBUS_PIN                (1UL << 30)
#define EP_BIT(idx)             (1UL << (idx))

static_assert(USBDEV_NUM_ENDPOINTS > 0 && USBDEV_NUM_ENDPOINTS <= 16,
              "LPC23xx supports at most 16 logical endpoints");
static_assert(F_CCO % 96000000 == 0, "USB requires an even multiple of 48 MHz");
static_assert(CLOCK_CORECLOCK >= 18000000, "USB requires a CPU clock of at least 18 MHz");
#ifndef XTAL_HZ
#error "USB requires a crystal clock source"
#endif

typedef struct {
    usbdev_ep_t ep;
    uint8_t *buf;
    size_t size;
    size_t transferred;
    bool active;
    bool enabled;
} lpc23xx_usb_ep_t;

static usbdev_t _usbdev;
static lpc23xx_usb_ep_t _eps[2 * USBDEV_NUM_ENDPOINTS];
static uint8_t _address;
static bool _attached;

static void _isr(void) __attribute__((interrupt("IRQ")));
static const usbdev_driver_t _driver;

static unsigned _ep_index(const usbdev_ep_t *ep)
{
    return 2 * ep->num + (ep->dir == USB_EP_DIR_IN);
}

static void _wait_dev_int(uint32_t mask)
{
    while (!(DEV_INT_STAT & mask)) {}
}

/* Sets the 2-bit pin function/mode field starting at bit `shift`. */
static void _pin_func(volatile unsigned long *reg, unsigned shift, unsigned val)
{
    *reg = (*reg & ~(3UL << shift)) | ((unsigned long)val << shift);
}

/* All Serial Interface Engine commands run in thread context; the ISR only masks interrupts. */
static void _cmd_phase(uint8_t value, uint32_t phase, uint32_t done)
{
    DEV_INT_CLR = DEV_INT_CCEMPTY | DEV_INT_CDFULL;
    CMD_CODE = ((uint32_t)value << 16) | phase;
    _wait_dev_int(done);
}

static void _command(uint8_t cmd)
{
    _cmd_phase(cmd, CMD_PHASE_COMMAND, DEV_INT_CCEMPTY);
}

static void _write_command(uint8_t cmd, uint8_t value)
{
    _command(cmd);
    _cmd_phase(value, CMD_PHASE_WRITE, DEV_INT_CCEMPTY);
}

static uint8_t _read_command(uint8_t cmd)
{
    _command(cmd);
    _cmd_phase(cmd, CMD_PHASE_READ, DEV_INT_CDFULL);
    return CMD_DATA;
}

static uint8_t _ep_ack(unsigned idx)
{
    /* Clearing an inactive interrupt does not produce a CDFULL handshake. */
    if (!(EP_INT_STAT & EP_BIT(idx))) {
        return _read_command(idx);
    }
    EP_INT_CLR = EP_BIT(idx);
    _wait_dev_int(DEV_INT_CDFULL);
    return CMD_DATA;
}

static void _clear_ep_interrupts(void)
{
    uint32_t pending = EP_INT_STAT;
    while (pending) {
        unsigned idx = bitarithm_lsb(pending);
        pending &= ~EP_BIT(idx);
        _ep_ack(idx);
    }
}

static size_t _min(size_t a, size_t b)
{
    return a < b ? a : b;
}

static void _fifo_write(const uint8_t *buf, size_t len)
{
    for (size_t pos = 0; pos < len; pos += sizeof(uint32_t)) {
        uint32_t word = 0;
        memcpy(&word, buf + pos, _min(len - pos, sizeof(word)));
        TX_DATA = word;
    }
}

/* Drains len bytes from the RX FIFO, storing only the first keep bytes. */
static void _fifo_read(uint8_t *buf, size_t len, size_t keep)
{
    for (size_t pos = 0; pos < len; pos += sizeof(uint32_t)) {
        uint32_t word = RX_DATA;
        if (pos < keep) {
            memcpy(buf + pos, &word, _min(keep - pos, sizeof(word)));
        }
    }
}

static void _ep_init(usbdev_ep_t *ep)
{
    unsigned idx = _ep_index(ep);
    lpc23xx_usb_ep_t *ctx = &_eps[idx];

    EP_INT_EN &= ~EP_BIT(idx);
    DEV_INT_CLR = DEV_INT_EP_REALIZED;
    REALIZE_EP |= EP_BIT(idx);
    EP_INDEX = idx;
    MAXPACKET_SIZE = ep->len;
    _wait_dev_int(DEV_INT_EP_REALIZED);
    DEV_INT_CLR = DEV_INT_EP_REALIZED;
    _write_command(CMD_EP_STATUS | idx, 0);
    _ep_ack(idx);
    ctx->active = false;
    ctx->enabled = true;
    ctx->transferred = 0;
    EP_INT_EN |= EP_BIT(idx);
}

static void _reset(void)
{
    EP_INT_EN = 0;
    _clear_ep_interrupts();
    _address = 0;
    for (unsigned i = 0; i < ARRAY_SIZE(_eps); ++i) {
        _eps[i].active = false;
        _eps[i].enabled = false;
        _eps[i].transferred = 0;
    }
    /* Bus reset leaves both control endpoints stalled. */
    _write_command(CMD_EP_STATUS, 0);
    _write_command(CMD_EP_STATUS | 1, 0);
}

static void _init(usbdev_t *dev)
{
    (void)dev;
    DEBUG("lpc23xx_usb: init PLLSTAT=%08lx CCLKCFG=%lu\n", PLLSTAT, CCLKCFG);
    memset(_eps, 0, sizeof(_eps));
    _attached = false;
#ifdef MODULE_PM_LAYERED
    /* Sleep and deeper modes stop the PLL that supplies the USB clock. */
    pm_block(2);
#endif
    PCONP |= PCUSB;
    USBCLKCFG = F_CCO / 48000000 - 1;
    USBClkCtrl = USB_CLOCKS;
    while ((USBClkSt & USB_CLOCKS) != USB_CLOCKS) {}
    DEBUG("lpc23xx_usb: clocks ready\n");

#ifdef CPU_MODEL_LPC2388
    USBClkCtrl |= USB_PORTSEL_CLOCK;
    while (!(USBClkSt & USB_PORTSEL_CLOCK)) {}
    USBPortSel = 0;
    USBClkCtrl &= ~USB_PORTSEL_CLOCK;
#endif

    /* USB pins are fixed */
    _pin_func(&PINSEL1, 26, 1);
    _pin_func(&PINSEL1, 28, 1);
    _pin_func(&PINSEL3, 28, 2);
    _pin_func(&PINMODE3, 28, 2);
    _pin_func(&PINSEL4, 18, 1);

    DEV_INT_EN = 0;
    EP_DMA_DIS = 0xffffffff;
    DMA_INT_EN = 0;
    EP_INT_PRIO = 0;
    DEV_INT_PRIO = 0;
    _write_command(CMD_DEVICE_STATUS, 0);
    _write_command(CMD_CONFIGURE, 0);
    _write_command(CMD_SET_MODE, MODE_ALWAYS_CLOCK);
    _write_command(CMD_SET_ADDRESS, DEV_ADDRESS_ENABLE);
    _reset();
    DEV_INT_CLR = DEV_INT_ALL;
    USB_INT_STAT = USB_INT_ENABLE;
    install_irq(USB_INT, _isr, 4);
    DEV_INT_EN = DEV_INT_MASK;
    DEBUG("lpc23xx_usb: init complete VBUS=%u\n", !!(FIO1PIN & VBUS_PIN));
}

/* Endpoint 0 is control, 15 is bulk, the others repeat interrupt/bulk/bulk
 * following the endpoint layout of the LPC23xx. */
static usb_ep_type_t _ep_type_supported(unsigned num)
{
    if (num == 0) {
        return USB_EP_TYPE_CONTROL;
    }
    if (num == 15 || num % 3 == 2) {
        return USB_EP_TYPE_BULK;
    }
    return num % 3 == 1 ? USB_EP_TYPE_INTERRUPT : USB_EP_TYPE_NONE;
}

static usbdev_ep_t *_new_ep(usbdev_t *dev, usb_ep_type_t type,
                            usb_ep_dir_t dir, size_t len)
{
    if ((dir != USB_EP_DIR_IN && dir != USB_EP_DIR_OUT) || !len || len > 64) {
        return NULL;
    }
    if ((type == USB_EP_TYPE_CONTROL || type == USB_EP_TYPE_BULK) &&
        len != 8 && len != 16 && len != 32 && len != 64) {
        return NULL;
    }
    for (unsigned num = 0; num < USBDEV_NUM_ENDPOINTS; ++num) {
        usb_ep_type_t supported = _ep_type_supported(num);
        lpc23xx_usb_ep_t *ctx = &_eps[2 * num + (dir == USB_EP_DIR_IN)];
        if (type == supported && supported != USB_EP_TYPE_NONE &&
            (num == 0 || ctx->ep.type == USB_EP_TYPE_NONE)) {
            ctx->ep = (usbdev_ep_t) {
                .dev = dev,
                .num = num,
                .dir = dir,
                .type = type,
                .len = len,
            };
            return &ctx->ep;
        }
    }
    DEBUG("lpc23xx_usb: no endpoint for type %u, direction %u\n", type, dir);
    return NULL;
}

/* Stores a value of `size` bytes in the caller's buffer and returns its size. */
static int _put(void *dst, size_t max_len, const void *src, size_t size)
{
    assert(max_len >= size);
    (void)max_len;
    memcpy(dst, src, size);
    return size;
}

static int _put_enable(void *dst, size_t max_len, bool enabled)
{
    usbopt_enable_t val = enabled ? USBOPT_ENABLE : USBOPT_DISABLE;
    return _put(dst, max_len, &val, sizeof(val));
}

static bool _get_enable(const void *value, size_t value_len)
{
    assert(value_len == sizeof(usbopt_enable_t));
    (void)value_len;
    return *(const usbopt_enable_t *)value == USBOPT_ENABLE;
}

static int _get(usbdev_t *dev, usbopt_t opt, void *value, size_t max_len)
{
    (void)dev;
    switch (opt) {
    case USBOPT_ADDRESS:
        return _put(value, max_len, &_address, sizeof(_address));
    case USBOPT_ATTACH:
        return _put_enable(value, max_len, _attached);
    case USBOPT_CONNECTED:
        return _put_enable(value, max_len, FIO1PIN & VBUS_PIN);
    case USBOPT_MAX_VERSION: {
        usb_version_t version = USB_VERSION_20;
        return _put(value, max_len, &version, sizeof(version));
    }
    case USBOPT_MAX_SPEED:
    case USBOPT_ENUMERATED_SPEED: {
        usb_speed_t speed = USB_SPEED_FULL;
        return _put(value, max_len, &speed, sizeof(speed));
    }
    default:
        return -ENOTSUP;
    }
}

static int _set(usbdev_t *dev, usbopt_t opt, const void *value, size_t value_len)
{
    (void)dev;
    switch (opt) {
    case USBOPT_ADDRESS:
        assert(value_len == sizeof(uint8_t));
        if (*(const uint8_t *)value > 127) {
            return -EINVAL;
        }
        _address = *(const uint8_t *)value;
        _write_command(CMD_SET_ADDRESS, DEV_ADDRESS_ENABLE | _address);
        return sizeof(uint8_t);
    case USBOPT_ATTACH:
        _attached = _get_enable(value, value_len);
        _write_command(CMD_DEVICE_STATUS, _attached ? DEV_STATUS_CONNECT : 0);
        DEBUG("lpc23xx_usb: attached=%u, pins=%08lx\n", _attached, FIO2PIN);
        return sizeof(usbopt_enable_t);
    default:
        return -ENOTSUP;
    }
}

static void _ep0_stall(usbdev_t *dev)
{
    (void)dev;
    _write_command(CMD_EP_STATUS, EP_STATUS_COND_STALL);
}

static void _ep_stall(usbdev_ep_t *ep, bool enable)
{
    unsigned idx = _ep_index(ep);
    _write_command(CMD_EP_STATUS | idx, enable ? EP_STATUS_STALL : 0);
    _eps[idx].active = false;
}

static int _ep_get(usbdev_ep_t *ep, usbopt_ep_t opt, void *value, size_t max_len)
{
    unsigned idx = _ep_index(ep);
    switch (opt) {
    case USBOPT_EP_AVAILABLE:
        return _put(value, max_len, &_eps[idx].transferred,
                    sizeof(_eps[idx].transferred));
    case USBOPT_EP_ENABLE:
        return _put_enable(value, max_len, _eps[idx].enabled);
    case USBOPT_EP_STALL:
        return _put_enable(value, max_len, _read_command(idx) & EP_SELECT_STALL);
    default:
        return -ENOTSUP;
    }
}

static int _ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                   const void *value, size_t value_len)
{
    unsigned idx = _ep_index(ep);
    switch (opt) {
    case USBOPT_EP_ENABLE:
        if (_get_enable(value, value_len)) {
            _ep_init(ep);
        }
        else {
            EP_INT_EN &= ~EP_BIT(idx);
            _write_command(CMD_EP_STATUS | idx, EP_STATUS_DISABLED);
            _ep_ack(idx);
            _eps[idx].enabled = false;
            _eps[idx].active = false;
        }
        /* There is no separate configuration option in the usbdev API. */
        bool configured = false;
        for (unsigned i = 2; i < ARRAY_SIZE(_eps); ++i) {
            configured |= _eps[i].enabled;
        }
        _write_command(CMD_CONFIGURE, configured);
        return sizeof(usbopt_enable_t);
    case USBOPT_EP_STALL:
        _ep_stall(ep, _get_enable(value, value_len));
        return sizeof(usbopt_enable_t);
    default:
        return -ENOTSUP;
    }
}

static int _ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    unsigned idx = _ep_index(ep);
    lpc23xx_usb_ep_t *ctx = &_eps[idx];

    if (!ctx->enabled) {
        return -EAGAIN;
    }
    if (len > ep->len || (len && !buf)) {
        return -EINVAL;
    }
    if (ctx->active) {
        return -EBUSY;
    }
    if (ep->num && (_read_command(idx) & EP_SELECT_STALL)) {
        _ep_stall(ep, false);
    }
    ctx->buf = buf;
    ctx->size = len;
    ctx->active = true;
    if (ep->dir == USB_EP_DIR_IN) {
        _command(idx);
        USB_CTRL = CTRL_WRITE | (ep->num << CTRL_EP_SHIFT);
        TX_PLENGTH = len;
        _fifo_write(buf, len);
        USB_CTRL = 0;
        _command(idx);
        _command(CMD_VALIDATE_BUFFER);
    }
    else if (_read_command(idx) & EP_SELECT_FULL) {
        /* An OUT packet may have arrived before the receive buffer was armed. */
        EP_INT_SET = EP_BIT(idx);
    }
    return len;
}

/* Reads one packet from the RX FIFO into the endpoint buffer, releases the
 * hardware buffer and returns whether the packet was valid. */
static bool _ep_read_packet(usbdev_ep_t *ep, lpc23xx_usb_ep_t *ctx, unsigned idx,
                            bool *overwritten)
{
    USB_CTRL = CTRL_READ | (ep->num << CTRL_EP_SHIFT);
    while (!(RX_PLENGTH & RX_PACKET_READY)) {}
    uint32_t packet = RX_PLENGTH;
    size_t len = packet & RX_LENGTH_MASK;
    ctx->transferred = _min(len, ctx->size);
    if (len > ctx->size) {
        DEBUG("lpc23xx_usb: truncating OUT packet (%u > %u)\n",
              (unsigned)len, (unsigned)ctx->size);
    }
    _fifo_read(ctx->buf, len, ctx->transferred);
    USB_CTRL = 0;
    _command(idx);
    *overwritten = _read_command(CMD_CLEAR_BUFFER) & BUFFER_OVERWRITTEN;
    return (packet & RX_DATA_VALID) != 0;
}

static void _ep_esr(usbdev_ep_t *ep)
{
    unsigned idx = _ep_index(ep);
    lpc23xx_usb_ep_t *ctx = &_eps[idx];

    if (!ctx->enabled || !(EP_INT_STAT & EP_BIT(idx))) {
        return;
    }
    uint8_t status = _ep_ack(idx);
    bool complete = ctx->active;
    if (ep->dir == USB_EP_DIR_OUT) {
        bool is_setup = ep->num == 0 && (status & EP_SELECT_SETUP);
        /* SETUP must also work after a stall, without another xmit() call. */
        if (!(status & EP_SELECT_FULL) || (!ctx->active && !(is_setup && ctx->buf))) {
            return;
        }
        bool overwritten;
        bool valid;
        do {
            if (status & EP_SELECT_SETUP) {
                /* A new SETUP invalidates the previous control IN transfer. */
                _eps[1].active = false;
                _ep_ack(1);
                complete = true;
            }
            valid = _ep_read_packet(ep, ctx, idx, &overwritten);
            if (overwritten) {
                status = _ep_ack(idx);
            }
        } while (overwritten);
        if (!valid) {
            DEBUG("lpc23xx_usb: discarding invalid OUT packet\n");
            return;
        }
    }
    else {
        ctx->transferred = ctx->size;
    }
    ctx->active = false;
    if (complete) {
        ep->dev->epcb(ep, USBDEV_EVENT_TR_COMPLETE);
    }
}

static void _esr(usbdev_t *dev)
{
    if (DEV_INT_STAT & DEV_INT_STATUS) {
        DEV_INT_CLR = DEV_INT_STATUS;
        uint8_t status = _read_command(CMD_DEVICE_STATUS);
        if (status & DEV_STATUS_CONNECT_CH) {
            _attached = (status & DEV_STATUS_CONNECT) != 0;
            dev->cb(dev, _attached ? USBDEV_EVENT_HOST_CONNECT :
                                    USBDEV_EVENT_HOST_DISCONNECT);
        }
        if (status & DEV_STATUS_RESET) {
            _reset();
            dev->cb(dev, USBDEV_EVENT_RESET);
        }
        else if (status & DEV_STATUS_SUSPEND_CH) {
            dev->cb(dev, (status & DEV_STATUS_SUSPEND) ? USBDEV_EVENT_SUSPEND :
                                                      USBDEV_EVENT_RESUME);
        }
    }
    uint32_t pending = EP_INT_STAT & EP_INT_EN;
    while (pending) {
        unsigned idx = bitarithm_lsb(pending);
        pending &= ~EP_BIT(idx);
        _ep_esr(&_eps[idx].ep);
    }
    /* EP_INT_EN selects slave versus DMA mode, not just interrupt masking.
     * Keep it enabled while servicing endpoints so packets cannot be missed. */
    if (!EP_INT_STAT) {
        DEV_INT_CLR = DEV_INT_EP_SLOW;
    }
    DEV_INT_EN = DEV_INT_MASK;
}

static void _isr(void)
{
    DEV_INT_EN = 0;
    _usbdev.cb(&_usbdev, USBDEV_EVENT_ESR);
    VICVectAddr = 0;
}

static const usbdev_driver_t _driver = {
    .init = _init,
    .new_ep = _new_ep,
    .get = _get,
    .set = _set,
    .esr = _esr,
    .ep0_stall = _ep0_stall,
    .ep_init = _ep_init,
    .ep_stall = _ep_stall,
    .ep_get = _ep_get,
    .ep_set = _ep_set,
    .ep_esr = _ep_esr,
    .xmit = _ep_xmit,
};

void usbdev_init_lowlevel(void)
{
    _usbdev.driver = &_driver;
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    assert(num == 0);
    (void)num;
    return &_usbdev;
}
