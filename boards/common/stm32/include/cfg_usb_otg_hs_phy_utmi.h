/*
 * Copyright (C) 2019 Koen Zandberg
 *               2022 Gunar Schorcht
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
 * @brief       Common configuration for STM32 OTG HS peripheral with internal UTMI HS PHY
 *
 * All STM32 boards which use the internal UTMI HS PHY for the USB OTG HS
 * peripheral use the same configuration. Therefore a common configuration file
 * can be used for these boards.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef CFG_USB_OTG_HS_PHY_UTMI_H
#define CFG_USB_OTG_HS_PHY_UTMI_H

#include "periph_cpu.h"
#include "usbdev_synopsys_dwc2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the high speed USB OTG peripheral
 */
#define DWC2_USB_OTG_HS_ENABLED

#ifndef USBPHYC_TUNE_VALUE
/**
 * @brief Default value of USBPHYC tuning control register
 *
 * The value of the USBPHYC tuning control register (USBPHYC_TUNE) is used by
 * the USB HS PHY controller for the tuning interface of the internal
 * USB HS PHY, please refer the Reference Manual for STM32F72xxx and STM32F73xxx
 * for details.
 *
 * The value as defined in the [STM32CubeF7 HAL Driver MCU Component for F7]
 * (https://bit.ly/3es9eFA) is used as default value.
 * If necessary, it can be overridden by the board configuration in
 * `periph_conf.h` by defining the value before this file is included.
 */
#define USBPHYC_TUNE_VALUE  0x00000f13U
#endif

/**
 * @brief Common USB OTG HS configuration
 */
static const dwc2_usb_otg_fshs_config_t dwc2_usb_otg_fshs_config[] = {
    {
        .periph   = USB_OTG_HS_PERIPH_BASE,
        .type     = DWC2_USB_OTG_HS,
        .phy      = DWC2_USB_OTG_PHY_UTMI,
        .rcc_mask = RCC_AHB1ENR_OTGHSEN,
        .irqn     = OTG_HS_IRQn,
        .ahb      = AHB1,
        .dm       = GPIO_PIN(PORT_B, 14),
        .dp       = GPIO_PIN(PORT_B, 15),
        .af       = GPIO_AF10,
        .phy_tune = USBPHYC_TUNE_VALUE,
    }
};

/**
 * @brief Number of available USB OTG peripherals
 */
#define USBDEV_NUMOF           ARRAY_SIZE(dwc2_usb_otg_fshs_config)

#ifdef __cplusplus
}
#endif

#endif /* CFG_USB_OTG_HS_PHY_UTMI_H */
/** @} */
