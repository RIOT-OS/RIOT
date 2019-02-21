/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Low-level MTD flash drive implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "common.h"
#include "irq_arch.h"
#include "log.h"
#include "mtd.h"

#include "c_types.h"
#include "esp/spiflash.h"
#include "spi_flash.h"
#include "sdk/rom.h"

#define SDK_FLASH_FUNCTIONS

/* the external pointer to the system MTD device */
mtd_dev_t* mtd0 = 0;

mtd_dev_t  _flash_dev;
mtd_desc_t _flash_driver;

/* forward declaration of mtd functions */
static int _flash_init  (mtd_dev_t *dev);
static int _flash_read  (mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size);
static int _flash_write (mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size);
static int _flash_erase (mtd_dev_t *dev, uint32_t addr, uint32_t size);
static int _flash_power (mtd_dev_t *dev, enum mtd_power_state power);

static uint32_t _flash_beg;  /* first byte addr of the flash drive in SPI flash */
static uint32_t _flash_end;  /* first byte addr after the flash drive in SPI flash */
static uint32_t _flash_size; /* resulting size of the flash drive in SPI flash */

#define SPIFFS_FLASH_BEGIN 0x80000 /* TODO determine real possible value */

void flash_drive_init (void)
{
    DEBUG("%s\n", __func__);

    _flash_driver.init  = &_flash_init;
    _flash_driver.read  = &_flash_read;
    _flash_driver.write = &_flash_write;
    _flash_driver.erase = &_flash_erase;
    _flash_driver.power = &_flash_power;

    _flash_beg  = SPIFFS_FLASH_BEGIN;
    _flash_end  = flashchip->chip_size - 5 * flashchip->sector_size;
    _flash_size = _flash_end - _flash_beg;

    _flash_dev.driver = &_flash_driver;
    _flash_dev.sector_count = _flash_size / flashchip->sector_size;

    mtd0 = &_flash_dev;

    _flash_dev.pages_per_sector = flashchip->sector_size / flashchip->page_size;
    _flash_dev.page_size = flashchip->page_size;

    DEBUG("%s flashchip chip_size=%d block_size=%d sector_size=%d page_size=%d\n", __func__,
          flashchip->chip_size, flashchip->block_size,
          flashchip->sector_size, flashchip->page_size);
    DEBUG("%s flash_dev sector_count=%d pages_per_sector=%d page_size=%d\n", __func__,
          _flash_dev.sector_count, _flash_dev.pages_per_sector, _flash_dev.page_size);
    DEBUG("\n");
}

static int _flash_init  (mtd_dev_t *dev)
{
    DEBUG("%s dev=%p driver=%p\n", __func__, dev, &_flash_driver);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);

    #ifdef SPI_FLASH_CHIP_SIZE
    if (SPI_FLASH_CHIP_SIZE <= SPIFFS_FLASH_BEGIN) {
    #else
    if (flashchip->chip_size <= SPIFFS_FLASH_BEGIN) {
    #endif
        LOG_ERROR("Flash size is equal or less than %d Byte, "
                  "SPIFFS cannot be used\n", SPIFFS_FLASH_BEGIN);
        return -ENODEV;
    }

    return 0;
}

#define SPI_FLASH_BUF_SIZE 64
uint8_t _flash_buf[SPI_FLASH_BUF_SIZE];

static int _flash_read  (mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    DEBUG("%s dev=%p addr=%08x size=%u buf=%p\n", __func__, dev, addr, size, buff);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);
    CHECK_PARAM_RET (buff != NULL, -ENOTSUP);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (_flash_beg + addr + size <= _flash_end, -EOVERFLOW);

    #ifndef SDK_FLASH_FUNCTIONS
    bool result = spiflash_read (_flash_beg + addr, buff, size);
    return result ? (int)size : -EIO;
    #else
    critical_enter();

    /* it would be great if would work in that way, but would be too easy :-( */
    /* memcpy(buff, (void*)(_flash_beg + addr + 0x40200000), size); */

    int result = SPI_FLASH_RESULT_OK;
    uint32_t len = size;

    /* if addr is not 4 byte aligned, we need to read the first full word */
    if (addr & 0x3) {
        uint32_t word_addr = addr & ~0x3;
        uint32_t pos_in_word = addr & 0x3;
        uint32_t len_in_word = 4 - pos_in_word;
        len_in_word = (len_in_word < len) ? len_in_word : len;

        result = spi_flash_read (_flash_beg + word_addr, (uint32_t*)_flash_buf, 4);
        memcpy(buff, _flash_buf + pos_in_word, len_in_word);

        buff  = (uint8_t*)buff + len_in_word;
        addr += len_in_word;
        len  -= len_in_word;
    }

    /* read all full words, maximum SPI_FLASH_BUF_SIZE in one write operation */
    while (result == SPI_FLASH_RESULT_OK && len > 4) {
        uint32_t len_full_words = len & ~0x3;

        if (len_full_words > SPI_FLASH_BUF_SIZE) {
            len_full_words = SPI_FLASH_BUF_SIZE;
        }

        result |= spi_flash_read (_flash_beg + addr, (uint32_t*)_flash_buf, len_full_words);
        memcpy(buff, _flash_buf, len_full_words);

        buff  = (uint8_t*)buff + len_full_words;
        addr += len_full_words;
        len  -= len_full_words;
    }

    /* if there is some remaining, we need to prepare last word */
    if (result == SPI_FLASH_RESULT_OK && len) {
        result |= spi_flash_read (_flash_beg + addr, (uint32_t*)_flash_buf, 4);
        memcpy(buff, _flash_buf, len);
    }

    critical_exit();
    return (result == SPI_FLASH_RESULT_OK) ? (int)size : -EIO;
    #endif
}

static int _flash_write (mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size)
{
    DEBUG("%s dev=%p addr=%08x size=%u buf=%p\n", __func__, dev, addr, size, buff);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);
    CHECK_PARAM_RET (buff != NULL, -ENOTSUP);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (_flash_beg + addr + size <= _flash_end, -EOVERFLOW);

    #ifndef SDK_FLASH_FUNCTIONS

    bool result = spiflash_write (_flash_beg + addr, (uint8_t*)buff, size);
    return result ? (int)size : -EIO;

    #else

    critical_enter();

    int result = SPI_FLASH_RESULT_OK;
    uint32_t len = size;

    /* if addr is not 4 byte aligned, we need to prepare first full word */
    if (addr & 0x3) {
        uint32_t word_addr = addr & ~0x3;
        uint32_t pos_in_word = addr & 0x3;
        uint32_t len_in_word = 4 - pos_in_word;
        len_in_word = (len_in_word < len) ? len_in_word : len;

        result = spi_flash_read (_flash_beg + word_addr, (uint32_t*)_flash_buf, 4);
        memcpy(_flash_buf + pos_in_word, buff, len_in_word);
        result |= spi_flash_write (_flash_beg + word_addr, (uint32_t*)_flash_buf, 4);

        buff  = (uint8_t*)buff + len_in_word;
        addr += len_in_word;
        len  -= len_in_word;
    }

    /* write all full words, maximum SPI_FLASH_BUF_SIZE in one write operation */
    while (result == SPI_FLASH_RESULT_OK && len > 4) {
        uint32_t len_full_words = len & ~0x3;

        if (len_full_words > SPI_FLASH_BUF_SIZE) {
            len_full_words = SPI_FLASH_BUF_SIZE;
        }

        memcpy(_flash_buf, buff, len_full_words);
        result |= spi_flash_write (_flash_beg + addr, (uint32_t*)_flash_buf, len_full_words);

        buff  = (uint8_t*)buff + len_full_words;
        addr += len_full_words;
        len  -= len_full_words;
    }

    /* if there is some remaining, we need to prepare last word */
    if (result == SPI_FLASH_RESULT_OK && len) {
        result |= spi_flash_read (_flash_beg + addr, (uint32_t*)_flash_buf, 4);
        memcpy(_flash_buf, buff, len);
        result |= spi_flash_write (_flash_beg + addr, (uint32_t*)_flash_buf, 4);
    }

    critical_exit();
    return (result == SPI_FLASH_RESULT_OK) ? (int)size : -EIO;
    #endif
}

static int _flash_erase (mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    DEBUG("%s dev=%p addr=%08x size=%u\n", __func__, dev, addr, size);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (_flash_beg + addr + size <= _flash_end, -EOVERFLOW);

    /* size must be a multiple of sector_size && at least one sector */
    CHECK_PARAM_RET (size >= flashchip->sector_size, -ENOTSUP);
    CHECK_PARAM_RET (size % flashchip->sector_size == 0, -ENOTSUP)

    #ifndef SDK_FLASH_FUNCTIONS
    bool result = false;
    uint32_t count = size / flashchip->sector_size;
    while (count--) {
        uint32_t sec = _flash_beg + addr + count * flashchip->sector_size;
        if (!(result = spiflash_erase_sector (sec))) {
            break;
        }
    }
    return result ? 0 : -EIO;
    #else
    critical_enter();

    uint32_t result = SPI_FLASH_RESULT_OK;
    uint32_t count = size / flashchip->sector_size;
    while (count--) {
        uint32_t sec = (_flash_beg + addr) / flashchip->sector_size + count;
        if ((result = spi_flash_erase_sector (sec)) != SPI_FLASH_RESULT_OK) {
            break;
        }
    }

    critical_exit();
    return result;
    #endif
}

static int _flash_power (mtd_dev_t *dev, enum mtd_power_state power)
{
    DEBUG("%s\n", __func__);

    return -ENOTSUP;
}
