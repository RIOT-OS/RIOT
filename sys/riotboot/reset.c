/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_riotboot_serial
 * @{
 *
 * @file
 * @brief       Trigger reset to riotboot.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 * @}
 */

#include <string.h>
#include "periph/pm.h"
#include "riotboot/magic.h"

__attribute__((weak))
void usb_board_reset_in_bootloader(void)
{
    uint32_t *magic = (void *)(uintptr_t)RIOTBOOT_MAGIC_ADDR;

    irq_disable();
    *magic = RIOTBOOT_MAGIC;
    pm_reboot();
}
