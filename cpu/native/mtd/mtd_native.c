/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <errno.h>

#include "mtd.h"
#include "mtd_native.h"

#include "native_internal.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static mtd_sta_t _init(mtd_dev_t *dev)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;
    const char *name;

    DEBUG("mtd_native: init\n");

    if (_native_mtd_file != NULL) {
        name = _native_mtd_file;
    }
    else {
        name = _dev->fname;
    }

    FILE *f = real_fopen(name, "r");

    if (!f) {
        DEBUG("mtd_native: init: creating file %s\n", name);
        f = real_fopen(name, "w+");
        if (!f) {
            return MTD_STA_NODISK;
        }
        for (unsigned long i = 0; i < MTD_NATIVE_FLASH_SIZE; i++) {
            real_fputc(0xff, f);
        }
    }

    real_fclose(f);

    return MTD_STA_INIT;
}

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;
    const char *name;

    DEBUG("mtd_native: read from page %" PRIu32 " count %" PRIu32 "\n", addr, size);

    if (_native_mtd_file != NULL) {
        name = _native_mtd_file;
    }
    else {
        name = _dev->fname;
    }

    if (addr + size > MTD_NATIVE_FLASH_SIZE) {
        return -EOVERFLOW;
    }

    FILE *f = real_fopen(name, "r");
    if (!f) {
        return -EAGAIN;
    }
    real_fseek(f, addr, SEEK_SET);
    size = real_fread(buff, 1, size, f);
    real_fclose(f);

    return size;
}

static int _write(mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size)
{
    mtd_native_dev_t *_dev = (mtd_native_dev_t*) dev;
    const char *name;

    DEBUG("mtd_native: write from page %" PRIu32 " count %" PRIu32 "\n", addr, size);

    if (_native_mtd_file != NULL) {
        name = _native_mtd_file;
    }
    else {
        name = _dev->fname;
    }

    if (addr + size > MTD_NATIVE_FLASH_SIZE) {
        return -EOVERFLOW;
    }
    if (((addr % MTD_NATIVE_SECTOR_SIZE) + size) > MTD_NATIVE_SECTOR_SIZE) {
        return -EOVERFLOW;
    }

    FILE *f = real_fopen(name, "r+");
    if (!f) {
        return -EAGAIN;
    }
    fseek(f, addr, SEEK_SET);
    for (unsigned long i = 0; i < size; i++) {
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
    const char *name;

    DEBUG("mtd_native: erase from sector %" PRIu32 " count %" PRIu32 "\n", addr, size);

    if (_native_mtd_file != NULL) {
        name = _native_mtd_file;
    }
    else {
        name = _dev->fname;
    }

    if (addr + size > MTD_NATIVE_FLASH_SIZE) {
        return -EOVERFLOW;
    }
    if (((addr % MTD_NATIVE_SECTOR_SIZE) != 0) || ((size % MTD_NATIVE_SECTOR_SIZE) != 0)) {
        return -EOVERFLOW;
    }

    FILE *f = real_fopen(name, "r+");
    if (!f) {
        return -EAGAIN;
    }
    fseek(f, addr, SEEK_SET);
    for (unsigned long i = 0; i < size; i++) {
        real_fputc(0xff, f);
    }
    real_fclose(f);

    return size;
}

static int _ioctl(mtd_dev_t *dev, unsigned char ctl, void *buf)
{
    int ret = -EAGAIN;
    (void) dev;
    (void) ctl;
    (void) buf;

    return ret;
}


const mtd_desc_t native_flash_driver = {
    .read = _read,
    .ioctl = _ioctl,
    .write = _write,
    .erase = _erase,
    .init = _init,
};

