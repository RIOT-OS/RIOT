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
 * The SoC capability definitions are often included indirectly in the
 * ESP-IDF files, although all ESP-IDF files require them. Since not all
 * ESP-IDF header files are included in RIOT, the SoC capability definitions
 * are unknown if they are only indirectly included. Therefore, the SoC
 * capabilities are included in this file and are thus available to all
 * ESP-IDF files. This avoids to update vendor code.
 */
#ifndef LD_FILE_GEN
#  include "soc/soc_caps.h"
#endif

/**
 * @brief   SDK version number
 *
 * Determined with `git describe --tags` in `$ESP32_SDK_DIR`
 */
#if !defined(IDF_VER)
#  include "esp_idf_ver.h"
#endif

#ifndef DOXYGEN

/**
 * Default console configuration
 *
 * STDIO_UART_BAUDRATE is used as CONFIG_ESP_CONSOLE_UART_BAUDRATE and
 * can be overridden by an application specific configuration.
 */
#ifdef CONFIG_CONSOLE_UART_NUM
#  define CONFIG_ESP_CONSOLE_UART_NUM           CONFIG_CONSOLE_UART_NUM
#else
#  define CONFIG_ESP_CONSOLE_UART_NUM           0
#endif
#define CONFIG_ESP_CONSOLE_UART_BAUDRATE        STDIO_UART_BAUDRATE

#define CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM  CONFIG_ESP_CONSOLE_UART_NUM

/**
 * Log output configuration (DO NOT CHANGE)
 */
#ifndef CONFIG_LOG_DEFAULT_LEVEL
#  define CONFIG_LOG_DEFAULT_LEVEL              LOG_LEVEL
#endif
#define CONFIG_LOG_MAXIMUM_LEVEL                LOG_LEVEL

/**
 * System specific configuration (DO NOT CHANGE)
 */
#if MODULE_NEWLIB_NANO
#  define CONFIG_NEWLIB_NANO_FORMAT             1
#endif

#define CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4     1
#define CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE      32
#define CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE 2560
#define CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE      1

#define CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER     1
#define CONFIG_ESP_TIMER_TASK_STACK_SIZE        3584
#define CONFIG_ESP_TIMER_INTERRUPT_LEVEL        1
#define CONFIG_ESP_TIMER_TASK_AFFINITY          0
#define CONFIG_ESP_TIMER_ISR_AFFINITY_CPU0      1

#define CONFIG_TIMER_TASK_STACK_SIZE            CONFIG_ESP_TIMER_TASK_STACK_SIZE

#define CONFIG_APP_BUILD_TYPE_APP_2NDBOOT       1
#define CONFIG_APP_BUILD_GENERATE_BINARIES      1
#define CONFIG_APP_BUILD_BOOTLOADER             1
#define CONFIG_APP_BUILD_USE_FLASH_SECTIONS     1
#define CONFIG_APP_COMPILE_TIME_DATE            1
#define CONFIG_APP_EXCLUDE_PROJECT_VER_VAR      1
#define CONFIG_APP_RETRIEVE_LEN_ELF_SHA         9

#define CONFIG_PARTITION_TABLE_CUSTOM_FILENAME  "partitions.csv"
#define CONFIG_PARTITION_TABLE_FILENAME         "partitions_singleapp.csv"
#define CONFIG_PARTITION_TABLE_SINGLE_APP       1
#define CONFIG_PARTITION_TABLE_OFFSET           0x8000

/**
 * BLE driver configuration (DO NOT CHANGE)
 */
#if MODULE_ESP_BLE
#  define CONFIG_BT_ENABLED                     1
#  define CONFIG_BT_CONTROLLER_ENABLED          1
#  define CONFIG_BT_CONTROLLER_ONLY             1
#  define CONFIG_SOC_BT_SUPPORTED               SOC_BT_SUPPORTED
#  define CONFIG_SOC_PM_SUPPORT_BT_PD           SOC_PM_SUPPORT_BT_PD
#  define CONFIG_SOC_PM_SUPPORT_BT_WAKEUP       SOC_PM_SUPPORT_BT_WAKEUP
#endif

/**
 * SPI RAM configuration (DO NOT CHANGE)
 */
#if MODULE_ESP_SPI_RAM
#  define CONFIG_SPIRAM                             1
#  define CONFIG_SPIRAM_TYPE_AUTO                   1
#  define CONFIG_SPIRAM_SIZE                        -1
#  define CONFIG_SPIRAM_SPEED_40M                   1
#  define CONFIG_SPIRAM_SPEED                       40
#  define CONFIG_SPIRAM_BOOT_INIT                   1
#  define CONFIG_SPIRAM_USE_MALLOC                  1   /* using malloc requires QStaticQueue */
#  define CONFIG_SPIRAM_USE_CAPS_ALLOC              0   /* using cap instead of malloc */
#  define CONFIG_SPIRAM_MEMTEST                     1
#  define CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL       16384
#  define CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL     32768
#  define CONFIG_SOC_SPIRAM_SUPPORTED               SOC_SPIRAM_SUPPORTED
#  define CONFIG_ESP_SLEEP_PSRAM_LEAKAGE_WORKAROUND 1
#endif

/**
 * SPI Flash driver configuration (DO NOT CHANGE)
 */
#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH           1
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
 * RTC Clock configuration
 */
#if MODULE_ESP_RTC_TIMER_32K
#  define CONFIG_RTC_CLK_SRC_EXT_CRYS               1
#else
#  define CONFIG_RTC_CLK_SRC_INT_RC                 1
#endif

/**
 * Ethernet driver configuration (DO NOT CHANGE)
 */
#if MODULE_ESP_ETH
#  define CONFIG_ETH_ENABLED                        1
#endif

/**
 * Serial flasher config (defined by CFLAGS, only sanity check here)
 */
#if !defined(CONFIG_FLASHMODE_DOUT) && \
    !defined(CONFIG_FLASHMODE_DIO) && \
    !defined(CONFIG_FLASHMODE_QOUT) && \
    !defined(CONFIG_FLASHMODE_QIO)
#  error "Flash mode not configured"
#endif

/**
 * Wi-Fi driver configuration (DO NOT CHANGE)
 */
#if MODULE_ESP_WIFI_ANY
#  define CONFIG_ESP_WIFI_ENABLED                       1
#  define CONFIG_ESP_WIFI_AMPDU_RX_ENABLED              1
#  define CONFIG_ESP_WIFI_AMPDU_TX_ENABLED              1
#  define CONFIG_ESP_WIFI_AUTH_WPA2_PSK                 1
#  define CONFIG_ESP_WIFI_CACHE_TX_BUFFER_NUM           32
#  define CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM         32
#  define CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUF           0
#  define CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER             1
#  define CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM         32
#  define CONFIG_ESP_WIFI_ENABLE_SAE_PK                 0   /* default 1 for WPA3 */
#  define CONFIG_ESP_WIFI_ENABLE_WPA3_OWE_STA           0   /* default 1 for WPA3 */
#  define CONFIG_ESP_WIFI_ENABLE_WPA3_SAE               0   /* default 1 for WPA3 */
#  define CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM        7
#  define CONFIG_ESP_WIFI_GMAC_SUPPORT                  1
#  define CONFIG_ESP_WIFI_IRAM_OPT                      0   /* default 1 */
#  define CONFIG_ESP_WIFI_MBEDTLS_CRYPTO                0   /* default 1 for WPA3 */
#  define CONFIG_ESP_WIFI_MBEDTLS_TLS_CLIENT            0   /* default 1 for WPA3 */
#  define CONFIG_ESP_WIFI_MGMT_SBUF_NUM                 32
#  define CONFIG_ESP_WIFI_NVS_ENABLED                   MODULE_ESP_IDF_NVS_FLASH
#  define CONFIG_ESP_WIFI_PW_ID                         ""
#  define CONFIG_ESP_WIFI_RX_BA_WIN                     6
#  define CONFIG_ESP_WIFI_RX_IRAM_OPT                   0   /* default 1 */
#  define CONFIG_ESP_WIFI_RX_MGMT_BUF_NUM_DEF           5
#  define CONFIG_ESP_WIFI_SOFTAP_BEACON_MAX_LEN         752
#  define CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE    1
#  define CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM          10
#  define CONFIG_ESP_WIFI_STATIC_RX_MGMT_BUFFER         1
#  define CONFIG_ESP_WIFI_TASK_PINNED_TO_CORE_0         1
#  define CONFIG_ESP_WIFI_TX_BA_WIN                     6
#  define CONFIG_ESP_WIFI_TX_BUFFER_TYPE                1

#  define CONFIG_CRYPTO_INTERNAL                        1

#  if MODULE_ESP_WIFI_AP || MODULE_ESP_NOW
#    define CONFIG_ESP_WIFI_SOFTAP_SUPPORT              1
#  endif

#  if MODULE_ESP_WIFI_ENTERPRISE
#    define CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT          1
#  endif

#endif

#define CONFIG_ESP_WIFI_SLP_DEFAULT_MAX_ACTIVE_TIME 10
#define CONFIG_ESP_WIFI_SLP_DEFAULT_MIN_ACTIVE_TIME 50
#define CONFIG_ESP_WIFI_SLP_DEFAULT_WAIT_BROADCAST_DATA_TIME    15

/**
 * PHY configuration
 */
#if SOC_PHY_SUPPORTED
#  define CONFIG_ESP_PHY_ENABLED                        1
#  define CONFIG_ESP_PHY_CALIBRATION_MODE               0
#  define CONFIG_ESP_PHY_MAX_TX_POWER                   20
#  define CONFIG_ESP_PHY_MAX_WIFI_TX_POWER              20
#  define CONFIG_ESP_PHY_RF_CAL_PARTIAL                 1
#  if MODULE_ESP_IDF_NVS_FLASH
#    define CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE 1
#  endif
#endif

/**
 * Coexist configuration (DO NOT CHANGE)
 */
#if !SOC_WIRELESS_HOST_SUPPORTED
#  define CONFIG_ESP_COEX_ENABLED                       1
#  if CONFIG_ESP_WIFI_ENABLED && CONFIG_BT_ENABLED
#    define CONFIG_ESP_COEX_SW_COEXIST_ENABLE           1
#  endif
#  if 0
   /* TODO:
    * CONFIG_SW_COEXIST_ENABLE is deprecated but still used in code.
    * It is not defined in IDF sdkconfigs and does not work if defined. */
#    define CONFIG_SW_COEXIST_ENABLE                    1
#  endif
#endif /* !SOC_WIRELESS_HOST_SUPPORTED */

/**
 * Flashpage configuration
 */
#ifndef CONFIG_ESP_FLASHPAGE_CAPACITY

#if MODULE_PERIPH_FLASHPAGE
#  if CONFIG_ESP_FLASHPAGE_CAPACITY_64K
#    define CONFIG_ESP_FLASHPAGE_CAPACITY               0x10000
#  elif CONFIG_ESP_FLASHPAGE_CAPACITY_128K
#    define CONFIG_ESP_FLASHPAGE_CAPACITY               0x20000
#  elif CONFIG_ESP_FLASHPAGE_CAPACITY_256K
#    define CONFIG_ESP_FLASHPAGE_CAPACITY               0x40000
#  elif CONFIG_ESP_FLASHPAGE_CAPACITY_512K
#    define CONFIG_ESP_FLASHPAGE_CAPACITY               0x80000
#  elif CONFIG_ESP_FLASHPAGE_CAPACITY_1M
#    define CONFIG_ESP_FLASHPAGE_CAPACITY               0x100000
#  elif CONFIG_ESP_FLASHPAGE_CAPACITY_2M
#    define CONFIG_ESP_FLASHPAGE_CAPACITY               0x200000
#  else
#    define CONFIG_ESP_FLASHPAGE_CAPACITY               0x80000
#  endif
#else /* MODULE_PERIPH_FLASHPAGE */
#  define CONFIG_ESP_FLASHPAGE_CAPACITY                 0x0
#endif /* MODULE_PERIPH_FLASHPAGE */

#endif /* !CONFIG_ESP_FLASHPAGE_CAPACITY */

/**
 * LCD driver configuration
 */
#if MODULE_ESP_IDF_LCD
#  ifndef CONFIG_LCD_DATA_BUF_SIZE
#    define CONFIG_LCD_DATA_BUF_SIZE                    512
#  endif
#  define CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE           CONFIG_LCD_DATA_BUF_SIZE
#endif

/**
 * @brief   Include ESP32x family specific SDK configuration
 */
#if defined(CPU_FAM_ESP32)
#  include "sdkconfig_esp32.h"
#elif defined(CPU_FAM_ESP32C3)
#  include "sdkconfig_esp32c3.h"
#elif defined(CPU_FAM_ESP32S2)
#  include "sdkconfig_esp32s2.h"
#elif defined(CPU_FAM_ESP32S3)
#  include "sdkconfig_esp32s3.h"
#else
#  error "ESP32x family implementation missing"
#endif

#ifndef CONFIG_MMU_PAGE_SIZE
#  define CONFIG_MMU_PAGE_SIZE_64KB                 1
#  define CONFIG_MMU_PAGE_SIZE                      0x10000
#endif

#ifndef CONFIG_FREERTOS_NUMBER_OF_CORES
#  define CONFIG_FREERTOS_NUMBER_OF_CORES           1
#endif

#define CONFIG_ESP_DEBUG_OCDAWARE                   1

#define CONFIG_ADC_SUPPRESS_DEPRECATE_WARN          1

#define CONFIG_HEAP_POISONING_DISABLED              1
#define CONFIG_HEAP_TRACING_OFF                     1
#define CONFIG_LOG_TAG_LEVEL_CACHE_BINARY_MIN_HEAP  1

#define CONFIG_ULP_COPROC_RESERVE_MEM               0

#ifdef SOC_RTC_MEM_SUPPORTED
#  define CONFIG_SOC_RTC_MEM_SUPPORTED              1
#endif
#ifdef SOC_RTC_FAST_MEM_SUPPORTED
#  define CONFIG_SOC_RTC_FAST_MEM_SUPPORTED         1
#endif
#ifdef SOC_RTC_SLOW_SUPPORTED
#  define CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED         1
#endif

/**
 * SDMMC Host configuration
 */
#ifdef SOC_SDMMC_HOST_SUPPORTED
#  define CONFIG_SOC_SDMMC_HOST_SUPPORTED           SOC_SDMMC_HOST_SUPPORTED
#  define CONFIG_SOC_SDMMC_DELAY_PHASE_NUM          SOC_SDMMC_DELAY_PHASE_NUM
#  define CONFIG_SOC_SDMMC_NUM_SLOTS                SOC_SDMMC_NUM_SLOTS
#  define CONFIG_SOC_SDMMC_SUPPORT_XTAL_CLOCK       SOC_SDMMC_SUPPORT_XTAL_CLOCK
#  define CONFIG_SOC_SDMMC_USE_GPIO_MATRIX          SOC_SDMMC_USE_GPIO_MATRIX
#  define CONFIG_SOC_SDMMC_USE_IOMUX                SOC_SDMMC_USE_IOMUX
#endif /* SOC_SDMMC_HOST_SUPPORTED */

/**
 * USB Serial/JTAG configuration
 */
#ifdef SOC_USB_SERIAL_JTAG_SUPPORTED
#  ifndef CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
#    define CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG    1
#  endif
#  ifndef CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#    define CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG              0
#  endif
#  define CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED        (CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG || \
                                                            CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG)
#  define CONFIG_SOC_EFUSE_DIS_USB_JTAG                     SOC_EFUSE_DIS_USB_JTAG
#  define CONFIG_SOC_EFUSE_HARD_DIS_JTAG                    SOC_EFUSE_HARD_DIS_JTAG
#  define CONFIG_SOC_EFUSE_SOFT_DIS_JTAG                    SOC_EFUSE_SOFT_DIS_JTAG
#  define CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED              1
#  define CONFIG_USJ_ENABLE_USB_SERIAL_JTAG                 CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
#endif /* SOC_USB_SERIAL_JTAG_SUPPORTED */

/* should be RIOT_APPLICATION but PROJECT_NAME must be less than 24 characters */
#define PROJECT_NAME "RIOT-OS Application"

#endif /* DOXYGEN */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
