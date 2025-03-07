/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotconf_slot
 * @{
 *
 * @file
 * @brief       RIOT configuration slot API
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "byteorder.h"
#include "log.h"
#include "macros/utils.h"
#include "riotconf/hdr.h"
#include "riotconf/slot.h"
#include "riotconf/storage.h"

#define ENABLE_DEBUG 0
#include "debug.h"

riotconf_slot_t riotconf_slot_highest_seq(riotconf_slot_compat_cb_t compatible)
{
    riotconf_slot_t slot = -1;
    uint32_t highest = 0;
    riotconf_hdr_t hdr;
    for (riotconf_slot_t i = 0; i < RIOTCONF_SLOT_NUMOF; i++) {
        if (riotconf_slot_validate(i, &hdr) < 0) {
            continue;
        }
        if (hdr.sequence >= highest) {
            if (compatible && !compatible(&hdr)) {
                continue;
            }
            highest = hdr.sequence;
            slot = i;
        }
    }
    return slot;
}

riotconf_slot_t riotconf_slot_other(riotconf_slot_t current)
{
    riotconf_slot_t slot = -1;
    uint32_t lowest = UINT32_MAX;
    riotconf_hdr_t hdr;
    for (riotconf_slot_t i = 0; i < RIOTCONF_SLOT_NUMOF; i++) {
        if (i == current) {
            continue;
        }
        if (!riotconf_slot_validate(i, &hdr)) {
            slot = i;
            break; /* any invalid slot may be overridden for an update */
        }
        else if (hdr.sequence <= lowest) {
            slot = i;
        }
    }
    return slot;
}

int riotconf_slot_set_state(riotconf_slot_t slot, uint32_t state)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return -ENODEV;
    }
    riotconf_hdr_t hdr;
    if (riotconf_slot_validate(slot, &hdr)) {
        return -EINVAL;
    }
    /* the validation flag is reserved for riotconf */
    state &= ~((uint32_t)RIOTCONF_HDR_STATE_CHECKSUM_VALIDATED);
    hdr.state = state | (hdr.state & RIOTCONF_HDR_STATE_CHECKSUM_VALIDATED);
    if (riotconf_storage_start_write(dev)) {
        return -EIO;
    }
    riotconf_hdr_hton(&hdr);
    riotconf_storage_finish_write(dev, &hdr);
    return 0;
}

int riotconf_slot_read(riotconf_slot_t slot, void *data, size_t offset, size_t size)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return -ENODEV;
    }
    offset += sizeof(riotconf_hdr_t);
    return riotconf_storage_read(dev, data, offset, size);
}

int riotconf_slot_start_read(riotconf_slot_t slot, void **buf, size_t *size)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return -ENODEV;
    }
    riotconf_storage_start_read(dev, buf, size);
    riotconf_storage_read(dev, *buf, 0, sizeof(riotconf_hdr_t));
    return 0;
}

void riotconf_slot_finish_read(riotconf_slot_t slot)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return;
    }
    riotconf_storage_finish_read(dev);
}

int riotconf_slot_start_write(riotconf_slot_t slot, riotconf_hdr_t *hdr)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return -ENODEV;
    }
    int ret = riotconf_storage_start_write(dev);
    if (hdr) {
        memcpy(hdr, dev->sector_buf, sizeof(*hdr));
        riotconf_hdr_ntoh(hdr);
    }
    return ret;
}

int riotconf_slot_write(riotconf_slot_t slot, const void *data, size_t offset, size_t size)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return -ENODEV;
    }
    offset += sizeof(riotconf_hdr_t);
    return riotconf_storage_write(dev, data, offset, size);
}

static void _riotconf_checksum(riotconf_storage_t *dev, void *sec_buf, size_t sec_size, riotconf_hdr_t *hdr)
{
    riotconf_hdr_t *h = (riotconf_hdr_t *)sec_buf;
    riotconf_hdr_checksum_ctx_t chk;
    riotconf_hdr_checksum_init(&chk, h);
    riotconf_hdr_state_t state_backup = hdr->state;
    memcpy(hdr, h, sizeof(*hdr));
    size_t total = sizeof(*h) + ntohl(h->size);
    size_t size = MIN(total, sec_size);
    riotconf_hdr_checksum_update(&chk, sec_buf, size);
    total -= size;
    for (unsigned i = 1; total > 0; i++) {
        size = MIN(total, sec_size);
        riotconf_storage_read(dev, sec_buf, i * sec_size, size);
        riotconf_hdr_checksum_update(&chk, sec_buf, size);
        total -= size;
    }
    riotconf_hdr_checksum_final(&chk, hdr);
    riotconf_hdr_ntoh(hdr);
    hdr->state = state_backup;
}

void riotconf_slot_finish_write(riotconf_slot_t slot, uint32_t seq, uint32_t version, size_t size)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return;
    }
    riotconf_hdr_t hdr = {
        .magic = 0,
        .sequence = seq,
        .version = version,
        .size = size,
        .state = 0
    };
    riotconf_hdr_hton(&hdr);
    riotconf_storage_finish_write(dev, &hdr);

    void *sector;
    size_t sector_size;
    riotconf_storage_start_read(dev, &sector, &sector_size);
    riotconf_storage_read(dev, sector, 0, sector_size);
    memcpy(sector, &hdr, sizeof(hdr));
    _riotconf_checksum(dev, sector, sector_size, &hdr);
    riotconf_storage_finish_read(dev);

    assert(hdr.sequence == seq);
    assert(hdr.version == version);
    assert(hdr.size == size);
    riotconf_hdr_hton(&hdr);
    riotconf_storage_start_write(dev);
    riotconf_storage_finish_write(dev, &hdr);
}

int riotconf_slot_validate(riotconf_slot_t slot, riotconf_hdr_t *hdr)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return -ENODEV;
    }
    void *sector;
    size_t sector_size;
    riotconf_storage_start_read(dev, &sector, &sector_size);
    if (riotconf_storage_read(dev, sector, 0, sector_size)) {
        return -EIO;
    }
    riotconf_hdr_t *h = (riotconf_hdr_t *)sector;
    memcpy(hdr, h, sizeof(*hdr));
    riotconf_hdr_ntoh(hdr);
    if (riotconf_hdr_validate(hdr)) {
        return -EINVAL;
    }
    uint32_t checksum = hdr->checksum;
    _riotconf_checksum(dev, sector, sector_size, hdr);
    riotconf_storage_finish_read(dev);
    if (checksum != hdr->checksum) {
        LOG_DEBUG("%s: riotconf_slot checksum invalid\n", __func__);
        return -EINVAL;
    }
    /* technically this write could fail making it invalid */
    if (!(hdr->state & RIOTCONF_HDR_STATE_CHECKSUM_VALIDATED)) {
        hdr->state |= RIOTCONF_HDR_STATE_CHECKSUM_VALIDATED;
        riotconf_hdr_hton(hdr);
        riotconf_storage_start_write(dev);
        riotconf_storage_finish_write(dev, hdr);
        riotconf_hdr_ntoh(hdr);
    }
    return 0;
}

int riotconf_slot_invalidate(riotconf_slot_t slot)
{
    riotconf_storage_t *dev = riotconf_storage_get(slot);
    if (!dev) {
        return -ENODEV;
    }
    if (riotconf_storage_start_write(dev)) {
        return -EIO;
    }
    riotconf_hdr_t hdr = { 0 };
    riotconf_storage_finish_write(dev, &hdr);
    return 0;
}

#if IS_USED(MODULE_RIOTCONF_SLOT_PRINT)
#include "od.h"
void riotconf_slot_print(riotconf_slot_t slot)
{
    void *sec;
    size_t sec_size;
    int ret = riotconf_slot_start_read(slot, &sec, &sec_size);
    if (ret) {
        DEBUG("Cannot read slot %u\n", slot);
        return;
    }
    riotconf_hdr_t h = *(riotconf_hdr_t *)sec;
    riotconf_hdr_ntoh(&h);
    if (h.magic != RIOTCONF_MAGIC) {
        riotconf_slot_finish_read(slot);
        DEBUG("Slot %u is not valid\n", slot);
        return;
    }
    printf("Slot: %d, seq: %"PRIu32", version: %"PRIu32", size %"PRIu32"\n",
           slot, h.sequence, h.version, h.size);
    size_t off = 0;
    while (off < h.size) {
        riotconf_slot_read(slot, sec, off, sec_size);
        od_hex_dump(sec, MIN(sec_size, h.size - off), OD_WIDTH_DEFAULT);
        off += sec_size;
    }
    riotconf_slot_finish_read(slot);
}
#endif
