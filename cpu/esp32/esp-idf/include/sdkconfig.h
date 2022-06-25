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
 * @brief       SDK configuration compatible to the ESP-IDF
 *
 * The SDK configuration can be partially overridden by application-specific
 * board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef SDKCONFIG_H
#define SDKCONFIG_H

/*
 * The SoC capability definitions are often included indirectly in the
 * ESP-IDF files, although all ESP-IDF files require them. Since not all
 * ESP-IDF header files are included in RIOT, the SoC capability definitions
 * are unknown if they are only indirectly included. Therefore, the SoC
 * capabilities are included in this file and are thus available to all
 * ESP-IDF files. This avoids to update vendor code.
 */
#include "soc/soc_caps.h"

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SDK version number
 *
 * Determined with `git describe --tags` in `$ESP32_SDK_DIR`
 */
#if !defined(IDF_VER) || DOXYGEN
#include "esp_idf_ver.h"
#endif

/**
 * @name    Clock configuration
 * @{
 */

#ifndef DOXYGEN
/* Mapping of Kconfig defines to the respective enumeration values */
#if CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_2
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ   2
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_40
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ   40
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_80
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ   80
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_160
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ   160
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_240
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ   240
#endif
#endif

/**
 * @brief   Defines the CPU frequency [values = 2, 40, 80, 160 and 240]
 */
#ifndef CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ   80
#endif
/**
 * @brief   Mapping configured ESP32 default clock to CLOCK_CORECLOCK define
 */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
/** @} */

/**
 * Default console configuration
 *
 * STDIO_UART_BAUDRATE is used as CONFIG_CONSOLE_UART_BAUDRATE and
 * can be overridden by an application specific configuration.
 */
#define CONFIG_CONSOLE_UART_NUM 0
#define CONFIG_ESP_CONSOLE_UART_NUM     CONFIG_CONSOLE_UART_NUM

#ifndef CONFIG_CONSOLE_UART_BAUDRATE
#define CONFIG_CONSOLE_UART_BAUDRATE    STDIO_UART_BAUDRATE
#endif

/**
 * Log output configuration (DO NOT CHANGE)
 */
#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL    LOG_LEVEL
#endif
#define CONFIG_LOG_MAXIMUM_LEVEL    LOG_LEVEL

/**
 * ESP32 specific configuration
 *
 * Main clock crystal frequency (MHz). Zero means to auto-configure.
 * This is configured at the board level, defaulting to 40.
 */
#ifndef CONFIG_ESP32_XTAL_FREQ
#define CONFIG_ESP32_XTAL_FREQ                  0
#endif

#ifdef MODULE_ESP_RTC_TIMER_32K
#define CONFIG_ESP32_RTC_CLK_SRC_EXT_CRYS       1
#endif
#define CONFIG_ESP32_RTC_XTAL_BOOTSTRAP_CYCLES  100
#define CONFIG_ESP32_RTC_CLK_CAL_CYCLES         (8 * 1024)

/**
 * System specific configuration (DO NOT CHANGE)
 */
#ifdef MODULE_NEWLIB_NANO
#define CONFIG_NEWLIB_NANO_FORMAT               1
#endif

#define CONFIG_TRACEMEM_RESERVE_DRAM            0
#define CONFIG_ULP_COPROC_RESERVE_MEM           0

#define CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4     1
#define CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE      32
#define CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE 2560
#define CONFIG_NUMBER_OF_UNIVERSAL_MAC_ADDRESS  4

#define CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY    2000

#define CONFIG_ESP_TIMER_INTERRUPT_LEVEL        1
#define CONFIG_ESP_TIMER_TASK_STACK_SIZE        3584
#define CONFIG_ESP_TIMER_IMPL_FRC2              1
#define CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER     1

#define CONFIG_APP_BUILD_USE_FLASH_SECTIONS     1
#define CONFIG_EFUSE_MAX_BLK_LEN                192

#define CONFIG_PARTITION_TABLE_OFFSET           0x8000

/**
 * Bluetooth configuration (DO NOT CHANGE)
 */
#define CONFIG_BT_ENABLED                       0
#define CONFIG_BT_RESERVE_DRAM                  0

/**
 * SPI RAM configuration (DO NOT CHANGE)
 */
#ifdef  MODULE_ESP_SPI_RAM
#define CONFIG_SOC_SPIRAM_SUPPORTED             1
#define CONFIG_ESP32_SPIRAM_SUPPORT             1
#define CONFIG_D0WD_PSRAM_CLK_IO                17
#define CONFIG_D0WD_PSRAM_CS_IO                 16
#define CONFIG_D2WD_PSRAM_CLK_IO                9
#define CONFIG_D2WD_PSRAM_CS_IO                 10
#define CONFIG_PICO_PSRAM_CS_IO                 10
#define CONFIG_SPIRAM_SUPPORT                   CONFIG_ESP32_SPIRAM_SUPPORT
#define CONFIG_SPIRAM                           1
#define CONFIG_SPIRAM_BANKSWITCH_ENABLE         1
#define CONFIG_SPIRAM_BANKSWITCH_RESERVE        8
#define CONFIG_SPIRAM_BOOT_INIT                 1
#define CONFIG_SPIRAM_CACHE_WORKAROUND          1
#define CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_MEMW 1
#define CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL     16384
#define CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL   32768
#define CONFIG_SPIRAM_MEMTEST                   1
#define CONFIG_SPIRAM_SIZE                      -1
#define CONFIG_SPIRAM_SPEED_40M                 1
#define CONFIG_SPIRAM_SPIWP_SD3_PIN             7
#define CONFIG_SPIRAM_TYPE_AUTO                 1
#define CONFIG_SPIRAM_USE_MALLOC                0   /* using malloc requires QStaticQueue */
#endif

/**
 * SPI Flash driver configuration (DO NOT CHANGE)
 */
#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH       1
#define CONFIG_SPI_FLASH_USE_LEGACY_IMPL        1

/**
 * Ethernet driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_ETH
#define CONFIG_ETH_ENABLED                      1
#endif
#define CONFIG_ETH_USE_ESP32_EMAC               1
#define CONFIG_ETH_PHY_INTERFACE_RMII           1
#define CONFIG_ETH_RMII_CLK_INPUT               1
#define CONFIG_ETH_RMII_CLK_IN_GPIO             0
#define CONFIG_ETH_DMA_BUFFER_SIZE              512
#define CONFIG_ETH_DMA_RX_BUFFER_NUM            10
#define CONFIG_ETH_DMA_TX_BUFFER_NUM            10

/**
 * Serial flasher config (DO NOT CHANGE)
 */
#define CONFIG_ESPTOOLPY_FLASHFREQ_40M          1
#if defined(FLASH_MODE_QIO)
#define CONFIG_FLASHMODE_QIO                    1
#define CONFIG_ESPTOOLPY_FLASHMODE_QIO          1
#elif defined(FLASH_MODE_QOUT)
#define CONFIG_FLASHMODE_QOUT                   1
#define CONFIG_ESPTOOLPY_FLASHMODE_QOUT         1
#elif defined(FLASH_MODE_DIO)
#define CONFIG_FLASHMODE_DIO                    1
#define CONFIG_ESPTOOLPY_FLASHMODE_DIO          1
#elif defined(FLASH_MODE_DOUT)
#define CONFIG_FLASHMODE_DOUT                   1
#define CONFIG_ESPTOOLPY_FLASHMODE_DOUT         1
#else
#error "Unknown flash mode selected."
#endif

/**
 * Wi-Fi driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_WIFI_ANY
#define CONFIG_ESP32_WIFI_ENABLED               1
#endif
#if defined(MODULE_ESP_WIFI_AP) || defined(MODULE_ESP_NOW)
#define CONFIG_ESP_WIFI_SOFTAP_SUPPORT          1
#endif
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
#if MODULE_ESP_IDF_NVS_ENABLED
#define CONFIG_ESP32_WIFI_NVS_ENABLED           1
#endif
#define CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_0 1
#define CONFIG_ESP32_WIFI_SOFTAP_BEACON_MAX_LEN 752
#define CONFIG_ESP32_WIFI_MGMT_SBUF_NUM         32
#define CONFIG_ESP32_WIFI_IRAM_OPT              1
#define CONFIG_ESP32_WIFI_RX_IRAM_OPT           1
#define CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE       1

/**
 * PHY configuration
 */
#if MODULE_ESP_IDF_NVS_ENABLED
#define CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE   1
#endif

#define CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION   0
#define CONFIG_ESP_PHY_MAX_TX_POWER             20
#define CONFIG_ESP_PHY_MAX_WIFI_TX_POWER        20
#define CONFIG_ESP_PHY_REDUCE_TX_POWER          1

#define CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE   CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE
#define CONFIG_ESP32_PHY_MAX_WIFI_TX_POWER              CONFIG_ESP_PHY_MAX_WIFI_TX_POWER
#define CONFIG_ESP32_REDUCE_PHY_TX_POWER                CONFIG_ESP_PHY_REDUCE_TX_POWER
#define CONFIG_REDUCE_PHY_TX_POWER                      CONFIG_ESP_PHY_REDUCE_TX_POWER

/**
 * EMAC driver configuration (DO NOT CHANGE)
 */
#define CONFIG_EMAC_L2_TO_L3_RX_BUF_MODE        1

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SDKCONFIG_H */
/** @} */
