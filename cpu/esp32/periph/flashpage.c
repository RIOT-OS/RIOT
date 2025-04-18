/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral flashpage interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <assert.h>

#include "architecture.h"
#include "cpu.h"
#include "irq.h"
#include "periph/flashpage.h"

#include "irq.h"
#include "log.h"

#include "esp_private/cache_utils.h"
#include "esp_flash.h"
#include "esp_flash_internal.h"
#include "hal/cache_hal.h"
#include "hal/mmu_hal.h"
#include "rom/cache.h"
#include "rom/spi_flash.h"
#include "soc/soc.h"
#include "soc/ext_mem_defs.h"
#include "spi_flash_mmap.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ESP_PART_TABLE_ADDR         0x8000 /* TODO configurable as used in Makefile.include */
#define ESP_PART_TABLE_SIZE         0xC00
#define ESP_PART_ENTRY_SIZE         0x20
#define ESP_PART_ENTRY_MAGIC        ESP_PARTITION_MAGIC

extern uint8_t _fp_mmu_start;
extern uint8_t _fp_mmu_end;
extern uint8_t _fp_mem_start;      /* start address in CPU address space */
extern uint8_t _fp_mem_end;
extern uint8_t _end_fw;

static uint32_t _fp_flash_start;    /* start address in flash */

void IRAM_ATTR esp_sync_cache(uint32_t target_addr, size_t len)
{
    DEBUG("%s sync cache from @0x%08"PRIx32" for %u\n",
          __func__, target_addr, len);

    for (uint32_t addr = target_addr; addr < (target_addr + len);
                                      addr += SPI_FLASH_MMU_PAGE_SIZE) {
#if defined(CPU_FAM_ESP32)
        extern void cache_sync(void);
        cache_sync();
#else
        cache_hal_invalidate_addr((uint32_t)addr, SPI_FLASH_MMU_PAGE_SIZE);
#endif
    }
}

void IRAM_ATTR esp_flashpage_init(void)
{
    /* CONFIG_ESP_FLASHPAGE_CAPACITY has to be a multiple of SPI_FLASH_MMU_PAGE_SIZE */
    assert((CONFIG_ESP_FLASHPAGE_CAPACITY % SPI_FLASH_MMU_PAGE_SIZE) == 0);
    assert((FLASHPAGE_ADDR_START % SPI_FLASH_MMU_PAGE_SIZE) == 0);

    DEBUG("%s pages in CPU address space @0x%08"PRIx32"...0x%08"PRIx32"\n", __func__,
          CPU_FLASH_BASE, CPU_FLASH_BASE + CONFIG_ESP_FLASHPAGE_CAPACITY - 1);

    _fp_flash_start = FLASHPAGE_ADDR_START;

    DEBUG("%s pages in flash @0x%08"PRIx32"...0x%08"PRIx32"\n", __func__,
          _fp_flash_start, _fp_flash_start + CONFIG_ESP_FLASHPAGE_CAPACITY - 1);

    uint32_t state = irq_disable();

    uint32_t p_numof = CONFIG_ESP_FLASHPAGE_CAPACITY / SPI_FLASH_MMU_PAGE_SIZE;
    uint32_t p_addr = FLASHPAGE_ADDR_START;
    uint32_t p_mmu = ((uint32_t)&_fp_mmu_start - SOC_DROM_LOW) / SPI_FLASH_MMU_PAGE_SIZE;

    while (p_numof--) {
        uint32_t p_flash = p_addr / SPI_FLASH_MMU_PAGE_SIZE;
        uint32_t mmu_addr = SOC_DROM_LOW + (p_mmu * SPI_FLASH_MMU_PAGE_SIZE);

        DEBUG("%s map MMU page %"PRIu32" @0x%08"PRIx32" to "
              "flash page %"PRIu32" @0x%08"PRIx32"\n", __func__,
              p_mmu, mmu_addr,
              p_flash, p_flash * SPI_FLASH_MMU_PAGE_SIZE);

        spi_flash_mmap_handle_t t_handle;
        const void *t_mem;
        esp_err_t res;

        res = spi_flash_mmap(p_addr, SPI_FLASH_MMU_PAGE_SIZE, SPI_FLASH_MMAP_DATA,
                             &t_mem, &t_handle);

        assert((uint32_t)t_mem == mmu_addr);
        DEBUG("%s map MMU page %"PRIu32" @0x%08"PRIx32" to "
              "flash page %"PRIu32" @0x%08"PRIx32" %s\n", __func__,
              p_mmu, (uint32_t)t_mem,
              p_flash, p_flash * SPI_FLASH_MMU_PAGE_SIZE,
              res == ESP_OK ? "OK" : "NOK");

        p_addr += SPI_FLASH_MMU_PAGE_SIZE;
        p_mmu++;
    }

    irq_restore(state);

    if (ENABLE_DEBUG) {
        spi_flash_mmap_dump();
    }
}

void IRAM_ATTR flashpage_erase(unsigned page)
{
    assert(page < FLASHPAGE_NUMOF);

    uint32_t flash_addr = _fp_flash_start + (page * FLASHPAGE_SIZE);

    DEBUG("%s erase page in flash @0x%08"PRIx32"\n", __func__, flash_addr);

    uint32_t state = irq_disable();

    int res = esp_flash_erase_region(esp_flash_default_chip, flash_addr, FLASHPAGE_SIZE);
    esp_sync_cache((uint32_t)&_fp_mmu_start + (page * FLASHPAGE_SIZE), FLASHPAGE_SIZE);

    irq_restore(state);

    if (res != ESP_OK) {
        LOG_TAG_ERROR("flashpage", "Could not erase page %u, error %d\n",
                      page, res);
    }
}

void IRAM_ATTR flashpage_write(void *target_addr, const void *data, size_t len)
{
    DEBUG("%s write %u byte from @%p to @%p\n",
          __func__, len, data, target_addr);

    /* assert multiples of FLASHPAGE_WRITE_BLOCK_SIZE are written */
    assert(!(len % FLASHPAGE_WRITE_BLOCK_SIZE));

    /* ensure writes to flash are aligned */
    assert(!((unsigned)target_addr % FLASHPAGE_WRITE_BLOCK_ALIGNMENT));

    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <=
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)));

    uint32_t flash_addr = ((uint32_t)target_addr - (uint32_t)&_fp_mmu_start) + _fp_flash_start;

    DEBUG("%s write to CPU address @%p (flash @0x%08"PRIx32")\n",
          __func__, target_addr, flash_addr);

    uint32_t state = irq_disable();

    int res = esp_flash_write(esp_flash_default_chip, data, flash_addr, len);
    esp_sync_cache((uint32_t)target_addr, len);

    irq_restore(state);

    if (res != ESP_OK) {
        LOG_TAG_ERROR("flashpage", "Could not write to CPU address @%p "
                      "(flash @0x%08"PRIx32"), error %d\n",
                      target_addr, flash_addr, res);
    }
}

unsigned IRAM_ATTR flashpage_first_free(void)
{
    /* _end_fw is page aligned */
    return flashpage_page(&_end_fw);
}

unsigned IRAM_ATTR flashpage_last_free(void)
{
    return flashpage_page((void *)(CPU_FLASH_BASE + CONFIG_ESP_FLASHPAGE_CAPACITY)) - 1;
}
