/*
 * Copyright (C) 2020 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit_storage
 * @{
 *
 * @file
 * @brief       SUIT flashwrite storage module implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <string.h>

#include "architecture.h"
#include "kernel_defines.h"
#include "log.h"
#include "xfa.h"

#include "suit.h"
#include "suit/storage.h"
#include "suit/storage/flashwrite.h"
#include "riotboot/flashwrite.h"
#include "riotboot/slot.h"

XFA_USE(suit_storage_t, suit_storage_reg);

static inline suit_storage_flashwrite_t *_get_fw(suit_storage_t *storage)
{
    return container_of(storage, suit_storage_flashwrite_t, storage);
}

static int _flashwrite_init(suit_storage_t *storage)
{
    (void)storage;

    LOG_DEBUG("Storage size %" PRIuSIZE "\n", sizeof(suit_storage_flashwrite_t));

    return 0;
}

static int _flashwrite_start(suit_storage_t *storage,
                             const suit_manifest_t *manifest,
                             size_t len)
{
    (void)manifest;
    (void)len;
    suit_storage_flashwrite_t *fw = _get_fw(storage);
    int target_slot = riotboot_slot_other();

    return riotboot_flashwrite_init(&fw->writer, target_slot);
}

static int _flashwrite_write(suit_storage_t *storage,
                             const suit_manifest_t *manifest,
                             const uint8_t *buf, size_t offset, size_t len)
{
    (void)manifest;
    suit_storage_flashwrite_t *fw = _get_fw(storage);

    if (offset == 0) {
        if (len < RIOTBOOT_FLASHWRITE_SKIPLEN) {
            LOG_WARNING("_suit_flashwrite(): offset==0, len<4. aborting\n");
            return -1;
        }
        offset = RIOTBOOT_FLASHWRITE_SKIPLEN;
        buf += RIOTBOOT_FLASHWRITE_SKIPLEN;
        len -= RIOTBOOT_FLASHWRITE_SKIPLEN;
    }

    if (offset != fw->writer.offset) {
        LOG_ERROR("Unexpected offset: %u - expected: %u\n", (unsigned)offset,
                  (unsigned)fw->writer.offset);
        return SUIT_ERR_STORAGE;
    }

    return riotboot_flashwrite_putbytes(&fw->writer, buf, len, 1);
}

static int _flashwrite_finish(suit_storage_t *storage,
                              const suit_manifest_t *manifest)
{
    (void)manifest;
    suit_storage_flashwrite_t *fw = _get_fw(storage);

    return riotboot_flashwrite_flush(&fw->writer) <
           0 ? SUIT_ERR_STORAGE : SUIT_OK;
}

static int _flashwrite_install(suit_storage_t *storage,
                               const suit_manifest_t *manifest)
{
    (void)manifest;
    suit_storage_flashwrite_t *fw = _get_fw(storage);

    int fin = riotboot_flashwrite_finish(&fw->writer);
    if (fin < 0) {
        return SUIT_ERR_STORAGE;
    }
    return fin;
}

static int _flashwrite_read(suit_storage_t *storage, uint8_t *buf,
                            size_t offset, size_t len)
{
    suit_storage_flashwrite_t *fw = _get_fw(storage);

    const uint32_t magic = RIOTBOOT_MAGIC;
    int target_slot = riotboot_slot_other();
    size_t slot_size = riotboot_slot_size(target_slot);

    /* Insert the magic number */
    if (offset < (sizeof(magic))) {
        size_t prefix_to_copy = sizeof(magic) - offset;
        memcpy(buf, &magic + offset, prefix_to_copy);
        len -= prefix_to_copy;
        offset = sizeof(magic);
        buf += prefix_to_copy;

    }

#if CONFIG_RIOTBOOT_FLASHWRITE_RAW
    /* Insert the first chunk from the separate buffer here, there are cases
     * where the chunk size is 4 bytes and we can skip this because it only
     * contains the magic number already copied above. */
    if (offset < RIOTBOOT_FLASHPAGE_BUFFER_SIZE) {
        const size_t chunk_remaining =
            RIOTBOOT_FLASHPAGE_BUFFER_SIZE - sizeof(magic);
        /* How much of the first page must be copied */
        size_t firstpage_to_copy = len > chunk_remaining ?
            (chunk_remaining) : len;
        /* Copy the first buffer */
        memcpy(buf, fw->writer.firstblock_buf + offset, firstpage_to_copy);

        offset += firstpage_to_copy;
        buf += firstpage_to_copy;
        len -= firstpage_to_copy;
    }
#else
    (void)fw;
#endif /* CONFIG_RIOTBOOT_FLASHWRITE_RAW */

    if (offset + len > slot_size) {
        return -1;
    }

    uint8_t *slot = (uint8_t *)riotboot_slot_get_hdr(target_slot);

    memcpy(buf, slot + offset, len);
    return 0;
}

static bool _flashwrite_has_location(const suit_storage_t *storage,
                                     const char *location)
{
    (void)storage;

    /* Firmware matches at zero length string */
    return (location[0] == '\0');
}

static int _flashwrite_set_active_location(suit_storage_t *storage,
                                           const char *location)
{
    (void)storage;
    (void)location;
    return 0;
}

static bool _flashwrite_match_offset(const suit_storage_t *storage,
                                     size_t offset)
{
    (void)storage;

    int target_slot = riotboot_slot_other();
    uintptr_t slot_start = (uintptr_t)riotboot_slot_offset(target_slot);

    return (slot_start == (uintptr_t)offset);
}

static int _flashwrite_get_seq_no(const suit_storage_t *storage,
                                  uint32_t *seq_no)
{
    (void)storage;
    uint32_t max_seq_no = 0;
    bool valid = false;

    for (unsigned i = 0; i < riotboot_slot_numof; i++) {
        const riotboot_hdr_t *riot_hdr = riotboot_slot_get_hdr(i);
        if (riotboot_slot_validate(i)) {
            /* skip slot if metadata broken */
            continue;
        }
        if (!valid || riotboot_hdr_get_version(riot_hdr) > max_seq_no) {
            max_seq_no = riotboot_hdr_get_version(riot_hdr);
            valid = true;
        }
    }
    if (valid) {
        *seq_no = max_seq_no;
        return SUIT_OK;
    }

    return -1;
}

static int _flashwrite_set_seq_no(suit_storage_t *storage,
                                  uint32_t seq_no)
{
    (void)storage;
    int target_slot = riotboot_slot_other();
    const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(target_slot);

    if (riotboot_hdr_get_version(hdr) == seq_no) {
        return SUIT_OK;
    }

    return -1;
}

static const suit_storage_driver_t suit_storage_flashwrite_driver = {
    .init = _flashwrite_init,
    .start = _flashwrite_start,
    .write = _flashwrite_write,
    .finish = _flashwrite_finish,
    .read = _flashwrite_read,
    .install = _flashwrite_install,
    .has_location = _flashwrite_has_location,
    .set_active_location = _flashwrite_set_active_location,
    .match_offset = _flashwrite_match_offset,
    .get_seq_no = _flashwrite_get_seq_no,
    .set_seq_no = _flashwrite_set_seq_no,
    .separator = '\0',
};

static suit_storage_flashwrite_t suit_storage_flashwrite = {
    .storage = {
        .driver = &suit_storage_flashwrite_driver,
    },
};

XFA(suit_storage_t *, suit_storage_reg, 0) suit_storage_flashwrite_ptr = &suit_storage_flashwrite.storage;
