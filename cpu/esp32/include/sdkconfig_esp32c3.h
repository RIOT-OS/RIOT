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
 * @brief       SDK configuration used by the ESP-IDF for ESP32C3
 *
 * The SDK configuration can be partially overridden by application-specific
 * board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef SDKCONFIG_ESP32C3_H
#define SDKCONFIG_ESP32C3_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */

/* Mapping of Kconfig defines to the respective enumeration values */
#if CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_2
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     2
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_5
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     5
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_10
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     10
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_20
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     20
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_40
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     40
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_80
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     80
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_160
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     160
#endif

/**
 * @brief   Defines the CPU frequency [values = 2, 5, 10, 10, 40, 80, 160]
 */
#ifndef CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ 80
#endif
/**
 * @brief   Mapping configured ESP32 default clock to CLOCK_CORECLOCK define
 */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ)
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
 * RTC clock configuration
 */
#ifdef MODULE_ESP_RTC_TIMER_32K
#define CONFIG_ESP32_RTC_CLK_SRC_EXT_CRYS       1
#endif

#define CONFIG_ESP32C3_RTC_CLK_CAL_CYCLES       (8 * 1024)

/**
 * System specific configuration (DO NOT CHANGE)
 */
#ifdef MODULE_NEWLIB_NANO
#define CONFIG_NEWLIB_NANO_FORMAT               1
#endif

#define CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4     1
#define CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE      32
#define CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE 2560

#define CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER     1
#define CONFIG_ESP_TIMER_IMPL_SYSTIMER          1
#define CONFIG_ESP_TIMER_INTERRUPT_LEVEL        1
#define CONFIG_ESP_TIMER_TASK_STACK_SIZE        3584
#define CONFIG_TIMER_TASK_STACK_SIZE            CONFIG_ESP_TIMER_TASK_STACK_SIZE

#define CONFIG_APP_BUILD_USE_FLASH_SECTIONS     1
#define CONFIG_APP_BUILD_BOOTLOADER 1
#define CONFIG_APP_BUILD_GENERATE_BINARIES 1
#define CONFIG_APP_BUILD_TYPE_APP_2NDBOOT 1
#define CONFIG_APP_BUILD_USE_FLASH_SECTIONS 1

#define CONFIG_EFUSE_MAX_BLK_LEN                256

#define CONFIG_PARTITION_TABLE_OFFSET           0x8000
#define CONFIG_PARTITION_TABLE_CUSTOM_FILENAME  "partitions.csv"
#define CONFIG_PARTITION_TABLE_FILENAME         "partitions_singleapp.csv"
#define CONFIG_PARTITION_TABLE_SINGLE_APP       1

/**
 * Bluetooth configuration (DO NOT CHANGE)
 */
#define CONFIG_BT_ENABLED                       0
#define CONFIG_BT_RESERVE_DRAM                  0

/**
 * SPI Flash driver configuration (DO NOT CHANGE)
 */
#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH                   1
#define CONFIG_SPI_FLASH_USE_LEGACY_IMPL                    1
#define CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS             1
#define CONFIG_SPI_FLASH_ENABLE_ENCRYPTED_READ_WRITE        1
#define CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS            20
#define CONFIG_SPI_FLASH_ERASE_YIELD_TICKS                  1
#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH                   1
#define CONFIG_SPI_FLASH_SUPPORT_BOYA_CHIP                  1
#define CONFIG_SPI_FLASH_SUPPORT_GD_CHIP                    1
#define CONFIG_SPI_FLASH_SUPPORT_ISSI_CHIP                  1
#define CONFIG_SPI_FLASH_SUPPORT_MXIC_CHIP                  1
#define CONFIG_SPI_FLASH_SUPPORT_WINBOND_CHIP               1
#define CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE                   8192
#define CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ABORTS   CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS
#define CONFIG_SPI_FLASH_YIELD_DURING_ERASE                 1

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

#define CONFIG_ESP32_PHY_MAX_WIFI_TX_POWER      CONFIG_ESP_PHY_MAX_WIFI_TX_POWER
#define CONFIG_ESP32_WIFI_AMPDU_RX_ENABLED      1
#define CONFIG_ESP32_WIFI_AMPDU_TX_ENABLED      1
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER     1
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM 32
#define CONFIG_ESP32_WIFI_ENABLED               1
#define CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE       1
#define CONFIG_ESP32_WIFI_IRAM_OPT              1
#define CONFIG_ESP32_WIFI_MGMT_SBUF_NUM         32
#if MODULE_ESP_IDF_NVS_ENABLED
#define CONFIG_ESP32_WIFI_NVS_ENABLED           1
#endif
#define CONFIG_ESP32_WIFI_RX_BA_WIN             6
#define CONFIG_ESP32_WIFI_RX_IRAM_OPT           1
#define CONFIG_ESP32_WIFI_SOFTAP_BEACON_MAX_LEN 752
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM  10
#define CONFIG_ESP32_WIFI_TX_BA_WIN             6
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE        1

#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP    1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_STA   1
#define CONFIG_ESP_PHY_MAX_WIFI_TX_POWER        20

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

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SDKCONFIG_ESP32C3_H */
/** @} */
