/*
 * Copyright (C) 2019 Inria
 *               2019 Freie Universität Berlin
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_flashwrite
 * @{
 *
 * @file
 * @brief       Firmware update helper functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "riotboot/flashwrite.h"
#include "od.h"

#define LOG_PREFIX "riotboot_flashwrite: "
#include "log.h"

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

size_t riotboot_flashwrite_slotsize(
        const riotboot_flashwrite_t *state)
{
    return riotboot_slot_size(state->target_slot);
}

int riotboot_flashwrite_init_raw(riotboot_flashwrite_t *state, int target_slot,
                             size_t offset)
{
    assert(offset <= FLASHPAGE_SIZE);

    LOG_INFO(LOG_PREFIX "initializing update to target slot %i\n",
             target_slot);

    memset(state, 0, sizeof(riotboot_flashwrite_t));

    state->offset = offset;
    state->target_slot = target_slot;
    state->flashpage = flashpage_page((void *)riotboot_slot_get_hdr(target_slot));

    return 0;
}

int riotboot_flashwrite_flush(riotboot_flashwrite_t *state)
{
    if (flashpage_write_and_verify(state->flashpage, state->flashpage_buf) != FLASHPAGE_OK) {
        LOG_WARNING(LOG_PREFIX "error writing flashpage %u!\n", state->flashpage);
        return -1;
    }
    return 0;
}

int riotboot_flashwrite_putbytes(riotboot_flashwrite_t *state,
                                 const uint8_t *bytes, size_t len, bool more)
{
    LOG_DEBUG(LOG_PREFIX "processing bytes %u-%u\n", state->offset, state->offset + len - 1);

    while (len) {
        size_t flashpage_pos = state->offset % FLASHPAGE_SIZE;
        size_t flashpage_avail = FLASHPAGE_SIZE - flashpage_pos;

        size_t to_copy = min(flashpage_avail, len);

        memcpy(state->flashpage_buf + flashpage_pos, bytes, to_copy);
        flashpage_avail -= to_copy;

        state->offset += to_copy;
        bytes += to_copy;
        len -= to_copy;
        if ((!flashpage_avail) || (!more)) {
            if (flashpage_write_and_verify(state->flashpage, state->flashpage_buf) != FLASHPAGE_OK) {
                LOG_WARNING(LOG_PREFIX "error writing flashpage %u!\n", state->flashpage);
                return -1;
            }
            state->flashpage++;
        }
    }

    return 0;
}

int riotboot_flashwrite_finish_raw(riotboot_flashwrite_t *state,
                               const uint8_t *bytes, size_t len)
{
    assert(len <= FLASHPAGE_SIZE);

    int res = -1;

    uint8_t *slot_start = (uint8_t *)riotboot_slot_get_hdr(state->target_slot);

    uint8_t *firstpage;

    if (len < FLASHPAGE_SIZE) {
        firstpage = state->flashpage_buf;
        memcpy(firstpage, bytes, len);
        memcpy(firstpage + len,
               slot_start + len,
               FLASHPAGE_SIZE - len);
    }
    else {
        firstpage = (void *)bytes;
    }

    int flashpage = flashpage_page((void *)slot_start);
    if (flashpage_write_and_verify(flashpage, firstpage) != FLASHPAGE_OK) {
        LOG_WARNING(LOG_PREFIX "re-flashing first block failed!\n");
        goto out;
    }

    LOG_INFO(LOG_PREFIX "riotboot flashing completed successfully\n");
    res = 0;

out:
    return res;
}
