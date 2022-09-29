/*
 * Copyright (C) 2019 Koen Zandberg
 *               2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_periph_usbdev
 * @{
 *
 * @file
 * @brief   Low level USB FS/HS driver definitions for MCUs with Synopsys DWC2 IP core
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef USBDEV_SYNOPSYS_DWC2_H
#define USBDEV_SYNOPSYS_DWC2_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USB OTG peripheral type.
 *
 * High speed peripheral is assumed to have DMA support available.
 *
 * @warning Only one of each type is supported at the moment, it is not
 * supported to have two FS type or two HS type peripherals enabled on a
 * single MCU.
 */
typedef enum {
    DWC2_USB_OTG_FS = 0,   /**< Full speed peripheral */
    DWC2_USB_OTG_HS = 1,   /**< High speed peripheral */
} dwc2_usb_otg_fshs_type_t;

#if defined(MCU_STM32)
/**
 * @brief Type of USB OTG peripheral phy.
 *
 * The FS type only supports the built-in type, the HS phy can have either the
 * FS built-in phy enabled or the HS ULPI interface enabled.
 */
typedef enum {
    DWC2_USB_OTG_PHY_BUILTIN,
    DWC2_USB_OTG_PHY_ULPI,
} dwc2_usb_otg_fshs_phy_t;
#endif

/**
 * @brief stm32 USB OTG configuration
 */
typedef struct {
    uintptr_t periph;               /**< USB peripheral base address */
    dwc2_usb_otg_fshs_type_t type;  /**< FS or HS type */
#if defined(MCU_STM32)
    dwc2_usb_otg_fshs_phy_t phy;    /**< Built-in or ULPI phy */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t ahb;                    /**< AHB bus */
    gpio_t dm;                      /**< Data- gpio */
    gpio_t dp;                      /**< Data+ gpio */
    gpio_af_t af;                   /**< Alternative function */
#endif /* defined(MCU_STM32) */
} dwc2_usb_otg_fshs_config_t;

#ifdef __cplusplus
}
#endif

#endif /* USBDEV_SYNOPSYS_DWC2_H */
/** @} */
