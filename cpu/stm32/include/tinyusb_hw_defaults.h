/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

/**
 * @ingroup     pkg_tinyusb
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       STM32 specific default configurations for tinyUSB
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */


#include "periph_conf.h"

#if !DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Definition of ports used by tinyUSB host and device stack
 *
 * The tinyUSB device driver used for the STM32 USB-OTG controller supposes
 * that the USB-OTG-FS port is port 0 and the USB-OTG-HS port is port 1.
 * Therefore, the following default definitions can be used:
 *
 * - If both USB OTG FS and USB OTG HS ports are used, the device stack uses
 *   port 0 and the host stack uses port 1.
 * - If only USB OTG HS is used, both the device and the host stack use port 1.
 * - In all other cases, both the device and the host stack use port 0.
 *   This also applies if only the USB FS controller is used.
 *
 * @warning
 * - tinyUSB does not support host mode for STM32 MCUs yet.
 * - tinyUSB does not support to use multiple ports with device stack
 * @{
 */
#if defined(DWC2_USB_OTG_HS_ENABLED) && defined(DWC2_USB_OTG_FS_ENABLED)

#ifndef TINYUSB_TUD_RHPORT
#define TINYUSB_TUD_RHPORT  0
#endif

#ifndef TINYUSB_TUH_RHPORT
#define TINYUSB_TUH_RHPORT  1
#endif

#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#define CFG_TUSB_RHPORT1_MODE   (OPT_MODE_HOST | OPT_MODE_HIGH_SPEED)

#elif defined(DWC2_USB_OTG_HS_ENABLED)

#ifndef TINYUSB_TUD_RHPORT
#define TINYUSB_TUD_RHPORT  1
#endif

#ifndef TINYUSB_TUH_RHPORT
#define TINYUSB_TUH_RHPORT  1
#endif

/*
 * Only if the module `periph_usbdev_hs_utmi` or `periph_usbdev_hs_ulpi` is
 * enabled for the HS port, a HS PHY is used and OPT_MODE_HIGH_SPEED must be
 * set. Otherwise, the built-in on-chip FS PHY is used and
 * OPT_MODE_DEFAULT_SPEED should be set so tinyUSB can select the correct speed.
 */
#if (MODULE_PERIPH_USBDEV_HS_UTMI || MODULE_PERIPH_USBDEV_HS_ULPI)
#define TINYUSB_OPT_SPEED       OPT_MODE_HIGH_SPEED
#else
#define TINYUSB_OPT_SPEED       OPT_MODE_DEFAULT_SPEED
#endif

/*
 * Since tinyUSB does not support host mode for STM32 MCUs yet, only
 * OPT_MODE_DEVICE is enabled for the port. Once tinyUSB supports the host mode,
 * OPT_MODE_HOST could be added to CFG_TUSB_RHPORT1_MODE
 */
#define CFG_TUSB_RHPORT1_MODE   (OPT_MODE_DEVICE | TINYUSB_OPT_SPEED)

#else

#ifndef TINYUSB_TUD_RHPORT
#define TINYUSB_TUD_RHPORT  0
#endif

#ifndef TINYUSB_TUH_RHPORT
#define TINYUSB_TUH_RHPORT  0
#endif

/*
 * Since tinyUSB does not support host mode for STM32 MCUs yet, only
 * OPT_MODE_DEVICE is enabled for the port. Once tinyUSB supports the host mode,
 * OPT_MODE_HOST could be added to CFG_TUSB_RHPORT0_MODE.
 */
#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)

#endif
/** @} */

#ifdef USB_HS_PHYC
/**
 * @brief    Definition of HSE clock value
 *
 * If the CMSIS defines that the internal UTMI HS PHY is used, the Synopsys DWC2
 * driver needs the definition of the HSE clock value.
 */
#define HSE_VALUE           CONFIG_CLOCK_HSE
#endif

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
/** @} */
