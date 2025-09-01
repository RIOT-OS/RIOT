/*
 * SPDX-FileCopyrightText: 2019 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 OTG FS peripheral
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "periph_cpu.h"
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
        .rcc_mask = RCC_AHB2ENR_OTGFSEN,
        .irqn     = OTG_FS_IRQn,
        .ahb      = AHB2,
        .dm       = GPIO_PIN(PORT_A, 11),
        .dp       = GPIO_PIN(PORT_A, 12),
        .af       = GPIO_AF10,
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
