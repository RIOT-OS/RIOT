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
 * @brief       SDK configuration used by the ESP-IDF for ESP32 SoC variant (family)
 *
 * The SDK configuration can be partially overridden by application-specific
 * board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef SDKCONFIG_ESP32_H
#define SDKCONFIG_ESP32_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ESP32 specific clock configuration
 * @{
 */

/* Mapping of Kconfig defines to the respective enumeration values */
#if CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_2
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       2
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_5
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       5
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_10
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       10
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_20
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       20
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_40
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       40
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_80
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       80
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_160
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       160
#elif CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ_240
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       240
#endif

/**
 * @brief Defines the CPU frequency [values = 2, 5, 10, 20, 40, 80, 160, 240]
 */
#ifndef CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ       80
#endif

/**
 * @brief Mapping configured ESP32 default clock to CLOCK_CORECLOCK define
 */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
/** @} */

/**
 * ESP32 specific RTC clock configuration
 */
#define CONFIG_ESP32_RTC_CLK_CAL_CYCLES         (8 * 1024)

/**
 * ESP32 specific EFUSE configuration
 */
#define CONFIG_EFUSE_MAX_BLK_LEN                192
#define CONFIG_EFUSE_CODE_SCHEME_COMPAT_3_4     1

/**
 * ESP32 specific MAC configuration
 */
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_STA   1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP    1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_BT         1
#define CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH        1
#define CONFIG_ESP32_UNIVERSAL_MAC_ADDRESSES    4

/**
 * ESP32 specific system configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP_TIMER_IMPL_FRC2              1
#define CONFIG_ESP_CONSOLE_MULTIPLE_UART        1

#define CONFIG_ESP32_DEBUG_OCDAWARE             1
#define CONFIG_ESP32_REV_MIN                    0

#define CONFIG_ESP32_BROWNOUT_DET               1
#define CONFIG_ESP32_BROWNOUT_DET_LVL           0
#define CONFIG_BROWNOUT_DET                     CONFIG_ESP32_BROWNOUT_DET

#define CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY    2000
#define CONFIG_ESP32_TRACEMEM_RESERVE_DRAM      0
#define CONFIG_ESP32_ULP_COPROC_RESERVE_MEM     0

/**
 * ESP32 specific ADC calibration
 */
#define CONFIG_ADC_CAL_EFUSE_TP_ENABLE          1
#define CONFIG_ADC_CAL_EFUSE_VREF_ENABLE        1
#define CONFIG_ADC_CAL_LUT_ENABLE               1

/**
 * ESP32 specific PHY configuration
 */
#define CONFIG_ESP_PHY_REDUCE_TX_POWER          1
#define CONFIG_ESP32_REDUCE_PHY_TX_POWER        CONFIG_ESP_PHY_REDUCE_TX_POWER
#define CONFIG_REDUCE_PHY_TX_POWER              CONFIG_ESP_PHY_REDUCE_TX_POWER

/**
 * ESP32 specific XTAL configuration
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

/**
 * ESP32 specific SPI RAM configuration
 */
#ifdef  MODULE_ESP_SPI_RAM
#define CONFIG_ESP32_SPIRAM_SUPPORT             1
#define CONFIG_D0WD_PSRAM_CLK_IO                17
#define CONFIG_D0WD_PSRAM_CS_IO                 16
#define CONFIG_D2WD_PSRAM_CLK_IO                9
#define CONFIG_D2WD_PSRAM_CS_IO                 10
#define CONFIG_PICO_PSRAM_CS_IO                 10
#define CONFIG_SPIRAM_BANKSWITCH_ENABLE         1
#define CONFIG_SPIRAM_BANKSWITCH_RESERVE        8
#define CONFIG_SPIRAM_CACHE_WORKAROUND          1
#define CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_MEMW 1
#define CONFIG_SPIRAM_SPIWP_SD3_PIN             7
#define CONFIG_SPIRAM_SUPPORT                   CONFIG_ESP32_SPIRAM_SUPPORT
#endif

/**
 * ESP32 specific ETH configuration
 */
#ifdef MODULE_ESP_ETH
#define CONFIG_ETH_USE_ESP32_EMAC               1
#define CONFIG_ETH_PHY_INTERFACE_RMII           1
#define CONFIG_ETH_RMII_CLK_INPUT               1
#define CONFIG_ETH_RMII_CLK_IN_GPIO             0
#define CONFIG_ETH_DMA_BUFFER_SIZE              512
#define CONFIG_ETH_DMA_RX_BUFFER_NUM            10
#define CONFIG_ETH_DMA_TX_BUFFER_NUM            10
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SDKCONFIG_ESP32_H */
/** @} */
