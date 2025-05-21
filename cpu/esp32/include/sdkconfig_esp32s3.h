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
 * @brief       SDK configuration used by the ESP-IDF for ESP32-S3 SoC variant (family)
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
 * @name ESP32-S3 specific clock configuration
 * @{
 */

/* Mapping of Kconfig defines to the respective enumeration values */
#if CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_2
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     2
#elif CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_5
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     5
#elif CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_10
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     10
#elif CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_20
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     20
#elif CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_40
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     40
#elif CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_80
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     80
#elif CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_160
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     160
#elif CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ_240
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     240
#endif

/**
 * @brief Defines the CPU frequency [values = 2, 5, 10, 10, 40, 80, 160, 240]
 */
#ifndef CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ     80
#endif
/** @} */

/**
 * ESP32-S3 specific RTC clock configuration
 */
#define CONFIG_ESP32S3_RTC_CLK_CAL_CYCLES       (8 * 1024)

/**
 * ESP32-S3 specific EFUSE configuration
 */
#define CONFIG_EFUSE_MAX_BLK_LEN                256

/**
 * ESP32-S3 specific MAC configuration
 */
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_STA   1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP    1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_BT         1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH        1
#define CONFIG_ESP32S3_UNIVERSAL_MAC_ADDRESSES  4

/**
 * ESP32-S3 specific serial flasher config (DO NOT CHANGE)
 */
#define CONFIG_ESPTOOLPY_FLASHFREQ_80M          1
#define CONFIG_ESPTOOLPY_FLASHFREQ              "80m"

/**
 * ESP32-S3 specific system configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_TIMER_IMPL_SYSTIMER          1
#define CONFIG_ESP_CONSOLE_MULTIPLE_UART        1

#define CONFIG_ESP32S3_DEBUG_OCDAWARE           1

#define CONFIG_ESP32S3_BROWNOUT_DET             1
#define CONFIG_ESP32S3_BROWNOUT_DET_LVL         7

#define CONFIG_ESP32S3_DEEP_SLEEP_WAKEUP_DELAY  2000
#define CONFIG_ESP32S3_TRACEMEM_RESERVE_DRAM    0x0
#define CONFIG_ESP32S3_ULP_COPROC_RESERVE_MEM   0

/**
 * ESP32-S3 specific sleep configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_SLEEP_RTC_BUS_ISO_WORKAROUND 1
#define CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND  1

/**
 * ESP32-S3 specific USB configuration
 */
#define CONFIG_ESP_PHY_ENABLE_USB               1
#ifdef MODULE_ESP_IDF_USB
#define CONFIG_USB_OTG_SUPPORTED                1
#endif

/**
 * ESP32-S3 specific SPI RAM configuration
 */
#ifdef  MODULE_ESP_SPI_RAM
#define CONFIG_ESP32S3_SPIRAM_SUPPORT           1
#ifdef MODULE_ESP_SPI_OCT
#define CONFIG_SPIRAM_MODE_OCT                  1
#else
#define CONFIG_SPIRAM_MODE_QUAD                 1
#endif
#define CONFIG_DEFAULT_PSRAM_CLK_IO             30
#define CONFIG_DEFAULT_PSRAM_CS_IO              26
#define CONFIG_SPIRAM_SUPPORT                   CONFIG_ESP32S3_SPIRAM_SUPPORT
#endif

/**
 * ESP32-S3 specific Cache config
 */
#define CONFIG_ESP32S3_INSTRUCTION_CACHE_16KB       1
#define CONFIG_ESP32S3_INSTRUCTION_CACHE_SIZE       0x4000
#define CONFIG_ESP32S3_INSTRUCTION_CACHE_8WAYS      1
#define CONFIG_ESP32S3_ICACHE_ASSOCIATED_WAYS       8
#define CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_32B   1
#define CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_SIZE  32
#define CONFIG_ESP32S3_DATA_CACHE_32KB              1
#define CONFIG_ESP32S3_DATA_CACHE_SIZE              0x8000
#define CONFIG_ESP32S3_DATA_CACHE_8WAYS             1
#define CONFIG_ESP32S3_DCACHE_ASSOCIATED_WAYS       8
#define CONFIG_ESP32S3_DATA_CACHE_LINE_32B          1
#define CONFIG_ESP32S3_DATA_CACHE_LINE_SIZE         32

/**
 * ESP32-S3 BLE driver configuration (DO NOT CHANGE)
 */
#ifdef MODULE_ESP_BLE
#define CONFIG_BT_CONTROLLER_ONLY                       1
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
#define CONFIG_BT_CTRL_PINNED_TO_CORE_0                 1
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
