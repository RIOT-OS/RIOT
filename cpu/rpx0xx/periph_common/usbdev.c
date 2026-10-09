/*
 * SPDX-FileCopyrightText: 2024 Frank Engelhardt
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Low level USB device driver for the RP2040
 *
 * The USB controller of the RP2040 uses a 4 KiB dual port RAM (DPRAM) which
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
#include <string.h>

#include "bitarithm.h"
#include "busy_wait.h"
#include "container.h"
#include "cpu.h"
#include "irq.h"
#include "periph/usbdev.h"
#include "periph_cpu.h"

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

#define BUF_CTRL_FULL           USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_Msk
#define BUF_CTRL_PID            USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_PID_0_Msk
#define BUF_CTRL_STALL          USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk
#define BUF_CTRL_AVAILABLE      USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_Msk
#define BUF_CTRL_LENGTH         USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_LENGTH_0_Msk

/**
 * @brief   Interrupts handled on device level
 */
#define DEV_IRQS                (USBCTRL_REGS_INTS_BUS_RESET_Msk | \
                                 USBCTRL_REGS_INTS_DEV_SUSPEND_Msk | \
                                 USBCTRL_REGS_INTS_DEV_RESUME_FROM_HOST_Msk)

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
    return &USBCTRL_DPRAM->EP1_IN_CONTROL + _ep_idx(ep->num - 1, ep->dir);
}

/* EPx_IN_BUFFER_CONTROL / EPx_OUT_BUFFER_CONTROL */
static inline volatile uint32_t *_buf_ctrl(const usbdev_ep_t *ep)
{
    return &USBCTRL_DPRAM->EP0_IN_BUFFER_CONTROL + _ep_idx(ep->num, ep->dir);
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

    periph_reset(RESETS_RESET_usbctrl_Msk);
    periph_reset_done(RESETS_RESET_usbctrl_Msk);

    memset((void *)USBCTRL_DPRAM, 0, DPRAM_SIZE);

    /* connect the controller to the internal PHY */
    USBCTRL_REGS->USB_MUXING = USBCTRL_REGS_USB_MUXING_TO_PHY_Msk |
                               USBCTRL_REGS_USB_MUXING_SOFTCON_Msk;
    /* VBUS is not connected to the controller, pretend it is always there */
    USBCTRL_REGS->USB_PWR = USBCTRL_REGS_USB_PWR_VBUS_DETECT_Msk |
                            USBCTRL_REGS_USB_PWR_VBUS_DETECT_OVERRIDE_EN_Msk;
    USBCTRL_REGS->MAIN_CTRL = USBCTRL_REGS_MAIN_CTRL_CONTROLLER_EN_Msk;
    /* generate a buffer status interrupt for every EP0 buffer */
    USBCTRL_REGS->SIE_CTRL = USBCTRL_REGS_SIE_CTRL_EP0_INT_1BUF_Msk;
    USBCTRL_REGS->INTE = USBCTRL_REGS_INTE_BUFF_STATUS_Msk |
                         USBCTRL_REGS_INTE_SETUP_REQ_Msk |
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
        USBCTRL_REGS->ADDR_ENDP = *(const uint8_t *)value &
                                  USBCTRL_REGS_ADDR_ENDP_ADDRESS_Msk;
        return sizeof(uint8_t);
    case USBOPT_ATTACH:
        assert(value_len == sizeof(usbopt_enable_t));
        if (*(const usbopt_enable_t *)value) {
            io_reg_atomic_set(&USBCTRL_REGS->SIE_CTRL,
                              USBCTRL_REGS_SIE_CTRL_PULLUP_EN_Msk);
        }
        else {
            io_reg_atomic_clear(&USBCTRL_REGS->SIE_CTRL,
                                USBCTRL_REGS_SIE_CTRL_PULLUP_EN_Msk);
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

    hw_ep->dpram_buf = (uint8_t *)USBCTRL_DPRAM + hw_ep->dpram_offset;
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
    io_reg_atomic_set(&USBCTRL_REGS->EP_STALL_ARM,
                      USBCTRL_REGS_EP_STALL_ARM_EP0_IN_Msk |
                      USBCTRL_REGS_EP_STALL_ARM_EP0_OUT_Msk);
    *_buf_ctrl(&_get_ep(0, USB_EP_DIR_IN)->ep) = BUF_CTRL_STALL;
    *_buf_ctrl(&_get_ep(0, USB_EP_DIR_OUT)->ep) = BUF_CTRL_STALL;
}

static void _ep_stall(usbdev_ep_t *ep, bool enable)
{
    _rpx0xx_ep_t *hw_ep = _hw_ep(ep);

    if (ep->num == 0) {
        uint32_t mask = (ep->dir == USB_EP_DIR_IN)
                      ? USBCTRL_REGS_EP_STALL_ARM_EP0_IN_Msk
                      : USBCTRL_REGS_EP_STALL_ARM_EP0_OUT_Msk;
        if (enable) {
            io_reg_atomic_set(&USBCTRL_REGS->EP_STALL_ARM, mask);
        }
        else {
            io_reg_atomic_clear(&USBCTRL_REGS->EP_STALL_ARM, mask);
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
            [USB_EP_TYPE_CONTROL] = USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Control,
            [USB_EP_TYPE_INTERRUPT] = USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Interrupt,
            [USB_EP_TYPE_BULK] = USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Bulk,
            [USB_EP_TYPE_ISOCHRONOUS] = USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Isochronous,
        };
        *_ep_ctrl(ep) = USBCTRL_DPRAM_EP1_IN_CONTROL_ENABLE_Msk |
                        USBCTRL_DPRAM_EP1_IN_CONTROL_INTERRUPT_PER_BUFF_Msk |
                        (ep_types[ep->type] << USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Pos) |
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
        memcpy(hw_ep->dpram_buf, buf, len);
    }
    _buf_arm(hw_ep, len);

    return 0;
}

static void _bus_reset(void)
{
    /* disarm all buffers and disable all endpoints except EP0, they are
     * enabled again by USBUS once the device is configured */
    for (uint8_t idx = 0; idx < ARRAY_SIZE(_usbdev.eps); idx++) {
        (&USBCTRL_DPRAM->EP0_IN_BUFFER_CONTROL)[idx] = 0;
        if (idx >= 2) {
            (&USBCTRL_DPRAM->EP1_IN_CONTROL)[idx - 2] = 0;
        }
        _usbdev.eps[idx].pending = false;
    }
    _usbdev.setup_pending = false;
}

static void _esr(usbdev_t *dev)
{
    unsigned state = irq_disable();
    uint32_t events = _usbdev.events;
    _usbdev.events = 0;
    irq_restore(state);

    if (events & USBCTRL_REGS_INTS_BUS_RESET_Msk) {
        DEBUG("[rpx0xx usb] bus reset\n");
        _bus_reset();
        dev->cb(dev, USBDEV_EVENT_RESET);
    }
    if (events & USBCTRL_REGS_INTS_DEV_SUSPEND_Msk) {
        dev->cb(dev, USBDEV_EVENT_SUSPEND);
    }
    if (events & USBCTRL_REGS_INTS_DEV_RESUME_FROM_HOST_Msk) {
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
        memcpy(ep0_out->buf, (void *)USBCTRL_DPRAM, SETUP_PACKET_LEN);
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
                memcpy(hw_ep->buf, hw_ep->dpram_buf, len);
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
    uint32_t ints = USBCTRL_REGS->INTS;

    if (ints & USBCTRL_REGS_INTS_SETUP_REQ_Msk) {
        /* SIE_STATUS bits are write 1 to clear */
        USBCTRL_REGS->SIE_STATUS = USBCTRL_REGS_SIE_STATUS_SETUP_REC_Msk;
        _usbdev.setup_pending = true;
        _usbdev.dev.epcb(&_get_ep(0, USB_EP_DIR_OUT)->ep, USBDEV_EVENT_ESR);
    }

    if (ints & USBCTRL_REGS_INTS_BUFF_STATUS_Msk) {
        uint32_t status = USBCTRL_REGS->BUFF_STATUS;
        /* BUFF_STATUS bits are write 1 to clear */
        USBCTRL_REGS->BUFF_STATUS = status;
        while (status) {
            uint8_t idx;
            status = bitarithm_test_and_clear(status, &idx);
            _usbdev.eps[idx].pending = true;
            _usbdev.dev.epcb(&_usbdev.eps[idx].ep, USBDEV_EVENT_ESR);
        }
    }

    if (ints & DEV_IRQS) {
        uint32_t clear = 0;
        if (ints & USBCTRL_REGS_INTS_BUS_RESET_Msk) {
            clear |= USBCTRL_REGS_SIE_STATUS_BUS_RESET_Msk;
            USBCTRL_REGS->ADDR_ENDP = 0;
        }
        if (ints & USBCTRL_REGS_INTS_DEV_SUSPEND_Msk) {
            clear |= USBCTRL_REGS_SIE_STATUS_SUSPENDED_Msk;
        }
        if (ints & USBCTRL_REGS_INTS_DEV_RESUME_FROM_HOST_Msk) {
            clear |= USBCTRL_REGS_SIE_STATUS_RESUME_Msk;
        }
        USBCTRL_REGS->SIE_STATUS = clear;
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
