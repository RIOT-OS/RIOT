/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 OTG HS peripheral with on-chip FS PHY
 *
 * All STM32 boards which use the on-chip FS PHY for the USB OTG HS peripheral
 * use the same configuration. Therefore a common configuration file can be
 * used for these boards.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef CFG_USB_OTG_HS_PHY_FS_H
#define CFG_USB_OTG_HS_PHY_FS_H

#include "periph_cpu.h"
#include "usbdev_synopsys_dwc2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the full speed USB OTG peripheral
 */
#define DWC2_USB_OTG_HS_ENABLED

/**
 * @brief Common USB OTG FS configuration
 */
static const dwc2_usb_otg_fshs_config_t dwc2_usb_otg_fshs_config[] = {
    {
        .periph   = USB_OTG_HS_PERIPH_BASE,
        .type     = DWC2_USB_OTG_HS,
        .phy      = DWC2_USB_OTG_PHY_BUILTIN,
        .rcc_mask = RCC_AHB1ENR_OTGHSEN,
        .irqn     = OTG_HS_IRQn,
        .ahb      = AHB1,
        .dm       = GPIO_PIN(PORT_B, 14),
        .dp       = GPIO_PIN(PORT_B, 15),
        .af       = GPIO_AF12,
    }
};

/**
 * @brief Number of available USB OTG peripherals
 */
#define USBDEV_NUMOF           ARRAY_SIZE(dwc2_usb_otg_fshs_config)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CFG_USB_OTG_HS_PHY_FS_H */
