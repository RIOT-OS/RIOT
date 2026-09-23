/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *               2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     bootloaders
 * @{
 *
 * @file
 * @brief      RIOT-based bootloader with USB-DFU support
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 * @author     Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "riotboot/bootloader.h"
#include "riotboot/slot.h"
#include "riotboot/usb_dfu.h"
#include "ztimer.h"

#include "riotboot/bootloader_selection.h"

#ifdef BTN_BOOTLOADER_PIN
#include "periph/gpio.h"
#endif

static bool _bootloader_alternative_mode(void)
{
#if defined (BTN_BOOTLOADER_PIN) && defined (BTN_BOOTLOADER_MODE)
    bool state;

    gpio_init(BTN_BOOTLOADER_PIN, BTN_BOOTLOADER_MODE);
    state = gpio_read(BTN_BOOTLOADER_PIN);
    /* If button configures w/ internal or external pullup, then it is an
       active-low, thus reverts the logic */
    if (BTN_BOOTLOADER_EXT_PULLUP || BTN_BOOTLOADER_MODE == GPIO_IN_PU ||
        BTN_BOOTLOADER_MODE == GPIO_OD_PU ) {
       return !state;
    } else {
        return state;
    }
#else
    return false;
#endif
}

void kernel_init(void)
{
    riotboot_hdr_t riot_hdr;
    int slot = riotboot_bootloader_get_slot(&riot_hdr);

    /* Init ztimer before starting DFU mode */
    ztimer_init();

    /* Flash the unused slot if magic word is set */
    riotboot_usb_dfu_init(0);

    if (slot != -1 && !_bootloader_alternative_mode()) {
        riotboot_slot_jump(slot);
    }

    /* Nothing to boot, stay in DFU mode to flash a slot */
    riotboot_usb_dfu_init(1);
}
