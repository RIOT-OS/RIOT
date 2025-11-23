/*
 * SPDX-FileCopyrightText: 2020 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_riotboot_usb_dfu   Initialization of USB Device Firmware
 *                                     Upgrade for riotboot
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       USB DFU initialization for riotboot
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "riotboot/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name USB DFU Default slots name
 * @{
 */
#ifndef USB_DFU_MODE_SLOT0_NAME
#define USB_DFU_MODE_SLOT0_NAME         "RIOT-OS Slot 0"
#endif
#ifndef USB_DFU_MODE_SLOT1_NAME
#define USB_DFU_MODE_SLOT1_NAME         "RIOT-OS Slot 1"
#endif
#ifndef USB_APP_MODE_SLOT_NAME
#define USB_APP_MODE_SLOT_NAME          "RIOT-OS bootloader"
#endif

/** @} */

/**
 * @brief  Initialize usbus DFU for riotboot bootloader
 */
void riotboot_usb_dfu_init(unsigned forced);

#ifdef __cplusplus
}
#endif
