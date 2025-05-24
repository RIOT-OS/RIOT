/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_gd32v
 * @{
 *
 * @file
 * @brief       Default USB OTG configuration for GD32 RISC-V board
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "vendor/usbdev_gd32v.h"
#include "usbdev_synopsys_dwc2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   USB OTG configuration
 * @{
 */
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
        .rcu_mask = RCU_AHBEN_USBFSEN_Msk,
        .irqn     = USBFS_IRQn,
        .bus      = AHB,
    }
};

/**
 * @brief Number of available USB OTG peripherals
 */
#define USBDEV_NUMOF           ARRAY_SIZE(dwc2_usb_otg_fshs_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
