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
 * @brief       SDK configuration used by the ESP-IDF for ESP32-C3 SoC variant (family)
 *
 * The SDK configuration can be partially overridden by application-specific
 * board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

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

/**
 * ESP32-C3 specific USB configuration
 */
#define CONFIG_ESP_PHY_ENABLE_USB               1

/**
 * ESP32-C3 BLE driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_BLE
#define CONFIG_BT_CTRL_ADV_DUP_FILT_MAX                 30
#define CONFIG_BT_CTRL_BLE_ADV_REPORT_DISCARD_THRSHOLD  20
#define CONFIG_BT_CTRL_BLE_ADV_REPORT_FLOW_CTRL_NUM     100
#define CONFIG_BT_CTRL_BLE_ADV_REPORT_FLOW_CTRL_SUPP    1
#define CONFIG_BT_CTRL_BLE_MAX_ACT                      10
#define CONFIG_BT_CTRL_BLE_MAX_ACT_EFF                  10
#define CONFIG_BT_CTRL_BLE_SCAN_DUPL                    1
#define CONFIG_BT_CTRL_BLE_STATIC_ACL_TX_BUF_NB         0
#define CONFIG_BT_CTRL_CE_LENGTH_TYPE_EFF               0
#define CONFIG_BT_CTRL_CE_LENGTH_TYPE_ORIG              1
#define CONFIG_BT_CTRL_COEX_PHY_CODED_TX_RX_TLIM_DIS    1
#define CONFIG_BT_CTRL_COEX_PHY_CODED_TX_RX_TLIM_EFF    0
#define CONFIG_BT_CTRL_DFT_TX_POWER_LEVEL_EFF           10
#define CONFIG_BT_CTRL_DFT_TX_POWER_LEVEL_P3            1
#define CONFIG_BT_CTRL_HCI_MODE_VHCI                    1
#define CONFIG_BT_CTRL_HCI_TL                           1
#define CONFIG_BT_CTRL_HCI_TL_EFF                       1
#define CONFIG_BT_CTRL_HW_CCA_EFF                       0
#define CONFIG_BT_CTRL_HW_CCA_VAL                       20
#define CONFIG_BT_CTRL_MODE_EFF                         1
#define CONFIG_BT_CTRL_PINNED_TO_CORE                   0
#define CONFIG_BT_CTRL_RX_ANTENNA_INDEX_0               1
#define CONFIG_BT_CTRL_RX_ANTENNA_INDEX_EFF             0
#define CONFIG_BT_CTRL_SCAN_DUPL_CACHE_SIZE             100
#define CONFIG_BT_CTRL_SCAN_DUPL_TYPE                   0
#define CONFIG_BT_CTRL_SCAN_DUPL_TYPE_DEVICE            1
#define CONFIG_BT_CTRL_SLEEP_CLOCK_EFF                  0
#define CONFIG_BT_CTRL_SLEEP_MODE_EFF                   0
#define CONFIG_BT_CTRL_TX_ANTENNA_INDEX_0               1
#define CONFIG_BT_CTRL_TX_ANTENNA_INDEX_EFF             0
#define CONFIG_BT_ENABLED                               1
#define CONFIG_BT_SOC_SUPPORT_5_0                       1
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
