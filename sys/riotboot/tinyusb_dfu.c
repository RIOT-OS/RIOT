/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_riotboot_usb_dfu
 * @ingroup pkg_tinyusb_dfu
 * @{
 * @file    tinyUSB Device Firmware Upgrade initialization for riotboot
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "riotboot/magic.h"

#include "tinyusb.h"
#include "tinyusb_dfu.h"

#define ENABLE_DEBUG    0
#include "debug.h"

extern void tinyusb_dfu_init(void);

void riotboot_usb_dfu_init(unsigned forced)
{
    uint32_t *reset_addr = (uint32_t *)RIOTBOOT_MAGIC_ADDR;

    if (forced == 1 || *reset_addr == RIOTBOOT_MAGIC_NUMBER) {
        *reset_addr = 0;
        tinyusb_setup();
        tinyusb_dfu_init();
    }
}
