/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup drivers_mtd_native
 * @brief   MTD flash emulation for native
 * @author  Vincent Dupont <vincent@otakeys.com>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

#include "macros/utils.h"
#include "mtd.h"
#include "mtd_native.h"
#include "native_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _init(mtd_dev_t *dev)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;

    DEBUG("mtd_native: init, filename=%s\n", _dev->fname);

    FILE *f = real_fopen(_dev->fname, "r");

    if (!f) {
        DEBUG("mtd_native: init: creating file %s\n", _dev->fname);
        f = real_fopen(_dev->fname, "w+");
        if (!f) {
            return -EIO;
        }
        size_t size = dev->sector_count * dev->pages_per_sector * dev->page_size;
        for (size_t i = 0; i < size; i++) {
            real_fputc(0xff, f);
        }
    } else {
        real_fseek(f, 0, SEEK_END);
        size_t size = real_ftell(f);
        dev->sector_count = size / (dev->pages_per_sector * dev->page_size);
    }

    real_fclose(f);

    return 0;
}

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;
    size_t mtd_size = dev->sector_count * dev->pages_per_sector * dev->page_size;

    DEBUG("mtd_native: read from page %" PRIu32 " count %" PRIu32 "\n", addr, size);

    if (addr + size > mtd_size) {
        return -EOVERFLOW;
    }

    FILE *f = real_fopen(_dev->fname, "r");
    if (!f) {
        return -EIO;
    }
    real_fseek(f, addr, SEEK_SET);
    size_t nread = real_fread(buff, 1, size, f);
    real_fclose(f);

    return (nread == size) ? 0 : -EIO;
}

static int _write_page(mtd_dev_t *dev, const void *buff, uint32_t page, uint32_t offset,
                       uint32_t size)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;
    uint32_t addr = page * dev->page_size + offset;

    DEBUG("mtd_native: write from page %" PRIx32 ", offset 0x%" PRIx32 " count %" PRIu32 "\n",
          page, offset, size);

    if (page > dev->sector_count * dev->pages_per_sector) {
        return -EOVERFLOW;
    }

    if (offset > dev->page_size) {
        return -EOVERFLOW;
    }

    uint32_t remaining = dev->page_size - offset;
    size = MIN(remaining, size);

    FILE *f = real_fopen(_dev->fname, "r+");
    if (!f) {
        return -EIO;
    }
    real_fseek(f, addr, SEEK_SET);
    for (size_t i = 0; i < size; i++) {
        uint8_t c = real_fgetc(f);
        real_fseek(f, -1, SEEK_CUR);
        real_fputc(c & ((uint8_t*)buff)[i], f);
    }
    real_fclose(f);

    return size;
}

static int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;
    size_t mtd_size = dev->sector_count * dev->pages_per_sector * dev->page_size;
    size_t sector_size = dev->pages_per_sector * dev->page_size;

    DEBUG("mtd_native: erase from sector %" PRIu32 " count %" PRIu32 "\n", addr, size);

    if (addr + size > mtd_size) {
        return -EOVERFLOW;
    }
    if (((addr % sector_size) != 0) || ((size % sector_size) != 0)) {
        return -EOVERFLOW;
    }

    FILE *f = real_fopen(_dev->fname, "r+");
    if (!f) {
        return -EIO;
    }
    real_fseek(f, addr, SEEK_SET);
    for (size_t i = 0; i < size; i++) {
        real_fputc(0xff, f);
    }
    real_fclose(f);

    return 0;
}

static int _power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void) dev;
    (void) power;

    return -ENOTSUP;
}

const mtd_desc_t native_flash_driver = {
    .read = _read,
    .power = _power,
    .write_page = _write_page,
    .erase = _erase,
    .init = _init,
};
