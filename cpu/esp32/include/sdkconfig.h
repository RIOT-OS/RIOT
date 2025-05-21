/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       SDK configuration used by ESP-IDF for all ESP32x SoC variants (families)
 *
 * The SDK configuration can be partially overridden by application-specific
 * board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */


/*
 * Some files in ESP-IDF use functions from `stdlib.h` without including the
 * header. To avoid having to patch all these files, `stdlib.h` is included
 * in this header file, which in turn is included by every ESP-IDF file.
 */
#if !defined(__ASSEMBLER__) && !defined(LD_FILE_GEN)
#include <stdlib.h>
#endif

/*
 * The SoC capability definitions are often included indirectly in the
 * ESP-IDF files, although all ESP-IDF files require them. Since not all
 * ESP-IDF header files are included in RIOT, the SoC capability definitions
 * are unknown if they are only indirectly included. Therefore, the SoC
 * capabilities are included in this file and are thus available to all
 * ESP-IDF files. This avoids to update vendor code.
 */
#include "soc/soc_caps.h"

/**
 * @brief   SDK version number
 *
 * Determined with `git describe --tags` in `$ESP32_SDK_DIR`
 */
#if !defined(IDF_VER)
#include "esp_idf_ver.h"
#endif

#ifndef DOXYGEN

/**
 * Default console configuration
 *
 * STDIO_UART_BAUDRATE is used as CONFIG_ESP_CONSOLE_UART_BAUDRATE and
 * can be overridden by an application specific configuration.
 */
#ifdef CONFIG_CONSOLE_UART_NUM
#define CONFIG_ESP_CONSOLE_UART_NUM         CONFIG_CONSOLE_UART_NUM
#else
#define CONFIG_ESP_CONSOLE_UART_NUM         0
#endif
#define CONFIG_ESP_CONSOLE_UART_BAUDRATE    STDIO_UART_BAUDRATE

/**
 * Log output configuration (DO NOT CHANGE)
 */
#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL    LOG_LEVEL
#endif
#define CONFIG_LOG_MAXIMUM_LEVEL    LOG_LEVEL

/**
 * System specific configuration (DO NOT CHANGE)
 */
#ifdef MODULE_NEWLIB_NANO
#define CONFIG_NEWLIB_NANO_FORMAT               1
#endif

#define CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4     1
#define CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE      32
#define CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE 2560
#define CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE      1

#define CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER     1
#define CONFIG_ESP_TIMER_TASK_STACK_SIZE        3584
#define CONFIG_ESP_TIMER_INTERRUPT_LEVEL        1
#define CONFIG_TIMER_TASK_STACK_SIZE            CONFIG_ESP_TIMER_TASK_STACK_SIZE

#define CONFIG_APP_BUILD_TYPE_APP_2NDBOOT       1
#define CONFIG_APP_BUILD_GENERATE_BINARIES      1
#define CONFIG_APP_BUILD_BOOTLOADER             1
#define CONFIG_APP_BUILD_USE_FLASH_SECTIONS     1

#define CONFIG_PARTITION_TABLE_CUSTOM_FILENAME  "partitions.csv"
#define CONFIG_PARTITION_TABLE_FILENAME         "partitions_singleapp.csv"
#define CONFIG_PARTITION_TABLE_SINGLE_APP       1
#define CONFIG_PARTITION_TABLE_OFFSET           0x8000

/**
 * BLE driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_BLE
#define CONFIG_ESP32_WIFI_ENABLED                       1   /* WiFi module has to be enabled */
#define CONFIG_BT_ENABLED                               1
#define CONFIG_BT_CONTROLLER_ONLY                       1
#else
#define CONFIG_BT_ENABLED                               0
#endif

/**
 * SPI RAM configuration (DO NOT CHANGE)
 */
#ifdef  MODULE_ESP_SPI_RAM
#define CONFIG_SPIRAM_TYPE_AUTO                 1
#define CONFIG_SPIRAM_SIZE                      -1
#define CONFIG_SPIRAM_SPEED_40M                 1
#define CONFIG_SPIRAM                           1
#define CONFIG_SPIRAM_BOOT_INIT                 1
#define CONFIG_SPIRAM_USE_MALLOC                0   /* using malloc requires QStaticQueue */
#define CONFIG_SPIRAM_MEMTEST                   1
#define CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL     16384
#define CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL   32768
#endif

/**
 * SPI Flash driver configuration (DO NOT CHANGE)
 */
#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH           1
#define CONFIG_SPI_FLASH_USE_LEGACY_IMPL            1
#define CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS     1
#define CONFIG_SPI_FLASH_YIELD_DURING_ERASE         1
#define CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS    20
#define CONFIG_SPI_FLASH_ERASE_YIELD_TICKS          1
#define CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE           8192
#define CONFIG_SPI_FLASH_SUPPORT_ISSI_CHIP          1
#define CONFIG_SPI_FLASH_SUPPORT_MXIC_CHIP          1
#define CONFIG_SPI_FLASH_SUPPORT_GD_CHIP            1
#define CONFIG_SPI_FLASH_SUPPORT_WINBOND_CHIP       1
#define CONFIG_SPI_FLASH_SUPPORT_BOYA_CHIP          1
#define CONFIG_SPI_FLASH_SUPPORT_TH_CHIP            1
#define CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP      1

/**
 * Ethernet driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_ETH
#define CONFIG_ETH_ENABLED                      1
#endif

/**
 * Serial flasher config (defined by CFLAGS, only sanity check here)
 */
#if !defined(CONFIG_FLASHMODE_DOUT) && \
    !defined(CONFIG_FLASHMODE_DIO) && \
    !defined(CONFIG_FLASHMODE_QOUT) && \
    !defined(CONFIG_FLASHMODE_QIO)
#error "Flash mode not configured"
#endif

/**
 * Wi-Fi driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_WIFI_ANY
#define CONFIG_ESP32_WIFI_ENABLED               1
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM  10
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER     1
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE        1
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM 32
#define CONFIG_ESP32_WIFI_CACHE_TX_BUFFER_NUM   32  /* required when CONFIG_SPIRAM_USE_MALLOC=0 */
#define CONFIG_ESP32_WIFI_AMPDU_TX_ENABLED      1
#define CONFIG_ESP32_WIFI_TX_BA_WIN             6
#define CONFIG_ESP32_WIFI_AMPDU_RX_ENABLED      1
#define CONFIG_ESP32_WIFI_RX_BA_WIN             6
#if defined(MODULE_ESP_IDF_NVS_FLASH) && !defined(CPU_FAM_ESP32C3)
#define CONFIG_ESP32_WIFI_NVS_ENABLED           1
#endif
#define CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_0 1
#define CONFIG_ESP32_WIFI_SOFTAP_BEACON_MAX_LEN 752
#define CONFIG_ESP32_WIFI_MGMT_SBUF_NUM         32
#define CONFIG_ESP32_WIFI_IRAM_OPT              1
#define CONFIG_ESP32_WIFI_RX_IRAM_OPT           1
#define CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE       1
#if defined(MODULE_ESP_WIFI_AP) || defined(MODULE_ESP_NOW)
#define CONFIG_ESP_WIFI_SOFTAP_SUPPORT          1
#endif
#ifdef MODULE_ESP_BLE
#define CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE     1
#endif
#endif

/**
 * PHY configuration
 */
#if MODULE_ESP_IDF_NVS_ENABLED
#define CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE     1
#endif

#define CONFIG_ESP_PHY_MAX_TX_POWER                     20
#define CONFIG_ESP_PHY_MAX_WIFI_TX_POWER                20

#define CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE   CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE
#define CONFIG_ESP32_PHY_MAX_WIFI_TX_POWER              CONFIG_ESP_PHY_MAX_WIFI_TX_POWER

/**
 * Flashpage configuration
 */
#ifndef CONFIG_ESP_FLASHPAGE_CAPACITY

#ifdef MODULE_PERIPH_FLASHPAGE
#if CONFIG_ESP_FLASHPAGE_CAPACITY_64K
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x10000
#elif CONFIG_ESP_FLASHPAGE_CAPACITY_128K
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x20000
#elif CONFIG_ESP_FLASHPAGE_CAPACITY_256K
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x40000
#elif CONFIG_ESP_FLASHPAGE_CAPACITY_512K
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x80000
#elif CONFIG_ESP_FLASHPAGE_CAPACITY_1M
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x100000
#elif CONFIG_ESP_FLASHPAGE_CAPACITY_2M
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x200000
#else
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x80000
#endif
#else /* MODULE_PERIPH_FLASHPAGE_IN_ADDRESS_SPACE */
#define CONFIG_ESP_FLASHPAGE_CAPACITY                   0x0
#endif /* MODULE_PERIPH_FLASHPAGE_IN_ADDRESS_SPACE */

#endif /* !CONFIG_ESP_FLASHPAGE_CAPACITY */

/**
 * LCD driver configuration
 */
#if MODULE_ESP_IDF_LCD
#ifndef CONFIG_LCD_DATA_BUF_SIZE
#define CONFIG_LCD_DATA_BUF_SIZE                        512
#endif

#define CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE             CONFIG_LCD_DATA_BUF_SIZE
#endif

#endif /* DOXYGEN */

/**
 * @brief   Include ESP32x family specific SDK configuration
 */
#if defined(CPU_FAM_ESP32)
#include "sdkconfig_esp32.h"
#elif defined(CPU_FAM_ESP32C3)
#include "sdkconfig_esp32c3.h"
#elif defined(CPU_FAM_ESP32S2)
#include "sdkconfig_esp32s2.h"
#elif defined(CPU_FAM_ESP32S3)
#include "sdkconfig_esp32s3.h"
#else
#error "ESP32x family implementation missing"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
