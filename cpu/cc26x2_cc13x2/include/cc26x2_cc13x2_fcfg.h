/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc26x2_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 FCFG register definitions
 */

#ifndef CC26X2_CC13X2_FCFG_H
#define CC26X2_CC13X2_FCFG_H

#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   FCFG registers
 */
typedef struct {
    reg8_t __reserved1[0xA0]; /**< Reserved */
    reg32_t MISC_CONF_1; /**< misc config */
    reg32_t MISC_CONF_2; /**< misc config */
    reg32_t __reserved2[0x2]; /**< Reserved */
    reg32_t HPOSC_MEAS_5; /**< Internal */
    reg32_t HPOSC_MEAS_4; /**< Internal */
    reg32_t HPOSC_MEAS_3; /**< Internal */
    reg32_t HPOSC_MEAS_2; /**< Internal */
    reg32_t HPOSC_MEAS_1; /**< Internal */
    reg32_t CONFIG_FE_CC26; /**< Internal */
    reg32_t CONFIG_FE_CC13; /**< Internal */
    reg32_t CONFIG_RF_COMMON; /**< Internal */
    reg32_t CONFIG_SYNTH_DIV2_CC26_2G4; /**< Config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV2_CC13_2G4; /**< Config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV2_CC26_1G; /**< Config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV2_CC13_1G; /**< Config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV4_CC26; /**< Config of synthesizer in divide-by-4-mode */
    reg32_t CONFIG_SYNTH_DIV4_CC13; /**< Config of synthesizer in divide-by-4-mode */
    reg32_t CONFIG_SYNTH_DIV5; /**< Config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV6_CC26; /**< Config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV6_CC13; /**< Config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV10; /**< Config of synthesizer in divide-by-10-mode */
    reg32_t CONFIG_SYNTH_DIV12_CC26; /**< Config of synthesizer in divide-by-12-mode */
    reg32_t CONFIG_SYNTH_DIV12_CC13; /**< Config of synthesizer in divide-by-12-mode */
    reg32_t CONFIG_SYNTH_DIV15; /**< Config of synthesizer in divide-by-15-mode */
    reg32_t CONFIG_SYNTH_DIV30; /**< Config of synthesizer in divide-by-30-mode */
    reg32_t __reserved3[0x17]; /**< Reserved */
    reg32_t FLASH_NUMBER; /**< Manufacturing lot number */
    reg32_t __reserved4; /**< Reserved */
    reg32_t FLASH_COORDINATE; /**< Chip coordinates on a wafer */
    reg32_t FLASH_E_P; /**< Internal */
    reg32_t FLASH_C_E_P_R; /**< Internal */
    reg32_t FLASH_P_R_PV; /**< Internal */
    reg32_t FLASH_EH_SEQ; /**< Internal */
    reg32_t FLASH_VHV_E; /**< Internal */
    reg32_t FLASH_PP; /**< Internal */
    reg32_t FLASH_PROG_EP; /**< Internal */
    reg32_t FLASH_ERA_PW; /**< Internal */
    reg32_t FLASH_VHV; /**< Internal */
    reg32_t FLASH_VHV_PV; /**< Internal */
    reg32_t FLASH_V; /**< Internal */
    reg32_t __reserved5[0x3E]; /**< Reserved */
    reg32_t USER_ID; /* User identification */
    reg32_t __reserved6[0x6]; /**< Reserved */
    reg32_t FLASH_OTP_DATA3; /**< Internal */
    reg32_t ANA2_TRIM; /**< Internal */
    reg32_t LDO_TRIM; /**< Internal */
    reg32_t __reserved7[0xB]; /**< Reserved */
    reg32_t MAC_BLE_0; /**< MAC BLE address 0 */
    reg32_t MAC_BLE_1; /**< MAC BLE address 1 */
    reg32_t MAC_15_4_0; /**< MAC IEEE 802.15.4 address 0 */
    reg32_t MAC_15_4_1; /**< MAC IEEE 802.15.4 address 1 */
    reg32_t __reserved8[0x4]; /**< Reserved */
    reg32_t FLASH_OTP_DATA4; /**< Internal */
    reg32_t MISC_TRIM; /**< Miscellaneous trim parameters */
    reg32_t RCOSC_HF_TEMPCOMP; /**< Internal */
    reg32_t __reserved9; /**< Reserved */
    reg32_t ICEPICK_DEVICE_ID; /**< IcePick Device Identification */
    reg32_t FCFG1_REVISION; /**< Factory configuration revision */
    reg32_t MISC_OTP_DATA; /**< Miscelanous OTP data */
    reg32_t __reserved10[0x8]; /**< Reserved */
    reg32_t IOCONF; /**< I/O Configuration */
    reg32_t __reserved11; /**< Reserved */
    reg32_t CONFIG_IF_ADC; /**< Internal */
    reg32_t CONFIG_OSC_TOP; /**< Internal */
    reg32_t __reserved12[0x2]; /**< Reserved */
    reg32_t SOC_ADC_ABS_GAIN; /**< AUX_ADC gain in absolute reference mode */
    reg32_t SOC_ADC_REL_GAIN; /**< AUX_ADC gain in relative reference mode */
    reg32_t __reserved13; /**< Reserved */
    reg32_t SOC_ADC_OFFSET_INT; /**< AUX_ADC temperature offsets in absolute reference mode */
    reg32_t SOC_ADC_REF_TRIM_AND_OFFSET_EXT; /**< Internal */
    reg32_t AMPCOMP_TH1; /**< Internal */
    reg32_t AMPCOMP_TH2; /**< Internal */
    reg32_t AMPCOMP_CTRL1; /**< Internal */
    reg32_t ANABYPASS_VALUE2; /**< Internal */
    reg32_t __reserved14[0x2]; /**< Reserved */
    reg32_t VOLT_TRIM; /**< Internal */
    reg32_t OSC_CONF; /**< OSC configuration */
    reg32_t FREQ_OFFSET; /**< Internal */
    reg32_t __reserved15; /**< Reserved */
    reg32_t MISC_OTP_DATA_1; /**< Internal */
    reg32_t __reserved16[0xC]; /**< Reserved */
    reg32_t SHDW_DIE_ID_0; /**< Shadow of JTAG_TAP::EFUSE::DIE_ID_0.* */
    reg32_t SHDW_DIE_ID_1; /**< Shadow of JTAG_TAP::EFUSE::DIE_ID_1.* */
    reg32_t SHDW_DIE_ID_2; /**< Shadow of JTAG_TAP::EFUSE::DIE_ID_2.* */
    reg32_t SHDW_DIE_ID_3; /**< Shadow of JTAG_TAP::EFUSE::DIE_ID_3.* */
    reg32_t __reserved17[0x7]; /**< Reserved */
    reg32_t SHDW_OSC_BIAS_LDO_TRIM; /**< Internal */
    reg32_t SHDW_ANA_TRIM; /**< Internal */
    reg32_t __reserved18[0x3]; /**< Reserved */
    reg32_t DAC_BIAS_CNF; /**< Internal */
    reg32_t __reserved19[0x2]; /**< Reserved */
    reg32_t TFW_PROBE; /**< Internal */
    reg32_t TFW_FT; /**< Internal */
    reg32_t DAC_CAL0; /**< Internal */
    reg32_t DAC_CAL1; /**< Internal */
    reg32_t DAC_CAL2; /**< Internal */
    reg32_t DAC_CAL3; /**< Internal */
} fcfg_regs_t;

/**
 * @brief   FCFG1 register values
 * @{
 */
#define FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_m       0x0003F000
#define FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_s       12
#define FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_m 0x00000E00
#define FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_s 9
#define FCFG1_DAC_BIAS_CNF_LPM_BIAS_BACKUP_EN    0x00000100
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   FCFG1 base address
 */
#define FCFG_BASE            (0x50001000)
/** @} */

/**
 * @brief   FCFG1 register bank
 */
#define FCFG                 ((fcfg_regs_t *) (FCFG_BASE))

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_FCFG_H */

/** @} */
