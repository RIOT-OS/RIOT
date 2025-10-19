/*
 * SPDX-FileCopyrightText: 2021 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup cpu_stm32
 * @{
 * @file
 * @brief   Low level USB interface functions for the stm32 FS devices
 *
 * @author  Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "bitarithm.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "periph/pm.h"
#include "periph/gpio.h"
#include "periph/usbdev.h"
#include "usbdev_stm32.h"
#include "pm_layered.h"
#include "ztimer.h"
#include "busy_wait.h"

#include <string.h>
/**
 * Be careful with enabling debug here. As with all timing critical systems it
 * is able to interfere with USB functionality and you might see different
 * errors than debug disabled
 */
#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef USB1_PMAADDR
#define USB1_PMAADDR USB_PMAADDR
#endif /* ndef USB1_PMAADDR */

/**
 * There are two schemes for accessing the packet buffer area (PMA) from the CPU:
 *
 * - 2 x 16 bit/word access scheme where two 16-bit half-words per word can be
 *   accessed. With this scheme the access can be half-word aligned and the
 *   PMA address offset corresponds therefore to the local USB IP address.
 *   The size of the PMA SRAM is usually 1024 byte.
 * - 1 x 16 bit/word access scheme where one 16-bit half word per word can be
 *   accessed. With this scheme the access can only be word-aligned and the
 *   PMA address offset to a half-word is therefore twice the local USB IP
 *   address. The size of the PMA SRAM is usually 512 byte.
 *
 * Which access scheme is used depends on the STM32 model.
 */
#if defined(CPU_LINE_STM32F303xD) || defined(CPU_LINE_STM32F303xE) || \
    defined(CPU_LINE_STM32WB35xx) || defined(CPU_LINE_STM32WB55xx)

#define _ENDPOINT_NUMOF         (8)     /* Number of endpoints */
#define _PMA_SRAM_SIZE          (1024)  /* Size of Packet buffer Memory Area (PMA) SRAM */
#define _PMA_ACCESS_SCHEME      (2)     /* 2 x 16-bit/word */
#define _PMA_ACCESS_STEP_SIZE   (1)     /* Step size in 16-bit half-words when
                                         * accessing the PMA SRAM from CPU */
#elif defined(CPU_LINE_STM32F103xB) || \
      defined(CPU_LINE_STM32F303xB) || defined(CPU_LINE_STM32F303xC)

#define _ENDPOINT_NUMOF         (8)     /* Number of endpoints */
#define _PMA_SRAM_SIZE          (512)   /* Size of Packet buffer Memory Area (PMA) SRAM */
#define _PMA_ACCESS_SCHEME      (1)     /* 1 x 16-bit/word */
#define _PMA_ACCESS_STEP_SIZE   (2)     /* Step size in 16-bit half-words when
                                         * accessing the PMA SRAM from CPU */
#else
#error "STM32 line is not supported"
#endif

/* USB IP local base address for the endpoints buffers in PMA SRAM */
#define _PMA_BUF_BASE_OFFSET    (_ENDPOINT_NUMOF * 8)

/* List of instantiated USB peripherals */
static stm32_usbdev_fs_t _usbdevs[USBDEV_NUMOF];

static usbdev_ep_t _ep_in[_ENDPOINT_NUMOF];
static usbdev_ep_t _ep_out[_ENDPOINT_NUMOF];

/* 16-bit addresses used by the USB IP as endpoint buffers in the PMA SRAM */
static uint16_t _ep_in_buf[_ENDPOINT_NUMOF];
static uint16_t _ep_out_buf[_ENDPOINT_NUMOF];

/* Buffers used for receiving packets on OUT EPs. */
static uint8_t* _app_pbuf[_ENDPOINT_NUMOF];

/* Forward declaration for the usb device driver */
const usbdev_driver_t driver;

static USB_TypeDef *_global_regs(const stm32_usbdev_fs_config_t *conf)
{
    return (USB_TypeDef *)conf->base_addr;
}

#if _PMA_ACCESS_SCHEME == 1

/* Endpoint Buffer Descriptor for the 1 x 16-bit/word access scheme
 * (word-aligned access). Even though the members define 16-bit USB IP local
 * addresses, they have to be defined as 32-bit unsigned for word-aligned
 * access */
typedef struct {
    uint32_t addr_tx;       /* buffer address (16-bit USB IP local address) */
    uint32_t count_tx;      /* byte count (16-bit) */
    uint32_t addr_rx;       /* buffer address (16-bit USB IP local address) */
    uint32_t count_rx;      /* byte count (16-bit) */
} ep_buf_desc_t;

#elif _PMA_ACCESS_SCHEME == 2

/* Endpoint Buffer Descriptor for the 2 x 16-bits/word access scheme
 * (half-word-aligned access) */
typedef struct {
    uint16_t addr_tx;
    uint16_t count_tx;
    uint16_t addr_rx;
    uint16_t count_rx;
} ep_buf_desc_t;

#else
#error "_PMA_ACCESS_SCHEME is not defined"
#endif

/* Bit mask containing all toggleable bits in a EP_REG(x) register */
#define _EP_REG_TOGGLE_ONLY_BITS_MASK (USB_EP_DTOG_TX | USB_EP_DTOG_RX | \
                                       USB_EPTX_STAT  | USB_EPRX_STAT  )

#define EP_DESC ((ep_buf_desc_t*)USB1_PMAADDR)
#define EP_REG(x) (*((volatile uint32_t*) (((uintptr_t)(USB)) + (4*x))))

usbdev_t *usbdev_get_ctx(unsigned num)
{
    assert(num < USBDEV_NUMOF);
    return &_usbdevs[num].usbdev;
}

static inline void _enable_irq(void)
{
    stm32_usbdev_fs_t *usbdev = &_usbdevs[0];
    const stm32_usbdev_fs_config_t *conf = usbdev->config;
    _global_regs(conf)->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM  |
                               USB_CNTR_ERRM | USB_CNTR_PMAOVRM;
}

static void _setup(stm32_usbdev_fs_t *usbdev,
                      const stm32_usbdev_fs_config_t *config)
{
    usbdev->usbdev.driver = &driver;
    usbdev->config = config;
    usbdev->out = _ep_out;
    usbdev->in = _ep_in;
}

static uint32_t _type_to_reg(usb_ep_type_t type)
{
    switch (type) {
        case USB_EP_TYPE_CONTROL:
            return USB_EP_CONTROL;
        case USB_EP_TYPE_ISOCHRONOUS:
            return USB_EP_ISOCHRONOUS;
        case USB_EP_TYPE_BULK:
            return USB_EP_BULK;
        case USB_EP_TYPE_INTERRUPT:
            return USB_EP_INTERRUPT;
        default:
            assert(false);
            return 0;
    }
}

void usbdev_init_lowlevel(void)
{
    for (size_t i = 0; i < USBDEV_NUMOF; i++) {
        _setup(&_usbdevs[i], &stm32_usbdev_fs_config[i]);
    }
}

/*TODO: rework this function (maybe move it outside this file)
        for other ST families support */
static void _enable_usb_clk(void)
{
#if defined(PWR_CR2_USV)
    /* Validate USB Supply */
    PWR->CR2 |= PWR_CR2_USV;
#endif

#if defined(RCC_APB1SMENR_USBSMEN)
    RCC->APB1SMENR |= RCC_APB1SMENR_USBSMEN;
#elif defined(RCC_APB1SMENR1_USBSMEN)
    RCC->APB1SMENR1 |= RCC_APB1SMENR1_USBSMEN;
#endif

#if defined(CRS_CR_AUTOTRIMEN) && defined(CRS_CR_CEN)
    /* Enable CRS with auto trim enabled */
    CRS->CR |=  (CRS_CR_AUTOTRIMEN |  CRS_CR_CEN);
#endif
}

static void _enable_gpio(const stm32_usbdev_fs_config_t *conf)
{
    if (conf->af == GPIO_AF_UNDEF && conf->disconn == GPIO_UNDEF) {
        /* If the MCU does not have an internal D+ pullup and there is no
         * dedicated GPIO on the board to simulate a USB disconnect, the D+ GPIO
         * is temporarily configured as an output and pushed down to simulate
         * a disconnect/connect cycle to allow the host to recognize the device.
         * This requires an external pullup on D+ signal to work. */
        gpio_init(conf->dp, GPIO_OUT);
        gpio_clear(conf->dp);
        /* wait a 1 ms */
        if (IS_USED(MODULE_ZTIMER_MSEC)) {
            ztimer_sleep(ZTIMER_MSEC, 1);
        }
        else if(IS_USED(MODULE_ZTIMER_USEC)) {
            ztimer_sleep(ZTIMER_USEC, 1 * US_PER_MS);
        }
        else {
            busy_wait_us(1 * US_PER_MS);
        }
        gpio_init(conf->dp, GPIO_IN);
    }
    if (conf->af != GPIO_AF_UNDEF) {
        /* Configure AF for the pins */
        gpio_init_af(conf->dp, conf->af);
        gpio_init_af(conf->dm, conf->af);
    }

    if (conf->disconn != GPIO_UNDEF) {
        /* In case the MCU has no internal D+ pullup, a GPIO is used to
         * connect/disconnect from USB bus */
        gpio_init(conf->disconn, GPIO_OUT);
        gpio_write(conf->disconn, (conf->flags & STM32_USBDEV_FS_CONFIG_FLAG_DISCONN_INVERTED));
    }
}

static void _set_ep_in_status(uint16_t *val, uint16_t mask)
{
    /* status endpoints bits can only be toggled, writing 0
       as no effect. Thus, check which bits should be set
       then XOR it with their current value */
    if (mask & USB_EPTX_DTOG1) {
        *val ^= USB_EPTX_DTOG1;
    }
    if (mask & USB_EPTX_DTOG2) {
        *val ^= USB_EPTX_DTOG2;
    }
}

static void _set_ep_out_status(uint16_t *val, uint16_t mask)
{
    /* status endpoints bits can only be toggled, writing 0
     * as no effect. Thus, check which bits should be set
     * then XOR it with their current value */

    if (mask & USB_EPRX_DTOG1) {
        *val ^= USB_EPRX_DTOG1;
    }
    if (mask & USB_EPRX_DTOG2) {
        *val ^= USB_EPRX_DTOG2;
    }
}

static inline void _usb_attach(stm32_usbdev_fs_t *usbdev)
{
    const stm32_usbdev_fs_config_t *conf = usbdev->config;

    /* Some ST USB IP doesn't have this feature */
#ifdef USB_BCDR_DPPU
    /* Enable DP pullup to signal connection */
    _global_regs(conf)->BCDR |= USB_BCDR_DPPU;
    while (!(CRS->ISR & CRS_ISR_ESYNCF)) {}
#else
    /* If configuration uses a GPIO for USB connect/disconnect */
    if (conf->disconn != GPIO_UNDEF) {
        gpio_write(conf->disconn, !(conf->flags & STM32_USBDEV_FS_CONFIG_FLAG_DISCONN_INVERTED));
    }
#endif /* USB_BCDR_DPPU */
}

static inline void _usb_detach(stm32_usbdev_fs_t *usbdev)
{
    const stm32_usbdev_fs_config_t *conf = usbdev->config;

    /* Some ST USB IP doesn't have this feature */
#ifdef USB_BCDR_DPPU
    DEBUG_PUTS("usbdev_fs: Detaching from host");

    /* Disable DP pullup to signal disconnection */
    CLRBIT(_global_regs(conf)->BCDR, USB_BCDR_DPPU);
#else
    /* If configuration uses a GPIO for USB connect/disconnect */
    if (conf->disconn != GPIO_UNDEF) {
        gpio_write(conf->disconn, (conf->flags & STM32_USBDEV_FS_CONFIG_FLAG_DISCONN_INVERTED));
    }
#endif
}

static void _set_address(stm32_usbdev_fs_t *usbdev, uint8_t address)
{
    const stm32_usbdev_fs_config_t *conf = usbdev->config;
    _global_regs(conf)->DADDR = USB_DADDR_EF | (address & USB_DADDR_ADD);
}

void USBDEV_ISR(void) {
    stm32_usbdev_fs_t *usbdev = &_usbdevs[0];
    const stm32_usbdev_fs_config_t *conf = usbdev->config;
    int irq_status = _global_regs(conf)->ISTR;
    /* Disable interrupts temporarily */
    _global_regs(conf)->CNTR = 0;
    /* Correct transfer Interrupt */
    if (_global_regs(conf)->ISTR & USB_ISTR_CTR) {
        /* Clear pending interrupts */
        _global_regs(conf)->ISTR = 0;
        /* Get endpoint number and associated ep pointer */
        unsigned epnum = irq_status & 0x000F;
        usbdev_ep_t *ep = (irq_status & USB_ISTR_DIR) ? &_ep_out[epnum] : &_ep_in[epnum];
        /* get type and add endpoint address */
        uint16_t reg = (_type_to_reg(ep->type) | ep->num);
        /* Avoid toggling by mistake these bits */
        CLRBIT(reg, (_EP_REG_TOGGLE_ONLY_BITS_MASK));
        if (irq_status & USB_ISTR_DIR) {
            /* Clear RX CTR by writing 0, avoid clear TX CTR so leave it to one */
            EP_REG(epnum) = reg | USB_EP_CTR_TX;
            usbdev->usbdev.epcb(ep, USBDEV_EVENT_ESR);
        } else {
            /* Clear TX CTR by writing 0, avoid clear RX CTR so leave it to one */
            EP_REG(epnum) = reg | USB_EP_CTR_RX;
            usbdev->usbdev.epcb(ep, USBDEV_EVENT_ESR);
        }
    } else if (_global_regs(conf)->ISTR & USB_ISTR_RESET) {
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_ESR);
    } else if (_global_regs(conf)->ISTR & USB_ISTR_PMAOVR) {
        _global_regs(conf)->ISTR = 0;
    } else {
        _global_regs(conf)->ISTR = 0;
    }
    cortexm_isr_end();
}

static void _usbdev_init(usbdev_t *dev)
{
    stm32_usbdev_fs_t *usbdev = (stm32_usbdev_fs_t *)dev;
    const stm32_usbdev_fs_config_t *conf = usbdev->config;

    DEBUG_PUTS("usbdev_fs: initialization");
    /* Block STOP/STANDBY */
    pm_block(STM32_PM_STOP);
    pm_block(STM32_PM_STANDBY);
    pm_block(STM32_PM_IDLE);

#if defined(RCC_CFGR_USBPRE)
    /* If `RCC_CFGR_USBPRE` is defined, the USB device FS clock of 48 MHz is
     * derived from the PLL clock. In this case the PLL clock must be configured
     * and must be either 48 MHz or 72 MHz. If the PLL clock is 72 MHz it is
     * pre-divided by 1.5, the PLL clock of 48 MHz is used directly. */
#if defined(CLOCK_PLL_SRC) && (CLOCK_CORECLOCK == MHZ(72))
    RCC->CFGR &= ~RCC_CFGR_USBPRE;
#elif defined(CLOCK_PLL_SRC) && (CLOCK_CORECLOCK == MHZ(48))
    RCC->CFGR |= RCC_CFGR_USBPRE;
#else
#error "PLL must be configured to output 48 or 72 MHz"
#endif
#endif

    /* Enable the clock to the peripheral */
    periph_clk_en(conf->apb, conf->rcc_mask);
    /* Enable USB clock */
    _enable_usb_clk();
    /* Configure GPIOs */
    _enable_gpio(conf);

    /* Reset and power down USB IP */
    _global_regs(conf)->CNTR = USB_CNTR_FRES | USB_CNTR_PDWN;
    /* Clear power down */
    _global_regs(conf)->CNTR &= ~USB_CNTR_PDWN;
    /* Clear reset */
    _global_regs(conf)->CNTR &= ~USB_CNTR_FRES;
    /* Clear interrupt register */
    _global_regs(conf)->ISTR = 0x0000;
    /* Set BTABLE at start of USB SRAM */
    _global_regs(conf)->BTABLE = 0x0000;
    /* Set default address after reset */
    _global_regs(conf)->DADDR = USB_DADDR_EF;
    /* Unmask the interrupt in the NVIC */
    _enable_irq();

    /* Disable remapping of USB IRQs if remapping defined */
#ifdef SYSCFG_CFGR1_USB_IT_RMP
    SYSCFG->CFGR1 &= ~SYSCFG_CFGR1_USB_IT_RMP;
#endif

    /* Enable USB IRQ */
    NVIC_EnableIRQ(conf->irqn);
    /* fill USB SRAM with zeroes */
    memset((uint8_t*)USB1_PMAADDR, 0, 1024);
}

static usbdev_ep_t *_get_ep(stm32_usbdev_fs_t *usbdev, unsigned num,
                            usb_ep_dir_t dir)
{
    if (num >= _ENDPOINT_NUMOF) {
        return NULL;
    }
    return dir == USB_EP_DIR_IN ? &usbdev->in[num] : &usbdev->out[num];
}

static bool _ep_check(stm32_usbdev_fs_t *usbdev, unsigned idx,
                      usb_ep_dir_t dir, usb_ep_type_t type)
{
    usbdev_ep_t* ep_in = &usbdev->in[idx];
    usbdev_ep_t* ep_out = &usbdev->out[idx];

    if (dir == USB_EP_DIR_IN) {
        if (ep_out->type == type || ep_out->type == USB_EP_TYPE_NONE) {
            return true;
        }
    }
    else {
        if (ep_in->type == type || ep_in->type == USB_EP_TYPE_NONE) {
            return true;
        }
    }
    return false;
}

static void _ep_enable(usbdev_ep_t *ep)
{
    uint16_t reg = EP_REG(ep->num);
    /* Avoid modification of the following bits in the register as writing
       1 has no effect on these bits */
    SETBIT(reg, USB_EP_CTR_RX | USB_EP_CTR_TX);
    CLRBIT(reg, USB_EP_DTOG_RX | USB_EP_DTOG_TX);
    if (ep->dir == USB_EP_DIR_OUT) {
        _set_ep_out_status(&reg, USB_EP_RX_NAK);
        /* Avoid toggling the other direction */
        CLRBIT(reg, USB_EP_TX_VALID);
    }
    else {
        _set_ep_in_status(&reg, USB_EP_TX_NAK);
        /* Avoid toggling the other direction */
        CLRBIT(reg, USB_EP_RX_VALID);
    }

    EP_REG(ep->num) = reg;
}

static void _ep_disable(usbdev_ep_t *ep)
{
    uint16_t reg = EP_REG(ep->num);
    /* Avoid modification of the following bits in the register as writing
       1 has no effect on these bits */
    SETBIT(reg, USB_EP_CTR_RX | USB_EP_CTR_TX);

    if (ep->dir == USB_EP_DIR_OUT) {
        _set_ep_out_status(&reg, USB_EP_RX_DIS);
        /* Avoid toggling the other direction */
        CLRBIT(reg, USB_EP_TX_VALID);
    }
    else {
        _set_ep_in_status(&reg, USB_EP_TX_DIS);
        /* Avoid toggling the other direction */
        CLRBIT(reg, USB_EP_RX_VALID);
    }
    EP_REG(ep->num) = reg;
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t len)
{
    stm32_usbdev_fs_t *usbdev = (stm32_usbdev_fs_t *)dev;
    /* The IP supports all types for all endpoints */
    usbdev_ep_t *new_ep = NULL;

    /* Always return endpoint 0 for control types as requested by IP */
    if (type == USB_EP_TYPE_CONTROL) {
        new_ep = _get_ep(usbdev, 0, dir);
        new_ep->num = 0;
    }
    else {
        /* Find the first unassigned ep with proper dir */
        for (unsigned idx = 1; idx < _ENDPOINT_NUMOF && !new_ep; idx++) {
            usbdev_ep_t *ep = _get_ep(usbdev, idx, dir);
            /* Check if endpoint is available */
            if (ep->type == USB_EP_TYPE_NONE) {
                bool avail = _ep_check(usbdev, idx, dir, type);
                if (avail) {
                    new_ep = ep;
                    new_ep->num = idx;
                }
            }
        }
    }
    if (new_ep) {
        if (usbdev->used + len < _PMA_SRAM_SIZE) {
            if (dir == USB_EP_DIR_IN) {
                _ep_in_buf[new_ep->num] = (_PMA_BUF_BASE_OFFSET + usbdev->used);
            } else {
                _ep_out_buf[new_ep->num] = (_PMA_BUF_BASE_OFFSET + usbdev->used);
            }
            usbdev->used += len;
            new_ep->len = len;
            new_ep->type = type;
            new_ep->dev = dev;
            new_ep->dir = dir;
            DEBUG("usbdev_fs: register new %s endpoint as EP%d\n",
                   new_ep->dir == USB_EP_DIR_IN ? "IN": "OUT", new_ep->num);
            DEBUG("usbdev_fs: %d bytes used\n", usbdev->used);
        }
        else {
            assert(0);
        }
    }

    return new_ep;
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
    (void)value_len;
    stm32_usbdev_fs_t *usbdev = (stm32_usbdev_fs_t *)dev;

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
                _usb_attach(usbdev);
            }
            else {
                _usb_detach(usbdev);
            }
            res = sizeof(usbopt_enable_t);
            break;
        default:
            DEBUG("usbdev_fs: Unhandled set call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _usbdev_esr(usbdev_t *dev)
{
    stm32_usbdev_fs_t *usbdev = (stm32_usbdev_fs_t *)dev;
    const stm32_usbdev_fs_config_t *conf = usbdev->config;

    if (_global_regs(conf)->ISTR & USB_ISTR_RESET) {
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESET);
        _global_regs(conf)->ISTR = (uint16_t)~USB_ISTR_RESET;
    }
    else {
        _global_regs(conf)->ISTR = 0x0000;
    }
    _enable_irq();
}

static void _usbdev_ep_init(usbdev_ep_t *ep)
{
    uint16_t reg = EP_REG(ep->num);
    /* Set endpoint type */
    reg &= ~(USB_EP_TYPE_MASK);
    reg |= _type_to_reg(ep->type);
    /* Keep the state ouf the other endpoint direction if already sets */
    CLRBIT(reg, _EP_REG_TOGGLE_ONLY_BITS_MASK);
    /* Assign EP address */
    reg |= (ep->num & 0x000F);
    /* Ensure interrupts are cleared */
    reg &= ~(USB_EP_CTR_RX | USB_EP_CTR_TX);
    /* Write the configuration to the register */
    EP_REG(ep->num) = reg;
    if (ep->dir == USB_EP_DIR_IN) {
        EP_DESC[ep->num].addr_tx = _ep_in_buf[ep->num];
        EP_DESC[ep->num].count_tx = 64;
    } else {
        EP_DESC[ep->num].addr_rx = _ep_out_buf[ep->num];
        /* Set BLSIZE + NUM_BLOCK (1) */
        EP_DESC[ep->num].count_rx = (1 << 10) | (1 << 15);
    }
}

static usbopt_enable_t _ep_get_stall(usbdev_ep_t *ep)
{
    usbopt_enable_t res;

    if (ep->dir == USB_EP_DIR_IN) {
        res = ((EP_REG(ep->num) & USB_EPTX_STAT) == USB_EP_TX_STALL) ?
              USBOPT_ENABLE : USBOPT_DISABLE;
    } else {
        res = ((EP_REG(ep->num) & USB_EPRX_STAT) == USB_EP_RX_STALL) ?
              USBOPT_ENABLE : USBOPT_DISABLE;
    }
    return res;
}

static size_t _ep_get_available(usbdev_ep_t *ep)
{
    size_t size;

    if (ep->dir == USB_EP_DIR_OUT) {
        size = EP_DESC[ep->num].count_rx & 0x03FF;
    } else {
        size = EP_DESC[ep->num].count_tx;
    }
    return size;
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
            DEBUG("usbdev_fs: Unhandled get call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _ep_set_stall(usbdev_ep_t *ep, usbopt_enable_t enable)
{
    uint16_t reg = EP_REG(ep->num);
    /* Avoid modification of the following bits in the register as writing
       1 has no effect on these bits */
    SETBIT(reg, USB_EP_CTR_RX | USB_EP_CTR_TX);

    if (ep->dir == USB_EP_DIR_IN) {
        _set_ep_in_status(&reg, enable ? USB_EP_TX_STALL : USB_EP_TX_NAK);
        /* Avoid toggling the other direction */
        CLRBIT(reg, USB_EP_RX_VALID);
    }
    else {
        _set_ep_out_status(&reg, enable ? USB_EP_RX_STALL : USB_EP_RX_NAK);
        /* Avoid toggling the other direction */
        CLRBIT(reg, USB_EP_TX_VALID);
    }

    EP_REG(ep->num) = reg;
}

static void _usbdev_ep0_stall(usbdev_t *usbdev)
{
    (void)usbdev;
    _ep_set_stall(&_ep_in[0], true);
    _ep_set_stall(&_ep_out[0], true);
}

static void _usbdev_ep_stall(usbdev_ep_t *ep, bool enable)
{
    _ep_set_stall(ep, enable);
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
                _usbdev_ep_init(ep);
                _ep_enable(ep);
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
            DEBUG("usbdev_fs: Unhandled endpoint set call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _usbdev_ep_esr(usbdev_ep_t *ep)
{
    if (ep->dir == USB_EP_DIR_OUT) {
        /* Copy PMA SRAM buffer to OUT buffer */
        /* `addr_rx` is the USB IP local address offset in half-words. The PMA
         * address offset depends on the access scheme.
         * - 2 x 16 bits/word access scheme: `_PMA_ACCESS_STEP_SIZE` is 1
         *   and the PMA address offset corresponds to USB IP local address.
         * - 1 x 16 bits/word access scheme: `_PMA_ACCESS_STEP_SIZE` is 2
         *   and the PMA address offset is the double of the USB IP local
         *   address. */
        uint16_t* pma_ptr = (uint16_t *)(USB1_PMAADDR +
                                         (EP_DESC[ep->num].addr_rx * _PMA_ACCESS_STEP_SIZE));
        uint16_t hword;
        uint8_t cpt = 0;
        while (cpt < 64) {
            hword = *pma_ptr;
            _app_pbuf[ep->num][cpt++] = hword & 0x00ff;
            _app_pbuf[ep->num][cpt++] = (hword & 0xff00) >> 8;
            pma_ptr += _PMA_ACCESS_STEP_SIZE;
        }
    }
    ep->dev->epcb(ep, USBDEV_EVENT_TR_COMPLETE);
    _enable_irq();
}

static int _usbdev_ep_xmit(usbdev_ep_t *ep, uint8_t* buf, size_t len)
{
    unsigned irq = irq_disable();
    uint16_t reg = EP_REG(ep->num);
    /* Avoid modification of the following bits in the register as writing
       1 has no effect on these bits */
    SETBIT(reg, USB_EP_CTR_RX | USB_EP_CTR_TX);
    if (ep->dir == USB_EP_DIR_OUT) {
        _set_ep_out_status(&reg, USB_EP_RX_VALID);
        CLRBIT(reg, USB_EPTX_STAT);
        if (len == 0) {
            len = ep->len;
        }
        if (len < 63) {
            /* Should write len / 2 in count_rx if len <= 62 */
            EP_DESC[ep->num].count_rx = (len >> 1) << 10;
        } else {
            /* BL_SIZE = 1, NUM_BLOCK = 1 */
            EP_DESC[ep->num].count_rx = 1 << 15 | 1 << 10;
        }
        CLRBIT(reg, USB_EP_DTOG_TX | USB_EP_DTOG_RX);
        _app_pbuf[ep->num] = buf;
    } else {
        _set_ep_in_status(&reg, USB_EP_TX_VALID);
        CLRBIT(reg, USB_EPRX_STAT);
        /* Transfer IN buffer content to USB PMA SRAM */
        uint16_t* pma_ptr = (uint16_t *)(USB1_PMAADDR +
                                         (EP_DESC[ep->num].addr_tx * _PMA_ACCESS_STEP_SIZE));
        for (size_t i = 0; i < len; i += 2) {
            *pma_ptr = (buf[i + 1] << 8) | buf[i];
            pma_ptr += _PMA_ACCESS_STEP_SIZE;
        }
        /* Manage odd transfer size */
        if (len % 2 == 1) {
            *pma_ptr =  0x00ff & buf[len - 1];
        }

        EP_DESC[ep->num].count_tx = len;
        CLRBIT(reg, USB_EP_DTOG_TX | USB_EP_DTOG_RX);
    }

    EP_REG(ep->num) = reg;
    irq_restore(irq);
    return 0;
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
