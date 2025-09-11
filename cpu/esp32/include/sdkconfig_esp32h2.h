/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       SDK configuration used by the ESP-IDF for ESP32-H2 SoC variant (family)
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
 * @name ESP32-H2 specific clock configuration
 * @{
 */

/* external crystal frequency */
#define CONFIG_XTAL_FREQ 32

/* Mapping of Kconfig defines to the respective enumeration values */
#if CONFIG_ESP32H2_DEFAULT_CPU_FREQ_MHZ_16
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       16
#elif CONFIG_ESP32H2_DEFAULT_CPU_FREQ_MHZ_32
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       32
#elif CONFIG_ESP32H2_DEFAULT_CPU_FREQ_MHZ_48
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       48
#elif CONFIG_ESP32H2_DEFAULT_CPU_FREQ_MHZ_64
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       64
#elif CONFIG_ESP32H2_DEFAULT_CPU_FREQ_MHZ_96
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       96
#endif

/**
 * @brief Defines the CPU frequency [values = 16, 32, 48, 64, 96]
 */
#ifndef CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ       96
#endif

/** @} */

/**
 * ESP32-H2 MMU configuration (DO NOT CHANGE)
 */
#ifndef CONFIG_MMU_PAGE_SIZE
#  define CONFIG_MMU_PAGE_SIZE_32KB             1
#  define CONFIG_MMU_PAGE_SIZE                  0x8000
#endif

/**
 * ESP32-H2 specific RTC clock configuration
 */
#define CONFIG_RTC_CLK_CAL_CYCLES               1024

#ifdef MODULE_ESP_RTC_TIMER_32K
#endif

/**
 * ESP32-H2 specific EFUSE configuration
 */
#define CONFIG_EFUSE_MAX_BLK_LEN                256
#define CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL     0
#define CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL     99

/**
 * ESP32-H2 specific MAC configuration
 */
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_STA   1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP    1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_BT         1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_IEEE802154 1
#define CONFIG_ESP32H2_UNIVERSAL_MAC_ADDRESSES  2

/**
 * ESP32-H2 specific serial flasher config (DO NOT CHANGE)
 */
#define CONFIG_ESPTOOLPY_FLASHFREQ_64M          1
#define CONFIG_ESPTOOLPY_FLASHFREQ              "64m"

/**
 * ESP32-H2 specific system configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP32H2_REV_MIN_FULL             0
#define CONFIG_ESP32H2_REV_MAX_FULL             99

#define CONFIG_ESP_TIMER_IMPL_SYSTIMER          1
#define CONFIG_ESP_TIMER_INTERRUPT_LEVEL        1

#define CONFIG_ESP_DEBUG_OCDAWARE               1

#define CONFIG_ESP_BROWNOUT_DET                 1
#define CONFIG_ESP_BROWNOUT_DET_LVL             0

/**
 * ESP32-H2 specific sleep configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB   1
#define CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND       1
#define CONFIG_ESP_SLEEP_GPIO_ENABLE_INTERNAL_RESISTORS 0   /* we realize it */
#define CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND          1
#define CONFIG_ESP_SLEEP_WAIT_FLASH_READY_EXTRA_DELAY   0
#define CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP         0

/**
 * ESP32-H2 BLE driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_BLE
#  define CONFIG_BT_ALARM_MAX_NUM                           50
#  define CONFIG_BT_CTRL_BLE_ADV_REPORT_DISCARD_THRSHOLD    20
#  define CONFIG_BT_CTRL_BLE_ADV_REPORT_FLOW_CTRL_NUM       100
#  define CONFIG_BT_CTRL_BLE_ADV_REPORT_FLOW_CTRL_SUPP      1
#  define CONFIG_BT_LE_50_FEATURE_SUPPORT                   1
#  define CONFIG_BT_LE_ACL_BUF_COUNT                        10
#  define CONFIG_BT_LE_ACL_BUF_SIZE                         517
#  define CONFIG_BT_LE_COEX_PHY_CODED_TX_RX_TLIM_DIS        1
#  define CONFIG_BT_LE_COEX_PHY_CODED_TX_RX_TLIM_EFF        0
#  define CONFIG_BT_LE_CONTROLLER_NPL_OS_PORTING_SUPPORT    1
#  define CONFIG_BT_LE_CONTROLLER_TASK_STACK_SIZE           4096
#  define CONFIG_BT_LE_CRYPTO_STACK_MBEDTLS                 0   /* default 1 */
#  define CONFIG_BT_LE_DFT_TX_POWER_LEVEL_DBM_EFF           9
#  define CONFIG_BT_LE_DFT_TX_POWER_LEVEL_P9                1
#  define CONFIG_BT_LE_ENABLE_PERIODIC_ADV                  1
#  define CONFIG_BT_LE_EXT_ADV_MAX_SIZE                     1650
#  define CONFIG_BT_LE_EXT_ADV                              1
#  define CONFIG_BT_LE_HCI_EVT_BUF_SIZE                     257
#  define CONFIG_BT_LE_HCI_EVT_HI_BUF_COUNT                 30
#  define CONFIG_BT_LE_HCI_EVT_LO_BUF_COUNT                 8
#  define CONFIG_BT_LE_HCI_INTERFACE_USE_RAM                1
#  define CONFIG_BT_LE_LL_CFG_FEAT_LE_2M_PHY                1
#  define CONFIG_BT_LE_LL_CFG_FEAT_LE_CODED_PHY             1
#  define CONFIG_BT_LE_LL_CFG_FEAT_LE_ENCRYPTION            1
#  define CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT               20
#  define CONFIG_BT_LE_LL_RESOLV_LIST_SIZE                  4
#  define CONFIG_BT_LE_LL_SCA                               60
#  define CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL                 1
#  define CONFIG_BT_LE_MAX_CONNECTIONS                      3
#  define CONFIG_BT_LE_MAX_EXT_ADV_INSTANCES                1
#  define CONFIG_BT_LE_MAX_PERIODIC_ADVERTISER_LIST         5
#  define CONFIG_BT_LE_MAX_PERIODIC_SYNCS                   1
#  define CONFIG_BT_LE_MSYS_1_BLOCK_COUNT                   12
#  define CONFIG_BT_LE_MSYS_1_BLOCK_SIZE                    256
#  define CONFIG_BT_LE_MSYS_2_BLOCK_COUNT                   24
#  define CONFIG_BT_LE_MSYS_2_BLOCK_SIZE                    320
#  define CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER              1
#  define CONFIG_BT_LE_PERIODIC_ADV_SYNC_TRANSFER           1
#  define CONFIG_BT_LE_SCAN_DUPL_CACHE_REFRESH_PERIOD       0
#  define CONFIG_BT_LE_SCAN_DUPL_TYPE                       0
#  define CONFIG_BT_LE_SCAN_DUPL_TYPE_DEVICE                1
#  define CONFIG_BT_LE_SCAN_DUPL                            1
#  define CONFIG_BT_LE_SECURITY_ENABLE                      1
#  define CONFIG_BT_LE_SM_LEGACY                            1
#  define CONFIG_BT_LE_SM_SC                                1
#  define CONFIG_BT_LE_USE_ESP_TIMER                        1
#  define CONFIG_BT_LE_WHITELIST_SIZE                       12
#  define CONFIG_BT_NIMBLE_COEX_PHY_CODED_TX_RX_TLIM_DIS    1
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
