/*
 * Copyright (C) 2024 Frank engelhardt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_rpx0xx
 * @{
 * @file
 * @brief   Low level USB interface for rpx0xx cpu family
 *
 * @author  Frank engelhardt <frank@f9e.de>
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "architecture.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/usbdev.h"
#include "bitarithm.h"
#include "panic.h"

#include "cpu_conf.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define USB_BUFFER_SPACE_START ((uint8_t*) USBCTRL_DPRAM + 0x180u)
#define USB_BUFFER_SPACE_END   ((uint8_t*) USBCTRL_DPRAM + 0x1000u)

typedef struct rpx0xx_usb_ep {
    uint8_t *dpram_buf;
    size_t dpram_bufsize;
    usbdev_ep_t ep;
} rpx0xx_usb_ep_t;

typedef struct rpx0xx_usb_dev {
    /* Information about each allocated endpoint. */
    rpx0xx_usb_ep_t hw_ep[2 * USBDEV_NUM_ENDPOINTS];
    usbdev_t dev;
    bool suspended;
    bool connected;
} rpx0xx_usb_dev_t;

/* Forward declaration for the usb device driver. */
const usbdev_driver_t driver;

/* There is only one usb device. */
static rpx0xx_usb_dev_t _hw_usb_dev;

static uint8_t *_dpram_next_buf_ptr = (uint8_t*) USB_BUFFER_SPACE_START;

static inline __IOM uint32_t * _get_ep_out_ctrl_reg(uint8_t ep_num)
{
    assert(ep_num >= 1 && ep_num < 16);
    __IOM uint32_t * reg_map[] = {
        &USBCTRL_DPRAM->EP1_OUT_CONTROL,
        &USBCTRL_DPRAM->EP2_OUT_CONTROL,
        &USBCTRL_DPRAM->EP3_OUT_CONTROL,
        &USBCTRL_DPRAM->EP4_OUT_CONTROL,
        &USBCTRL_DPRAM->EP5_OUT_CONTROL,
        &USBCTRL_DPRAM->EP6_OUT_CONTROL,
        &USBCTRL_DPRAM->EP7_OUT_CONTROL,
        &USBCTRL_DPRAM->EP8_OUT_CONTROL,
        &USBCTRL_DPRAM->EP9_OUT_CONTROL,
        &USBCTRL_DPRAM->EP10_OUT_CONTROL,
        &USBCTRL_DPRAM->EP11_OUT_CONTROL,
        &USBCTRL_DPRAM->EP12_OUT_CONTROL,
        &USBCTRL_DPRAM->EP13_OUT_CONTROL,
        &USBCTRL_DPRAM->EP14_OUT_CONTROL,
        &USBCTRL_DPRAM->EP15_OUT_CONTROL,
    };
    return reg_map[ep_num-1];
}

static inline __IOM uint32_t * _get_ep_in_ctrl_reg(uint8_t ep_num)
{
    assert(ep_num >= 1 && ep_num < 16);
    __IOM uint32_t * reg_map[] = {
        &USBCTRL_DPRAM->EP1_IN_CONTROL,
        &USBCTRL_DPRAM->EP2_IN_CONTROL,
        &USBCTRL_DPRAM->EP3_IN_CONTROL,
        &USBCTRL_DPRAM->EP4_IN_CONTROL,
        &USBCTRL_DPRAM->EP5_IN_CONTROL,
        &USBCTRL_DPRAM->EP6_IN_CONTROL,
        &USBCTRL_DPRAM->EP7_IN_CONTROL,
        &USBCTRL_DPRAM->EP8_IN_CONTROL,
        &USBCTRL_DPRAM->EP9_IN_CONTROL,
        &USBCTRL_DPRAM->EP10_IN_CONTROL,
        &USBCTRL_DPRAM->EP11_IN_CONTROL,
        &USBCTRL_DPRAM->EP12_IN_CONTROL,
        &USBCTRL_DPRAM->EP13_IN_CONTROL,
        &USBCTRL_DPRAM->EP14_IN_CONTROL,
        &USBCTRL_DPRAM->EP15_IN_CONTROL,
    };
    return reg_map[ep_num-1];
}

static inline __IOM uint32_t * _get_ep_out_buf_ctrl_reg(uint8_t ep_num)
{
    assert(ep_num < 16);
    __IOM uint32_t * reg_map[] = {
        &USBCTRL_DPRAM->EP0_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP1_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP2_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP3_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP4_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP5_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP6_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP7_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP8_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP9_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP10_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP11_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP12_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP13_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP14_OUT_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP15_OUT_BUFFER_CONTROL,
    };
    return reg_map[ep_num];
}

static inline __IOM uint32_t * _get_ep_in_buf_ctrl_reg(uint8_t ep_num)
{
    assert(ep_num < 16);
    __IOM uint32_t * reg_map[] = {
        &USBCTRL_DPRAM->EP0_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP1_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP2_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP3_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP4_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP5_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP6_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP7_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP8_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP9_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP10_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP11_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP12_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP13_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP14_IN_BUFFER_CONTROL,
        &USBCTRL_DPRAM->EP15_IN_BUFFER_CONTROL,
    };
    return reg_map[ep_num];
}

/* Read the ep-related bit from a status register. */
static inline bool _reg_get_flag_for_ep(__IOM uint32_t *reg, usbdev_ep_t *ep)
{
    uint32_t val = *reg;
    return val & 1 << (2 * ep->num + (ep->dir == USB_EP_DIR_IN ? 0 : 1));
}

/* Clear the ep-related bit from a status register. */
static inline void _reg_clear_flag_for_ep(__IOM uint32_t *reg, usbdev_ep_t *ep)
{
    io_reg_atomic_clear(reg, 
                        1 << (2 * ep->num + (ep->dir == USB_EP_DIR_IN ? 0 : 1)));
}

static inline __IOM uint32_t * _get_ep_buf_ctrl_reg(uint8_t ep_num,
                                                    usb_ep_dir_t dir)
{
    if (dir == USB_EP_DIR_IN) {
        return _get_ep_in_buf_ctrl_reg(ep_num);
    } else {
        return _get_ep_out_buf_ctrl_reg(ep_num);
    }
}

static inline __IOM uint32_t * _get_ep_ctrl_reg(uint8_t ep_num,
                                                usb_ep_dir_t dir)
{
    if (dir == USB_EP_DIR_IN) {
        return _get_ep_in_ctrl_reg(ep_num);
    } else {
        return _get_ep_out_ctrl_reg(ep_num);
    }
}

/** See @io_reg_write_dont_corrupt() */
static inline void _ep_ctrl_reg_write(uint8_t ep_num, usb_ep_dir_t dir,
                                      uint32_t value, uint32_t mask)
{
    __IOM uint32_t *reg = _get_ep_ctrl_reg(ep_num, dir);
    io_reg_write_dont_corrupt(reg, value, mask);
}

/** See @io_reg_write_dont_corrupt() */
static inline void _ep_buf_ctrl_reg_write(uint8_t ep_num, usb_ep_dir_t dir, 
                                          uint32_t value, uint32_t mask)
{
    __IOM uint32_t *reg = _get_ep_buf_ctrl_reg(ep_num, dir);
    io_reg_write_dont_corrupt(reg, value, mask);
}

/* Get DPRAM buffer offset. */
static inline uintptr_t _dpram_offset(uint8_t *buf)
{
    return (uintptr_t) buf ^ (uintptr_t) USBCTRL_DPRAM_BASE;
}

static rpx0xx_usb_ep_t *_get_ep(uint8_t ep_num, usb_ep_dir_t dir)
{
    return &_hw_usb_dev.hw_ep[2 * ep_num + (dir == USB_EP_DIR_OUT ? 0 : 1)];
}

static void _poweron(void)
{
    periph_reset(RESETS_RESET_usbctrl_Msk);
    periph_reset_done(RESETS_RESET_usbctrl_Msk);
    io_reg_atomic_set(&USBCTRL_REGS->MAIN_CTRL,
                      USBCTRL_REGS_MAIN_CTRL_CONTROLLER_EN_Msk);
    io_reg_atomic_set(&USBCTRL_REGS->USB_MUXING,
                      USBCTRL_REGS_USB_MUXING_TO_PHY_Msk);
}

/*
static void _poweroff(void)
{
    io_reg_atomic_set(&USBCTRL_REGS->MAIN_CTRL, 0);
    periph_reset(RESETS_RESET_usbctrl_Msk);
}
*/

void usbdev_init_lowlevel(void)
{
    DEBUG("[rpx0xx usb] Call usbdev_init_lowlevel()\n");
    memset(&_hw_usb_dev, 0, sizeof(rpx0xx_usb_dev_t));
    _hw_usb_dev.dev.driver = &driver;
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    (void)num;
    return &_hw_usb_dev.dev;
}

static void _enable_irq(void) {
    io_reg_atomic_set(&USBCTRL_REGS->INTE,
                      USBCTRL_REGS_INTE_BUS_RESET_Msk |
                      USBCTRL_REGS_INTE_DEV_SUSPEND_Msk |
                      USBCTRL_REGS_INTE_DEV_CONN_DIS_Msk);
    io_reg_atomic_set(&USBCTRL_REGS->SIE_CTRL,
                      USBCTRL_REGS_SIE_CTRL_EP0_INT_1BUF_Msk);
}

static void _disable_irq(void) {
    io_reg_atomic_clear(&USBCTRL_REGS->INTE,
                        USBCTRL_REGS_INTE_BUS_RESET_Msk |
                        USBCTRL_REGS_INTE_DEV_SUSPEND_Msk |
                        USBCTRL_REGS_INTE_DEV_CONN_DIS_Msk);
    io_reg_atomic_clear(&USBCTRL_REGS->SIE_CTRL,
                        USBCTRL_REGS_SIE_CTRL_EP0_INT_1BUF_Msk);
}

static void _usbdev_init(usbdev_t *dev)
{
    (void)dev;
    DEBUG("[rpx0xx usb] Call _usbdev_init()\n");

    memset(USBCTRL_DPRAM, 0, sizeof(USBCTRL_DPRAM_Type));
    memset(USBCTRL_REGS, 0, sizeof(USBCTRL_REGS_Type));

    _poweron();
    
    _enable_irq();
}

static int _usbdev_get(usbdev_t *dev, usbopt_t opt,
                       void *value, size_t max_len)
{
    (void)dev;
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
        DEBUG("[rpx0xx usb] Unhandled get call: 0x%x\n", opt);
        break;
    }
    return res;
}

static void _set_address(uint8_t addr)
{
    DEBUG("[rpx0xx usb] Call _set_address(addr = %hhx)\n", addr);
    io_reg_atomic_set(&USBCTRL_REGS->ADDR_ENDP,
                      addr & USBCTRL_REGS_ADDR_ENDP_ADDRESS_Msk);
}

static void _usb_attach(void)
{
    DEBUG("[rpx0xx usb] Attaching to host\n");
    io_reg_atomic_set(&USBCTRL_REGS->SIE_CTRL,
                      USBCTRL_REGS_SIE_CTRL_PULLUP_EN_Msk);
}

static void _usb_detach(void)
{
    DEBUG("[rpx0xx usb] Detaching from host\n");
    io_reg_atomic_clear(&USBCTRL_REGS->SIE_CTRL,
                        USBCTRL_REGS_SIE_CTRL_PULLUP_EN_Msk);
}

static int _usbdev_set(usbdev_t *dev, usbopt_t opt,
                       const void *value, size_t value_len)
{
    (void)dev;
    (void)opt;
    (void)value_len;

    int res = -ENOTSUP;

    switch (opt) {
    case USBOPT_ADDRESS:
        assert(value_len == sizeof(uint8_t));
        uint8_t addr = (*((uint8_t *)value));
        _set_address(addr);
        break;
    case USBOPT_ATTACH:
        assert(value_len == sizeof(usbopt_enable_t));
        if (*((usbopt_enable_t *)value)) {
            _usb_attach();
        }
        else {
            _usb_detach();
        }
        res = sizeof(usbopt_enable_t);
        break;
    default:
        DEBUG("[rpx0xx usb] Unhandled set call: 0x%x\n", opt);
        break;
    }

    return res;
}

/* This mechanic of DPRAM allocation is taken from the TinyUSB reference implementation. */
static void _ep_dpram_alloc(rpx0xx_usb_ep_t *ep, size_t size)
{
    uint32_t transfer_type = 0;
    
    /* Buffer length must be multiple of 64 bytes. */
    assert(size % 64 == 0);

    if (ep->ep.num != 0) {
        assert(ep->ep.type != USB_EP_TYPE_CONTROL);

        ep->dpram_buf = _dpram_next_buf_ptr;
        _dpram_next_buf_ptr += size;

        assert(((uintptr_t) _dpram_next_buf_ptr & 0b111111u) == 0);
        size_t dpram_offset = _dpram_offset(ep->dpram_buf);

        if (_dpram_next_buf_ptr > USB_BUFFER_SPACE_END) {
            core_panic(PANIC_MEM_MANAGE, "[rpx0xx usb] available DPRAM space exceeded");
        }

        DEBUG("[rpx0xx usb] Allocated %d bytes at offset 0x%x (0x%p)\n",
            size, dpram_offset, ep->dpram_buf);

        switch (ep->ep.type) {
        case USB_EP_TYPE_INTERRUPT:
            transfer_type = USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Interrupt;
            break;
        case USB_EP_TYPE_BULK:
            transfer_type = USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Bulk;
            break;
        case USB_EP_TYPE_ISOCHRONOUS:
            transfer_type = USBCTRL_DPRAM_EP1_IN_CONTROL_ENDPOINT_TYPE_Isochronous;
            break;
        default:
            core_panic(PANIC_GENERAL_ERROR, "[rpx0xx usb] illegal endpoint type");
        }

        const uint32_t reg = transfer_type | dpram_offset;
        
        _ep_buf_ctrl_reg_write(ep->ep.num, ep->ep.dir, reg, ~0UL);
    } else {
        assert(ep->ep.type = USB_EP_TYPE_CONTROL);
        assert(size == 64);

        ep->dpram_buf = (uint8_t *)(USBCTRL_DPRAM_BASE + 0x100UL);
    }
    ep->dpram_bufsize = size;
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t size)
{
    rpx0xx_usb_ep_t *hw_ep = NULL;

    DEBUG("[rpx0xx usb] Call _usbdev_new_ep(dev = ..., type = %d," 
          "dir = %d, size = %u)\n", type, dir, size);

    /* Always return endpoint 0 for control types */
    if (type == USB_EP_TYPE_CONTROL) {
        hw_ep = _get_ep(0, dir);
        hw_ep->ep.num = 0;
    }
    else {
        /* Find the first unassigned ep with proper dir */
        for (unsigned i = 1; i < USBDEV_NUM_ENDPOINTS; i++) {
            hw_ep = _get_ep(i, dir);
            if (hw_ep->ep.type == USB_EP_TYPE_NONE) {
                hw_ep->ep.num = i;
                break;
            }
        }
    }
    if (hw_ep) {
        hw_ep->ep.dev = dev;
        hw_ep->ep.dir = dir;
        hw_ep->ep.type = type;
        hw_ep->ep.len = size;
    }

    return &hw_ep->ep;
}

static void _enable_ep_irq(usbdev_ep_t *ep) {
    if (ep->num != 0) {
        uint32_t flags = USBCTRL_DPRAM_EP1_IN_CONTROL_INTERRUPT_PER_BUFF_Msk
                         | USBCTRL_DPRAM_EP1_IN_CONTROL_INTERRUPT_ON_STALL_Msk;
        _ep_ctrl_reg_write(ep->num, ep->dir, flags, flags);
    } else {
        uint32_t flags = USBCTRL_REGS_SIE_CTRL_EP0_INT_1BUF_Msk
                         | USBCTRL_REGS_SIE_CTRL_EP0_INT_STALL_Msk;
        io_reg_atomic_set(&USBCTRL_REGS->SIE_CTRL, flags);
    }
}

static void _disable_ep_irq(usbdev_ep_t *ep) {
    if (ep->num != 0) {
        uint32_t flags = USBCTRL_DPRAM_EP1_IN_CONTROL_INTERRUPT_PER_BUFF_Msk
                         | USBCTRL_DPRAM_EP1_IN_CONTROL_INTERRUPT_ON_STALL_Msk;
        _ep_ctrl_reg_write(ep->num, ep->dir, 0, flags);
    } else {
        uint32_t flags = USBCTRL_REGS_SIE_CTRL_EP0_INT_1BUF_Msk
                         | USBCTRL_REGS_SIE_CTRL_EP0_INT_STALL_Msk;
        io_reg_atomic_clear(&USBCTRL_REGS->SIE_CTRL, flags);
    }
}

static void _usbdev_ep_init(usbdev_ep_t *ep)
{
    rpx0xx_usb_ep_t *hw_ep = _get_ep(ep->num, ep->dir);

    DEBUG("[rpx0xx usb] Call _usbdev_ep_init(ep.num = %d," 
          "ep.dir = %d)\n", ep->num, ep->dir);

    _ep_dpram_alloc(hw_ep, ep->len);

    _enable_ep_irq(ep);
}

static void _ep_enable(usbdev_ep_t *ep) {
    if (ep->num != 0) {
        _ep_ctrl_reg_write(ep->num, ep->dir,
                           USBCTRL_DPRAM_EP1_IN_CONTROL_ENABLE_Msk,
                           USBCTRL_DPRAM_EP1_IN_CONTROL_ENABLE_Msk);
    }
}

static void _ep_disable(usbdev_ep_t *ep) {
    if (ep->num != 0) {
        _ep_ctrl_reg_write(ep->num, ep->dir, 0,
                           USBCTRL_DPRAM_EP1_IN_CONTROL_ENABLE_Msk);
    }
}

static void _ep_set_stall(usbdev_ep_t *ep, usbopt_enable_t enable)
{
    (void)ep;
    (void)enable;
    if (ep->num == 0) {
        uint32_t bit = (ep->dir == USB_EP_DIR_IN) ?
                        USBCTRL_REGS_EP_STALL_ARM_EP0_IN_Msk 
                        : USBCTRL_REGS_EP_STALL_ARM_EP0_OUT_Msk;
        if (enable) {
            io_reg_atomic_set(&USBCTRL_REGS->EP_STALL_ARM, bit);
        } else {
            io_reg_atomic_clear(&USBCTRL_REGS->EP_STALL_ARM, bit);
        }
    }
    uint32_t bit = (enable) ? USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk : 0;
    _ep_buf_ctrl_reg_write(ep->num, ep->dir, bit,
                           USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk);
}

static void _usbdev_ep0_stall(usbdev_t *dev)
{
    (void)dev;
    _ep_set_stall(&_get_ep(0, USB_EP_DIR_IN)->ep, USBOPT_ENABLE);
    _ep_set_stall(&_get_ep(0, USB_EP_DIR_OUT)->ep, USBOPT_ENABLE);
}

usbopt_enable_t _ep_get_stall(usbdev_ep_t *ep)
{
    __IOM uint32_t reg = *_get_ep_buf_ctrl_reg(ep->num, ep->dir);    
    return reg | USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_STALL_Msk;
}

static void _usbdev_ep_stall(usbdev_ep_t *ep, bool enable)
{
    _ep_set_stall(ep, enable);
}

/* available: is the processor in control of the buffer? */
static bool _ep_buf_is_available(usbdev_ep_t *ep)
{
    __IOM uint32_t buf_ctrl_reg = *_get_ep_buf_ctrl_reg(ep->num, ep->dir);
    return !(buf_ctrl_reg & USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_Msk);
}

static bool _ep_buf_is_full(usbdev_ep_t *ep)
{
    __IOM uint32_t buf_ctrl_reg = *_get_ep_buf_ctrl_reg(ep->num, ep->dir);
    return buf_ctrl_reg & USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_Msk;
}


static size_t _ep_get_available(usbdev_ep_t *ep)
{
    if (_ep_buf_is_available(ep)) {
        if (ep->dir == USB_EP_DIR_IN) {
            rpx0xx_usb_ep_t *hw_ep = _get_ep(ep->num, ep->dir);
            return hw_ep->dpram_bufsize;
        } else if(_ep_buf_is_full(ep)) {
            __IOM uint32_t *reg = _get_ep_buf_ctrl_reg(ep->num, ep->dir);
            return *reg | USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_LENGTH_0_Msk;
        }
    }
    return 0UL;
}

static int _usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
                          void *value, size_t max_len)
{
    (void)max_len;
    int ret = -ENOTSUP;

    switch (opt) {
    case USBOPT_EP_STALL:
        assert(max_len == sizeof(usbopt_enable_t));
        *(usbopt_enable_t *)value = _ep_get_stall(ep);
        ret = sizeof(usbopt_enable_t);
        break;
    case USBOPT_EP_AVAILABLE:
        assert(max_len == sizeof(size_t));
        *(size_t *)value = _ep_get_available(ep);
        ret = sizeof(size_t);
        break;
    default:
        DEBUG("[rpx0xx usb]: Unhandled get call: 0x%x\n", opt);
        break;
    }
    return ret;
}

static int _usbdev_ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                          const void *value, size_t value_len)
{
    (void)value_len;
    int ret = -ENOTSUP;

    switch (opt) {
    case USBOPT_EP_ENABLE:
        assert(value_len == sizeof(usbopt_enable_t));
        if (*((usbopt_enable_t *)value)) {
            _ep_enable(ep);
        }
        else {
            _ep_disable(ep);
        }
        ret = sizeof(usbopt_enable_t);
        break;
    case USBOPT_EP_STALL:
        assert(value_len == sizeof(usbopt_enable_t));
        _ep_set_stall(ep, *(usbopt_enable_t *)value);
        ret = sizeof(usbopt_enable_t);
        break;
    default:
        DEBUG("[rpx0xx usb]: Unhandled set call: 0x%x\n", opt);
        break;
    }
    return ret;
}

static void _mark_ep_buf_ready(usbdev_ep_t *ep)
{
    uint32_t buf_filled_bit = 0UL;
    if (ep->dir == USB_EP_DIR_IN) {
        buf_filled_bit = USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_Msk;
    }
    _ep_buf_ctrl_reg_write(ep->num, ep->dir, 
                           USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_FULL_0_Pos, 
                           buf_filled_bit);
    _ep_buf_ctrl_reg_write(ep->num, ep->dir, 
                           USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_Pos, 
                           USBCTRL_DPRAM_EP0_IN_BUFFER_CONTROL_AVAILABLE_0_Msk);
}

static int _usbdev_ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    rpx0xx_usb_ep_t *hw_ep = _get_ep(ep->num, ep->dir);

    assert(_ep_buf_is_available(ep));

    size_t available_len = _ep_get_available(ep);

    if (available_len < len) {
        len = available_len;
    }

    if (ep->dir == USB_EP_DIR_IN) {
        memcpy(hw_ep->dpram_buf, buf, len);
    } else {
        memcpy(buf, hw_ep->dpram_buf, len);
    }
    _ep_set_stall(ep, 0);
    _mark_ep_buf_ready(ep);

    return len;
}

static void _usbdev_esr(usbdev_t *dev)
{
    (void)dev;
    uint32_t status = USBCTRL_REGS->INTS;
    
    if (status & USBCTRL_REGS_INTS_DEV_SOF_Msk) {
        /* Start of frame received */
        uint32_t frame_number = USBCTRL_REGS->SOF_RD;
        /* we have to read the frame number to reset the interrupt,
           but do not need it */
        (void) frame_number;
        _hw_usb_dev.dev.cb(dev, USBDEV_EVENT_SOF);
    }
    if (status & USBCTRL_REGS_INTS_BUS_RESET_Msk) {
        /* Bus reset received */
        _hw_usb_dev.dev.cb(dev, USBDEV_EVENT_RESET);
        io_reg_atomic_clear(&USBCTRL_REGS->SIE_STATUS,
                            USBCTRL_REGS_SIE_STATUS_BUS_RESET_Msk);
    }
    if (status & USBCTRL_REGS_INTS_DEV_SUSPEND_Msk) {
        /* Bus suspend state has changed */
        if (_hw_usb_dev.suspended) {
            _hw_usb_dev.suspended = false;
            _hw_usb_dev.dev.cb(dev, USBDEV_EVENT_RESUME);
            /* TODO: pm_block() for MODULE_PM_LAYERED */
        } else {
            _hw_usb_dev.suspended = true;
            _hw_usb_dev.dev.cb(dev, USBDEV_EVENT_SUSPEND);
            /* TODO: pm_unblock() for MODULE_PM_LAYERED */
        }
        io_reg_atomic_clear(&USBCTRL_REGS->SIE_STATUS,
                            USBCTRL_REGS_SIE_STATUS_SUSPENDED_Msk);
    }
    if (status & USBCTRL_REGS_INTS_DEV_CONN_DIS_Msk) {
        /* Host connected or disconnected */
        if (_hw_usb_dev.connected) {
            _hw_usb_dev.connected = false;
            _hw_usb_dev.dev.cb(dev, USBDEV_EVENT_HOST_DISCONNECT);
        } else {
            _hw_usb_dev.connected = true;
            _hw_usb_dev.dev.cb(dev, USBDEV_EVENT_HOST_CONNECT);
        }
        io_reg_atomic_clear(&USBCTRL_REGS->SIE_STATUS,
                            USBCTRL_REGS_SIE_STATUS_CONNECTED_Msk);
    }
    /* all interrupts should be now cleared */
    status = USBCTRL_REGS->INTS;
    if (status) {
        DEBUG("[rpx0xx usb] Unhandled interrupt, INTS=%lx", status);
    }
    _enable_irq();
}

static void _usbdev_ep_esr(usbdev_ep_t *ep)
{
    if (_reg_get_flag_for_ep(&USBCTRL_REGS->BUFF_STATUS, ep)) {
        _reg_clear_flag_for_ep(&USBCTRL_REGS->BUFF_STATUS, ep);
        ep->dev->epcb(ep, USBDEV_EVENT_TR_COMPLETE);
    }
    if (_reg_get_flag_for_ep(&USBCTRL_REGS->EP_STATUS_STALL_NAK, ep)) {
        _reg_clear_flag_for_ep(&USBCTRL_REGS->EP_STATUS_STALL_NAK, ep);
        ep->dev->epcb(ep, USBDEV_EVENT_TR_STALL);
    }
    _enable_ep_irq(ep);
}

void isr_usbctrl(void)
{
    if (USBCTRL_REGS->BUFF_STATUS) {
        /* Endpoint specific interrupt */
        for (uint8_t i = 0; i < USBDEV_NUM_ENDPOINTS * 2; i++) {
            rpx0xx_usb_ep_t* hw_ep = _hw_usb_dev.hw_ep;
            if (_reg_get_flag_for_ep(&USBCTRL_REGS->BUFF_STATUS, &hw_ep->ep)) {
                _disable_ep_irq(&hw_ep->ep);
                _hw_usb_dev.dev.epcb(&hw_ep->ep, USBDEV_EVENT_ESR);
            }
        }
    }
    if (USBCTRL_REGS->INTS) {
        /* Device specific interrupt */
        _disable_irq();
        _hw_usb_dev.dev.cb(&_hw_usb_dev.dev, USBDEV_EVENT_ESR);
    }
    cortexm_isr_end();
}

const usbdev_driver_t driver = {
    .init = _usbdev_init,
    .new_ep = _usbdev_new_ep,
    .get = _usbdev_get,
    .set = _usbdev_set,
    .esr = _usbdev_esr,
    .ep0_stall = _usbdev_ep0_stall,
    .ep_init = _usbdev_ep_init,
    .ep_stall = _usbdev_ep_stall,
    .ep_get = _usbdev_ep_get,
    .ep_set = _usbdev_ep_set,
    .ep_esr = _usbdev_ep_esr,
    .xmit = _usbdev_ep_xmit,
};
