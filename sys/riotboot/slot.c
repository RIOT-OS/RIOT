/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Inria
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_slot
 * @{
 *
 * @file
 * @brief       Slot management functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "cpu.h"
#include "riotboot/slot.h"
#include "riotboot/hdr.h"

/*
 * Store the start addresses of each slot.
 * Take into account that CPU_FLASH_BASE represents the starting
 * address of the bootloader, thus the header is located after the
 * space reserved to the bootloader.
 */
const riotboot_hdr_t *const riotboot_slots[] = {
    (riotboot_hdr_t *)(CPU_FLASH_BASE + SLOT0_OFFSET),      /* First slot address -> firmware image */
#if NUM_SLOTS == 2
    (riotboot_hdr_t *)(CPU_FLASH_BASE + SLOT1_OFFSET),      /* Second slot address -> firmware image */
#endif
};

/* Calculate the number of slots */
const unsigned riotboot_slot_numof = ARRAY_SIZE(riotboot_slots);

static void _riotboot_slot_jump_to_image(const riotboot_hdr_t *hdr)
{
    cpu_jump_to_image(hdr->start_addr);
}

int riotboot_slot_current(void)
{
    uint32_t base_addr = cpu_get_image_baseaddr();

    for (unsigned i = 0; i < riotboot_slot_numof; i++) {
        const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(i);
        if (base_addr == hdr->start_addr) {
            return i;
        }
    }

    return -1;
}

int riotboot_slot_other(void)
{
    return riotboot_slot_current() ? 0 : 1;
}

void riotboot_slot_jump(unsigned slot)
{
    _riotboot_slot_jump_to_image(riotboot_slot_get_hdr(slot));
}

uint32_t riotboot_slot_get_image_startaddr(unsigned slot)
{
    return riotboot_slot_get_hdr(slot)->start_addr;
}

void riotboot_slot_dump_addrs(void)
{
    for (unsigned slot = 0; slot < riotboot_slot_numof; slot++) {
        const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(slot);

        printf("slot %u: metadata: %p image: 0x%08" PRIx32 "\n", slot,
               hdr,
               hdr->start_addr);
    }
}

const riotboot_hdr_t *riotboot_slot_get_hdr(unsigned slot)
{
    assert(slot < riotboot_slot_numof);

    return riotboot_slots[slot];
}

size_t riotboot_slot_offset(unsigned slot)
{
    return (size_t)riotboot_slot_get_hdr(slot) - CPU_FLASH_BASE;
}
