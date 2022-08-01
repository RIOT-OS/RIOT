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
 * @brief       SDK configuration used by the ESP-IDF for ESP32-C3 SoC variant (family)
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
 * @name ESP32-C3 specific clock configuration
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
 * @brief Defines the CPU frequency [values = 2, 5, 10, 20, 40, 80, 160]
 */
#ifndef CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ     80
#endif

/**
 * @brief Mapping configured ESP32-C3 default clock to CLOCK_CORECLOCK define
 */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ)
/** @} */

/**
 * ESP32-C3 specific RTC clock configuration
 */
#define CONFIG_ESP32C3_RTC_CLK_CAL_CYCLES       (8 * 1024)

/**
 * ESP32-C3 specific EFUSE configuration
 */
#define CONFIG_EFUSE_MAX_BLK_LEN                256

/**
 * ESP32-C3 specific MAC configuration
 */
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_STA   1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP    1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_BT         1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH        1
#define CONFIG_ESP32C3_UNIVERSAL_MAC_ADDRESSES  4

/**
 * ESP32-C3 specific system configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_TIMER_IMPL_SYSTIMER          1

#define CONFIG_ESP32C3_DEBUG_OCDAWARE           1
#define CONFIG_ESP32C3_REV_MIN                  3

#define CONFIG_ESP32C3_BROWNOUT_DET             1
#define CONFIG_ESP32C3_BROWNOUT_DET_LVL         7

/**
 * ESP32-C3 specific sleep configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_SLEEP_POWER_DOWN_FLASH       1
#define CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND  1

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SDKCONFIG_ESP32C3_H */
/** @} */
