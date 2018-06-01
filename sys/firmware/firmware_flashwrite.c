/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_firmware
 * @{
 *
 * @file
 * @brief       Firmware update helper functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <string.h>

#include "firmware/flashwrite.h"
#include "od.h"

#define LOG_PREFIX "firmware_flashwrite: "
#include "log.h"

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

int firmware_flashwrite_init(firmware_flashwrite_t *state, int target_slot,
        size_t offset)
{
    LOG_INFO(LOG_PREFIX "initializing update to target slot %i\n",
             target_slot);

    memset(state, 0, sizeof(firmware_flashwrite_t));

    state->offset = offset;
    state->target_slot = target_slot;
    state->flashpage = flashpage_page(firmware_get_metadata(target_slot));

    return 0;
}

int firmware_flashwrite_putbytes(firmware_flashwrite_t *state,
        const uint8_t *bytes, size_t len, bool more)
{
    LOG_INFO(LOG_PREFIX "processing bytes %u-%u\n", state->offset, state->offset + len - 1);

    while (len) {
        size_t flashpage_pos = state->offset % FLASHPAGE_SIZE;
        size_t flashpage_avail = FLASHPAGE_SIZE - flashpage_pos;

        size_t to_copy = min(flashpage_avail, len);

        memcpy(state->flashpage_buf + flashpage_pos, bytes, to_copy);
        flashpage_avail -= to_copy;

        state->offset += to_copy;
        flashpage_pos += to_copy;
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

int firmware_flashwrite_finish(firmware_flashwrite_t *state,
        firmware_metadata_t *metadata, size_t len)
{
    int res = -1;

    void *slot_start = firmware_get_metadata(state->target_slot);
    size_t metadata_space = (void*)metadata->start_addr - slot_start;

    void *firstpage;

    if (len < FLASHPAGE_SIZE) {
        firstpage = state->flashpage_buf;
        memset(firstpage, 0, metadata_space);
        memcpy(firstpage, metadata, len);
        memcpy(firstpage + metadata_space, \
               slot_start + metadata_space, \
               FLASHPAGE_SIZE - metadata_space);
    }
    else {
        firstpage = metadata;
    }

    int flashpage = flashpage_page(slot_start);
    if (flashpage_write_and_verify(flashpage, firstpage) != FLASHPAGE_OK) {
        LOG_WARNING(LOG_PREFIX "re-flashing first block failed!\n");
        goto out;
    }

    LOG_INFO(LOG_PREFIX "firmware flashing completed successfully\n");
    res = 0;

out:
    return res;
}
