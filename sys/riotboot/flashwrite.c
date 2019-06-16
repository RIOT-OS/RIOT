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

#include <string.h>

#include "riotboot/flashwrite.h"
#include "od.h"

#define LOG_PREFIX "riotboot_flashwrite: "
#include "log.h"

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

size_t riotboot_flashwrite_slotsize(const riotboot_flashwrite_t *state)
{
    switch (state->target_slot) {
        case 0: return SLOT0_LEN;
#if NUMOF_SLOTS == 2
        case 1: return SLOT1_LEN;
#endif
        default: return 0;
    }
}

int riotboot_flashwrite_init_raw(riotboot_flashwrite_t *state, int target_slot,
                                 size_t offset)
{
    assert(offset <= FLASHPAGE_SIZE);

    LOG_INFO(LOG_PREFIX "initializing update to target slot %i\n",
             target_slot);

    memset(state, 0, sizeof(riotboot_flashwrite_t));
    /* set flashchunck_buf to erase state values*/
    memset(state->flashchunck_buf, FLASH_ERASE_STATE, offset);

    state->offset = offset;
    state->target_slot = target_slot;
    state->flashchunck = flashpage_page((void *)riotboot_slot_get_hdr(target_slot));
#ifdef MODULE_PERIPH_FLASHPAGE_RAW
    state->flashchunck = (FLASHCHUNK_PAGE * state->flashchunck);
    /* erase first page */
    flashpage_write((state->flashchunck / FLASHCHUNK_PAGE), NULL);
#endif
    return 0;
}

int riotboot_flashwrite_putbytes(riotboot_flashwrite_t *state,
                                 const uint8_t *bytes, size_t len, bool more)
{
    LOG_INFO(LOG_PREFIX "processing bytes %u-%u\n", state->offset,
             state->offset + len - 1);

    while (len) {
        size_t flashchunck_pos = state->offset % FLASHCHUNK_SIZE;
        size_t flashchunck_avail = FLASHCHUNK_SIZE - flashchunck_pos;

        size_t to_copy = min(flashchunck_avail, len);

        memcpy(state->flashchunck_buf + flashchunck_pos, bytes, to_copy);
        flashchunck_avail -= to_copy;

        state->offset += to_copy;
        bytes += to_copy;
        len -= to_copy;

#ifdef MODULE_PERIPH_FLASHPAGE_RAW
        if (((int)flashchunk_addr(state->flashchunck) % FLASHPAGE_SIZE) == 0) {
            LOG_DEBUG(LOG_PREFIX "Erasing page %u \n",
                        (state->flashchunck / FLASHCHUNK_PAGE));
            flashpage_write((state->flashchunck / FLASHCHUNK_PAGE), NULL);
        }
        if ((!flashchunck_avail) || (!more)) {
            if (flashpage_write_and_verify_raw(flashchunk_addr(state->flashchunck),
                                               state->flashchunck_buf,
                                               FLASHCHUNK_SIZE) != FLASHPAGE_OK) {
                LOG_WARNING(LOG_PREFIX "error writing flashchunck %u!\n",
                            state->flashchunck);
                return -1;
            }
            state->flashchunck++;
        }
#else
        if ((!flashchunck_avail) || (!more)) {
            LOG_WARNING(LOG_PREFIX "writing flashchunck %u!\n", state->flashchunck);
            if (flashpage_write_and_verify(state->flashchunck,
                                           state->flashchunck_buf) != FLASHPAGE_OK) {
                LOG_WARNING(LOG_PREFIX "error writing flashchunck %u!\n",
                            state->flashchunck);
                return -1;
            }
            state->flashchunck++;
        }
#endif
    }
    return 0;
}

int riotboot_flashwrite_finish_raw(riotboot_flashwrite_t *state,
                                   const uint8_t *bytes, size_t len)
{
    assert(len <= FLASHCHUNK_SIZE);

    int res = -1;
    uint8_t *slot_start = (uint8_t *)riotboot_slot_get_hdr(state->target_slot);
    uint8_t *firstchunck;

    if (len < FLASHCHUNK_SIZE) {
        firstchunck = state->flashchunck_buf;
        memcpy(firstchunck, slot_start, FLASHCHUNK_SIZE);
        memcpy(firstchunck, bytes, len);
    }
    else {
        firstchunck = (void *)bytes;
    }
#ifdef MODULE_PERIPH_FLASHPAGE_RAW
    if (flashpage_write_and_verify_raw((void *)slot_start, firstchunck,
                                       FLASHCHUNK_SIZE) != FLASHPAGE_OK) {
        LOG_WARNING(LOG_PREFIX "re-flashing first chunk failed!\n");
        goto out;
    }
#else
    int flashpage = flashpage_page((void *)slot_start);
    if (flashpage_write_and_verify(flashpage, firstchunck) != FLASHPAGE_OK) {
        LOG_WARNING(LOG_PREFIX "re-flashing first chunk failed!\n");
        goto out;
    }
#endif
    LOG_INFO(LOG_PREFIX "riotboot flashing completed successfully\n");
    res = 0;

out:
    return res;
}
