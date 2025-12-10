/*
 * SPDX-FileCopyrightText: 2024 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Default SDK configuration for the ESP32 SoC bootloader
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ 160
#endif

#define CONFIG_XTAL_FREQ 40

#define CONFIG_ESP_DEBUG_OCDAWARE 1

#define CONFIG_BOOTLOADER_OFFSET_IN_FLASH 0x1000
#define CONFIG_BOOTLOADER_VDDSDIO_BOOST_1_9V 1
#define CONFIG_EFUSE_CODE_SCHEME_COMPAT_3_4 1
#define CONFIG_EFUSE_MAX_BLK_LEN 192
#define CONFIG_IDF_FIRMWARE_CHIP_ID 0x0000
#define CONFIG_MMU_PAGE_SIZE 0x10000

#define CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4 1

#define CONFIG_ESP_INT_WDT 1
#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 300
#define CONFIG_ESP_INT_WDT_CHECK_CPU 1

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
