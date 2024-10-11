/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_silabs
 * @{
 *
 * @file
 * @brief       Common configuration for EFM32 OTG FS peripheral
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USBDEV_CFG_OTG_FS_H
#define USBDEV_CFG_OTG_FS_H

#include "periph_cpu.h"
#include "usbdev_efm32.h"
#include "usbdev_synopsys_dwc2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the full speed USB OTG peripheral
 */
#define DWC2_USB_OTG_FS_ENABLED

/**
 * @brief Common USB OTG FS configuration
 */
static const dwc2_usb_otg_fshs_config_t dwc2_usb_otg_fshs_config[] = {
    {
        .periph   = USB_OTG_FS_PERIPH_BASE,
        .type     = DWC2_USB_OTG_FS,
        .phy      = DWC2_USB_OTG_PHY_BUILTIN,
    }
};

/**
 * @brief Number of available USB OTG peripherals
 */
#define USBDEV_NUMOF    ARRAY_SIZE(dwc2_usb_otg_fshs_config)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* USBDEV_CFG_OTG_FS_H */
