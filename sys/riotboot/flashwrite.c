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
#include <stddef.h>
#include <string.h>

#include "architecture.h"
#include "macros/utils.h"
#include "riotboot/flashwrite.h"
#include "riotboot/slot.h"
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
#ifdef FLASHPAGE_SIZE
    assert(offset <= FLASHPAGE_SIZE);
    /* the flashpage size must be a multiple of the riotboot flashpage buffer */
    static_assert(!(FLASHPAGE_SIZE % RIOTBOOT_FLASHPAGE_BUFFER_SIZE),
                  "Flashpage size must be a multiple of riotboot flashpage buffer.");
#else
    /* The flashpage buffer must be a multiple of the write block size */
    static_assert(!(RIOTBOOT_FLASHPAGE_BUFFER_SIZE %
                    FLASHPAGE_WRITE_BLOCK_SIZE),
                  "Flashpage buffer must be a multiple of write block size.");
#endif

    LOG_INFO(LOG_PREFIX "initializing update to target slot %i\n",
             target_slot);

    memset(state, 0, sizeof(riotboot_flashwrite_t));

    state->offset = offset;
    state->target_slot = target_slot;
    state->flashpage = flashpage_page(riotboot_slot_get_hdr(target_slot));
    if (CONFIG_RIOTBOOT_FLASHWRITE_RAW) {
        /* If RAW mode is not supported writing to flash is only done by flashpage_write_verify()
         * which performs a page erase and always writes a full page. */
        flashpage_erase(state->flashpage);
    }

    return 0;
}

int riotboot_flashwrite_flush(riotboot_flashwrite_t *state)
{
    if (CONFIG_RIOTBOOT_FLASHWRITE_RAW) {
        /* Check if there is leftover data in the buffer */
        size_t flashwrite_buffer_pos = state->offset %
                                       RIOTBOOT_FLASHPAGE_BUFFER_SIZE;
        if (flashwrite_buffer_pos == 0) {
            return 0;
        }
        uint8_t *slot_start =
            (uint8_t *)riotboot_slot_get_hdr(state->target_slot);
        /* Get the offset of the remaining chunk */
        size_t flashpage_pos = state->offset - flashwrite_buffer_pos;
        /* Write remaining chunk */
        flashpage_write(slot_start + flashpage_pos,
                        state->flashpage_buf,
                        RIOTBOOT_FLASHPAGE_BUFFER_SIZE);
    }
    else {
        if (flashpage_write_and_verify(state->flashpage,
                                       state->flashpage_buf) != FLASHPAGE_OK) {
            LOG_WARNING(LOG_PREFIX "error writing flashpage %u!\n",
                        state->flashpage);
            return -1;
        }
    }
    return 0;
}

int riotboot_flashwrite_putbytes(riotboot_flashwrite_t *state,
                                 const uint8_t *bytes, size_t len, bool more)
{
    LOG_DEBUG(LOG_PREFIX "processing bytes %" PRIuSIZE "-%" PRIuSIZE "\n", state->offset,
              state->offset + len - 1);

    while (len) {
        /* Position within the page, calculated from state->offset by
         * subtracting the start offset of the current page */
        size_t flashpage_pos = state->offset -
                               ((uint8_t *)flashpage_addr(state->flashpage) -
                                (uint8_t *)riotboot_slot_get_hdr(state->target_slot));
        size_t flashwrite_buffer_pos = state->offset % RIOTBOOT_FLASHPAGE_BUFFER_SIZE;
        size_t flashpage_avail = RIOTBOOT_FLASHPAGE_BUFFER_SIZE -
                                 flashwrite_buffer_pos;

        size_t to_copy = min(flashpage_avail, len);

        if (CONFIG_RIOTBOOT_FLASHWRITE_RAW &&
            flashpage_pos == flashpage_size(state->flashpage)) {
            /* Erase the next page */
            state->flashpage++;
            flashpage_pos = 0;
            flashpage_erase(state->flashpage);
        }
        memcpy(state->flashpage_buf + flashwrite_buffer_pos, bytes, to_copy);
        flashpage_avail -= to_copy;
        if ((!flashpage_avail) || (!more)) {
#if CONFIG_RIOTBOOT_FLASHWRITE_RAW  /* Guards access to state::firstblock_buf */
            if (!state->offset) {
                /* Skip flashing the first block, store it for later to flash it
                 * during the flashwrite_finish function, when payload contains the boot header */
                memcpy(state->firstblock_buf,
                       state->flashpage_buf, RIOTBOOT_FLASHPAGE_BUFFER_SIZE);
            }
            else {
                void *addr = flashpage_addr(state->flashpage);
                flashpage_write((uint8_t *)addr + flashpage_pos,
                                state->flashpage_buf,
                                RIOTBOOT_FLASHPAGE_BUFFER_SIZE);
            }
#else
            if (!state->offset) {
                /* erase RIOTBOOT_MAGIC */
                memset(state->flashpage_buf, FLASHPAGE_ERASE_STATE, sizeof(RIOTBOOT_MAGIC));
            }
            int res = flashpage_write_and_verify(state->flashpage,
                                                 state->flashpage_buf);
            if (res != FLASHPAGE_OK) {
                LOG_WARNING(LOG_PREFIX "error writing flashpage %u!\n",
                            state->flashpage);
                return -1;
            }
            state->flashpage++;
#endif
        }
        state->offset += to_copy;
        bytes += to_copy;
        len -= to_copy;
    }

    return 0;
}

int riotboot_flashwrite_invalidate(int slot)
{
    if (riotboot_slot_numof == 1) {
        LOG_WARNING(LOG_PREFIX "abort, only one slot configured\n");
        return -1;
    }
    if (riotboot_slot_validate(1 - slot) != 0) {
        LOG_WARNING(
            LOG_PREFIX "abort, can not erase slot[%d], other slot[%d] is invalid\n", slot,
            1 - slot);
        return -2;
    }

    /* invalidate header (checksum and magic number must be invalidated),
       write the whole header to avoid running in memory alignment issues
       with FLASHPAGE_WRITE_BLOCK_SIZE */
    riotboot_hdr_t tmp_hdr;

    memset(&tmp_hdr, (~FLASHPAGE_ERASE_STATE), sizeof(riotboot_hdr_t));

    flashpage_write((void *)riotboot_slot_get_hdr(slot), &tmp_hdr,
                    sizeof(riotboot_hdr_t));

    return 0;
}

int riotboot_flashwrite_invalidate_latest(void)
{
    int _slot_to_revert;

    _slot_to_revert = (riotboot_slot_get_hdr(riotboot_slot_other())->version
                       > riotboot_slot_get_hdr(riotboot_slot_current())->version)
                      ? riotboot_slot_other() : riotboot_slot_current();
    return riotboot_flashwrite_invalidate(_slot_to_revert);
}

int riotboot_flashwrite_finish_raw(riotboot_flashwrite_t *state,
                                   const uint8_t *bytes, size_t len)
{
#ifndef PERIPH_FLASHPAGE_CUSTOM_PAGESIZES
    assert(len <= FLASHPAGE_SIZE);
#endif

    uint8_t *slot_start = (uint8_t *)riotboot_slot_get_hdr(state->target_slot);

#if IS_ACTIVE(CONFIG_RIOTBOOT_FLASHWRITE_RAW)
    const uint8_t *data = bytes;
    size_t remaining = len;
    size_t to_copy;
    if (!state->skip_len) {
        /* If there is a skip_len, the first block contains the header and must be
         * re-flashed to include the magic number and checksum. */
        to_copy = MIN(remaining, sizeof(state->firstblock_buf));
        memcpy(state->firstblock_buf, data, to_copy);
        flashpage_write(slot_start, state->firstblock_buf, sizeof(state->firstblock_buf));
        data += to_copy;
        remaining -= to_copy;
    }
    while (remaining) {
        to_copy = MIN(remaining, sizeof(state->flashpage_buf));
        memcpy(state->flashpage_buf, data, to_copy);
        flashpage_write(slot_start + (data - bytes), state->flashpage_buf, to_copy);
        data += to_copy;
        remaining -= to_copy;
    }
    if (state->skip_len > len) {
        remaining = state->skip_len - len;
        memset(state->flashpage_buf, 0, remaining);
        while (remaining) {
            to_copy = MIN(remaining, sizeof(state->flashpage_buf));
            flashpage_write(slot_start + len, state->flashpage_buf, to_copy);
            len += to_copy;
            remaining -= to_copy;
        }
    }
#else
    int flashpage = flashpage_page(slot_start);
    flashpage_read(flashpage, state->flashpage_buf);
    memcpy(state->flashpage_buf, bytes, len);
    if (flashpage_write_and_verify(flashpage, state->flashpage_buf) != FLASHPAGE_OK) {
        LOG_ERROR(LOG_PREFIX "re-flashing first block failed!\n");
        return -1;
    }
#endif
    LOG_INFO(LOG_PREFIX "riotboot flashing completed successfully\n");
    return 0;
}
