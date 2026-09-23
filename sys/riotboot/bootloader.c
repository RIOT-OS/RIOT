/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 */

/**
 * @ingroup     sys_riotboot_bootloader
 * @{
 *
 * @file
 * @brief       RIOT bootloader common functions
 *
 * Only contains functions shared between different bootloaders
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.loc>
 * @}
 */

#include "compiler_hints.h"
#include "panic.h"
#if MODULE_PERIPH_FLASHPAGE
#  include "periph/flashpage.h"
#endif
#include "riotboot/hdr.h"
#include "riotboot/slot.h"

#include <string.h>

static int _get_slot(void)
{
    int slot = -1;
    uint32_t revision = 0;

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
        int state = riotboot_hdr_get_img_state(riot_hdr);
        if (state == RIOTBOOT_HDR_IMG_STATE_DEACTIVATED ||
            state == RIOTBOOT_HDR_IMG_STATE_DISMISSED) {
            /* skip image which previously failed to boot or is not yet activated */
            continue;
        }
        if (slot == -1 || riotboot_hdr_get_version(riot_hdr) > revision) {
            revision = riotboot_hdr_get_version(riot_hdr);
            slot = (int)i;
        }
    }

    return slot;
}

static int _set_boot(int slot, riotboot_hdr_t *riot_hdr)
{
    int state;
    unsigned boot = 0;

    *riot_hdr = *riotboot_slot_get_hdr(slot);
    if (riotboot_hdr_is_v2(riot_hdr)) {
        state = riotboot_hdr_get_img_state(riot_hdr);
        boot = riotboot_hdr_get_boot_count(riot_hdr);
        if (state == RIOTBOOT_HDR_IMG_STATE_ACTIVATED) {
            /* not yet confirmed */
            if (boot == CONFIG_RIOTBOOT_MAX_ATTEMPTS) {
                /* failed to boot for CONFIG_RIOTBOOT_MAX_ATTEMPTS attempts */
                state = RIOTBOOT_HDR_IMG_STATE_DISMISSED;
                riotboot_hdr_set_img_state(riot_hdr, state);
            }
            else {
                riotboot_hdr_set_boot_count(riot_hdr, boot + 1);
            }
#if MODULE_PERIPH_FLASHPAGE
            if (memcmp(&riot_hdr->v2, &riotboot_slot_get_hdr(slot)->v2, sizeof(riot_hdr->v2)) != 0) {
                flashpage_write(flashpage_addr(flashpage_page(riotboot_slot_get_hdr(slot))),
                                &riot_hdr->v2, sizeof(riot_hdr->v2));
            }
#endif
        }
        if (state == RIOTBOOT_HDR_IMG_STATE_DISMISSED ||
            state == RIOTBOOT_HDR_IMG_STATE_DEACTIVATED) {
            return -1; /* no valid slot to boot */
        }
    }

    return (int)boot;
}

int riotboot_bootloader_get_slot(riotboot_hdr_t *riot_hdr)
{
    int slot;
    int boot;

    do {
        slot = _get_slot();
        if (slot == -1) {
            return -1;
        }
        boot = _set_boot(slot, riot_hdr);
    } while (boot == -1);

    return slot;
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
