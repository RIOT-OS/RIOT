/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Low-level MTD flash drive implementation for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "architecture.h"
#include "board.h"
#include "esp_common.h"
#include "irq_arch.h"
#include "log.h"

#include "mtd.h"

#include "esp_flash_data_types.h"
#include "esp_partition.h"

#ifdef MCU_ESP32

#include "rom/cache.h"
#include "rom/spi_flash.h"
#include "esp_spi_flash.h"

#else /* MCU_ESP32 */

#include "rom_functions.h"
#include "spi_flash.h"

#endif /* MCU_ESP32 */

#define ENABLE_DEBUG 0
#include "debug.h"

#define ESP_PART_TABLE_ADDR         0x8000 /* TODO configurable as used in Makefile.include */
#define ESP_PART_TABLE_SIZE         0xC00
#define ESP_PART_ENTRY_SIZE         0x20
#define ESP_PART_ENTRY_MAGIC        ESP_PARTITION_MAGIC

/* the external pointer to the system MTD device */
mtd_dev_t* mtd0 = 0;

static mtd_dev_t  _flash_dev;
static mtd_desc_t _flash_driver;

#ifdef MCU_ESP8266

/* for source code compatibility with ESP32 SDK */
#define esp_rom_spiflash_chip_t     esp_spi_flash_chip_t
#define g_rom_flashchip             flashchip

/* defined in vendor/esp-idf/spi_flash.c */
extern esp_spi_flash_chip_t flashchip;
extern uint32_t spi_flash_get_id(void);

#endif /* MCU_ESP8266 */

/* forward declaration of mtd functions */
static int _flash_init  (mtd_dev_t *dev);
static int _flash_read  (mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size);
static int _flash_write (mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size);
static int _flash_write_page (mtd_dev_t *dev, const void *buff, uint32_t page,
                              uint32_t offset, uint32_t size);
static int _flash_erase (mtd_dev_t *dev, uint32_t addr, uint32_t size);
static int _flash_power (mtd_dev_t *dev, enum mtd_power_state power);

static uint32_t _flash_beg;  /* first byte addr of the flash drive in SPI flash */
static uint32_t _flash_end;  /* first byte addr after the flash drive in SPI flash */
static uint32_t _flash_size; /* resulting size of the flash drive in SPI flash */

static esp_rom_spiflash_chip_t* _flashchip = NULL;

/* flash_id determines the flash size in kByte */
static const uint32_t flash_sizes[] = {
    256,        /* last byte of id is 0x12 */
    512,        /* last byte of id is 0x13 */
    1 * 1024,   /* last byte of id is 0x14 */
    2 * 1024,   /* last byte of id is 0x15 */
    4 * 1024,   /* last byte of id is 0x16 */
    8 * 1024,   /* last byte of id is 0x17 */
   16 * 1024    /* last byte of id is 0x18 */
};

void spi_flash_drive_init (void)
{
    DEBUG("%s\n", __func__);

    _flashchip = &g_rom_flashchip;
    assert(_flashchip);

#ifdef MCU_ESP8266
    _flashchip->deviceId = spi_flash_get_id();
    uint8_t devid_lb = _flashchip->deviceId >> 16 & 0xff;
    if (devid_lb >= 0x12 && devid_lb <= 0x18) {
        _flashchip->chip_size = flash_sizes[devid_lb - 0x12] << 10;
    }
    else {
        LOG_TAG_WARNING("spi_flash", "could not determine flash size, "
                        "4 MBytes are used as default size\n");
        _flashchip->chip_size = 4 << 20;
    }
#endif /* MCU_ESP8266 */

    _flash_driver.init  = &_flash_init;
    _flash_driver.read  = &_flash_read;
    _flash_driver.write = &_flash_write;
    _flash_driver.write_page = &_flash_write_page;
    _flash_driver.erase = &_flash_erase;
    _flash_driver.power = &_flash_power;

    /* first, set the beginning of flash to 0x0 to read partition table */
    _flash_beg  = 0x0;
    _flash_end  = _flashchip->chip_size - 5 * _flashchip->sector_size;
    _flash_size = _flash_end - _flash_beg;

    /* read in partition table an determine the top of all partitions */
    uint32_t part_addr = ESP_PART_TABLE_ADDR;
    uint8_t WORD_ALIGNED part_buf[ESP_PART_ENTRY_SIZE];
    bool     part_read = true;
    uint32_t part_top = 0;
    /* Use intermediate cast to uintptr_t to silence false positive of
     * -Wcast-align. We aligned part_buf to word size via attribute */
    esp_partition_info_t* part = (esp_partition_info_t*)(uintptr_t)part_buf;

    while (part_read && part_addr < ESP_PART_TABLE_ADDR + ESP_PART_TABLE_SIZE) {
        spi_flash_read (part_addr, (void*)part_buf, ESP_PART_ENTRY_SIZE);

        if (part->magic == ESP_PART_ENTRY_MAGIC) {
            DEBUG("%s partition @%08x size=%08x label=%s\n", __func__,
                  part->pos.offset, part->pos.size, part->label);
            if (part->pos.offset + part->pos.size > part_top) {
                part_top = part->pos.offset + part->pos.size;
            }
            part_addr += ESP_PART_ENTRY_SIZE;
        }
        else {
            part_read = false;
        }
    }

#ifdef MCU_ESP32
    /* map the partition top address to next higher multiple of 0x100000 (1 MB) */
    part_top = (part_top + 0x100000) & ~0xfffff;
#else /* MCU_ESP32 */
    /* map the partition top address to next higher multiple of 0x80000 (512 kB) */
    part_top = (part_top + 0x80000) & ~0x7ffff;
#endif /* MCU_ESP32 */

    /*
     * if flash drive start address is not configured, use the determined
     * one otherwise check the configured one and use it
     */
    #if SPI_FLASH_DRIVE_START
    if (part_top > SPI_FLASH_DRIVE_START) {
        LOG_TAG_ERROR("spi_flash", "configured MTD start address in SPI Flash is to less\n");
    }
    else if (SPI_FLASH_DRIVE_START % _flashchip->sector_size) {
        LOG_TAG_ERROR("spi_flash", "configured start address has to be a "
                      "multiple of %d byte\n", _flashchip->sector_size);
        part_top = ((SPI_FLASH_DRIVE_START +
                     _flashchip->sector_size)) & ~(_flashchip->sector_size-1);
    }
    else {
        part_top = SPI_FLASH_DRIVE_START;
    }
    #endif

    /* second, change flash parameters according to partition table */
    _flash_beg  = part_top;
    _flash_end  = _flashchip->chip_size - 5 * _flashchip->sector_size;
    _flash_size = _flash_end - _flash_beg; /* MUST be at least 3 sectors (0x3000) */

    LOG_TAG_DEBUG("spi_flash", "MTD in SPI flash starts at address 0x%08x "
                  "with a size of %d kbytes\n", _flash_beg, _flash_size >> 10);

    _flash_dev.driver = &_flash_driver;
    _flash_dev.sector_count = _flash_size / _flashchip->sector_size;

    mtd0 = &_flash_dev;

    _flash_dev.pages_per_sector = _flashchip->sector_size / _flashchip->page_size;
    _flash_dev.page_size = _flashchip->page_size;

    DEBUG("%s flashchip chip_size=%d block_size=%d sector_size=%d page_size=%d\n", __func__,
          _flashchip->chip_size, _flashchip->block_size,
          _flashchip->sector_size, _flashchip->page_size);
    DEBUG("%s flash_dev sector_count=%d pages_per_sector=%d page_size=%d\n", __func__,
          _flash_dev.sector_count, _flash_dev.pages_per_sector, _flash_dev.page_size);
    DEBUG("\n");
}

#ifdef MCU_ESP32

#define RETURN_WITH_ESP_ERR_CODE(err) do { \
    switch (err) { \
        case ESP_ROM_SPIFLASH_RESULT_OK     : return ESP_OK; \
        case ESP_ROM_SPIFLASH_RESULT_ERR    : return ESP_ERR_FLASH_OP_FAIL; \
        case ESP_ROM_SPIFLASH_RESULT_TIMEOUT: return ESP_ERR_FLASH_OP_TIMEOUT; \
    } \
    return ESP_FAIL; \
} while(0)

static uint32_t _flash_buf[ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM / sizeof(uint32_t)];

esp_err_t IRAM_ATTR spi_flash_read(size_t addr, void *buff, size_t size)
{
    DEBUG("%s addr=%08x size=%u buf=%p\n", __func__, addr, size, buff);

    CHECK_PARAM_RET (buff != NULL, -ENOTSUP);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (addr + size <= _flash_end, -EOVERFLOW);

    int result = ESP_ROM_SPIFLASH_RESULT_OK;
    uint32_t len = size;

    /* if addr is not 4 byte aligned, we need to read the first full word */
    if (addr & 0x3) {
        uint32_t word_addr = addr & ~0x3;
        uint32_t pos_in_word = addr & 0x3;
        uint32_t len_in_word = 4 - pos_in_word;
        len_in_word = (len_in_word < len) ? len_in_word : len;

        /* disable interrupts and the cache */
        critical_enter();
        Cache_Read_Disable(PRO_CPU_NUM);

        result = esp_rom_spiflash_read(word_addr, _flash_buf, 4);
        memcpy(buff, (uint8_t *)_flash_buf + pos_in_word, len_in_word);

        /* enable interrupts and the cache */
        Cache_Read_Enable(PRO_CPU_NUM);
        critical_exit();

        buff  = (uint8_t*)buff + len_in_word;
        addr += len_in_word;
        len  -= len_in_word;
    }

    /* read all full words, maximum ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM
       in one read operation */
    while (len > 4 && result == ESP_ROM_SPIFLASH_RESULT_OK) {
        uint32_t len_full_words = len & ~0x3;
        if (len_full_words > ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM) {
            len_full_words = ESP_ROM_SPIFLASH_BUFF_BYTE_READ_NUM;
        }

        /* disable interrupts and the cache */
        critical_enter();
        Cache_Read_Disable(PRO_CPU_NUM);

        result |= esp_rom_spiflash_read(addr, _flash_buf, len_full_words);
        memcpy(buff, _flash_buf, len_full_words);

        /* enable interrupts and the cache */
        Cache_Read_Enable(PRO_CPU_NUM);
        critical_exit();

        buff  = (uint8_t*)buff + len_full_words;
        addr += len_full_words;
        len  -= len_full_words;
    }

    /* if there is some remaining, we need to prepare last word */
    if (len && result == ESP_ROM_SPIFLASH_RESULT_OK) {
        /* disable interrupts and the cache */
        critical_enter();
        Cache_Read_Disable(PRO_CPU_NUM);

        result |= esp_rom_spiflash_read(addr, _flash_buf, 4);
        memcpy(buff, _flash_buf, len);

        /* enable interrupts and the cache */
        Cache_Read_Enable(PRO_CPU_NUM);
        critical_exit();
    }

    /* return with the ESP-IDF error code that is mapped from ROM error code */
    RETURN_WITH_ESP_ERR_CODE(result);
}

esp_err_t IRAM_ATTR spi_flash_write(size_t addr, const void *buff, size_t size)
{
    DEBUG("%s addr=%08x size=%u buf=%p\n", __func__, addr, size, buff);

    CHECK_PARAM_RET (buff != NULL, -ENOTSUP);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (addr + size <= _flash_end, -EOVERFLOW);

    /* prepare for write access */
    int result = esp_rom_spiflash_unlock();
    uint32_t len = size;

    /* if addr is not 4 byte aligned, we need to prepare first full word */
    if (addr & 0x3 && result == ESP_ROM_SPIFLASH_RESULT_OK) {
        uint32_t word_addr = addr & ~0x3;
        uint32_t pos_in_word = addr & 0x3;
        uint32_t len_in_word = 4 - pos_in_word;
        len_in_word = (len_in_word < len) ? len_in_word : len;

        /* disable interrupts and the cache */
        critical_enter();
        Cache_Read_Disable(PRO_CPU_NUM);

        result |= esp_rom_spiflash_read(word_addr, _flash_buf, 4);
        memcpy((uint8_t *)_flash_buf + pos_in_word, buff, len_in_word);
        result |= esp_rom_spiflash_write(word_addr, _flash_buf, 4);

        /* enable interrupts and the cache */
        Cache_Read_Enable(PRO_CPU_NUM);
        critical_exit();

        buff  = (uint8_t*)buff + len_in_word;
        addr += len_in_word;
        len  -= len_in_word;
    }

    /* write all full words, maximum ESP_ROM_SPIFLASH_BUFF_BYTE_WRITE_NUM
       in one write operation */
    while (len > 4 && result == ESP_ROM_SPIFLASH_RESULT_OK) {
        uint32_t len_full_words = len & ~0x3;
        if (len_full_words > ESP_ROM_SPIFLASH_BUFF_BYTE_WRITE_NUM) {
            len_full_words = ESP_ROM_SPIFLASH_BUFF_BYTE_WRITE_NUM;
        }

        /* disable interrupts and the cache */
        critical_enter();
        Cache_Read_Disable(PRO_CPU_NUM);

        memcpy(_flash_buf, buff, len_full_words);
        result |= esp_rom_spiflash_write(addr, _flash_buf, len_full_words);

        /* enable interrupts and the cache */
        Cache_Read_Enable(PRO_CPU_NUM);
        critical_exit();

        buff  = (uint8_t*)buff + len_full_words;
        addr += len_full_words;
        len  -= len_full_words;
    }

    /* if there is some remaining, we need to prepare last word */
    if (len && result == ESP_ROM_SPIFLASH_RESULT_OK) {
        /* disable interrupts and the cache */
        critical_enter();
        Cache_Read_Disable(PRO_CPU_NUM);

        result |= esp_rom_spiflash_read(addr, _flash_buf, 4);
        memcpy(_flash_buf, buff, len);
        result |= esp_rom_spiflash_write(addr, _flash_buf, 4);

        /* enable interrupts and the cache */
        Cache_Read_Enable(PRO_CPU_NUM);
        critical_exit();
    }

    /* reset write access */
    esp_rom_spiflash_lock();

    /* return with the ESP-IDF error code that is mapped from ROM error code */
    RETURN_WITH_ESP_ERR_CODE(result);
}

esp_err_t IRAM_ATTR spi_flash_erase_sector(size_t sector)
{
    return spi_flash_erase_range(sector * _flashchip->sector_size, 1);
}

esp_err_t IRAM_ATTR spi_flash_erase_range(size_t addr, size_t size)
{
    /* size must be within the flash address space */
    CHECK_PARAM_RET (addr + size <= _flash_end, -EOVERFLOW);

    /* size must be a multiple of sector_size && at least one sector */
    CHECK_PARAM_RET (size >= _flashchip->sector_size, -ENOTSUP);
    CHECK_PARAM_RET (size % _flashchip->sector_size == 0, -ENOTSUP)

    /* prepare for write access */
    uint32_t result = esp_rom_spiflash_unlock();

    /* erase as many sectors as necessary */
    uint32_t sec = addr / _flashchip->sector_size;
    uint32_t cnt = size / _flashchip->sector_size;
    uint32_t sec_per_block = _flashchip->block_size / _flashchip->sector_size;

    while (cnt && result == ESP_ROM_SPIFLASH_RESULT_OK) {
        /* disable interrupts and the cache */
        critical_enter();
        Cache_Read_Disable(PRO_CPU_NUM);

        /* erase block-wise (64 kByte) if cnt is at least sec_per_block */
        if (cnt >= sec_per_block) {
            result = esp_rom_spiflash_erase_block (sec / sec_per_block);
            sec += sec_per_block;
            cnt -= sec_per_block;
        }
        else {
            result = esp_rom_spiflash_erase_sector (sec++);
            cnt--;
        }

        /* enable interrupts and the cache */
        Cache_Read_Enable(PRO_CPU_NUM);
        critical_exit();
    }

    /* reset write access */
    esp_rom_spiflash_lock();

    /* return with the ESP-IDF error code that is mapped from ROM error code */
    RETURN_WITH_ESP_ERR_CODE(result);
}

#endif /* MCU_ESP32 */

const esp_partition_t* esp_partition_find_first(esp_partition_type_t type,
                                                esp_partition_subtype_t subtype,
                                                const char* label)
{
    uint32_t info_addr = ESP_PART_TABLE_ADDR;
    uint8_t WORD_ALIGNED info_buf[ESP_PART_ENTRY_SIZE];
    bool info_read = true;

    /* use intermediate cast to uintptr_t to silence false positive of
     * -Wcast-align. We used an attribute to align info_buf to word boundary */
    esp_partition_info_t* info = (esp_partition_info_t*)(uintptr_t)info_buf;
    esp_partition_t* part;

    while (info_read && info_addr < ESP_PART_TABLE_ADDR + ESP_PART_TABLE_SIZE) {
        spi_flash_read (info_addr, (void*)info_buf, ESP_PART_ENTRY_SIZE);

        if (info->magic == ESP_PART_ENTRY_MAGIC) {
            DEBUG("%s partition @%08x size=%08x label=%s\n", __func__,
                  info->pos.offset, info->pos.size, info->label);
            if ((info->type == type) &&
                (info->subtype == subtype || subtype == ESP_PARTITION_SUBTYPE_ANY) &&
                (label == NULL || strcmp((const char*)info->label, label) == 0)) {
                part = malloc(sizeof(esp_partition_t));
                part->type = info->type;
                part->subtype = info->subtype;
                part->address = info->pos.offset;
                part->size = info->pos.size;
                part->encrypted = info->flags & PART_FLAG_ENCRYPTED;
                strncpy(part->label, (const char*)info->label, sizeof(info->label));
                part->label[sizeof(part->label) - 1] = 0x0;

                return part;
            }
            info_addr += ESP_PART_ENTRY_SIZE;
        }
        else {
            info_read = false;
        }
    }
    return NULL;
}

esp_err_t esp_partition_erase_range(const esp_partition_t* part,
                                    size_t addr, size_t size)
{
    CHECK_PARAM_RET(part != NULL, ESP_ERR_INVALID_ARG);

    /* start addr and size must be inside the partition */
    CHECK_PARAM_RET(addr <= part->size, ESP_ERR_INVALID_ARG);
    CHECK_PARAM_RET(addr + size <= part->size, ESP_ERR_INVALID_SIZE);
    /* start addr and size must be a multiple of sector size */
    CHECK_PARAM_RET(addr % SPI_FLASH_SEC_SIZE == 0, ESP_ERR_INVALID_ARG);
    CHECK_PARAM_RET(size % SPI_FLASH_SEC_SIZE == 0, ESP_ERR_INVALID_SIZE);

    return spi_flash_erase_range(part->address + addr, size);
}

static int _flash_init  (mtd_dev_t *dev)
{
    DEBUG("%s dev=%p driver=%p\n", __func__, dev, &_flash_driver);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);

    if (_flashchip->chip_size <= _flash_beg) {
        LOG_ERROR("Flash size is equal or less than %d Byte, "
                  "SPIFFS cannot be used\n", _flash_beg);
        return -ENODEV;
    }

    return 0;
}

static int _flash_read  (mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    DEBUG("%s dev=%p addr=%08x size=%u buf=%p\n", __func__, dev, addr, size, buff);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);
    CHECK_PARAM_RET (buff != NULL, -ENOTSUP);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (_flash_beg + addr + size <= _flash_end, -EOVERFLOW);

    return (spi_flash_read(_flash_beg + addr, buff, size) == ESP_OK) ? 0 : -EIO;
}

static int _flash_write (mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size)
{
    DEBUG("%s dev=%p addr=%08x size=%u buf=%p\n", __func__, dev, addr, size, buff);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);
    CHECK_PARAM_RET (buff != NULL, -ENOTSUP);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (_flash_beg + addr + size <= _flash_end, -EOVERFLOW);

    /* addr + size must be within a page */
    CHECK_PARAM_RET (size <= _flashchip->page_size, -EOVERFLOW);
    CHECK_PARAM_RET ((addr % _flashchip->page_size) + size <= _flashchip->page_size, -EOVERFLOW);

    return (spi_flash_write(_flash_beg + addr, buff, size) == ESP_OK) ? 0 : -EIO;
}

static int _flash_write_page (mtd_dev_t *dev, const void *buff, uint32_t page,  uint32_t offset,
                              uint32_t size)
{
    uint32_t addr = _flash_beg + page * _flashchip->page_size + offset;
    uint32_t remaining = _flashchip->page_size - offset;
    size = MIN(size, remaining);

    return (spi_flash_write(addr, buff, size) == ESP_OK) ? (int) size : -EIO;
}

static int _flash_erase (mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    DEBUG("%s dev=%p addr=%08x size=%u\n", __func__, dev, addr, size);

    CHECK_PARAM_RET (dev == &_flash_dev, -ENODEV);

    /* size must be within the flash address space */
    CHECK_PARAM_RET (_flash_beg + addr + size <= _flash_end, -EOVERFLOW);

    /* size must be a multiple of sector_size && at least one sector */
    CHECK_PARAM_RET (size >= _flashchip->sector_size, -EOVERFLOW);
    CHECK_PARAM_RET (size % _flashchip->sector_size == 0, -EOVERFLOW)

    return (spi_flash_erase_range(_flash_beg + addr, size) == ESP_OK) ? 0 : -EIO;
}

static int _flash_power (mtd_dev_t *dev, enum mtd_power_state power)
{
    DEBUG("%s\n", __func__);

    return -ENOTSUP;
}
