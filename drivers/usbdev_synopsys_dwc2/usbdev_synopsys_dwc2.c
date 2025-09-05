/*
 * SPDX-FileCopyrightText: 2019 Koen Zandberg
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup drivers_periph_usbdev
 * @{
 * @file
 * @brief   Low level USB FS/HS driver for MCUs with Synopsys DWC2 IP core
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#ifdef CPU_ESP32

#if !defined(CPU_FAM_ESP32S2) && !defined(CPU_FAM_ESP32S3)
#error "ESP32x SoC family not supported"
#endif /* !defined(CPU_FAM_ESP32S2) && !defined(CPU_FAM_ESP32S3) */

#endif

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "architecture.h"
#include "bitarithm.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "log.h"
#include "periph/pm.h"
#include "periph/gpio.h"
#include "periph/usbdev.h"
#include "pm_layered.h"
#include "ztimer.h"

#if defined(CPU_STM32)
#include "usbdev_stm32.h"
#elif defined(CPU_ESP32)
#include "usbdev_esp32.h"
#elif defined(CPU_EFM32)
#include "usbdev_efm32.h"
#elif defined(CPU_GD32V)
#include "vendor/usbdev_gd32v.h"
#else
#error "MCU not supported"
#endif

/**
 * Be careful with enabling debug here. As with all timing critical systems it
 * is able to interfere with USB functionality and you might see different
 * errors than debug disabled
 */
#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef CPU_ESP32

#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_private/usb_phy.h"
#include "soc/soc_caps.h"

#endif

#ifdef CPU_GD32V

#define RCU_CFG0_SCS_PLL        (2UL << RCU_CFG0_SCS_Pos)   /* PLL used */

#define USB_OTG_GCCFG_PWRON     (1UL << 16) /* Power on */
#define USB_OTG_GCCFG_VBUSBCEN  (1UL << 19) /* VBUS B-device comparer enable */
#define USB_OTG_GCCFG_VBUSIG    (1UL << 21) /* VBUS detection ignore */

/* Although the following defines are done in `vendor/gd32vf103_rcu.h`, they
 * have to be defined here since `vendor/gd32vf103_rcu.h` can't be included due
 * to type conflicts with `vendor/gd32vf103_periph.h`. */
#define RCU_CKUSB_CKPLL_DIV1_5  (0UL << 22) /* USB clock is PLL clock/1.5 */
#define RCU_CKUSB_CKPLL_DIV1    (1UL << 22) /* USB clock is PLL clock */
#define RCU_CKUSB_CKPLL_DIV2_5  (2UL << 22) /* USB clock is PLL clock/2.5 */
#define RCU_CKUSB_CKPLL_DIV2    (3UL << 22) /* USB clock is PLL clock/2 */

#endif

#if defined(DWC2_USB_OTG_FS_ENABLED) && defined(DWC2_USB_OTG_HS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (DWC2_USB_OTG_FS_NUM_EP + \
                               DWC2_USB_OTG_HS_NUM_EP)
#elif defined(DWC2_USB_OTG_FS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (DWC2_USB_OTG_FS_NUM_EP)
#elif defined(DWC2_USB_OTG_HS_ENABLED)
#define _TOTAL_NUM_ENDPOINTS  (DWC2_USB_OTG_HS_NUM_EP)
#endif

/* Mask for the set of interrupts used */
#define DWC2_FSHS_USB_GINT_MASK    (USB_OTG_GINTMSK_USBSUSPM | \
                                    USB_OTG_GINTMSK_WUIM     | \
                                    USB_OTG_GINTMSK_ENUMDNEM | \
                                    USB_OTG_GINTMSK_USBRST   | \
                                    USB_OTG_GINTMSK_OTGINT   | \
                                    USB_OTG_GINTMSK_IEPINT   | \
                                    USB_OTG_GINTMSK_OEPINT)

#define DWC2_PKTSTS_GONAK          0x01    /**< Rx fifo global out nak */
#define DWC2_PKTSTS_DATA_UPDT      0x02    /**< Rx fifo data update    */
#define DWC2_PKTSTS_XFER_COMP      0x03    /**< Rx fifo data complete  */
#define DWC2_PKTSTS_SETUP_COMP     0x04    /**< Rx fifo setup complete */
#define DWC2_PKTSTS_SETUP_UPDT     0x06    /**< Rx fifo setup update   */

/* minimum depth of an individual transmit FIFO */
#define DWC2_USB_OTG_FIFO_MIN_WORD_SIZE (16U)
/* Offset for OUT endpoints in a shared IN/OUT endpoint bit flag register */
#define DWC2_USB_OTG_REG_EP_OUT_OFFSET  (16U)

/* Endpoint zero size values */
#define DWC2_USB_OTG_EP0_SIZE_64    (0x0)
#define DWC2_USB_OTG_EP0_SIZE_32    (0x1)
#define DWC2_USB_OTG_EP0_SIZE_16    (0x2)
#define DWC2_USB_OTG_EP0_SIZE_8     (0x3)

/* Endpoint type values */
#define DWC2_USB_OTG_EP_TYPE_CONTROL   (0x00 << USB_OTG_DOEPCTL_EPTYP_Pos)
#define DWC2_USB_OTG_EP_TYPE_ISO       (0x01 << USB_OTG_DOEPCTL_EPTYP_Pos)
#define DWC2_USB_OTG_EP_TYPE_BULK      (0x02 << USB_OTG_DOEPCTL_EPTYP_Pos)
#define DWC2_USB_OTG_EP_TYPE_INTERRUPT (0x03 << USB_OTG_DOEPCTL_EPTYP_Pos)

/**
 * @brief DWC2 USB OTG peripheral device out endpoint struct
 */
typedef struct {
    usbdev_ep_t ep;     /**< Inherited usbdev endpoint struct */
    uint8_t *out_buf;   /**< Requested data output buffer */
} dwc2_usb_otg_fshs_out_ep_t;

/**
 * @brief DWC2 USB OTG peripheral device context
 */
typedef struct {
    usbdev_t usbdev;                            /**< Inherited usbdev struct */
    const dwc2_usb_otg_fshs_config_t *config;   /**< USB peripheral config   */
    size_t fifo_pos;                            /**< FIFO space occupied */
    usbdev_ep_t *in;                            /**< In endpoints */
    dwc2_usb_otg_fshs_out_ep_t *out;            /**< Out endpoints */
    bool suspend;                               /**< Suspend status */
    bool setup_stage;      /**< Indicates that EP0 is in SETUP stage */
#ifdef DWC2_USB_OTG_HS_ENABLED
    uint8_t *ep0_out_buf;  /**< Points to the buffer of EP0 OUT handler */
#endif
} dwc2_usb_otg_fshs_t;

/* List of instantiated USB peripherals */
static dwc2_usb_otg_fshs_t _usbdevs[USBDEV_NUMOF] = { 0 };

static dwc2_usb_otg_fshs_out_ep_t _out[_TOTAL_NUM_ENDPOINTS];
static usbdev_ep_t _in[_TOTAL_NUM_ENDPOINTS];

/* Forward declaration for the usb device driver */
const usbdev_driver_t driver;

static void _flush_tx_fifo(const dwc2_usb_otg_fshs_config_t *conf,
                           uint8_t fifo_num);

/*************************************************************************
* Conversion function from the base address to specific register blocks *
*************************************************************************/
static USB_OTG_GlobalTypeDef *_global_regs(
    const dwc2_usb_otg_fshs_config_t *conf)
{
    return (USB_OTG_GlobalTypeDef *)(conf->periph + USB_OTG_GLOBAL_BASE);
}

static USB_OTG_DeviceTypeDef *_device_regs(
    const dwc2_usb_otg_fshs_config_t *conf)
{
    return (USB_OTG_DeviceTypeDef *)(conf->periph + USB_OTG_DEVICE_BASE);
}

static USB_OTG_INEndpointTypeDef *_in_regs(
    const dwc2_usb_otg_fshs_config_t *conf,
    size_t endpoint)
{
    return (USB_OTG_INEndpointTypeDef *)(conf->periph +
                                         USB_OTG_IN_ENDPOINT_BASE +
                                         USB_OTG_EP_REG_SIZE * endpoint);
}

static USB_OTG_OUTEndpointTypeDef *_out_regs(
    const dwc2_usb_otg_fshs_config_t *conf,
    size_t endpoint)
{
    return (USB_OTG_OUTEndpointTypeDef *)(conf->periph +
                                          USB_OTG_OUT_ENDPOINT_BASE +
                                          USB_OTG_EP_REG_SIZE * endpoint);
}

static __I uint32_t *_rx_fifo(const dwc2_usb_otg_fshs_config_t *conf)
{
    return (__I uint32_t *)(conf->periph + USB_OTG_FIFO_BASE);
}

static __O uint32_t *_tx_fifo(const dwc2_usb_otg_fshs_config_t *conf,
                              size_t num)
{
    return (__O uint32_t *)(conf->periph +
                            USB_OTG_FIFO_BASE +
                            USB_OTG_FIFO_SIZE * num);
}

static __IO uint32_t *_pcgcctl_reg(const dwc2_usb_otg_fshs_config_t *conf)
{
    return (__IO uint32_t *)(conf->periph + USB_OTG_PCGCCTL_BASE);
}
/* end of conversion functions */

/**
 * @brief Determine the number of available endpoints for the peripheral based
 *        on the type and the CID version
 *
 * @param config    configuration struct
 */
static size_t _max_endpoints(const dwc2_usb_otg_fshs_config_t *config)
{
    (void)config;
#if defined(DWC2_USB_OTG_FS_ENABLED) && defined(DWC2_USB_OTG_HS_ENABLED)
    return config->type == DWC2_USB_OTG_FS ? DWC2_USB_OTG_FS_NUM_EP
                                           : DWC2_USB_OTG_HS_NUM_EP
#elif defined(DWC2_USB_OTG_FS_ENABLED)
    return DWC2_USB_OTG_FS_NUM_EP;
#elif defined(DWC2_USB_OTG_HS_ENABLED)
    return DWC2_USB_OTG_HS_NUM_EP;
#else
    return 0;
#endif
}

static inline bool _uses_dma(const dwc2_usb_otg_fshs_config_t *config)
{
#ifdef DWC2_USB_OTG_HS_ENABLED
    return config->type == DWC2_USB_OTG_HS;
#else
    (void)config;
    return false;
#endif
}

static size_t _setup(dwc2_usb_otg_fshs_t *usbdev,
                     const dwc2_usb_otg_fshs_config_t *config, size_t idx)
{
    usbdev->usbdev.driver = &driver;
    usbdev->config = config;
    usbdev->out = &_out[idx];
    usbdev->in = &_in[idx];
    return _max_endpoints(config);
}

/**
 * @brief Low level usbdev struct setup
 *
 * Distributes the available endpoints among the enabled peripherals
 */
void usbdev_init_lowlevel(void)
{
    size_t ep_idx = 0;

    for (size_t i = 0; i < USBDEV_NUMOF; i++) {
        ep_idx += _setup(&_usbdevs[i], &dwc2_usb_otg_fshs_config[i], ep_idx);
    }
#ifdef NDEBUG
    (void)ep_idx;
#endif
    assert(ep_idx == _TOTAL_NUM_ENDPOINTS);
}

usbdev_t *usbdev_get_ctx(unsigned num)
{
    assert(num < USBDEV_NUMOF);
    return &_usbdevs[num].usbdev;
}

static void _enable_global_out_nak(const dwc2_usb_otg_fshs_config_t *conf)
{
    if (_device_regs(conf)->DCTL & USB_OTG_DCTL_GONSTS) {
        return;
    }
    _device_regs(conf)->DCTL |= USB_OTG_DCTL_SGONAK;
    while (!(_device_regs(conf)->DCTL & USB_OTG_DCTL_GONSTS)) {}
}

static void _disable_global_out_nak(const dwc2_usb_otg_fshs_config_t *conf)
{
    if (!(_device_regs(conf)->DCTL & USB_OTG_DCTL_GONSTS)) {
        return;
    }
    _device_regs(conf)->DCTL |= USB_OTG_DCTL_CGONAK;
    while ((_device_regs(conf)->DCTL & USB_OTG_DCTL_GONSTS)) {}
}

static void _enable_global_in_nak(const dwc2_usb_otg_fshs_config_t *conf)
{
    if (_device_regs(conf)->DCTL & USB_OTG_DCTL_GINSTS) {
        return;
    }
    _device_regs(conf)->DCTL |= USB_OTG_DCTL_SGINAK;
    while (!(_device_regs(conf)->DCTL & USB_OTG_DCTL_GINSTS)) {}
}

static void _disable_global_in_nak(const dwc2_usb_otg_fshs_config_t *conf)
{
    if (!(_device_regs(conf)->DCTL & USB_OTG_DCTL_GINSTS)) {
        return;
    }
    _device_regs(conf)->DCTL |= USB_OTG_DCTL_CGINAK;
    while ((_device_regs(conf)->DCTL & USB_OTG_DCTL_GINSTS)) {}
}

static void _disable_global_nak(const dwc2_usb_otg_fshs_config_t *conf)
{
    _disable_global_in_nak(conf);
    _disable_global_out_nak(conf);
}

static uint32_t _type_to_reg(usb_ep_type_t type)
{
    switch (type) {
        case USB_EP_TYPE_CONTROL:
            return DWC2_USB_OTG_EP_TYPE_CONTROL;
        case USB_EP_TYPE_ISOCHRONOUS:
            return DWC2_USB_OTG_EP_TYPE_ISO;
        case USB_EP_TYPE_BULK:
            return DWC2_USB_OTG_EP_TYPE_BULK;
        case USB_EP_TYPE_INTERRUPT:
            return DWC2_USB_OTG_EP_TYPE_INTERRUPT;
        default:
            assert(false);
            return 0;
    }
}

static uint32_t _ep0_size(size_t size)
{
    switch (size) {
        case 64:
            return DWC2_USB_OTG_EP0_SIZE_64;
        case 32:
            return DWC2_USB_OTG_EP0_SIZE_32;
        case 16:
            return DWC2_USB_OTG_EP0_SIZE_16;
        case 8:
            return DWC2_USB_OTG_EP0_SIZE_8;
        default:
            assert(false);
            return 0x00;
    }
}

/**
 * @brief Disables transfers on an IN type endpoint.
 *
 * Endpoint is only deactivated if it was activated.
 * The endpoint will still respond to traffic, but any transfers will be aborted
 */
static void _ep_in_disable(const dwc2_usb_otg_fshs_config_t *conf, size_t num)
{
    if (_in_regs(conf, num)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
        DEBUG("usbdev: Disabling EP%u-IN\n", num);
        /* Enable global nak according to procedure */
        _enable_global_in_nak(conf);
        /* Flush the fifo to clear pending data */
        _flush_tx_fifo(conf, num);
        /* disable endpoint and set NAK */
        _in_regs(conf, num)->DIEPCTL = USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK;
        /* Wait for the disable to take effect */
        while (_in_regs(conf, num)->DIEPCTL & USB_OTG_DIEPCTL_EPDIS) {}
        /* Disable global nak according to procedure */
        _disable_global_in_nak(conf);
    }
}

/**
 * @brief Disables transfers on an OUT type endpoint.
 *
 * Endpoint is only deactivated if it was activated
 * The endpoint will still respond to traffic, but any transfers will be aborted
 */
static void _ep_out_disable(const dwc2_usb_otg_fshs_config_t *conf, size_t num)
{
    if (_out_regs(conf, num)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) {
        DEBUG("usbdev: Disabling EP%u-OUT\n", num);
        /* Enable global nak according to procedure */
        _enable_global_out_nak(conf);
        /* No need to flush the fifo here, this works(tm) */
        /* disable endpoint and set NAK */
        _out_regs(conf, num)->DOEPCTL = USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK;
        /* Wait for the disable to take effect */
        while (_out_regs(conf, num)->DOEPCTL & USB_OTG_DOEPCTL_EPDIS) {}
        /* Disable global nak according to procedure */
        _disable_global_out_nak(conf);
    }
}

static void _ep_deactivate(usbdev_ep_t *ep)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)ep->dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        _ep_in_disable(conf, ep->num);
        _in_regs(conf, ep->num)->DIEPCTL &= ~USB_OTG_DIEPCTL_USBAEP;
    }
    else {
        _ep_out_disable(conf, ep->num);
        _out_regs(conf, ep->num)->DOEPCTL &= ~USB_OTG_DOEPCTL_USBAEP;
    }
}

static void _ep_activate(usbdev_ep_t *ep)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)ep->dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        _ep_in_disable(conf, ep->num);
        _device_regs(conf)->DAINTMSK |= 1 << ep->num;
        uint32_t diepctl = USB_OTG_DIEPCTL_SNAK |
                           USB_OTG_DIEPCTL_USBAEP |
                           _type_to_reg(ep->type) |
                           ep->num << USB_OTG_DIEPCTL_TXFNUM_Pos;
        if (ep->num == 0) {
            diepctl |= _ep0_size(ep->len);
        }
        else {
            diepctl |= ep->len;
            diepctl |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
        }
        _in_regs(conf, ep->num)->DIEPCTL |= diepctl;
    }
    else {
        _ep_out_disable(conf, ep->num);
        _device_regs(conf)->DAINTMSK |=
            1 << (ep->num + DWC2_USB_OTG_REG_EP_OUT_OFFSET);
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK |
                                             USB_OTG_DOEPCTL_USBAEP;
        _type_to_reg(ep->type);
        if (ep->num == 0) {
            _out_regs(conf, ep->num)->DOEPCTL |= _ep0_size(ep->len);
        }
        else {
            _out_regs(conf, ep->num)->DOEPCTL |= ep->len;
            _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
        }
    }
}

static inline void _usb_attach(dwc2_usb_otg_fshs_t *usbdev)
{
    DEBUG("usbdev: Attaching to host\n");
    /* Disable the soft disconnect feature */
    _device_regs(usbdev->config)->DCTL &= ~USB_OTG_DCTL_SDIS;
}

static inline void _usb_detach(dwc2_usb_otg_fshs_t *usbdev)
{
    DEBUG("usbdev: Detaching from host\n");
    /* Enable the soft disconnect feature */
    _device_regs(usbdev->config)->DCTL |= USB_OTG_DCTL_SDIS;
}

static void _set_address(dwc2_usb_otg_fshs_t *usbdev, uint8_t address)
{
    _device_regs(usbdev->config)->DCFG =
        (_device_regs(usbdev->config)->DCFG & ~(USB_OTG_DCFG_DAD_Msk)) |
        (address << USB_OTG_DCFG_DAD_Pos);
}

static usbdev_ep_t *_get_ep(dwc2_usb_otg_fshs_t *usbdev, unsigned num,
                            usb_ep_dir_t dir)
{
    if (num >= _max_endpoints(usbdev->config)) {
        return NULL;
    }
    return dir == USB_EP_DIR_IN ? &usbdev->in[num] : &usbdev->out[num].ep;
}

#if defined(DEVELHELP) && !defined(NDEBUG)
static size_t _total_fifo_size(const dwc2_usb_otg_fshs_config_t *conf)
{
    (void)conf;
#if defined(DWC2_USB_OTG_FS_ENABLED) && defined(DWC2_USB_OTG_HS_ENABLED)
    return conf->type == DWC2_USB_OTG_FS ? DWC2_USB_OTG_FS_TOTAL_FIFO_SIZE
                                         : DWC2_USB_OTG_HS_TOTAL_FIFO_SIZE;
#elif defined(DWC2_USB_OTG_FS_ENABLED)
    return DWC2_USB_OTG_FS_TOTAL_FIFO_SIZE;
#elif defined(DWC2_USB_OTG_HS_ENABLED)
    return DWC2_USB_OTG_HS_TOTAL_FIFO_SIZE;
#else
    return 0;
#endif
}
#endif /* defined(DEVELHELP) && !defined(NDEBUG) */

static void _configure_tx_fifo(dwc2_usb_otg_fshs_t *usbdev, size_t num,
                               size_t len)
{
    /* TX Fifo size must be at least 16 words long and must be word aligned */
    size_t wordlen = len < (DWC2_USB_OTG_FIFO_MIN_WORD_SIZE * sizeof(uint32_t))
                     ? DWC2_USB_OTG_FIFO_MIN_WORD_SIZE
                     : (len + (sizeof(uint32_t) - 1)) / sizeof(uint32_t);

    /* Check max size */
    assert(usbdev->fifo_pos + wordlen <=
           _total_fifo_size(usbdev->config) / sizeof(uint32_t));

    /* FIFO Array starts at FIFO 1 at index 0, FIFO 0 is special and has a
     * different register (DIEPTXF0_HNPTXFSIZ) */
    _global_regs(usbdev->config)->DIEPTXF[num - 1] =
        (wordlen << USB_OTG_TX0FD_Pos) |
        (usbdev->fifo_pos);
    usbdev->fifo_pos += wordlen;
}

/*
 * GRFXSIZ, DWC2_USB_OTG_{HS,FS}_RX_FIFO_SIZE, DWC2_USB_OTG_FIFO_MIN_WORD_SIZE
 * are given in 32-bit words
 *
 * +--------------------+
 * |   TX FIFO EPn IN   |
 * +--------------------+ GRFXSIZ + (n * DWC2_USB_OTG_FIFO_MIN_WORD_SIZE)
 * |         ...        |
 * +--------------------+ GRFXSIZ + (2 * DWC2_USB_OTG_FIFO_MIN_WORD_SIZE)
 * |   TX FIFO EP1 IN   |
 * +--------------------+ GRFXSIZ + (1 * DWC2_USB_OTG_FIFO_MIN_WORD_SIZE)
 * |   TX FIFO EP0 IN   |
 * +--------------------+ GRXFSIZ = DWC2_USB_OTG_{HS,FS}_RX_FIFO_SIZE
 * |                    |
 * |                    |
 * | RX FIFO Shared OUT |
 * |                    |
 * |                    |
 * +--------------------+ 0
 */
static void _configure_fifo(dwc2_usb_otg_fshs_t *usbdev)
{
    /* TODO: dynamic FIFO size handling */
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    size_t rx_size = 0;
#if defined(DWC2_USB_OTG_FS_ENABLED) && defined(DWC2_USB_OTG_HS_ENABLED)
    rx_size = (conf->type == DWC2_USB_OTG_FS) ? DWC2_USB_OTG_FS_RX_FIFO_SIZE
                                              : DWC2_USB_OTG_HS_RX_FIFO_SIZE;
#elif defined(DWC2_USB_OTG_FS_ENABLED)
    rx_size = DWC2_USB_OTG_FS_RX_FIFO_SIZE;
#elif defined(DWC2_USB_OTG_HS_ENABLED)
    rx_size = DWC2_USB_OTG_HS_RX_FIFO_SIZE;
#endif

    _global_regs(conf)->GRXFSIZ =
        (_global_regs(conf)->GRXFSIZ & ~USB_OTG_GRXFSIZ_RXFD) |
        rx_size;
    /* set size and position of TX FIFO for EP0 IN */
    _global_regs(conf)->DIEPTXF0_HNPTXFSIZ =
        (DWC2_USB_OTG_FIFO_MIN_WORD_SIZE << USB_OTG_TX0FD_Pos) |
        rx_size;
    /* position of TX FIFO for EP1 IN */
    usbdev->fifo_pos = (rx_size + DWC2_USB_OTG_FIFO_MIN_WORD_SIZE);
}

static usbdev_ep_t *_usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                   usb_ep_dir_t dir, size_t len)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)dev;
    usbdev_ep_t *ep = NULL;

    if (type == USB_EP_TYPE_CONTROL) {
        if (dir == USB_EP_DIR_IN) {
            ep = &usbdev->in[0];
        }
        else {
            ep = &usbdev->out[0].ep;
        }
        ep->num = 0;
    }
    else {
        /* Find the first unassigned ep with matching direction */
        for (unsigned idx = 1; idx < _max_endpoints(usbdev->config) && !ep; idx++) {
            usbdev_ep_t *candidate_ep = _get_ep(usbdev, idx, dir);
            if (candidate_ep->type == USB_EP_TYPE_NONE) {
                ep = candidate_ep;
                ep->num = idx;
            }
        }
    }

    if (ep && ep->type == USB_EP_TYPE_NONE) {
        ep->dir = dir;
        ep->type = type;
        ep->dev = dev;
        ep->len = len;
        if (ep->dir == USB_EP_DIR_IN && ep->num != 0) {
            _configure_tx_fifo(usbdev, ep->num, ep->len);
        }
    }
    return ep;
}

/**
 * @brief reset a TX fifo.
 *
 * @param   conf        usbdev context
 * @param   fifo_num    fifo number to reset, 0x10 for all fifos
 */
static void _flush_tx_fifo(const dwc2_usb_otg_fshs_config_t *conf,
                           uint8_t fifo_num)
{
    uint32_t reg = _global_regs(conf)->GRSTCTL & ~(USB_OTG_GRSTCTL_TXFNUM);

    reg |= fifo_num << USB_OTG_GRSTCTL_TXFNUM_Pos | USB_OTG_GRSTCTL_TXFFLSH;
    _global_regs(conf)->GRSTCTL = reg;
    /* Wait for flush to finish */
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) {}
}

static void _flush_rx_fifo(const dwc2_usb_otg_fshs_config_t *conf)
{
    _global_regs(conf)->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) {}
}

static void _sleep_periph(const dwc2_usb_otg_fshs_config_t *conf)
{
    *_pcgcctl_reg(conf) |= USB_OTG_PCGCCTL_STOPCLK;
#if defined(CPU_STM32)
    /* Unblocking STM32_PM_STOP during suspend on the stm32f446 breaks while
     * (un)blocking works on the stm32f401, needs more investigation.
     * Works with:
     * stm32f407vg  FS  CID: 1200, HWREV: 4f54281a, HWCFG: 229dcd20
     * stm32f429zi  HS  CID: 1100, HWREV: 4f54281a, HWCFG: 229ed590
     * stm32f439zi  FS  CID: 1200, HWREV: 4f54281a, HWCFG: 229dcd20
     * stm32f723ie  FS  CID: 3000, HWREV: 4f54330a, HWCFG: 229ed520
     * stm32f723ie  HS  CID: 3100, HWREV: 4f54330a, HWCFG: 229fe1d0
     * stm32f746ng  FS  CID: 2000, HWREV: 4f54320a, HWCFG: 229ed520
     * stm32f746ng  HS  CID: 2100, HWREV: 4f54320a, HWCFG: 229fe190
     * stm32f767zi  FS  CID: 2000, HWREV: 4f54320a, HWCFG: 229ed520
     */
    pm_unblock(STM32_PM_STOP);
#elif defined(CPU_EFM32)
    /* switch USB core clock source either to LFXO or LFRCO */
    CMU_ClockSelectSet(cmuClock_USB, CLOCK_LFA);
    pm_unblock(EFM32_PM_MODE_EM2);
#elif defined(CPU_ESP32)
    pm_unblock(ESP_PM_LIGHT_SLEEP);
#elif defined(CPU_GD32V)
    pm_unblock(GD32V_PM_DEEPSLEEP);
#endif
}

static void _wake_periph(const dwc2_usb_otg_fshs_config_t *conf)
{
#if defined(CPU_STM32)
    pm_block(STM32_PM_STOP);
#elif defined(CPU_EFM32)
    pm_block(EFM32_PM_MODE_EM2);
    /* switch USB core clock source either to USHFRCO or HFCLK */
#if defined(CPU_FAM_EFM32GG12B) || defined(CPU_FAM_EFM32GG11B)
    CMU_ClockSelectSet(cmuClock_USB, cmuSelect_USHFRCO);
#elif defined(CPU_FAM_EFM32GG) || defined(CPU_FAM_EFM32LG)
    CMU_ClockSelectSet(cmuClock_USB, cmuSelect_HFCLK);
#else
#error "EFM32 family not yet supported"
#endif /* defined(CPU_FAM_EFM32GG12B) || defined(CPU_FAM_EFM32GG11B) */
#elif defined(CPU_ESP32)
    pm_block(ESP_PM_LIGHT_SLEEP);
#elif defined(CPU_GD32V)
    pm_block(GD32V_PM_DEEPSLEEP);
#endif
    *_pcgcctl_reg(conf) &= ~USB_OTG_PCGCCTL_STOPCLK;
    _flush_rx_fifo(conf);
    _flush_tx_fifo(conf, 0x10);
}

static void _reset_eps(dwc2_usb_otg_fshs_t *usbdev)
{
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    /* Set the NAK for all endpoints */
    for (size_t i = 0; i < _max_endpoints(conf); i++) {
        _out_regs(conf, i)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
        _in_regs(conf, i)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
        _in_regs(conf, i)->DIEPCTL |= (i) << USB_OTG_DIEPCTL_TXFNUM_Pos;
    }
}

static void _reset_periph(dwc2_usb_otg_fshs_t *usbdev)
{
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    /* Wait for AHB idle */
    while (!(_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL)) {}
    _global_regs(conf)->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
    /* Wait for reset done */
    while (_global_regs(conf)->GRSTCTL & USB_OTG_GRSTCTL_CSRST) {}
}

#ifdef CPU_STM32
static void _enable_gpio(const dwc2_usb_otg_fshs_config_t *conf)
{
    (void)conf;
#ifndef MODULE_PERIPH_USBDEV_HS_ULPI
    /* Enables clock on the GPIO bus */
    gpio_init(conf->dp, GPIO_IN);
    gpio_init(conf->dm, GPIO_IN);
    /* Configure AF for the pins */
    gpio_init_af(conf->dp, conf->af);
    gpio_init_af(conf->dm, conf->af);
#endif /* MODULE_PERIPH_USBDEV_HS_ULPI */
}
#endif /* CPU_STM32 */

static void _set_mode_device(dwc2_usb_otg_fshs_t *usbdev)
{
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    /* Force device mode */
    _global_regs(conf)->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
    /* Spinlock to prevent a context switch here, needs a delay of 25 ms when
     * force switching mode */
    ztimer_spin(ZTIMER_MSEC, 25);
}

static void _usbdev_init(usbdev_t *dev)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

#if defined(CPU_STM32)

    /* Block both STOP and STANDBY, STOP is unblocked during USB suspend
     * status */
    pm_block(STM32_PM_STOP);
    pm_block(STM32_PM_STANDBY);

#if defined(PWR_CR2_USV)
    /*  on L4: Validate USB Supply */
    PWR->CR2 |= PWR_CR2_USV;
#endif /* PWR_CR2_USV */

#if defined(PWR_SVMCR_USV)
    /* on U5: Validate USB Supply */
    PWR->SVMCR |= PWR_SVMCR_USV;
#endif /* PWR_SVMCR_USV */

    /* Enable the clock to the peripheral */
    periph_clk_en(conf->ahb, conf->rcc_mask);

    _enable_gpio(conf);

#elif defined(CPU_ESP32)

    pm_block(ESP_PM_DEEP_SLEEP);
    pm_block(ESP_PM_LIGHT_SLEEP);

    usb_phy_handle_t phy_hdl;               /* only needed temporarily */

    usb_phy_config_t phy_conf = {
        .controller = USB_PHY_CTRL_OTG,
        .otg_mode = USB_OTG_MODE_DEVICE,
        .target = USB_PHY_TARGET_INT,       /* only internal PHY supported */
    };

    if (usb_new_phy(&phy_conf, &phy_hdl) != ESP_OK) {
        LOG_ERROR("usbdev: Install USB PHY failed\n");
    }

#elif defined(CPU_EFM32)

    /* Block EM2 and EM3. In EM2, most USB core registers are reset and the
     * FIFO content is lost. EM2 is unblocked during USB suspend */
    pm_block(EFM32_PM_MODE_EM3);
    pm_block(EFM32_PM_MODE_EM2);

#if defined(CPU_FAM_EFM32GG12B) || defined(CPU_FAM_EFM32GG11B)
    /* select USHFRCO as USB clock and set the tuning to 48 MHz */
    CMU_ClockSelectSet(cmuClock_USB, cmuSelect_USHFRCO);
    CMU_USHFRCOBandSet(cmuUSHFRCOFreq_48M0Hz);
    /* enable USB clock recovery */
    CMU->USBCRCTRL = CMU_USBCRCTRL_USBCREN;
    /* select USHFRCO as USB rate clock source and enable it */
    CMU_ClockSelectSet(cmuClock_USBR, cmuSelect_USHFRCO);
    CMU_ClockEnable(cmuClock_USBR, true);
#elif defined(CPU_FAM_EFM32GG) || defined(CPU_FAM_EFM32LG)
    /* select HFCLK as USB PHY clock source */
    CMU_ClockSelectSet(cmuClock_USB, cmuSelect_HFCLK);
    /* enable USB system clock */
    CMU_ClockEnable(cmuClock_USB, true);
    /* enable USB core clock */
    CMU_ClockEnable(cmuClock_USBC, true);
#else
#error "EFM32 family not yet supported"
#endif

    /* enable USB peripheral clock */
    CMU_ClockEnable(cmuClock_USB, true);

    /* USB PHY is enabled before core reset */
    USB->ROUTE = USB_ROUTE_PHYPEN;
    /* USB VBUSEN pin is not yet used */
    /* USB_ROUTELOC0 = location */

#elif defined(CPU_GD32V)

    /* Block both DEEP_SLEEP and STANDBY, TODO DEEP_SLEEP is unblocked during
     * USB suspend status */
    pm_block(GD32V_PM_DEEPSLEEP);
    pm_block(GD32V_PM_STANDBY);

    /* ensure that PLL clock is used */
    assert((RCU->CFG0 & RCU_CFG0_SCS_Msk) == RCU_CFG0_SCS_PLL);
    /* ensure that the 48 MHz USB clock can be generated */
    static_assert((CLOCK_CORECLOCK == MHZ(48)) || (CLOCK_CORECLOCK == MHZ(72)) ||
                  (CLOCK_CORECLOCK == MHZ(96)) || (CLOCK_CORECLOCK == MHZ(120)),
                  "CLOCK_CORECLOCK has to be 48, 72, 96 or 120");

    /* divide the core clock to get 48 MHz USB clock */
    RCU->CFG0 &= ~RCU_CFG0_USBFSPSC_Msk;
    switch (CLOCK_CORECLOCK) {
    case MHZ(48):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV1;
        break;
    case MHZ(72):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV1_5;
        break;
    case MHZ(96):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV2;
        break;
    case MHZ(120):
        RCU->CFG0 |= RCU_CKUSB_CKPLL_DIV2_5;
        break;
    }

    /* enable USB OTG clock */
    periph_clk_en(conf->bus, conf->rcu_mask);

    /* reset the USB OTG peripheral */
    RCU->AHBRST |= RCU_AHBRST_USBFSRST_Msk;
    RCU->AHBRST &= ~RCU_AHBRST_USBFSRST_Msk;

#else
#error "MCU not supported"
#endif

#if ENABLE_DEBUG
    uint32_t *_otg_core = (uint32_t *)&_global_regs(usbdev->config)->CID;
    DEBUG("CID: %04" PRIx32 ", HWREV: %08" PRIx32 ", HWCFG: %08" PRIx32 "\n",
          _otg_core[0], _otg_core[1], _otg_core[3]);
#endif

#ifdef DWC2_USB_OTG_HS_ENABLED
    if (conf->type == DWC2_USB_OTG_HS) {
        if (conf->phy == DWC2_USB_OTG_PHY_BUILTIN) {
            /* Disable the ULPI clock in low power mode, this is essential for the
             * peripheral when using the built-in PHY */
            periph_lpclk_dis(conf->ahb, RCC_AHB1LPENR_OTGHSULPILPEN);
            /* select on-chip builtin PHY */
            _global_regs(usbdev->config)->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
        }

#ifdef MODULE_PERIPH_USBDEV_HS_ULPI
        else if (conf->phy == DWC2_USB_OTG_PHY_ULPI) {
            /* initialize ULPI interface */
            gpio_init(conf->ulpi_clk, GPIO_IN);
            gpio_init(conf->ulpi_d0, GPIO_IN);
            gpio_init(conf->ulpi_d1, GPIO_IN);
            gpio_init(conf->ulpi_d2, GPIO_IN);
            gpio_init(conf->ulpi_d3, GPIO_IN);
            gpio_init(conf->ulpi_d4, GPIO_IN);
            gpio_init(conf->ulpi_d5, GPIO_IN);
            gpio_init(conf->ulpi_d6, GPIO_IN);
            gpio_init(conf->ulpi_d7, GPIO_IN);
            gpio_init(conf->ulpi_stp, GPIO_IN);
            gpio_init(conf->ulpi_dir, GPIO_IN);
            gpio_init(conf->ulpi_nxt, GPIO_IN);
            gpio_init_af(conf->ulpi_clk, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d0, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d1, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d2, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d3, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d4, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d5, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d6, conf->ulpi_af);
            gpio_init_af(conf->ulpi_d7, conf->ulpi_af);
            gpio_init_af(conf->ulpi_stp, conf->ulpi_af);
            gpio_init_af(conf->ulpi_dir, conf->ulpi_af);
            gpio_init_af(conf->ulpi_nxt, conf->ulpi_af);

            /* enable ULPI clock */
            periph_clk_en(conf->ahb, RCC_AHB1ENR_OTGHSULPIEN);

#ifdef USB_OTG_GUSBCFG_ULPI_UTMI_SEL
            /* select ULPI PHY */
            _global_regs(usbdev->config)->GUSBCFG |= USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
#endif
#ifdef USB_OTG_GUSBCFG_PHYIF
            /* use the 8-bit interface */
            _global_regs(usbdev->config)->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYIF;
#endif /* USB_OTG_GUSBCFG_PHYIF */
#ifdef USB_OTG_GUSBCFG_DDRSEL
            /* use single data rate */
            _global_regs(usbdev->config)->GUSBCFG &= ~USB_OTG_GUSBCFG_DDRSEL;
#endif /* USB_OTG_GUSBCFG_DDRSEL */

            /* disable the on-chip FS transceiver */
            _global_regs(usbdev->config)->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYSEL;

            /* use internal V_BUS valid indicator and internal charge pump */
            _global_regs(usbdev->config)->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD |
                                                       USB_OTG_GUSBCFG_ULPIEVBUSI);
            /* disable ULPI FS/LS serial interface */
            _global_regs(usbdev->config)->GUSBCFG &= ~USB_OTG_GUSBCFG_ULPIFSLS;
        }

#elif defined(MODULE_PERIPH_USBDEV_HS_UTMI)
        else if (conf->phy == DWC2_USB_OTG_PHY_UTMI) {
            /* enable ULPI clock */
            periph_clk_en(conf->ahb, RCC_AHB1ENR_OTGHSULPIEN);
            /* enable UTMI HS PHY Controller clock */
            periph_clk_en(APB2, RCC_APB2ENR_OTGPHYCEN);

#ifdef USB_OTG_GUSBCFG_ULPI_UTMI_SEL
            /* select UTMI+ PHY */
            _global_regs(usbdev->config)->GUSBCFG &= ~USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
#endif /* USB_OTG_GUSBCFG_ULPI_UTMI_SEL */
#ifdef USB_OTG_GUSBCFG_PHYIF
            /* use the 8-bit interface and single data rate */
            _global_regs(usbdev->config)->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYIF;
#endif /* USB_OTG_GUSBCFG_PHYIF */

            /* disable the on-chip FS transceiver */
            _global_regs(usbdev->config)->GUSBCFG &= ~USB_OTG_GUSBCFG_PHYSEL;

            /* configure the USB HS PHY Controller (USB_HS_PHYC),
             * USB_HS_PHYC and GCCFG are STM32 specific */
#ifdef USB_HS_PHYC
            /* enable USB HS PHY Controller */
            _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_PHYHSEN;

            /* determine the PLL input clock of the USB HS PHY from HSE clock */
            switch (CONFIG_CLOCK_HSE) {
                case 12000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_12MHZ;
                    break;
                case 12500000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_12_5MHZ;
                    break;
                case 16000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_16MHZ;
                    break;
                case 24000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_24MHZ;
                    break;
                case 25000000:
                    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_25MHZ;
                    break;
                default:
                    assert(0);
            }

            /* configure the tuning interface of the USB HS PHY */
            USB_HS_PHYC->USB_HS_PHYC_TUNE |= conf->phy_tune;

            /* check whether the LDO regulator is used by on the chip */
            if (USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_USED) {
                /* enable the LDO */
                USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;
                /* wait until the LDO is ready */
                while (!(USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS)) {}
            }

            /* enable the PLL of the USB HS PHY */
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL_PLLEN;
#endif /* USB_HS_PHYC */
        }

#else /* MODULE_PERIPH_USBDEV_HS_ULPI */
        else {
            /* only on-chip PHY support enabled */
            assert(conf->phy == DWC2_USB_OTG_PHY_BUILTIN);
        }
#endif /* MODULE_PERIPH_USBDEV_HS_ULPI */

    }
#endif /* DWC2_USB_OTG_HS_ENABLED */

    /* Reset the peripheral after phy selection */
    _reset_periph(usbdev);

    /* Reset clock */
    *_pcgcctl_reg(conf) = 0;

    /* Force the peripheral to device mode */
    _set_mode_device(usbdev);

#if defined(CPU_STM32)

    /* Disable Vbus detection and force the pull-up on, GCCFG is STM32 specific */
#if defined(STM32_USB_OTG_CID_1x)
    /* set No Vbus Sensing */
    _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
#elif defined(STM32_USB_OTG_CID_2x)
    /* clear Vbus Detect Enable */
    _global_regs(usbdev->config)->GCCFG &= ~USB_OTG_GCCFG_VBDEN;
    /* Force Vbus Detect values and ID detect values to device mode */
    _global_regs(usbdev->config)->GOTGCTL |= USB_OTG_GOTGCTL_VBVALOVAL |
                                             USB_OTG_GOTGCTL_VBVALOEN |
                                             USB_OTG_GOTGCTL_BVALOEN |
                                             USB_OTG_GOTGCTL_BVALOVAL;
#endif /* defined(STM32_USB_OTG_CID_1x) */
    if (conf->phy == DWC2_USB_OTG_PHY_BUILTIN) {
        /* set `Power Down Disable` to activate the on-chip FS transceiver */
        _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_PWRDWN;
    }
    else if (IS_USED(MODULE_PERIPH_USBDEV_HS_ULPI) && (conf->phy == DWC2_USB_OTG_PHY_ULPI)) {
        /* clear `Power Down Disable` to deactivate the on-chip FS transceiver */
        _global_regs(usbdev->config)->GCCFG &= ~USB_OTG_GCCFG_PWRDWN;
    }
    else if (IS_USED(MODULE_PERIPH_USBDEV_HS_UTMI) && (conf->phy == DWC2_USB_OTG_PHY_UTMI)) {
        /* clear `Power Down Disable` to deactivate the on-chip FS transceiver */
        _global_regs(usbdev->config)->GCCFG &= ~USB_OTG_GCCFG_PWRDWN;
    }

#elif defined(CPU_ESP32) || defined(CPU_EFM32)

    /* Force Vbus Detect values and ID detect values to device mode */
    _global_regs(usbdev->config)->GOTGCTL |= USB_OTG_GOTGCTL_VBVALOVAL |
                                             USB_OTG_GOTGCTL_VBVALOEN |
                                             USB_OTG_GOTGCTL_BVALOEN |
                                             USB_OTG_GOTGCTL_BVALOVAL;
#elif defined(CPU_GD32V)
    /* disable Vbus sensing */
    _global_regs(usbdev->config)->GCCFG |= USB_OTG_GCCFG_PWRON |
                                           USB_OTG_GCCFG_VBUSIG |
                                           USB_OTG_GCCFG_VBUSBCEN;
#else
#error "MCU not supported"
#endif

    /* disable fancy USB features */
    _global_regs(conf)->GUSBCFG &=
        ~(USB_OTG_GUSBCFG_HNPCAP | USB_OTG_GUSBCFG_SRPCAP);

#ifdef DWC2_USB_OTG_HS_ENABLED
    if ((conf->type == DWC2_USB_OTG_FS) || (conf->phy == DWC2_USB_OTG_PHY_BUILTIN)) {
        /* Device mode init */
        _device_regs(conf)->DCFG |= DWC2_USB_OTG_DSPD_FS;  /* Full speed is */
    }
    else {
        /* Device mode init */
        _device_regs(conf)->DCFG |= DWC2_USB_OTG_DSPD_HS;  /* High speed! */
    }
#else
    /* Device mode init */
    _device_regs(conf)->DCFG |= DWC2_USB_OTG_DSPD_FS;  /* Full speed! */
#endif

    _configure_fifo(usbdev);

    /* Reset the receive FIFO */
    _flush_rx_fifo(conf);

    /* Reset all TX FIFOs */
    _flush_tx_fifo(conf, 0x10);

    /* Values from the reference manual tables on TRDT configuration        *
     * 0x09 for 24Mhz AHB frequency, 0x06 for 32Mhz or higher AHB frequency */
    uint8_t trdt = conf->type == DWC2_USB_OTG_FS ? 0x06 : 0x09;
    _global_regs(conf)->GUSBCFG =
        (_global_regs(conf)->GUSBCFG & ~USB_OTG_GUSBCFG_TRDT) |
        (trdt << USB_OTG_GUSBCFG_TRDT_Pos);

    _reset_eps(usbdev);

    /* Disable the global NAK for both directions */
    _disable_global_nak(conf);

    uint32_t gint_mask = DWC2_FSHS_USB_GINT_MASK;

    if (_uses_dma(conf)) {
        _global_regs(usbdev->config)->GAHBCFG |=
            /* Configure DMA */
            USB_OTG_GAHBCFG_DMAEN |
            /* DMA configured as 8 x 32bit accesses */
            (0x05 << USB_OTG_GAHBCFG_HBSTLEN_Pos);
    }
    else {
        /* Use RXFLVL (RX FIFO Level) interrupt for IN EPs in non-DMA mode */
        gint_mask |= USB_OTG_GINTMSK_RXFLVLM;
    }

    /* Unmask XFRC (Transfer Completed) interrupt for IN and OUT EPs */
    _device_regs(conf)->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM;
    _device_regs(conf)->DOEPMSK |= USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_STUPM;

    /* Clear the interrupt flags and unmask those interrupts */
    _global_regs(conf)->GINTSTS |= gint_mask;
    _global_regs(conf)->GINTMSK |= gint_mask;

    DEBUG("usbdev: USB peripheral currently in %s mode\n",
          (_global_regs(conf)->GINTSTS & USB_OTG_GINTSTS_CMOD) ? "host"
                                                               : "device");

    /* Enable interrupts */
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;

#if defined(CPU_STM32)
    /* Unmask the interrupt in the NVIC */
    NVIC_EnableIRQ(conf->irqn);
#elif defined(CPU_EFM32)
    /* Unmask the interrupt in the NVIC */
    NVIC_EnableIRQ(USB_IRQn);
#elif defined(CPU_ESP32)
    void isr_otg_fs(void *arg);
    /* Allocate the interrupt and connect it with USB interrupt source */
    esp_intr_alloc(ETS_USB_INTR_SOURCE, ESP_INTR_FLAG_LOWMED, isr_otg_fs, NULL, NULL);
#elif defined(CPU_GD32V)
    void isr_otg_fs(unsigned irq);
    clic_set_handler(conf->irqn, isr_otg_fs);
    clic_enable_interrupt(conf->irqn, CPU_DEFAULT_IRQ_PRIO);
#else
#error "MCU not supported"
#endif
}

static usb_speed_t _get_max_speed(const usbdev_t *dev)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;
    if (IS_USED(DWC2_USB_OTG_HS_ENABLED) && (conf->type == DWC2_USB_OTG_HS)) {
        return USB_SPEED_HIGH;
    }
    return USB_SPEED_FULL;
}

static usb_speed_t _get_enumeration_speed(const usbdev_t *dev)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;
    unsigned speed = (_device_regs(conf)->DSTS & USB_OTG_DSTS_ENUMSPD_Msk) >>
                     USB_OTG_DSTS_ENUMSPD_Pos;
    switch (speed) {
        case 0x00:
            return USB_SPEED_HIGH;
        case 0x03:
            return USB_SPEED_FULL;
        default:
            assert(false); /* reserved values by peripheral */
    }
    return USB_SPEED_FULL; /* Should not be reached */
}

static int _usbdev_get(usbdev_t *dev, usbopt_t opt,
                       void *value, size_t max_len)
{
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
            *(usb_speed_t *)value = _get_max_speed(dev);
            res = sizeof(usb_speed_t);
            break;
        case USBOPT_ENUMERATED_SPEED:
            assert(max_len == sizeof(usb_speed_t));
            *(usb_speed_t *)value = _get_enumeration_speed(dev);
            res = sizeof(usb_speed_t);
            break;
        default:
            DEBUG("usbdev: Unhandled get call: 0x%x\n", opt);
            break;
    }
    return res;
}

static int _usbdev_set(usbdev_t *dev, usbopt_t opt,
                       const void *value, size_t value_len)
{
    (void)value_len;

    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)dev;
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
            DEBUG("usbdev: Unhandled set call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _usbdev_esr(usbdev_t *dev)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    uint32_t int_status = _global_regs(conf)->GINTSTS;
    uint32_t event = 0;

    if (int_status & USB_OTG_GINTSTS_ENUMDNE) {
        event = USB_OTG_GINTSTS_ENUMDNE;
        /* Reset condition done */
        DEBUG("usbdev: Reset done\n");
        usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESET);
    }
    else if (int_status & USB_OTG_GINTSTS_USBRST) {
        /* Start of reset condition */
        event = USB_OTG_GINTSTS_USBRST;

        DEBUG("usbdev: Reset start\n");
        if (usbdev->suspend) {
            usbdev->suspend = false;
            _wake_periph(conf);
            DEBUG("usbdev: PHY SUSP %" PRIx32 "\n", *_pcgcctl_reg(conf));
        }

        /* Reset all the things! */
        usbdev->setup_stage = false;
        _flush_rx_fifo(conf);
        _flush_tx_fifo(conf, 0x10);
        _reset_eps(usbdev);
        _set_address(usbdev, 0);
    }
    else if (int_status & USB_OTG_GINTSTS_SRQINT) {
        /* Reset done */
        event = USB_OTG_GINTSTS_SRQINT;
        DEBUG("usbdev: Session request\n");
    }
    else if (int_status & USB_OTG_GINTSTS_USBSUSP) {
        event = USB_OTG_GINTSTS_USBSUSP;
        if (!usbdev->suspend) {
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_SUSPEND);
            usbdev->suspend = true;
            /* Disable USB clock */
            _sleep_periph(conf);
        }
    }
    else if (int_status & USB_OTG_GINTSTS_WKUINT) {
        event = USB_OTG_GINTSTS_WKUINT;
        if (usbdev->suspend) {
            usbdev->suspend = false;
            /* re-enable USB clock */
            _wake_periph(conf);
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_RESUME);
        }
    }
    else if (int_status & USB_OTG_GINTMSK_OTGINT) {
        /* not handled yet, just clear GOTGINT to clear the interrupt */
        event = USB_OTG_GINTMSK_OTGINT;
        int_status = _global_regs(conf)->GOTGINT;
        _global_regs(conf)->GOTGINT |= int_status;
        DEBUG("usbdev: OTG interrupt reg %08"PRIx32"\n", int_status);
    }

    _global_regs(conf)->GINTSTS |= event;
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

static void _usbdev_ep_init(usbdev_ep_t *ep)
{
    DEBUG("usbdev: Initializing EP%u-%s\n", ep->num,
          ep->dir == USB_EP_DIR_IN ? "IN" : "OUT");
}

static size_t _get_available(usbdev_ep_t *ep)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)ep->dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    return ep->len -
           (_out_regs(conf, ep->num)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ_Msk);
}

static int _usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
                          void *value, size_t max_len)
{
    (void)max_len;
    int res = -ENOTSUP;
    switch (opt) {
        case USBOPT_EP_AVAILABLE:
            assert(max_len == sizeof(size_t));
            *(size_t *)value = _get_available(ep);
            res = sizeof(size_t);
            break;
        default:
            DEBUG("usbdev: Unhandled endpoint get call: 0x%x\n", opt);
            break;
    }
    return res;
}

static void _usbdev_ep0_stall(usbdev_t *usbdev)
{
    dwc2_usb_otg_fshs_t *st_usbdev = (dwc2_usb_otg_fshs_t *)usbdev;
    const dwc2_usb_otg_fshs_config_t *conf = st_usbdev->config;
    /* Stall both directions, cleared automatically on SETUP received */
    _in_regs(conf, 0)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
    _out_regs(conf, 0)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;

#ifdef DWC2_USB_OTG_HS_ENABLED
    if (_uses_dma(conf) && st_usbdev->ep0_out_buf) {
        /* The STALL condition is automatically cleared by the hardware as
         * specified in the API documentation. However, in DMA mode the
         * reception of the next SETUP must be prepared by setting the SETUP
         * packet counter (STUPCNT) and enabling the OUT endpoint. Otherwise
         * the USB OTG HS core will not generate an interrupt and the USB stack
         * cannot respond correctly to the next SETUP received. In addition,
         * the DMA address must be reset to the buffer address of the EP0
         * OUT handler. */
        _out_regs(conf, 0)->DOEPDMA = (uint32_t)(intptr_t)(st_usbdev->ep0_out_buf);
        _out_regs(conf, 0)->DOEPTSIZ |= 1 << USB_OTG_DOEPTSIZ_STUPCNT_Pos;
        _out_regs(conf, 0)->DOEPCTL |= USB_OTG_DOEPCTL_EPENA;
    }
#endif
}

static void _ep_set_stall(usbdev_ep_t *ep, bool enable)
{
    (void)enable;

    assert(ep->num != 0);
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)ep->dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    if (enable) {
        if (ep->dir == USB_EP_DIR_IN) {
            /* Disable first */
            _ep_in_disable(conf, ep->num);
            _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
        }
        else {
            /* Disable first */
            _ep_out_disable(conf, ep->num);
            _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
        }
    }
    else {
        if (ep->dir == USB_EP_DIR_IN) {
            /* Clear stall and set to DATA0 */
            uint32_t diepctl = _in_regs(conf, ep->num)->DIEPCTL;
            diepctl &= ~(USB_OTG_DIEPCTL_STALL);
            diepctl |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
            _in_regs(conf, ep->num)->DIEPCTL = diepctl;
        }
        else {
            /* Clear stall and set to DATA0 */
            uint32_t doepctl = _out_regs(conf, ep->num)->DOEPCTL;
            doepctl &= ~(USB_OTG_DIEPCTL_STALL);
            doepctl |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
            _out_regs(conf, ep->num)->DOEPCTL = doepctl;
        }
    }
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
                _ep_activate(ep);
            }
            else {
                _ep_deactivate(ep);
            }
            res = sizeof(usbopt_enable_t);
            break;
        case USBOPT_EP_STALL:
            assert(value_len == sizeof(usbopt_enable_t));
            _ep_set_stall(ep, *(usbopt_enable_t *)value);
            res = sizeof(usbopt_enable_t);
            break;
        default:
            DEBUG("usbdev: Unhandled endpoint set call: 0x%x\n", opt);
            break;
    }
    return res;
}

static int _usbdev_ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)ep->dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    if (_uses_dma(conf)) {
        /* Assert the alignment required for the buffers */
        assert(HAS_ALIGNMENT_OF(buf, USBDEV_CPU_DMA_ALIGNMENT));
    }

    if (ep->dir == USB_EP_DIR_IN) {
        /* Abort when the endpoint is not active, prevents hangs,
         * could be an assert in the future maybe */
        if (!(_in_regs(conf, ep->num)->DIEPCTL & USB_OTG_DIEPCTL_USBAEP)) {
            return -1;
        }

        if (_uses_dma(conf)) {
            _in_regs(conf, ep->num)->DIEPDMA = (uint32_t)(intptr_t)buf;
        }

        /* The order here is crucial (AFAIK), it is required to first set the
         * size and the packet count, then clear the NAK and enable the
         * endpoint, and finally fill the transmit FIFO with the packet data.
         * When DMA is enabled, filling the transmit FIFO is handled by the DMA
         * controller in the peripheral
         */

        /* PKTCNT has to be set for all IN EPs to use the XFRC interrupt */
        _in_regs(conf, ep->num)->DIEPTSIZ = (len & USB_OTG_DIEPTSIZ_XFRSIZ_Msk) |
                                            (1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos);

        /* Intentionally enabling this before the FIFO is filled, unmasking the
         * interrupts after the FIFO is filled doesn't always trigger the ISR */
        _device_regs(conf)->DAINTMSK |= 1 << ep->num;

        _in_regs(conf, ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_CNAK |
                                            USB_OTG_DIEPCTL_EPENA;

        if (len > 0 && !_uses_dma(conf)) {
            /* The FIFO requires 32 bit word reads/writes */
            size_t words = (len + 3) / 4;

            /* buffer alignment is asserted above. Use intermediate
             * cast to uintptr_t to silence -Wcast-align*/
            uint32_t *ep_buf = (uint32_t *)(uintptr_t)buf;
            __O uint32_t *fifo = _tx_fifo(conf, ep->num);
            for (size_t i = 0; i < words; i++) {
                fifo[i] = ep_buf[i];
            }
        }
    }
    else {
        /* Abort when the endpoint is not active, prevents hangs,
         * could be an assert in the future maybe */
        if (!(_out_regs(conf, ep->num)->DOEPCTL & USB_OTG_DOEPCTL_USBAEP)) {
            return -1;
        }

        if (_uses_dma(conf)) {
            _out_regs(conf, ep->num)->DOEPDMA = (uint32_t)(intptr_t)buf;
            /* store the buffer address of the EP0 OUT handler to use it in
             * _usbdev_ep0_stall */
#ifdef DWC2_USB_OTG_HS_ENABLED
            if (ep->num == 0) {
                usbdev->ep0_out_buf = buf;
            }
#endif
        }
        else {
            container_of(ep, dwc2_usb_otg_fshs_out_ep_t, ep)->out_buf = buf;
        }

        /* Configure to receive one packet with ep->len as max length */
        uint32_t doeptsiz = (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos) |
                            (ep->len & USB_OTG_DOEPTSIZ_XFRSIZ_Msk);
        doeptsiz |= (ep->num == 0) ? 1 << USB_OTG_DOEPTSIZ_STUPCNT_Pos : 0;
        _out_regs(conf, ep->num)->DOEPTSIZ = doeptsiz;
        _out_regs(conf, ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK |
                                             USB_OTG_DOEPCTL_EPENA |
                                             _type_to_reg(ep->type);
    }

    return 0;
}

static void _copy_rxfifo(dwc2_usb_otg_fshs_t *usbdev, uint8_t *buf, size_t len)
{
    /* The FIFO requires 32 bit word reads/writes. This is only called with
     * usbdev_ep_t::buf, which is aligned to four bytes in _usbdev_new_ep() */
    uint32_t *buf32 = (uint32_t *)(uintptr_t)buf;
    __I uint32_t *fifo32 = _rx_fifo(usbdev->config);
    size_t count = (len + 3) / 4;

    for (size_t i = 0; i < count; i++) {
        buf32[i] = fifo32[i];
    }
}

static void _read_packet(dwc2_usb_otg_fshs_t *usbdev)
{
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;
    /* Pop status from the receive fifo status register */
    uint32_t status = _global_regs(conf)->GRXSTSP;

    unsigned epnum = (status & USB_OTG_GRXSTSP_EPNUM_Msk) >>
                     USB_OTG_GRXSTSP_EPNUM_Pos;
    /* Packet status code */
    unsigned pkt_status = (status & USB_OTG_GRXSTSP_PKTSTS_Msk) >>
                          USB_OTG_GRXSTSP_PKTSTS_Pos;
    size_t len = (status & USB_OTG_GRXSTSP_BCNT_Msk) >>
                 USB_OTG_GRXSTSP_BCNT_Pos;

    /* RXFLVL is only used in non-DMA mode to copy the packet from the FIFO
     * and to set the flag to indicate the SETUP stage. */

    if (pkt_status == DWC2_PKTSTS_SETUP_UPDT) {
        /* Some versions of the USB OTG core, e.g. the GD32V USB OTG core,
         * may generate an additional XFRC (Transfer Completed) interrupt in
         * the SETUP stage before the STUP (SETUP phase done) interrupt. To
         * prevent this additional interrupt from being processed accidentally,
         * we must indicate that the SETUP stage has been started. */
         usbdev->setup_stage = true;
    }

    if (len && !_uses_dma(conf)) {
        /* in DMA mode, received data are directly written into memory */
        _copy_rxfifo(usbdev, usbdev->out[epnum].out_buf, len);
    }
}

/* This signals to the upper stack a completed transfer. Control transfers
 * behave slightly different with the interrupts, so a number of conditionals
 * filter interrupts to events
 */
static void _usbdev_ep_esr(usbdev_ep_t *ep)
{
    dwc2_usb_otg_fshs_t *usbdev = (dwc2_usb_otg_fshs_t *)ep->dev;
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    if (ep->dir == USB_EP_DIR_IN) {
        uint32_t status = _in_regs(conf, ep->num)->DIEPINT;

        /* XFRC (Transfer Complete) interrupt is used to signal that the
         * transfer is complete. XFRC interrupts only work if the packet
         * count (PKTCNT) is set in DIEPTSIZ register. */
        if (status & USB_OTG_DIEPINT_XFRC) {
            _in_regs(conf, ep->num)->DIEPINT = USB_OTG_DIEPINT_XFRC;
            usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
        }
    }
    else {
        uint32_t status = _out_regs(conf, ep->num)->DOEPINT;

        if (status & USB_OTG_DOEPINT_STUP) {
            _out_regs(conf, ep->num)->DOEPINT = USB_OTG_DOEPINT_STUP;
            /* Indicate that the SETUP stage has been finished */
            usbdev->setup_stage = false;
            usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
        }
        else if (status & USB_OTG_DOEPINT_XFRC) {
            _out_regs(conf, ep->num)->DOEPINT = USB_OTG_DOEPINT_XFRC;
            /* Some versions of the USB OTG core, e.g. the GD32V USB OTG core,
             * may generate an additional XFRC (Transfer Complete) interrupt in
             * the SETUP stage before the STUP (SETUP phase done) interrupt is
             * triggered. USBDEV_EVENT_TR_COMPLETE must not be generated
             * for EP0 in this case. */
            if (!usbdev->setup_stage || (ep->num != 0)) {
                usbdev->usbdev.epcb(ep, USBDEV_EVENT_TR_COMPLETE);
            }
        }
    }
    /* Enable the peripheral interrupts again */
    _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

static void _isr_ep(dwc2_usb_otg_fshs_t *usbdev)
{
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;
    /* Top 16 bits of the register is OUT endpoints, bottom 16 is IN
     * endpoints */
    uint32_t active_ep = _device_regs(conf)->DAINT;

    if (active_ep) {
        unsigned epnum = bitarithm_lsb(active_ep);
        if (epnum >= DWC2_USB_OTG_REG_EP_OUT_OFFSET) {
            usbdev->usbdev.epcb(&usbdev->out[epnum - DWC2_USB_OTG_REG_EP_OUT_OFFSET].ep,
                                USBDEV_EVENT_ESR);
        }
        else {
            usbdev->usbdev.epcb(&usbdev->in[epnum], USBDEV_EVENT_ESR);
        }
    }
}

void _isr_common(dwc2_usb_otg_fshs_t *usbdev)
{
    const dwc2_usb_otg_fshs_config_t *conf = usbdev->config;

    uint32_t status = _global_regs(conf)->GINTSTS;

    _global_regs(conf)->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;

    if (status) {
        if (status & USB_OTG_GINTSTS_RXFLVL) {
            /* The RXFLVL interrupt is only enabled in non-DMA mode. It is only
             * used to read out the RX FIFO and set the SETUP stage flasg and
             * can therefore be handled directly in the ISR. */
            _read_packet(usbdev);
            _global_regs(conf)->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
        }
        else if (status & (USB_OTG_GINTSTS_OEPINT | USB_OTG_GINTSTS_IEPINT)) {
            _isr_ep(usbdev);
        }
        else {
            /* Global interrupt */
            usbdev->usbdev.cb(&usbdev->usbdev, USBDEV_EVENT_ESR);
        }
    }

#ifdef MODULE_CORTEXM_COMMON
    cortexm_isr_end();
#endif
}

#if defined(CPU_STM32)

#ifdef DWC2_USB_OTG_FS_ENABLED
void isr_otg_fs(void)
{
    /* Take the first device from the list */
    dwc2_usb_otg_fshs_t *usbdev = &_usbdevs[0];

    _isr_common(usbdev);
}
#endif /* DWC2_USB_OTG_FS_ENABLED */

#ifdef DWC2_USB_OTG_HS_ENABLED
void isr_otg_hs(void)
{
    /* Take the last usbdev device from the list */
    dwc2_usb_otg_fshs_t *usbdev = &_usbdevs[USBDEV_NUMOF - 1];

    _isr_common(usbdev);
}
#endif /* DWC2_USB_OTG_HS_ENABLED */

#elif defined(CPU_ESP32)

void isr_otg_fs(void *arg)
{
    (void)arg;

    /* Take the first device from the list */
    dwc2_usb_otg_fshs_t *usbdev = &_usbdevs[0];

    _isr_common(usbdev);
}

#elif defined(CPU_EFM32)

void isr_usb(void)
{
    /* Take the first device from the list */
    dwc2_usb_otg_fshs_t *usbdev = &_usbdevs[0];

    _isr_common(usbdev);
}

#elif defined(CPU_GD32V)

void isr_otg_fs(unsigned irq)
{
    (void)irq;

    /* Take the first device from the list */
    dwc2_usb_otg_fshs_t *usbdev = &_usbdevs[0];

    _isr_common(usbdev);
}

#else
#error "CPU not supported"
#endif

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
