/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
