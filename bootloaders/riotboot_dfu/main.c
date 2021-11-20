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

#include "cpu.h"
#include "panic.h"
#include "riotboot/slot.h"
#include "riotboot/usb_dfu.h"
#include "ztimer.h"

#include "riotboot/bootloader_selection.h"

#ifdef BTN_BOOTLOADER_PIN
#include "periph/gpio.h"
#endif

static bool _bootloader_alternative_mode(void)
{
#ifdef BTN_BOOTLOADER_PIN
    gpio_init(BTN_BOOTLOADER_PIN, BTN_BOOTLOADER_MODE);
    return (bool)gpio_read(BTN_BOOTLOADER_PIN) != BTN_BOOTLOADER_INVERTED;
#else
    return false;
#endif
}

void kernel_init(void)
{
    uint32_t version = 0;
    int slot = -1;

    for (unsigned i = 0; i < riotboot_slot_numof; i++) {
        const riotboot_hdr_t *riot_hdr = riotboot_slot_get_hdr(i);
        if (riotboot_slot_validate(i)) {
            /* skip slot if metadata broken */
            continue;
        }
        if (riot_hdr->start_addr != riotboot_slot_get_image_startaddr(i)) {
            continue;
        }
        if (slot == -1 || riot_hdr->version > version) {
            version = riot_hdr->version;
            slot = i;
        }
    }

    /* Flash the unused slot if magic word is set */
    riotboot_usb_dfu_init(0);

    if (slot != -1 && !_bootloader_alternative_mode()) {
        riotboot_slot_jump(slot);
    }

    /* Init ztimer before starting DFU mode */
    ztimer_init();

    /* Nothing to boot, stay in DFU mode to flash a slot */
    riotboot_usb_dfu_init(1);
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
