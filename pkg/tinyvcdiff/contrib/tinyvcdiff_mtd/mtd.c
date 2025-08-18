/*
 * Copyright (C) 2022 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "architecture.h"
#include "vcdiff_mtd.h"
#include "assert.h"
#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static int _erase (void *dev, size_t offset, size_t len)
{
    int rc;
    vcdiff_mtd_t *mtd = dev;
    size_t addr = offset + len;
    size_t page = addr / mtd->dev->page_size;
    size_t sector = page / mtd->dev->pages_per_sector;
    size_t cnt = sector - mtd->next_erase_sector + 1;

    /* early exit if all sectors are already in the erased state */
    if (cnt == 0) {
        return 0;
    }

    /* erase sectors */
    rc = mtd_erase_sector(mtd->dev, mtd->next_erase_sector, cnt);
    if (rc != 0) {
        return rc;
    }

    mtd->next_erase_sector += cnt;

    return 0;
}

static int _read (void *dev, uint8_t *dest, size_t offset, size_t len)
{
    vcdiff_mtd_t *mtd = dev;

    /* mtd_read_page will take care of calculation the right page */
    int rc = mtd_read_page(mtd->dev, dest, 0, offset, len);

    return rc;
}

static int _align_write (vcdiff_mtd_t *mtd, uint8_t **src, size_t *len)
{
    size_t alignment_offset;
    size_t copy_len;

    alignment_offset = mtd->offset % CONFIG_TINYVCDIFF_MTD_WRITE_SIZE;
    if (alignment_offset == 0) {
        /* we are already aligned */
        return 0;
    }

    /* check how many bytes have to be copied to be aligned */
    copy_len = CONFIG_TINYVCDIFF_MTD_WRITE_SIZE - alignment_offset;
    if (copy_len > *len) {
        copy_len = *len;
    }

    /* copy unaligned bytes to the write buffer */
    memcpy(&mtd->write_buffer[alignment_offset], *src, copy_len);
    alignment_offset += copy_len;
    mtd->offset += copy_len;
    *len -= copy_len;
    *src += copy_len;
    DEBUG("_align_write: buffered %" PRIuSIZE "B for alignment\n", copy_len);

    if (alignment_offset < CONFIG_TINYVCDIFF_MTD_WRITE_SIZE) {
        /* we haven't collected enough bytes, yet */
        return 0;
    }

    assert(alignment_offset == CONFIG_TINYVCDIFF_MTD_WRITE_SIZE);

    DEBUG("_align_write: write buffered data\n");

    /* write buffer to mtd
     * mtd_read_page will take care of calculation the right page
     * it's safe to call mtd_read_page_raw: the erase driver already
     * prepared the sector for writing */
    return mtd_write_page_raw(mtd->dev, mtd->write_buffer, 0,
                              mtd->offset - alignment_offset,
                              CONFIG_TINYVCDIFF_MTD_WRITE_SIZE);
}

static int _write (void *dev, uint8_t *src, size_t offset, size_t len)
{
    int rc;
    size_t to_copy;
    vcdiff_mtd_t *mtd = dev;

    assert(offset == mtd->offset);
    (void)offset;   /* avoid compilation problems with NDEBUG */

    DEBUG("_write: 0x%" PRIxSIZE " + %" PRIuSIZE "B\n", mtd->offset, len);

    /* align writes */
    rc = _align_write(dev, &src, &len);
    if (rc < 0) {
        /* an error occurred */
        return rc;
    } else if (len == 0) {
        /* no bytes are left */
        return 0;
    }

    assert((mtd->offset % CONFIG_TINYVCDIFF_MTD_WRITE_SIZE) == 0);

    /* copy all aligned data */
    to_copy = (len / CONFIG_TINYVCDIFF_MTD_WRITE_SIZE) * CONFIG_TINYVCDIFF_MTD_WRITE_SIZE;
    rc = mtd_write_page_raw(mtd->dev, src, 0, mtd->offset, to_copy);
    if (rc < 0) {
        return rc;
    }
    mtd->offset += to_copy;
    len -= to_copy;
    src += to_copy;
    if (len == 0) {
        /* no bytes are left */
        return 0;
    }

    assert(len < CONFIG_TINYVCDIFF_MTD_WRITE_SIZE);

    /* copy remaining bytes into write_buffer */
    memcpy(mtd->write_buffer, src, len);
    mtd->offset += len;
    DEBUG("_write: buffered %" PRIuSIZE "B for alignment\n", len);

    return rc;
}

static int _flush (void *dev)
{
    vcdiff_mtd_t *mtd = dev;
    int rc;
    uint8_t buf[CONFIG_TINYVCDIFF_MTD_WRITE_SIZE];
    size_t alignment_offset;

    alignment_offset = mtd->offset % CONFIG_TINYVCDIFF_MTD_WRITE_SIZE;
    if (alignment_offset == 0) {
        /* we are already aligned -> no bytes are left in the buffer */
        return 0;
    }

    DEBUG("_flush: write last %" PRIuSIZE "B\n", alignment_offset);

    /* get present bytes from MTD to pad alignment */
    rc = mtd_read_page(mtd->dev, buf, 0, mtd->offset - alignment_offset,
                       CONFIG_TINYVCDIFF_MTD_WRITE_SIZE);
    if (rc < 0) {
        return rc;
    }

    /* pad write buffer */
    memcpy(&mtd->write_buffer[alignment_offset], &buf[alignment_offset],
           CONFIG_TINYVCDIFF_MTD_WRITE_SIZE - alignment_offset);

    /* write last buffer */
    rc = mtd_write_page_raw(mtd->dev, mtd->write_buffer, 0,
                            mtd->offset - alignment_offset,
                            CONFIG_TINYVCDIFF_MTD_WRITE_SIZE);

    return rc;
}

const vcdiff_driver_t vcdiff_mtd_driver = {
    .erase = _erase,
    .read = _read,
    .write = _write,
    .flush = _flush
};
