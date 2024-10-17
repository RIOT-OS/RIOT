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
 * @brief       SDK configuration used by the ESP-IDF for ESP32-S2 SoC variant (family)
 *
 * The SDK configuration can be partially overridden by application-specific
 * board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef SDKCONFIG_ESP32S2_H
#define SDKCONFIG_ESP32S2_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ESP32-S2 specific clock configuration
 * @{
 */

/* Mapping of Kconfig defines to the respective enumeration values */
#if CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_2
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     2
#elif CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_5
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     5
#elif CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_10
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     10
#elif CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_20
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     20
#elif CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_40
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     40
#elif CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_80
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     80
#elif CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_160
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     160
#elif CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ_240
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     240
#endif

/**
 * @brief Defines the CPU frequency [values = 2, 5, 10, 10, 40, 80, 160, 240]
 */
#ifndef CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ     80
#endif
/** @} */

/**
 * ESP32-S2 specific RTC clock configuration
 */
#define CONFIG_ESP32S2_RTC_CLK_CAL_CYCLES       (8 * 1024)

/**
 * ESP32-S2 specific EFUSE configuration
 */
#define CONFIG_EFUSE_MAX_BLK_LEN                256

/**
 * ESP32-S2 specific MAC configuration
 */
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_STA   1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP    1
#define CONFIG_ESP32S2_UNIVERSAL_MAC_ADDRESSES  2

/**
 * ESP32-S2 specific serial flasher config (DO NOT CHANGE)
 */
#define CONFIG_ESPTOOLPY_FLASHFREQ_80M          1
#define CONFIG_ESPTOOLPY_FLASHFREQ              "80m"

/**
 * ESP32-S2 specific system configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_TIMER_IMPL_SYSTIMER          1
#define CONFIG_ESP_CONSOLE_MULTIPLE_UART        1

#define CONFIG_ESP32S2_DEBUG_OCDAWARE           1

#define CONFIG_ESP32S2_BROWNOUT_DET             1
#define CONFIG_ESP32S2_BROWNOUT_DET_LVL         7

#define CONFIG_ESP32S2_TRACEMEM_RESERVE_DRAM    0x0
#define CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM   0

/**
 * ESP32-S2 specific sleep configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_SLEEP_RTC_BUS_ISO_WORKAROUND 1

/**
 * ESP32-S2 specific USB configuration
 */
#ifdef MODULE_ESP_IDF_USB
#define CONFIG_USB_OTG_SUPPORTED                1
#endif
/**
 * ESP32-S2 specific SPI RAM configuration
 */
#ifdef  MODULE_ESP_SPI_RAM
#define CONFIG_ESP32S2_SPIRAM_SUPPORT           1
#ifdef MODULE_ESP_SPI_OCT
#define CONFIG_SPIRAM_MODE_OCT                  1
#else
#define CONFIG_SPIRAM_MODE_QUAD                 1
#endif
#define CONFIG_DEFAULT_PSRAM_CLK_IO             30
#define CONFIG_DEFAULT_PSRAM_CS_IO              26
#define CONFIG_SPIRAM_SUPPORT                   CONFIG_ESP32S2_SPIRAM_SUPPORT
#endif

/**
 * ESP32-S2 specific Cache config
 */
#define CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB        1
#define CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_32B   1
#define CONFIG_ESP32S2_DATA_CACHE_8KB               1
#define CONFIG_ESP32S2_DATA_CACHE_LINE_32B          1

/**
 * ESP32-S2 specific system configuration
 */
#define CONFIG_ESP_SYSTEM_MEMPROT_DEPCHECK              1
#define CONFIG_ESP_SYSTEM_MEMPROT_FEATURE               0   /* default enabled */
#define CONFIG_ESP_SYSTEM_MEMPROT_FEATURE_LOCK          0   /* default enabled */
#define CONFIG_ESP_SYSTEM_MEMPROT_CPU_PREFETCH_PAD_SIZE 16
#define CONFIG_ESP_SYSTEM_MEMPROT_MEM_ALIGN_SIZE        4

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
#endif /* SDKCONFIG_ESP32S2_H */
