/*
 * SPDX-FileCopyrightText: 2024 Frank Engelhardt
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Low level USB device driver for the RP2040 and RP2350
 *
 * The USB controllers of the RP2040 and the RP2350 are identical. The
 * register names of the RP2040 vendor header are used, the RP2350 maps them
 * to the register names of the Pico SDK in `usbdev_regs.h`.
 *
 * The USB controller uses a 4 KiB dual port RAM (DPRAM) which
 * contains the endpoint configuration, the buffer control registers and the
 * data buffers. Setup packets are always written to the beginning of the
 * DPRAM, independent of the state of the EP0 OUT buffer. They are passed to
 * USBUS as regular transfer on EP0 OUT.
 *
 * All endpoints are used in single buffered mode. The data buffers of the
 * endpoints are allocated in the DPRAM and data is copied between the DPRAM
 * and the buffers provided by USBUS.
 *
 * @author      Frank Engelhardt <frank@f9e.de>
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "bitarithm.h"
#include "busy_wait.h"
#include "container.h"
#include "cpu.h"
#include "irq.h"
#include "periph/usbdev.h"
#include "periph_cpu.h"

#include "compat_layer.h"

#include "regs/resets.h"
#include "regs/usb.h"
#include "regs/usb_device_dpram.h"

#if defined(CPU_FAM_RP2040)
#  include "RP2040.h"
#elif defined(CPU_FAM_RP2350)
#  include "RP2350.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Size of the EP0 buffer, the controller uses a fixed buffer at
 *          offset 0x100 for both EP0 IN and EP0 OUT
 */
#define EP0_BUF_SIZE            (64U)
#define EP0_BUF_OFFSET          (0x100U)
#define EP_BUF_ALIGN            (64U)
#define DPRAM_BUF_START         (0x180U)
#define DPRAM_SIZE              (0x1000U)

#define SETUP_PACKET_LEN        (8U)

#ifdef CPU_FAM_RP2040
/**
 * @name    Workaround for RP2040 erratum RP2040-E5
 * @{
 */
#  define E5_DP_PIN             (15U)   /**< GPIO used to force the line state */
#  define E5_FUNCSEL_USB_DEBUG  (8U)    /**< USB debug muxing function of GPIO15 */
#  define E5_FORCE_J_US         (1000U) /**< Duration of the forced J state */
#  define E5_AFFECTED_REVISION  (1U)    /**< Chip revision of RP2040-B0/B1 */
/** @} */

/**
 * @name    Line states as reported in SIE_STATUS.LINE_STATE
 * @{
 */
#  define LINE_STATE_SE0        (0U)
#  define LINE_STATE_J          (1U)
/** @} */
#endif

#define BUF_CTRL_FULL           USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_BITS
#define BUF_CTRL_PID            USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_PID_0_BITS
#define BUF_CTRL_STALL          USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_BITS
#define BUF_CTRL_AVAILABLE      USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_BITS
#define BUF_CTRL_LENGTH         USB_DEVICE_DPRAM_EP0_IN_BUFFER_CONTROL_LENGTH_0_BITS

/**
 * @brief   Interrupts handled on device level
 */
#define DEV_IRQS                (USB_INTS_BUS_RESET_BITS | \
                                 USB_INTS_DEV_SUSPEND_BITS | \
                                 USB_INTS_DEV_RESUME_FROM_HOST_BITS)

typedef struct {
    usbdev_ep_t ep;             /**< Generic endpoint struct, must be first */
    uint8_t *dpram_buf;         /**< Endpoint buffer in the DPRAM */
    uint8_t *buf;               /**< Buffer passed by the last xmit call */
    size_t rx_len;              /**< Number of bytes received (OUT only) */
    uint16_t dpram_offset;      /**< Offset of the buffer in the DPRAM */
    bool next_pid;              /**< Data PID used for the next transfer */
    volatile bool pending;      /**< Transfer completed, not yet serviced */
} _rpx0xx_ep_t;

typedef struct {
    usbdev_t dev;                               /**< Generic usbdev struct, must be first */
    _rpx0xx_ep_t eps[2 * USBDEV_NUM_ENDPOINTS]; /**< Endpoints */
    uint16_t dpram_next;                        /**< Next free offset in the DPRAM */
    volatile uint32_t events;                   /**< Device events not yet serviced */
    volatile bool setup_pending;                /**< Setup packet not yet serviced */
} _rpx0xx_usbdev_t;

static const usbdev_driver_t _driver;
static _rpx0xx_usbdev_t _usbdev;

static inline uint8_t _ep_idx(uint8_t num, usb_ep_dir_t dir)
{
    /* same layout as the bits in BUFF_STATUS */
    return 2 * num + ((dir == USB_EP_DIR_OUT) ? 1 : 0);
}

static inline _rpx0xx_ep_t *_get_ep(uint8_t num, usb_ep_dir_t dir)
{
    return &_usbdev.eps[_ep_idx(num, dir)];
}

static inline _rpx0xx_ep_t *_hw_ep(usbdev_ep_t *ep)
{
    return container_of(ep, _rpx0xx_ep_t, ep);
}

/* EPx_IN_CONTROL / EPx_OUT_CONTROL, only for endpoints 1 to 15 */
static inline volatile uint32_t *_ep_ctrl(const usbdev_ep_t *ep)
{
    assert(ep->num > 0);
    return &USB_DPRAM->EP1_IN_CONTROL + _ep_idx(ep->num - 1, ep->dir);
}

/* EPx_IN_BUFFER_CONTROL / EPx_OUT_BUFFER_CONTROL */
static inline volatile uint32_t *_buf_ctrl(const usbdev_ep_t *ep)
{
    return &USB_DPRAM->EP0_IN_BUFFER_CONTROL + _ep_idx(ep->num, ep->dir);
}

static void _buf_arm(_rpx0xx_ep_t *hw_ep, size_t len)
{
    volatile uint32_t *reg = _buf_ctrl(&hw_ep->ep);
    uint32_t val = len & BUF_CTRL_LENGTH;

    if (hw_ep->ep.dir == USB_EP_DIR_IN) {
        val |= BUF_CTRL_FULL;
    }
    /* the data PID is toggled once the transfer completed, an endpoint may
     * be armed again without a completed transfer in between */
    if (hw_ep->next_pid) {
        val |= BUF_CTRL_PID;
    }

    /* The AVAILABLE bit must be set separately after the rest of the buffer
     * control register has been written. The controller runs at 48 MHz while
     * the CPU runs faster, so wait at least 3 clk_usb cycles in between
     * (see RP2040 datasheet, section 4.1.2.5.1) */
    *reg = val;
    busy_wait(4);
    *reg = val | BUF_CTRL_AVAILABLE;
}

/*
 * The DPRAM is mapped as device memory, which does not allow unaligned
 * accesses. memcpy() may use unaligned word accesses on cores that support
 * them for normal memory (e.g. the Cortex-M33 of the RP2350), so the DPRAM is
 * accessed byte wise.
 */
static void _dpram_write(uint8_t *dpram, const uint8_t *buf, size_t len)
{
    volatile uint8_t *dst = dpram;

    for (size_t i = 0; i < len; i++) {
        dst[i] = buf[i];
    }
}

static void _dpram_read(uint8_t *buf, const uint8_t *dpram, size_t len)
{
    const volatile uint8_t *src = dpram;

    for (size_t i = 0; i < len; i++) {
        buf[i] = src[i];
    }
}

static void _buf_disarm(_rpx0xx_ep_t *hw_ep)
{
    *_buf_ctrl(&hw_ep->ep) = 0;
}

void usbdev_init_lowlevel(void)
{
    _usbdev.dev.driver = &_driver;
    _usbdev.dpram_next = DPRAM_BUF_START;
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    (void)num;
    return &_usbdev.dev;
}

static void _init(usbdev_t *dev)
{
    (void)dev;
    DEBUG("[rpx0xx usb] init\n");

    atomic_set(&RESETS->RESET, RESETS_RESET_USBCTRL_BITS);
    reset_component(RESETS_RESET_USBCTRL_BITS, RESETS_RESET_USBCTRL_BITS);

    for (uint16_t i = 0; i < DPRAM_SIZE / sizeof(uint32_t); i++) {
        ((volatile uint32_t *)USB_DPRAM)[i] = 0;
    }

    /* connect the controller to the internal PHY */
    USB->USB_MUXING = USB_USB_MUXING_TO_PHY_BITS |
                               USB_USB_MUXING_SOFTCON_BITS;
    /* VBUS is not connected to the controller, pretend it is always there */
    USB->USB_PWR = USB_USB_PWR_VBUS_DETECT_BITS |
                            USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS;
    USB->MAIN_CTRL = USB_MAIN_CTRL_CONTROLLER_EN_BITS;
    /* generate a buffer status interrupt for every EP0 buffer */
    USB->SIE_CTRL = USB_SIE_CTRL_EP0_INT_1BUF_BITS;
    USB->INTE = USB_INTE_BUFF_STATUS_BITS |
                         USB_INTE_SETUP_REQ_BITS |
                         DEV_IRQS;

    NVIC_EnableIRQ(USBCTRL_IRQ_IRQn);
}

static int _get(usbdev_t *dev, usbopt_t opt, void *value, size_t max_len)
{
    (void)dev;
    (void)max_len;

    switch (opt) {
    case USBOPT_MAX_VERSION:
        assert(max_len == sizeof(usb_version_t));
        *(usb_version_t *)value = USB_VERSION_20;
        return sizeof(usb_version_t);
    case USBOPT_MAX_SPEED:
        assert(max_len == sizeof(usb_speed_t));
        *(usb_speed_t *)value = USB_SPEED_FULL;
        return sizeof(usb_speed_t);
    default:
        DEBUG("[rpx0xx usb] unhandled get call: 0x%x\n", opt);
        return -ENOTSUP;
    }
}

static int _set(usbdev_t *dev, usbopt_t opt, const void *value, size_t value_len)
{
    (void)dev;
    (void)value_len;

    switch (opt) {
    case USBOPT_ADDRESS:
        assert(value_len == sizeof(uint8_t));
        USB->ADDR_ENDP = *(const uint8_t *)value &
                                  USB_ADDR_ENDP_ADDRESS_BITS;
        return sizeof(uint8_t);
    case USBOPT_ATTACH:
        assert(value_len == sizeof(usbopt_enable_t));
        if (*(const usbopt_enable_t *)value) {
            atomic_set(&USB->SIE_CTRL,
                              USB_SIE_CTRL_PULLUP_EN_BITS);
        }
        else {
            atomic_clear(&USB->SIE_CTRL,
                                USB_SIE_CTRL_PULLUP_EN_BITS);
        }
        return sizeof(usbopt_enable_t);
    default:
        DEBUG("[rpx0xx usb] unhandled set call: 0x%x\n", opt);
        return -ENOTSUP;
    }
}

static usbdev_ep_t *_new_ep(usbdev_t *dev, usb_ep_type_t type,
                            usb_ep_dir_t dir, size_t size)
{
    _rpx0xx_ep_t *hw_ep = NULL;

    DEBUG("[rpx0xx usb] new_ep: type=%u dir=%u size=%u\n",
          (unsigned)type, (unsigned)dir, (unsigned)size);

    if (type == USB_EP_TYPE_CONTROL) {
        hw_ep = _get_ep(0, dir);
        if ((hw_ep->ep.type != USB_EP_TYPE_NONE) || (size > EP0_BUF_SIZE)) {
            return NULL;
        }
        hw_ep->ep.num = 0;
        hw_ep->dpram_offset = EP0_BUF_OFFSET;
    }
    else {
        /* buffers in the DPRAM must be 64 byte aligned */
        size_t alloc = (size + EP_BUF_ALIGN - 1) & ~(EP_BUF_ALIGN - 1);

        if (_usbdev.dpram_next + alloc > DPRAM_SIZE) {
            return NULL;
        }
        for (uint8_t num = 1; num < USBDEV_NUM_ENDPOINTS; num++) {
            if (_get_ep(num, dir)->ep.type == USB_EP_TYPE_NONE) {
                hw_ep = _get_ep(num, dir);
                hw_ep->ep.num = num;
                break;
            }
        }
        if (hw_ep == NULL) {
            return NULL;
        }
        hw_ep->dpram_offset = _usbdev.dpram_next;
        _usbdev.dpram_next += alloc;
    }

    hw_ep->dpram_buf = (uint8_t *)USB_DPRAM + hw_ep->dpram_offset;
    hw_ep->ep.dev = dev;
    hw_ep->ep.dir = dir;
    hw_ep->ep.type = type;
    hw_ep->ep.len = size;

    return &hw_ep->ep;
}

static void _ep0_stall(usbdev_t *dev)
{
    (void)dev;
    /* EP_STALL_ARM is cleared automatically on the next setup packet */
    atomic_set(&USB->EP_STALL_ARM,
                      USB_EP_STALL_ARM_EP0_IN_BITS |
                      USB_EP_STALL_ARM_EP0_OUT_BITS);
    *_buf_ctrl(&_get_ep(0, USB_EP_DIR_IN)->ep) = BUF_CTRL_STALL;
    *_buf_ctrl(&_get_ep(0, USB_EP_DIR_OUT)->ep) = BUF_CTRL_STALL;
}

static void _ep_stall(usbdev_ep_t *ep, bool enable)
{
    _rpx0xx_ep_t *hw_ep = _hw_ep(ep);

    if (ep->num == 0) {
        uint32_t mask = (ep->dir == USB_EP_DIR_IN)
                      ? USB_EP_STALL_ARM_EP0_IN_BITS
                      : USB_EP_STALL_ARM_EP0_OUT_BITS;
        if (enable) {
            atomic_set(&USB->EP_STALL_ARM, mask);
        }
        else {
            atomic_clear(&USB->EP_STALL_ARM, mask);
        }
    }
    if (enable) {
        *_buf_ctrl(ep) = BUF_CTRL_STALL;
    }
    else {
        _buf_disarm(hw_ep);
        /* clearing the halt feature resets the data toggle */
        hw_ep->next_pid = 0;
    }
}

static void _ep_init(usbdev_ep_t *ep)
{
    _rpx0xx_ep_t *hw_ep = _hw_ep(ep);

    DEBUG("[rpx0xx usb] ep_init: EP%u %s\n", ep->num,
          (ep->dir == USB_EP_DIR_IN) ? "IN" : "OUT");

    hw_ep->buf = NULL;
    hw_ep->rx_len = 0;
    hw_ep->pending = false;
    /* data stage of control transfers always starts with DATA1 */
    hw_ep->next_pid = (ep->num == 0);
    _buf_disarm(hw_ep);
}

static void _ep_enable(usbdev_ep_t *ep, bool enable)
{
    if (ep->num == 0) {
        /* EP0 is always enabled */
        return;
    }

    if (enable) {
        static const uint8_t ep_types[] = {
            [USB_EP_TYPE_CONTROL] = USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_CONTROL,
            [USB_EP_TYPE_INTERRUPT] = USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_INTERRUPT,
            [USB_EP_TYPE_BULK] = USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_BULK,
            [USB_EP_TYPE_ISOCHRONOUS] = USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_VALUE_ISOCHRONOUS,
        };
        *_ep_ctrl(ep) = USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENABLE_BITS |
                        USB_DEVICE_DPRAM_EP1_IN_CONTROL_INTERRUPT_PER_BUFF_BITS |
                        (ep_types[ep->type] << USB_DEVICE_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_LSB) |
                        _hw_ep(ep)->dpram_offset;
    }
    else {
        _buf_disarm(_hw_ep(ep));
        *_ep_ctrl(ep) = 0;
    }
}

static int _ep_get(usbdev_ep_t *ep, usbopt_ep_t opt, void *value, size_t max_len)
{
    (void)max_len;

    switch (opt) {
    case USBOPT_EP_STALL:
        assert(max_len == sizeof(usbopt_enable_t));
        *(usbopt_enable_t *)value = (*_buf_ctrl(ep) & BUF_CTRL_STALL)
                                  ? USBOPT_ENABLE : USBOPT_DISABLE;
        return sizeof(usbopt_enable_t);
    case USBOPT_EP_AVAILABLE:
        assert(max_len == sizeof(size_t));
        *(size_t *)value = _hw_ep(ep)->rx_len;
        return sizeof(size_t);
    default:
        DEBUG("[rpx0xx usb] unhandled ep get call: 0x%x\n", opt);
        return -ENOTSUP;
    }
}

static int _ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                   const void *value, size_t value_len)
{
    (void)value_len;

    switch (opt) {
    case USBOPT_EP_ENABLE:
        assert(value_len == sizeof(usbopt_enable_t));
        _ep_enable(ep, *(const usbopt_enable_t *)value);
        return sizeof(usbopt_enable_t);
    case USBOPT_EP_STALL:
        assert(value_len == sizeof(usbopt_enable_t));
        _ep_stall(ep, *(const usbopt_enable_t *)value);
        return sizeof(usbopt_enable_t);
    default:
        DEBUG("[rpx0xx usb] unhandled ep set call: 0x%x\n", opt);
        return -ENOTSUP;
    }
}

static int _xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    _rpx0xx_ep_t *hw_ep = _hw_ep(ep);

    assert(len <= ep->len);

    hw_ep->buf = buf;
    hw_ep->rx_len = 0;

    if (ep->dir == USB_EP_DIR_IN) {
        _dpram_write(hw_ep->dpram_buf, buf, len);
    }
    _buf_arm(hw_ep, len);

    return 0;
}

static void _bus_reset(void)
{
    /* disarm all buffers and disable all endpoints except EP0, they are
     * enabled again by USBUS once the device is configured */
    for (uint8_t idx = 0; idx < ARRAY_SIZE(_usbdev.eps); idx++) {
        (&USB_DPRAM->EP0_IN_BUFFER_CONTROL)[idx] = 0;
        if (idx >= 2) {
            (&USB_DPRAM->EP1_IN_CONTROL)[idx - 2] = 0;
        }
        _usbdev.eps[idx].pending = false;
    }
    _usbdev.setup_pending = false;
}

#ifdef CPU_FAM_RP2040
static uint32_t _line_state(void)
{
    return (USB->SIE_STATUS & USB_SIE_STATUS_LINE_STATE_Msk) >>
           USB_SIE_STATUS_LINE_STATE_Pos;
}

/*
 * Workaround for erratum RP2040-E5 "USB device fails to exit RESET state on
 * busy USB bus", taken from the Pico SDK (rp2040_usb_device_enumeration.c).
 *
 * After a bus reset, the controller expects 800 us of J state before it
 * considers the device connected. On a hub that forwards packets of other
 * devices this may never happen. Therefore the J state is forced for 1 ms by
 * temporarily routing the USB input path through GPIO15.
 *
 * The workaround busy waits until the bus reset is over, which takes up to
 * 15 ms, and modifies the configuration of GPIO15 in the meantime.
 */
static void _enumeration_fix(void)
{
    if (((SYSINFO->CHIP_ID & SYSINFO_CHIP_ID_REVISION_Msk) >>
         SYSINFO_CHIP_ID_REVISION_Pos) != E5_AFFECTED_REVISION) {
        /* fixed in RP2040-B2 */
        return;
    }
    if (!(USB->SIE_CTRL & USB_SIE_CTRL_PULLUP_EN_Msk)) {
        /* not attached to the host */
        return;
    }

    DEBUG("[rpx0xx usb] applying workaround for RP2040-E5\n");

    /* wait for the end of the bus reset */
    while (_line_state() == LINE_STATE_SE0) {}

    uint32_t io_prev = *gpio_io_register(E5_DP_PIN);
    uint32_t pad_prev = *gpio_pad_register(E5_DP_PIN);

    /* enable the bus keeper and tristate the pin so that the USB muxing does
     * not affect the pin state */
    atomic_set(gpio_pad_register(E5_DP_PIN),
                      PADS_BANK0_GPIO0_PUE_Msk | PADS_BANK0_GPIO0_PDE_Msk);
    io_reg_write_dont_corrupt(gpio_io_register(E5_DP_PIN),
                              OUTPUT_ENABLE_OVERRIDE_DISABLE << IO_BANK0_GPIO0_CTRL_OEOVER_Pos,
                              IO_BANK0_GPIO0_CTRL_OEOVER_Msk);
    io_reg_write_dont_corrupt(gpio_io_register(E5_DP_PIN),
                              E5_FUNCSEL_USB_DEBUG << IO_BANK0_GPIO0_CTRL_FUNCSEL_Pos,
                              IO_BANK0_GPIO0_CTRL_FUNCSEL_Msk);
    /* J state is DP = 1 and DM = 0 for a full speed device, DM is already
     * gated as long as GPIO16 is not set to the USB debug function */
    io_reg_write_dont_corrupt(gpio_io_register(E5_DP_PIN),
                              INPUT_OVERRIDE_HIGH << IO_BANK0_GPIO0_CTRL_INOVER_Pos,
                              IO_BANK0_GPIO0_CTRL_INOVER_Msk);

    /* keep the DP pull-up enabled while the PHY is not used */
    atomic_set(&USB->USBPHY_DIRECT,
                      USB_USBPHY_DIRECT_DP_PULLUP_EN_Msk);
    atomic_set(&USB->USBPHY_DIRECT_OVERRIDE,
                      USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_Msk);

    /* switch to the GPIO PHY with J state forced */
    USB->USB_MUXING = USB_USB_MUXING_TO_DIGITAL_PAD_Msk |
                               USB_USB_MUXING_SOFTCON_Msk;
    assert(_line_state() == LINE_STATE_J);

    busy_wait_us(E5_FORCE_J_US);
    while (!(USB->SIE_STATUS & USB_SIE_STATUS_CONNECTED_Msk)) {}

    /* switch back to the USB PHY and restore GPIO15 */
    USB->USB_MUXING = USB_USB_MUXING_TO_PHY_Msk |
                               USB_USB_MUXING_SOFTCON_Msk;
    atomic_clear(&USB->USBPHY_DIRECT_OVERRIDE,
                        USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_Msk);
    *gpio_io_register(E5_DP_PIN) = io_prev;
    *gpio_pad_register(E5_DP_PIN) = pad_prev;
}
#else
static inline void _enumeration_fix(void)
{
    /* RP2040-E5 does not affect the RP2350 */
}
#endif

static void _esr(usbdev_t *dev)
{
    unsigned state = irq_disable();
    uint32_t events = _usbdev.events;
    _usbdev.events = 0;
    irq_restore(state);

    if (events & USB_INTS_BUS_RESET_BITS) {
        DEBUG("[rpx0xx usb] bus reset\n");
        _bus_reset();
        _enumeration_fix();
        dev->cb(dev, USBDEV_EVENT_RESET);
    }
    if (events & USB_INTS_DEV_SUSPEND_BITS) {
        dev->cb(dev, USBDEV_EVENT_SUSPEND);
    }
    if (events & USB_INTS_DEV_RESUME_FROM_HOST_BITS) {
        dev->cb(dev, USBDEV_EVENT_RESUME);
    }
}

static void _setup_complete(void)
{
    _rpx0xx_ep_t *ep0_in = _get_ep(0, USB_EP_DIR_IN);
    _rpx0xx_ep_t *ep0_out = _get_ep(0, USB_EP_DIR_OUT);

    /* A setup packet aborts any ongoing control transfer, the data stage
     * of the new control transfer starts with DATA1 */
    _buf_disarm(ep0_in);
    _buf_disarm(ep0_out);
    ep0_in->next_pid = 1;
    ep0_out->next_pid = 1;
    ep0_in->pending = false;

    if (ep0_out->buf) {
        _dpram_read(ep0_out->buf, (uint8_t *)USB_DPRAM, SETUP_PACKET_LEN);
    }
    ep0_out->rx_len = SETUP_PACKET_LEN;
    _usbdev.dev.epcb(&ep0_out->ep, USBDEV_EVENT_TR_COMPLETE);
}

static void _ep_esr(usbdev_ep_t *ep)
{
    _rpx0xx_ep_t *hw_ep = _hw_ep(ep);

    unsigned state = irq_disable();
    bool pending = hw_ep->pending;
    hw_ep->pending = false;
    irq_restore(state);

    if (pending) {
        hw_ep->next_pid = !hw_ep->next_pid;
        if (ep->dir == USB_EP_DIR_OUT) {
            size_t len = *_buf_ctrl(ep) & BUF_CTRL_LENGTH;
            if (hw_ep->buf) {
                _dpram_read(hw_ep->buf, hw_ep->dpram_buf, len);
            }
            hw_ep->rx_len = len;
        }
        _usbdev.dev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
    }

    /* the setup packet has to be handled after a pending transfer on EP0,
     * as it was received after that transfer had been completed */
    if ((ep->num == 0) && (ep->dir == USB_EP_DIR_OUT)) {
        state = irq_disable();
        bool setup = _usbdev.setup_pending;
        _usbdev.setup_pending = false;
        irq_restore(state);

        if (setup) {
            _setup_complete();
        }
    }
}

void isr_usbctrl(void)
{
    uint32_t ints = USB->INTS;

    if (ints & USB_INTS_SETUP_REQ_BITS) {
        /* SIE_STATUS bits are write 1 to clear */
        USB->SIE_STATUS = USB_SIE_STATUS_SETUP_REC_BITS;
        _usbdev.setup_pending = true;
        _usbdev.dev.epcb(&_get_ep(0, USB_EP_DIR_OUT)->ep, USBDEV_EVENT_ESR);
    }

    if (ints & USB_INTS_BUFF_STATUS_BITS) {
        uint32_t status = USB->BUFF_STATUS;
        /* BUFF_STATUS bits are write 1 to clear */
        USB->BUFF_STATUS = status;
        while (status) {
            uint8_t idx = 0;
            status = bitarithm_test_and_clear(status, &idx);
            _usbdev.eps[idx].pending = true;
            _usbdev.dev.epcb(&_usbdev.eps[idx].ep, USBDEV_EVENT_ESR);
        }
    }

    if (ints & DEV_IRQS) {
        uint32_t clear = 0;
        if (ints & USB_INTS_BUS_RESET_BITS) {
            clear |= USB_SIE_STATUS_BUS_RESET_BITS;
            USB->ADDR_ENDP = 0;
        }
        if (ints & USB_INTS_DEV_SUSPEND_BITS) {
            clear |= USB_SIE_STATUS_SUSPENDED_BITS;
        }
        if (ints & USB_INTS_DEV_RESUME_FROM_HOST_BITS) {
            clear |= USB_SIE_STATUS_RESUME_BITS;
        }
        USB->SIE_STATUS = clear;
        _usbdev.events |= ints & DEV_IRQS;
        _usbdev.dev.cb(&_usbdev.dev, USBDEV_EVENT_ESR);
    }

    cortexm_isr_end();
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
    .xmit = _xmit,
};
