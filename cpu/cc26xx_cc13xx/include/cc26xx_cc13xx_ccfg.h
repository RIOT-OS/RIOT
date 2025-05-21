/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx CCFG register definitions
 */

#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CCFG registers
 */
typedef struct {
    reg32_t EXT_LF_CLK; /**< extern LF clock config */
    reg32_t MODE_CONF_1; /**< mode config 1 */
    reg32_t SIZE_AND_DIS_FLAGS; /**< CCFG size and disable flags */
    reg32_t MODE_CONF; /**< mmode config 0 */
    reg32_t VOLT_LOAD_0; /**< voltage load 0 */
    reg32_t VOLT_LOAD_1; /**< voltage load 1 */
    reg32_t RTC_OFFSET; /**< RTC offset */
    reg32_t FREQ_OFFSET; /**< frequency offset */
    reg32_t IEEE_MAC_0; /**< IEEE MAC address 0 */
    reg32_t IEEE_MAC_1; /**< IEEE MAC address 1 */
    reg32_t IEEE_BLE_0; /**< IEEE BLE address 0 */
    reg32_t IEEE_BLE_1; /**< IEEE BLE address 1 */
    reg32_t BL_CONFIG; /**< bootloader config */
    reg32_t ERASE_CONF; /**< erase config */
    reg32_t CCFG_TI_OPTIONS; /**< TI options */
    reg32_t CCFG_TAP_DAP_0; /**< test access points enable 0 */
    reg32_t CCFG_TAP_DAP_1; /**< test access points enable 1 */
    reg32_t IMAGE_VALID_CONF; /**< image valid */
    reg32_t CCFG_PROT_31_0; /**< protect sectors 0-31 */
    reg32_t CCFG_PROT_63_32; /**< protect sectors 32-63 */
    reg32_t CCFG_PROT_95_64; /**< protect sectors 64-95 */
    reg32_t CCFG_PROT_127_96; /**< protect sectors 96-127 */
} ccfg_regs_t;

/**
 * @brief   CCFG register values
 * @{
 */
#define CCFG_EXT_LF_CLK_DIO_m                          0xFF000000
#define CCFG_EXT_LF_CLK_DIO_s                          24
#define CCFG_EXT_LF_CLK_RTC_INCREMENT_m                0x00FFFFFF
#define CCFG_EXT_LF_CLK_RTC_INCREMENT_s                0
#define CCFG_MODE_CONF_1_TCXO_TYPE_m                   0x80000000
#define CCFG_MODE_CONF_1_TCXO_TYPE_s                   31
#define CCFG_MODE_CONF_1_TCXO_MAX_START_m              0x7F000000
#define CCFG_MODE_CONF_1_TCXO_MAX_START_s              24
#define CCFG_MODE_CONF_1_ALT_DCDC_VMIN_m               0x00F00000
#define CCFG_MODE_CONF_1_ALT_DCDC_VMIN_s               20
#define CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN_m          0x00080000
#define CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN_s          19
#define CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_m              0x00070000
#define CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_s              16
#define CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_m            0x0000F000
#define CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_s            12
#define CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_m          0x00000F00
#define CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_s          8
#define CCFG_MODE_CONF_1_XOSC_MAX_START_m              0x000000FF
#define CCFG_MODE_CONF_1_XOSC_MAX_START_s              0
#define CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_m         0xFFFF0000
#define CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_s         16
#define CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_m        0x0000FFF0
#define CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_s        4
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO_m             0x00000008
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO_s             3
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM_m            0x00000004
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM_s            2
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING   0x00000002
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING_m 0x00000002
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING_s 1
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_m         0x00000001
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_s         0
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_m         0xF0000000
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_s         28
#define CCFG_MODE_CONF_DCDC_RECHARGE_m                 0x08000000
#define CCFG_MODE_CONF_DCDC_RECHARGE_s                 27
#define CCFG_MODE_CONF_DCDC_ACTIVE_m                   0x04000000
#define CCFG_MODE_CONF_DCDC_ACTIVE_s                   26
#define CCFG_MODE_CONF_VDDR_EXT_LOAD                   0x02000000
#define CCFG_MODE_CONF_VDDR_EXT_LOAD_m                 0x02000000
#define CCFG_MODE_CONF_VDDR_EXT_LOAD_s                 25
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL                  0x01000000
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL_m                0x01000000
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL_s                24
#define CCFG_MODE_CONF_SCLK_LF_OPTION_m                0x00C00000
#define CCFG_MODE_CONF_SCLK_LF_OPTION_s                22
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC_m            0x00200000
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_TC_s            21
#define CCFG_MODE_CONF_RTC_COMP_m                      0x00100000
#define CCFG_MODE_CONF_RTC_COMP_s                      20
#define CCFG_MODE_CONF_XOSC_FREQ_m                     0x000C0000
#define CCFG_MODE_CONF_XOSC_FREQ_s                     18
#define CCFG_MODE_CONF_XOSC_CAP_MOD_m                  0x00020000
#define CCFG_MODE_CONF_XOSC_CAP_MOD_s                  17
#define CCFG_MODE_CONF_HF_COMP_m                       0x00010000
#define CCFG_MODE_CONF_HF_COMP_s                       16
#define CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_m           0x0000FF00
#define CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_s           8
#define CCFG_MODE_CONF_VDDR_CAP_m                      0x000000FF
#define CCFG_MODE_CONF_VDDR_CAP_s                      0
#define CCFG_BL_CONFIG_BOOTLOADER_ENABLE_m             0xFF000000
#define CCFG_BL_CONFIG_BOOTLOADER_ENABLE_s             24
#define CCFG_BL_CONFIG_BL_LEVEL_m                      0x00010000
#define CCFG_BL_CONFIG_BL_LEVEL_s                      16
#define CCFG_BL_CONFIG_BL_PIN_NUMBER_m                 0x0000FF00
#define CCFG_BL_CONFIG_BL_PIN_NUMBER_s                 8
#define CCFG_BL_CONFIG_BL_ENABLE_m                     0x000000FF
#define CCFG_BL_CONFIG_BL_ENABLE_s                     0
#define CCFG_ERASE_CONF_CHIP_ERASE_DIS_N_m             0x00000100
#define CCFG_ERASE_CONF_CHIP_ERASE_DIS_N_s             8
#define CCFG_ERASE_CONF_BANK_ERASE_DIS_N_m             0x00000001
#define CCFG_ERASE_CONF_BANK_ERASE_DIS_N_s             0
#define CCFG_CCFG_TI_OPTIONS_TI_FA_ENABLE_m            0x000000FF
#define CCFG_CCFG_TI_OPTIONS_TI_FA_ENABLE_s            0
#define CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE_m           0x00FF0000
#define CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE_s           16
#define CCFG_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE_m       0x0000FF00
#define CCFG_CCFG_TAP_DAP_0_PWRPROF_TAP_ENABLE_s       8
#define CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE_m          0x000000FF
#define CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE_s          0
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   CCFG base address
 */
#define CCFG_BASE            (0x50003000)
/** @} */

#ifdef CPU_VARIANT_X0
/**
 * @brief   CCFG register bank
 */
#define CCFG                 ((ccfg_regs_t *) (CCFG_BASE + 0xFA8))
#else
/**
 * @brief   CCFG register bank
 */
#define CCFG                 ((ccfg_regs_t *) (CCFG_BASE + 0x1FA8))
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
