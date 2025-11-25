/*
 * SPDX-FileCopyrightText: 2019 Koen Zandberg
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USB OTG peripheral requirement for setting the device address
 *
 * The address in the USB device has to be directly after the SETUP
 * stage on receipt of the `SET ADDRESS Request`.
 */
#define USBDEV_CPU_SET_ADDR_AFTER_STATUS    0

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

/**
 * @brief Device speed used
 */
enum {
    DWC2_USB_OTG_DSPD_HS = 0,           /**< High speed */
    DWC2_USB_OTG_DSPD_FS_PHY_HS = 1,    /**< Full speed on HS PHY */
    DWC2_USB_OTG_DSPD_LS = 2,           /**< Low speed */
    DWC2_USB_OTG_DSPD_FS = 3,           /**< Full speed */
};

/**
 * @brief Type of USB OTG peripheral PHY.
 *
 * The FS type only supports the built-in PHY, the HS type can have enabled
 * either
 * - the on-chip FS PHY,
 * - the external ULPI HS PHY interface, or
 * - the internal UTMI HS PHY.
 */
typedef enum {
    DWC2_USB_OTG_PHY_BUILTIN,   /**< on-chip FS PHY */
    DWC2_USB_OTG_PHY_ULPI,      /**< ULPI for external HS PHY */
    DWC2_USB_OTG_PHY_UTMI,      /**< UTMI for internal HS PHY */
} dwc2_usb_otg_fshs_phy_t;

/**
 * @brief USB OTG configuration
 */
typedef struct {
    uintptr_t periph;               /**< USB peripheral base address */
    dwc2_usb_otg_fshs_type_t type;  /**< FS or HS type */
    dwc2_usb_otg_fshs_phy_t phy;    /**< on-chip FS, ULPI HS or UTMI HS PHY */
#if defined(MODULE_PERIPH_USBDEV_HS_ULPI) || DOXYGEN
    gpio_t ulpi_clk;                /**< ULPI CLK gpio */
    gpio_t ulpi_d0;                 /**< ULPI D0 gpio */
    gpio_t ulpi_d1;                 /**< ULPI D1 gpio */
    gpio_t ulpi_d2;                 /**< ULPI D2 gpio */
    gpio_t ulpi_d3;                 /**< ULPI D3 gpio */
    gpio_t ulpi_d4;                 /**< ULPI D4 gpio */
    gpio_t ulpi_d5;                 /**< ULPI D5 gpio */
    gpio_t ulpi_d6;                 /**< ULPI D6 gpio */
    gpio_t ulpi_d7;                 /**< ULPI D7 gpio */
    gpio_t ulpi_dir;                /**< ULPI DIR gpio */
    gpio_t ulpi_stp;                /**< ULPI STP gpio */
    gpio_t ulpi_nxt;                /**< ULPI NXT gpio */
    gpio_af_t ulpi_af;              /**< Alternative function for ULPI */
#endif
#if defined(CPU_STM32) || DOXYGEN
    uint32_t rcc_mask;              /**< bit in clock enable register */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t ahb;                    /**< AHB bus */
    gpio_t dm;                      /**< Data- gpio */
    gpio_t dp;                      /**< Data+ gpio */
    gpio_af_t af;                   /**< Alternative function */
#if defined(MODULE_PERIPH_USBDEV_HS_UTMI) || DOXYGEN
    uint32_t phy_tune;              /**< USB HS PHY controller tuning register
                                      *  value (STM32-specific), see USBPHYC_TUNE
                                      *  register in STM32 Reference Manual */
#endif /* defined(MODULE_PERIPH_USBDEV_HS_UTMI) */
#endif /* defined(CPU_STM32) || DOXYGEN */
#if defined(CPU_GD32V)
    uint32_t rcu_mask;              /**< bit in clock enable register */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t bus;                    /**< Peripheral bus */
#endif
} dwc2_usb_otg_fshs_config_t;

#ifdef __cplusplus
}
#endif

/** @} */
