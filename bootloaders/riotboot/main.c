/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
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
 * @brief      Minimal riot-based bootloader
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "panic.h"
#include "periph/flashpage.h"
#include "riotboot/hdr.h"
#include "riotboot/slot.h"
#include "riotboot/flashwrite.h"
#include "riotboot/wdt.h"

void kernel_init(void)
{
    int slot;
    unsigned boot = 0;

    while (1) {
        int state = 0;
        uint32_t version = 0;
        slot = -1;
        for (unsigned i = 0; i < riotboot_slot_numof; i++) {
            const riotboot_hdr_t *riot_hdr = riotboot_slot_get_hdr(i);
            if (riotboot_slot_validate(i)) {
                /* skip slot if metadata broken */
                continue;
            }
            if (riotboot_hdr_get_start_addr(riot_hdr) !=
                riotboot_slot_get_image_startaddr(i)) {
                continue;
            }
            state = riotboot_hdr_get_img_state(riot_hdr);
            if (state == RIOTBOOT_HDR_IMG_STATE_DEACTIVATED ||
                state == RIOTBOOT_HDR_IMG_STATE_DISMISSED) {
                /* skip image which previously failed to boot or is not yet activated */
                continue;
            }
            if (slot == -1 || riotboot_hdr_get_version(riot_hdr) > version) {
                version = riotboot_hdr_get_version(riot_hdr);
                slot = i;
            }
        }

        if (slot != -1) {
            riotboot_hdr_t riot_hdr = *riotboot_slot_get_hdr(slot);
            if (riotboot_hdr_is_v2(&riot_hdr)) {
                boot = riotboot_hdr_get_boot_count(&riot_hdr);
                if (state == RIOTBOOT_HDR_IMG_STATE_ACTIVATED) {
                    /* not yet confirmed */
                    if (boot == CONFIG_RIOTBOOT_MAX_ATTEMPTS) {
                        /* failed to boot for CONFIG_RIOTBOOT_MAX_ATTEMPTS attempts */
                        riotboot_hdr_set_img_state(&riot_hdr, RIOTBOOT_HDR_IMG_STATE_DISMISSED);
                    }
                    else {
                        riotboot_hdr_set_boot_count(&riot_hdr, boot + 1);
                    }
                }
                if (memcmp(&riot_hdr, riotboot_slot_get_hdr(slot), sizeof(riot_hdr.v2))) {
                    flashpage_write(flashpage_addr(flashpage_page(riotboot_slot_get_hdr(slot))),
                                    &riot_hdr.v2, sizeof(riot_hdr.v2));
                }
                if (riotboot_hdr_get_img_state(&riot_hdr) == RIOTBOOT_HDR_IMG_STATE_DISMISSED) {
                    continue; /* try next slot */
                }
            }
            break; /* found a slot to boot */
        }
    }

#if IS_USED(MODULE_RIOTBOOT_WDT)
    riotboot_wdt_start(CONFIG_RIOTBOOT_WDT_TIMEOUT_MSEC << boot);
#endif
    riotboot_slot_jump(slot);

    /* serious trouble! nothing to boot */
    while (1) {}
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
