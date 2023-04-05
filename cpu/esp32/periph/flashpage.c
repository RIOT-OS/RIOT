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

#include "esp_flash_partitions.h"
#include "esp_spi_flash.h"
#include "rom/cache.h"
#include "rom/spi_flash.h"
#include "soc/mmu.h"
#include "soc/soc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ESP_PART_TABLE_ADDR         0x8000 /* TODO configurable as used in Makefile.include */
#define ESP_PART_TABLE_SIZE         0xC00
#define ESP_PART_ENTRY_SIZE         0x20
#define ESP_PART_ENTRY_MAGIC        ESP_PARTITION_MAGIC

extern uint8_t _fp_mem_start;      /* start address in CPU address space */
extern uint8_t _fp_mem_end;
extern uint8_t _end_fw;

static uint32_t _fp_flash_start;    /* start address in flash */

void IRAM_ATTR esp_flashpage_init(void)
{
    /* CONFIG_ESP_FLASHPAGE_CAPACITY has to be a multiple of SPI_FLASH_MMU_PAGE_SIZE */
    assert((CONFIG_ESP_FLASHPAGE_CAPACITY % SPI_FLASH_MMU_PAGE_SIZE) == 0);

    DEBUG("%s pages in CPU address space @0x%08"PRIx32"...0x%08"PRIx32"\n", __func__,
          CPU_FLASH_BASE, CPU_FLASH_BASE + CONFIG_ESP_FLASHPAGE_CAPACITY - 1);

    _fp_flash_start = FLASHPAGE_ADDR_START;

    DEBUG("%s pages in flash @0x%08"PRIx32"...0x%08"PRIx32"\n", __func__,
          _fp_flash_start, _fp_flash_start + CONFIG_ESP_FLASHPAGE_CAPACITY - 1);

    uint32_t state = irq_disable();

    uint32_t p_numof = CONFIG_ESP_FLASHPAGE_CAPACITY / SPI_FLASH_MMU_PAGE_SIZE;
    uint32_t p_addr = FLASHPAGE_ADDR_START;

#if CPU_FAM_ESP32S2
    /* ESP32-S2 requires special handling to enable the MMU pages in Cache
     * explicitly */

    uint32_t autoload = Cache_Suspend_ICache();
    Cache_Invalidate_ICache_All();
    int res = Cache_Ibus_MMU_Set(MMU_ACCESS_FLASH, (uint32_t)&_fp_mem_start,
                                 p_addr, 64, p_numof, 0);
    Cache_Resume_ICache(autoload);

    DEBUG("%s DCache MMU set paddr=%08"PRIx32" vaddr=%08"PRIx32" size=%d n=%"PRIu32"\n",
           __func__, p_addr, (uint32_t)&_fp_mem_start, CONFIG_ESP_FLASHPAGE_CAPACITY,
           p_numof);

    if (res != ESP_OK) {
        LOG_TAG_ERROR("flashpage",
                      "Could not map MMU pages in DCache, error: %d\n", res);
    }
#else
    uint32_t p_mmu = ((uint32_t)&_fp_mem_start - SOC_DROM_LOW) / SPI_FLASH_MMU_PAGE_SIZE;

    while (p_numof--) {
        uint32_t p_flash = p_addr / SPI_FLASH_MMU_PAGE_SIZE;
        DEBUG("%s map MMU page %"PRIu32" @0x%08"PRIx32" to "
              "flash page %"PRIu32" @0x%08"PRIx32"\n", __func__,
              p_mmu, (p_mmu * SPI_FLASH_MMU_PAGE_SIZE) + SOC_DROM_LOW,
              p_flash, p_flash * SPI_FLASH_MMU_PAGE_SIZE);
        SOC_MMU_DPORT_PRO_FLASH_MMU_TABLE[p_mmu] = SOC_MMU_PAGE_IN_FLASH(p_flash);
        p_addr += SPI_FLASH_MMU_PAGE_SIZE;
        p_mmu++;
    }
#endif
    irq_restore(state);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        spi_flash_mmap_dump();
    }
}

void flashpage_erase(unsigned page)
{
    assert(page < FLASHPAGE_NUMOF);

    uint32_t flash_addr = _fp_flash_start + (page * FLASHPAGE_SIZE);

    DEBUG("%s erase page in flash @0x%08"PRIx32"\n", __func__, flash_addr);

    int res = spi_flash_erase_range(flash_addr, FLASHPAGE_SIZE);
    if (res != ESP_OK) {
        LOG_TAG_ERROR("flashpage", "Could not erase page %u, error %d\n",
                      page, res);
    }
}

void flashpage_write(void *target_addr, const void *data, size_t len)
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

    uint32_t flash_addr = ((uint32_t)target_addr - (uint32_t)&_fp_mem_start) + _fp_flash_start;

    DEBUG("%s write to CPU address @%p (flash @0x%08"PRIx32")\n",
          __func__, target_addr, flash_addr);

    int res = spi_flash_write(flash_addr, data, len);
    if (res != ESP_OK) {
        LOG_TAG_ERROR("flashpage", "Could not write to CPU address @%p "
                      "(flash @0x%08"PRIx32"), error %d\n",
                      target_addr, flash_addr, res);
    }
}

unsigned flashpage_first_free(void)
{
    /* _end_fw is page aligned */
    return flashpage_page(&_end_fw);
}

unsigned flashpage_last_free(void)
{
//    return flashpage_page((const void *)SOC_DROM_HIGH) - 1;
    return flashpage_page((void *)(CPU_FLASH_BASE + CONFIG_ESP_FLASHPAGE_CAPACITY)) - 1;
}
