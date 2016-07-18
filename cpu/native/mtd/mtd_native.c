/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "mtd.h"
#include "mtd_native.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static const char _fname[] = "MEMORY.bin";

static mtd_sta_t _init(mtd_dev_t *dev)
{
    (void) dev;

    DEBUG("mtd_native: init\n");

    FILE *f = fopen(_fname, "r");

    if (!f) {
        DEBUG("mtd_native: init: creating file %s\n" _fname);
        f = fopen(_fname, "w+");
        if (!f) {
            return MTD_STA_NODISK;
        }
        for (unsigned long i = 0; i < NATIVE_MTD_FLASH_SIZE; i++) {
            fputc(0xff, f);
        }
    }

    fclose(f);

    return MTD_STA_INIT;
}

static mtd_res_t _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    (void) dev;

    DEBUG("mtd_native: read from page %lu count %lu\n", addr, size);

    if (addr + size > NATIVE_MTD_FLASH_SIZE) {
        return MTD_RES_PARERR;
    }

    FILE *f = fopen(_fname, "r");
    if (!f) {
        return MTD_RES_NOTRDY;
    }
    fseek(f, addr, SEEK_SET);
    fread(buff, 1, size, f);
    fclose(f);

    return MTD_RES_OK;
}

static mtd_res_t _write(mtd_dev_t *mtd_dev, const void *buff, uint32_t addr, uint32_t size)
{
    (void) mtd_dev;

    DEBUG("mtd_native: write from page %lu count %lu\n", addr, size);

    if (addr + size > NATIVE_MTD_FLASH_SIZE) {
        return MTD_RES_PARERR;
    }
    if (((addr % NATIVE_MTD_SECTOR_SIZE) + size) > NATIVE_MTD_SECTOR_SIZE) {
        return MTD_RES_PARERR;
    }

    FILE *f = fopen(_fname, "r+");
    if (!f) {
        return MTD_RES_NOTRDY;
    }
    fseek(f, addr, SEEK_SET);
    for (unsigned long i = 0; i < size; i++) {
        uint8_t c = fgetc(f);
        fseek(f, -1, SEEK_CUR);
        fputc(c & buff[i], f);
    }
    fclose(f);

    return MTD_RES_OK;
}

static mtd_res_t _erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    (void) dev;

    DEBUG("mtd_native: erase from sector %lu count %lu\n", addr, size);

    if (addr + size > NATIVE_MTD_FLASH_SIZE) {
        return MTD_RES_PARERR;
    }
    if (((addr % NATIVE_MTD_SECTOR_SIZE) != 0) || ((size % NATIVE_MTD_SECTOR_SIZE) != 0)) {
        return MTD_RES_PARERR;
    }

    FILE *f = fopen(_fname, "r+");
    if (!f) {
        return MTD_RES_NOTRDY;
    }
    fseek(f, addr, SEEK_SET);
    for (unsigned long i = 0; i < size; i++) {
        fputc(0xff, f);
    }
    fclose(f);

    return MTD_RES_OK;
}

static mtd_res_t _ioctl(mtd_dev_t *dev, unsigned char ctl, void *buf)
{
    mtd_res_t ret = MTD_RES_WRPRT;
    (void) dev;

    return ret;
}


const mtdi_desc_t native_flash_driver = {
    .read = _read,
    .ioctl = _ioctl,
    .write = _write,
    .erase = _erase,
    .init = _init,
};

