/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc13x2_definitions
 * @{
 *
 * @file
 * @brief           CC13x2 FCFG register definitions
 */

#ifndef CC13X2_FCFG_H
#define CC13X2_FCFG_H

#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define FCFG_BASE                           0x50001000 /**<  base address of FCFG memory */
/*@}*/

/**
 * FCFG registers
 */
typedef struct {
    uint8_t __reserved1[0xA0]; /**< meh */
    /* TODO does it pad here? */
    reg32_t MISC_CONF_1; /**< misc config */
    reg32_t MISC_CONF_2; /**< misc config */
    reg32_t __reserved2[7]; /**< meh */
    reg32_t CONFIG_FE_CC26; /**< internal */
    reg32_t CONFIG_FE_CC13; /**< internal */
    reg32_t CONFIG_RF_COMMON; /**< internal */
    reg32_t CONFIG_SYNTH_DIV2_CC26_2G4; /**< config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV2_CC13_2G4; /**< config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV2_CC26_1G; /**< config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV2_CC13_1G; /**< config of synthesizer in divide-by-2-mode */
    reg32_t CONFIG_SYNTH_DIV4_CC26; /**< config of synthesizer in divide-by-4-mode */
    reg32_t CONFIG_SYNTH_DIV4_CC13; /**< config of synthesizer in divide-by-4-mode */
    reg32_t CONFIG_SYNTH_DIV5; /**< config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV6_CC26; /**< config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV6_CC13; /**< config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV10; /**< config of synthesizer in divide-by-10-mode */
    reg32_t CONFIG_SYNTH_DIV12_CC26; /**< config of synthesizer in divide-by-12-mode */
    reg32_t CONFIG_SYNTH_DIV12_CC13; /**< config of synthesizer in divide-by-12-mode */
    reg32_t CONFIG_SYNTH_DIV15; /**< config of synthesizer in divide-by-15-mode */
    reg32_t CONFIG_SYNTH_DIV30; /**< config of synthesizer in divide-by-30-mode */
    reg32_t __reserved3[23]; /**< meh */
    reg32_t FLASH_NUMBER; /**< manufacturing lot number */
    reg32_t __reserved4; /**< meh */
    reg32_t FLASH_COORDINATE; /**< chip coordinates on a wafer */
    reg32_t FLASH_E_P; /**< internal */
    reg32_t FLASH_C_E_P_R; /**< internal */
    reg32_t FLASH_P_R_PV; /**< internal */
    reg32_t FLASH_EH_SEQ; /**< internal */
    reg32_t FLASH_VHV_E; /**< internal */
    reg32_t FLASH_PP; /**< internal */
    reg32_t FLASH_PROG_EP; /**< internal */
    reg32_t FLASH_ERA_PW; /**< internal */
    reg32_t FLASH_VHV; /**< internal */
    reg32_t FLASH_VHV_PV; /**< internal */
    reg32_t FLASH_V; /**< internal */
    reg32_t __reserved5[38]; /**< meh */
    reg32_t USER_ID; /* user identification */
    reg32_t __reserved6[6]; /**< meh */
    reg32_t FLASH_OTP_DATA3; /**< internal */
    reg32_t ANA2_TRIM; /**< internal */
    reg32_t LDO_TRIM; /**< internal */
    reg32_t __reserved7[11]; /**< meh */
    reg32_t MAC_BLE_0; /**< MAC BLE address 0 */
    reg32_t MAC_BLE_1; /**< MAC BLE address 1 */
    reg32_t MAC_15_4_0; /**< MAC IEEE 802.15.4 address 0 */
    reg32_t MAC_15_4_1; /**< MAC IEEE 802.15.4 address 1 */
    reg32_t __reserved8[4]; /**< meh */
    reg32_t FLASH_OTP_DATA4; /**< Internal */
    reg32_t MISC_TRIM; /**< Miscelaneous trim parameters */
    reg32_t RCOSC_HF_TEMPCOMP; /**< Internal */
    reg32_t __reserved9; /**< meh */
    reg32_t ICEPICK_DEVICE_ID; /**< IcePick Device Identification */
    reg32_t FCFG1_REVISION; /**< Factory configuration revision */
    reg32_t MISC_OTP_DATA; /**< Miscelanous OTP data */
    reg32_t __reserved10[8]; /**< meh */
    reg32_t IOCONF; /**< I/O Configuration */
    reg32_t __reserved11; /**< meh */
    reg32_t CONFIG_IF_ADC; /**< Internal */
    reg32_t CONFIG_OSC_TOP; /**< Internal */
    reg32_t __reserved12[2]; /**< meh */
    reg32_t SOC_ADC_ABS_GAIN; /**< AUX_ADC gain in absolute reference mode */
    reg32_t SOC_ADC_REL_GAIN; /**< AUX_ADC gain in relative reference mode */
    reg32_t __reserved13; /**< meh */
    reg32_t SOC_ADC_OFFSET_INT; /**< AUX_ADC temperature offsets in absolute reference mode */
    reg32_t SOC_ADC_REF_TRIM_AND_OFFSET_EXT; /**< Internal */
    reg32_t AMPCOMP_TH1; /**< Internal */
    reg32_t AMPCOMP_TH2; /**< Internal */
    reg32_t AMPCOMP_CTRL1; /**< Internal */
    reg32_t ANABYPASS_VALUE2; /**< Internal */
    reg32_t __reserved14[2]; /**< meh */
    reg32_t VOLT_TRIM; /**< Internal */
    reg32_t OSC_CONF; /**< OSC configuration */
    reg32_t FREQ_OFFSET; /**< Internal */
    reg32_t __reserved15; /**< meh */
    reg32_t MISC_OTP_DATA_1; /**< Internal */
    reg32_t PWD_CURR_20C; /**< power down current control 20C */
    reg32_t PWD_CURR_35C; /**< power down current control 35C */
    reg32_t PWD_CURR_50C; /**< power down current control 50C */
    reg32_t PWD_CURR_65C; /**< power down current control 65C */
    reg32_t PWD_CURR_80C; /**< power down current control 80C */
    reg32_t PWD_CURR_95C; /**< power down current control 95C */
    reg32_t PWD_CURR_110C; /**< power down current control 110C */
    reg32_t PWD_CURR_125C; /**< power down current control 125C */
    reg32_t __reserved16[5]; /**< meh */
    reg32_t SHDW_DIE_ID_0; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_0.* */
    reg32_t SHDW_DIE_ID_1; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_1.* */
    reg32_t SHDW_DIE_ID_2; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_2.* */
    reg32_t SHDW_DIE_ID_3; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_3.* */
    reg32_t __reserved17[2]; /**< meh */
    reg32_t SHDW_OSC_BIAS_LDO_TRIM; /**< Internal */
    reg32_t SHDW_ANA_TRIM; /**< Internal */
    reg32_t __reserved18[3]; /**< meh */
    reg32_t DAC_BIAS_CNF; /**< Internal */
    reg32_t __reserved19[2]; /**< meh */
    reg32_t TFW_PROBE; /**< Internal */
    reg32_t TFW_FT; /**< Internal */
    reg32_t DAC_CAL0; /**< Internal */
    reg32_t DAC_CAL1; /**< Internal */
    reg32_t DAC_CAL2; /**< Internal */
    reg32_t DAC_CAL3; /**< Internal */
} fcfg_regs_t;

#define FCFG ((fcfg_regs_t *) (FCFG_BASE)) /**< FCFG register bank */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC13X2_FCFG_H */

/*@}*/
