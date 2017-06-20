/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @brief       mtd flash emulation for native
 *
 * @file
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <errno.h>

#include "mtd.h"
#include "mtd_native.h"

#include "native_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _init(mtd_dev_t *dev)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;

    DEBUG("mtd_native: init, filename=%s\n", _dev->fname);

    FILE *f = real_fopen(_dev->fname, "r");

    if (!f) {
        DEBUG("mtd_native: init: creating file %s\n", name);
        f = real_fopen(_dev->fname, "w+");
        if (!f) {
            return -EIO;
        }
        size_t size = dev->sector_count * dev->pages_per_sector * dev->page_size;
        for (size_t i = 0; i < size; i++) {
            real_fputc(0xff, f);
        }
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
    size = real_fread(buff, 1, size, f);
    real_fclose(f);

    return size;
}

static int _write(mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;
    size_t mtd_size = dev->sector_count * dev->pages_per_sector * dev->page_size;
    size_t sector_size = dev->pages_per_sector * dev->page_size;

    DEBUG("mtd_native: write from page %" PRIu32 " count %" PRIu32 "\n", addr, size);

    if (addr + size > mtd_size) {
        return -EOVERFLOW;
    }
    if (((addr % sector_size) + size) > sector_size) {
        return -EOVERFLOW;
    }

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
    .write = _write,
    .erase = _erase,
    .init = _init,
};

/** @} */
