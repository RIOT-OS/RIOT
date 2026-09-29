/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_tinyusb_dfu
 * @{
 * @file
 * @brief       TinyUSB specific DFU definitions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "riotboot/flashwrite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   tinyUSB DFU device interface context
 */
typedef struct tinyusb_dfu_device {
    bool skip_signature;                    /**< Skip RIOTBOOT signature status */
    uint8_t slot;                           /**< Download slot */
#ifdef MODULE_RIOTBOOT_TINYUSB_DFU
    riotboot_flashwrite_t writer;           /**< DFU firmware update state structure */
#endif
} tinyusb_dfu_device_t;

/**
 * @brief   Initialize the tinyUSB DFU device interface context
 */
void tinyusb_dfu_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
