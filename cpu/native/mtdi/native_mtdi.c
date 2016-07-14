/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "mtdi.h"
#include "native_mtdi.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static const char _fname[] = "MEMORY.bin";

#ifndef NATIVE_MTDI_PAGE_SIZE
#define NATIVE_MTDI_PAGE_SIZE     256
#endif
#ifndef NATIVE_MTDI_PAGE_NUM
#define NATIVE_MTDI_PAGE_NUM      32768
#endif
#ifndef NATIVE_MTDI_SECTOR_SIZE
#define NATIVE_MTDI_SECTOR_SIZE   4096
#endif
#ifndef NATIVE_MTDI_SECTOR_NUM
#define NATIVE_MTDI_SECTOR_NUM    2048
#endif
#ifndef NATIVE_MTDI_FLASH_SIZE
#define NATIVE_MTDI_FLASH_SIZE    (NATIVE_MTDI_SECTOR_SIZE * NATIVE_MTDI_SECTOR_NUM)
#endif

static mtdi_sta_t _init(mtdi_dev_t *mtdi_dev)
{
    (void) mtdi_dev;

    FILE *f = fopen(_fname, "r");

    if (!f) {
        f = fopen(_fname, "w+");
        if (!f) {
            return MTDI_STA_NODISK;
        }
        for (unsigned long i = 0; i < NATIVE_MTDI_FLASH_SIZE; i++) {
            fputc(0xff, f);
        }
    }

    fclose(f);

    return MTDI_STA_INIT;
}

static mtdi_result_t _read(mtdi_dev_t *mtdi_dev, unsigned char *buff, unsigned long addr, unsigned long size)
{
    (void) mtdi_dev;

    DEBUG("native_mtdi: read from page %lu count %lu\n", addr, size);

    if (addr + size > NATIVE_MTDI_FLASH_SIZE) {
        return MTDI_RES_PARERR;
    }

    FILE *f = fopen(_fname, "r");
    if (!f) {
        return MTDI_RES_NOTRDY;
    }
    fseek(f, addr, SEEK_SET);
    fread(buff, 1, size, f);
    fclose(f);

    return MTDI_RES_OK;
}

static mtdi_result_t _write(mtdi_dev_t *mtdi_dev, const unsigned char *buff, unsigned long addr, unsigned long size)
{
    (void) mtdi_dev;

    DEBUG("native_mtdi: write from page %lu count %lu\n", addr, size);

    if (addr + size > NATIVE_MTDI_FLASH_SIZE) {
        return MTDI_RES_PARERR;
    }
    if (addr % NATIVE_MTDI_SECTOR_SIZE + size > NATIVE_MTDI_SECTOR_SIZE) {
        return MTDI_RES_PARERR;
    }

    FILE *f = fopen(_fname, "r+");
    if (!f) {
        return MTDI_RES_NOTRDY;
    }
    fseek(f, addr, SEEK_SET);
    for (unsigned long i = 0; i < size; i++) {
        uint8_t c = fgetc(f);
        fseek(f, -1, SEEK_CUR);
        fputc(c & buff[i], f);
    }
    fclose(f);

    return MTDI_RES_OK;
}

static mtdi_result_t _erase(mtdi_dev_t *mtdi_dev, unsigned long addr, unsigned long size)
{
    (void) mtdi_dev;

    DEBUG("native_mtdi: erase from sector %lu count %lu\n", addr, size);

    if (addr + size > NATIVE_MTDI_FLASH_SIZE) {
        return MTDI_RES_PARERR;
    }
    if (addr % NATIVE_MTDI_SECTOR_SIZE != 0 || size % NATIVE_MTDI_SECTOR_SIZE != 0) {
        return MTDI_RES_PARERR;
    }

    FILE *f = fopen(_fname, "r+");
    if (!f) {
        return MTDI_RES_NOTRDY;
    }
    fseek(f, addr, SEEK_SET);
    for (unsigned long i = 0; i < size; i++) {
        fputc(0xff, f);
    }
    fclose(f);

    return MTDI_RES_OK;
}

static mtdi_result_t _ioctl(mtdi_dev_t *dev, unsigned char ctl, void *buf)
{
    mtdi_result_t ret = MTDI_RES_WRPRT;
    (void) dev;

    switch (ctl) {
        case MTDI_GET_PAGE_SIZE:
            *((unsigned long*) buf) = NATIVE_MTDI_PAGE_SIZE;
            ret = MTDI_RES_OK;
            break;
        case MTDI_GET_PAGE_COUNT:
            *((unsigned long*) buf) = NATIVE_MTDI_PAGE_NUM;
            ret = MTDI_RES_OK;
            break;
        case MTDI_GET_SECTOR_SIZE:
            *((unsigned long*) buf) = NATIVE_MTDI_SECTOR_SIZE;
            ret = MTDI_RES_OK;
            break;
        case MTDI_GET_SECTOR_COUNT:
            *((unsigned long*) buf) = NATIVE_MTDI_SECTOR_NUM;
            ret = MTDI_RES_OK;
            break;
    }

    return ret;
}


const mtdi_desc_t native_flash_driver = {
    .mtdi_read = _read,
    .mtdi_ioctl = _ioctl,
    .mtdi_write = _write,
    .mtdi_erase = _erase,
    .mtdi_init = _init,
};

