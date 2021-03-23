/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_usb_dfu
 * @{
 *
 * @file
 * @brief       USB Device Firmware Upgrade initialization for riotboot
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "thread.h"

#include "usb/usbus.h"
#include "usb/dfu.h"
#include "usb/usbus/dfu.h"
#include "riotboot/usb_dfu.h"

static usbus_dfu_device_t dfu;
static char _stack[USBUS_STACKSIZE];
static usbus_t usbus;

void riotboot_usb_dfu_init(unsigned forced)
{
    uint32_t *reset_addr = (uint32_t *)RIOTBOOT_DFU_ADDR;

    if (forced == 1 || *reset_addr == RIOTBOOT_MAGIC_NUMBER) {
        *reset_addr = 0;
        usbus_init(&usbus, usbdev_get_ctx(0));
        usbus_dfu_init(&usbus, &dfu, USB_DFU_PROTOCOL_DFU_MODE);
        usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);
    }
}
