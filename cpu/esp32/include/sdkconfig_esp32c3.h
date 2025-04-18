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
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       2
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_5
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       5
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_10
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       10
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_20
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       20
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_40
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       40
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_80
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       80
#elif CONFIG_ESP32C3_DEFAULT_CPU_FREQ_MHZ_160
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       160
#endif

/**
 * @brief Defines the CPU frequency [values = 2, 5, 10, 20, 40, 80, 160]
 */
#ifndef CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       80
#endif

/** @} */

/**
 * ESP32-C3 specific RTC clock configuration
 */
#define CONFIG_RTC_CLK_CAL_CYCLES               1024

/**
 * ESP32-C3 specific EFUSE configuration
 */
#define CONFIG_EFUSE_MAX_BLK_LEN                256
#define CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL     0
#define CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL     199

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
#define CONFIG_ESP32C3_LIGHTSLEEP_GPIO_RESET_WORKAROUND 1
#define CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB   1
#define CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND          1
#define CONFIG_ESP_SLEEP_GPIO_ENABLE_INTERNAL_RESISTORS 0   /* we realize it */
#define CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND       1
#define CONFIG_ESP_SLEEP_POWER_DOWN_FLASH               1
#define CONFIG_ESP_SLEEP_WAIT_FLASH_READY_EXTRA_DELAY   0
#define CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP         1

/**
 * ESP32-C3 specific USB configuration
 */
#define CONFIG_ESP_PHY_ENABLE_USB                       1

/**
 * ESP32-C3 BLE driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_BLE
#  define CONFIG_BT_ALARM_MAX_NUM                           50
#  define CONFIG_BT_BLE_CCA_MODE                            0
#  define CONFIG_BT_BLE_CCA_MODE_NONE                       1
#  define CONFIG_BT_CTRL_ADV_DUP_FILT_MAX                   30
#  define CONFIG_BT_CTRL_BLE_ADV_REPORT_DISCARD_THRSHOLD    20
#  define CONFIG_BT_CTRL_BLE_ADV_REPORT_FLOW_CTRL_NUM       100
#  define CONFIG_BT_CTRL_BLE_ADV_REPORT_FLOW_CTRL_SUPP      1
#  define CONFIG_BT_CTRL_BLE_MAX_ACT                        10
#  define CONFIG_BT_CTRL_BLE_MAX_ACT_EFF                    10
#  define CONFIG_BT_CTRL_BLE_SCAN_DUPL                      1
#  define CONFIG_BT_CTRL_BLE_STATIC_ACL_TX_BUF_NB           0
#  define CONFIG_BT_CTRL_CE_LENGTH_TYPE_EFF                 0
#  define CONFIG_BT_CTRL_CE_LENGTH_TYPE_ORIG                1
#  define CONFIG_BT_CTRL_CHAN_ASS_EN                        1
#  define CONFIG_BT_CTRL_COEX_PHY_CODED_TX_RX_TLIM_DIS      1
#  define CONFIG_BT_CTRL_COEX_PHY_CODED_TX_RX_TLIM_EFF      0
#  define CONFIG_BT_CTRL_DFT_TX_POWER_LEVEL_EFF             11
#  define CONFIG_BT_CTRL_DFT_TX_POWER_LEVEL_P9              1
#  define CONFIG_BT_CTRL_DUPL_SCAN_CACHE_REFRESH_PERIOD     0
#  define CONFIG_BT_CTRL_HCI_MODE_VHCI                      1
#  define CONFIG_BT_CTRL_HCI_TL                             1
#  define CONFIG_BT_CTRL_HCI_TL_EFF                         1
#  define CONFIG_BT_CTRL_HW_CCA_EFF                         0
#  define CONFIG_BT_CTRL_HW_CCA_VAL                         20
#  define CONFIG_BT_CTRL_LE_PING_EN                         1
#  define CONFIG_BT_CTRL_MODE_EFF                           1
#  define CONFIG_BT_CTRL_PINNED_TO_CORE                     0
#  define CONFIG_BT_CTRL_PINNED_TO_CORE_0                   1
#  define CONFIG_BT_CTRL_RX_ANTENNA_INDEX_0                 1
#  define CONFIG_BT_CTRL_RX_ANTENNA_INDEX_EFF               0
#  define CONFIG_BT_CTRL_SCAN_DUPL_CACHE_SIZE               100
#  define CONFIG_BT_CTRL_SCAN_DUPL_TYPE                     0
#  define CONFIG_BT_CTRL_SCAN_DUPL_TYPE_DEVICE              1
#  define CONFIG_BT_CTRL_SLEEP_CLOCK_EFF                    0
#  define CONFIG_BT_CTRL_SLEEP_MODE_EFF                     0
#  define CONFIG_BT_CTRL_TX_ANTENNA_INDEX_0                 1
#  define CONFIG_BT_CTRL_TX_ANTENNA_INDEX_EFF               0
#  define CONFIG_BT_NIMBLE_COEX_PHY_CODED_TX_RX_TLIM_DIS    1
#endif

/* According to the ESP32-C3 Errata Sheet ADC2 does not work correctly.
 * To use ADC2 and GPIO5 as ADC channel, CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3
 * has to be set (default). */
#ifndef CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3
#  define CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3           1
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
