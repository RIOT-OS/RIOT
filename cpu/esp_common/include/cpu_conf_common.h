/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_esp_common_conf ESP common compile configurations
 * @ingroup     cpu_esp_common
 * @ingroup     config
 * @brief       Compile-time configuration macros for Espressif ESP SoCs
 * @{
 *
 * @file
 * @brief       CPU specific configuration options
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef CPU_CONF_COMMON_H
#define CPU_CONF_COMMON_H

/**
 * @brief   Declare the heap_stats function as available
 *
 * Only if module esp_idf_heap is used, a platform specific heap_stats function
 * has to be used.
 */
#ifdef MODULE_ESP_IDF_HEAP
#define HAVE_HEAP_STATS
#endif

/**
 * @brief   Memory marked with this attribute is retained during deep sleep
 *          and initialized with 0 on cold boot.
 */
#define BACKUP_RAM      __attribute__((section(".rtc.bss")))

/**
 * @brief   Memory marked with this attribute is retained during deep sleep
 *          and initialized with user provided data on cold boot.
 */
#define BACKUP_RAM_DATA __attribute__((section(".rtc.data")))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_COMMON_H */
/** @} */
