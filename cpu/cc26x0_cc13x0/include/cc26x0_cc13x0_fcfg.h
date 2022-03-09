/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_cc26x0_cc13x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0/CC13x0 FCFG register definitions
 */

#ifndef CC26X0_CC13X0_FCFG_H
#define CC26X0_CC13X0_FCFG_H

#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief Base address of FCFG memory
 */
#define FCFG_BASE   (0x50001000)
/** @} */

/**
 * @brief FCFG registers
 */
typedef struct {
    uint8_t __reserved1[0xA0]; /**< Reserved */
    /* TODO does it pad here? */
    reg32_t MISC_CONF_1; /**< misc config */
    reg32_t __reserved2[8]; /**< Reserved */
    reg32_t CONFIG_RF_FRONTEND_DIV5; /**< config of RF frontend in divide-by-5 mode */
    reg32_t CONFIG_RF_FRONTEND_DIV6; /**< config of RF frontend in divide-by-6 mode */
    reg32_t CONFIG_RF_FRONTEND_DIV10; /**< config of RF frontend in divide-by-10 mode */
    reg32_t CONFIG_RF_FRONTEND_DIV12; /**< config of RF frontend in divide-by-12 mode */
    reg32_t CONFIG_RF_FRONTEND_DIV15; /**< config of RF frontend in divide-by-15 mode */
    reg32_t CONFIG_RF_FRONTEND_DIV30; /**< config of RF frontend in divide-by-30 mode */
    reg32_t CONFIG_SYNTH_DIV5; /**< config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV6; /**< config of synthesizer in divide-by-5-mode */
    reg32_t CONFIG_SYNTH_DIV10; /**< config of synthesizer in divide-by-10-mode */
    reg32_t CONFIG_SYNTH_DIV12; /**< config of synthesizer in divide-by-12-mode */
    reg32_t CONFIG_SYNTH_DIV15; /**< config of synthesizer in divide-by-15-mode */
    reg32_t CONFIG_SYNTH_DIV30; /**< config of synthesizer in divide-by-30-mode */
    reg32_t CONFIG_MISC_ADC_DIV5; /**< config of IFADC in divide-by-5-mode */
    reg32_t CONFIG_MISC_ADC_DIV6; /**< config of IFADC in divide-by-6-mode */
    reg32_t CONFIG_MISC_ADC_DIV10; /**< config of IFADC in divide-by-10-mode */
    reg32_t CONFIG_MISC_ADC_DIV12; /**< config of IFADC in divide-by-12-mode */
    reg32_t CONFIG_MISC_ADC_DIV15; /**< config of IFADC in divide-by-15-mode */
    reg32_t CONFIG_MISC_ADC_DIV30; /**< config of IFADC in divide-by-30-mode */
    reg32_t __reserved3[3]; /**< Reserved */
    reg32_t SHDW_DIE_ID_0; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_0.* */
    reg32_t SHDW_DIE_ID_1; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_1.* */
    reg32_t SHDW_DIE_ID_2; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_2.* */
    reg32_t SHDW_DIE_ID_3; /**< shadow of JTAG_TAP::EFUSE::DIE_ID_3.* */
    reg32_t __reserved4[4]; /**< Reserved */
    reg32_t SHDW_OSC_BIAS_LDO_TRIM; /**< shadow of JTAG_TAP::EFUSE::BIAS_LDO_TIM.* */
    reg32_t SHDW_ANA_TRIM; /**< shadow of JTAG_TAP::EFUSE::ANA_TIM.* */
    reg32_t __reserved5[9]; /**< Reserved */
    reg32_t FLASH_NUMBER; /**< number of manufactoring lot that produced this unit */
    reg32_t FLASH_COORDINATE; /**< X and Y coordinates of this unit on the wafer */
    reg32_t FLASH_E_P; /**< flash erase and program setup time */
    reg32_t FLASH_C_E_P_R; /**< flash compaction, execute, program, and read */
    reg32_t FLASH_P_R_PV; /**< flash program, read, and program verify */
    reg32_t FLASH_EH_SEQ; /**< flash erase hold and sequence */
    reg32_t FLASH_VHV_E; /**< flash VHV erase */
    reg32_t FLASH_PP; /**< flash program pulse */
    reg32_t FLASH_PROG_EP; /**< flash program and erase pulse */
    reg32_t FLASH_ERA_PW; /**< flash erase pulse width */
    reg32_t FLASH_VHV; /**< flash VHV */
    reg32_t FLASH_VHV_PV; /**< flash VHV program verify */
    reg32_t FLASH_V; /**< flash voltages */
    reg32_t __reserved6[0x3E]; /**< Reserved */
    reg32_t USER_ID; /**< user identification */
    reg32_t __reserved7[6]; /**< Reserved */
    reg32_t FLASH_OTP_DATA3; /**< flash OTP data 3 */
    reg32_t ANA2_TRIM; /**< misc analog trim */
    reg32_t LDO_TRIM; /**< LDO trim */
    reg32_t __reserved8[0xB]; /**< Reserved */
    reg32_t MAC_BLE_0; /**< MAC BLE address 0 */
    reg32_t MAC_BLE_1; /**< MAC BLE address 1 */
    reg32_t MAC_15_4_0; /**< MAC IEEE 820.15.4 address 0 */
    reg32_t MAC_15_4_1; /**< MAC IEEE 820.15.4 address 1 */
    reg32_t __reserved9[4]; /**< Reserved */
    reg32_t FLASH_OTP_DATA4; /**< flash OTP data 4 */
    reg32_t MISC_TRIM; /**< misc trim parameters */
    reg32_t RCOSC_HF_TEMPCOMP; /**< RFOSC HF temperature compensation */
    reg32_t __reserved10; /**< Reserved */
    reg32_t ICEPICK_DEVICE_ID; /**< IcePick device identification */
    reg32_t FCFG1_REVISION; /**< FCFG1 revision */
    reg32_t MISC_OTP_DATA; /**< misc OTP data */
    reg32_t __reserved11[8]; /**< Reserved */
    reg32_t IOCONF; /**< IO config */
    reg32_t __reserved12; /**< Reserved */
    reg32_t CONFIG_IF_ADC; /**< config of IF_ADC */
    reg32_t CONFIG_OSC_TOP; /**< config of OSC */
    reg32_t CONFIG_RF_FRONTEND; /**< config of RF frontend in dividy-by-2-mode */
    reg32_t CONFIG_SYNTH; /**< config of synthesizer in dividy-by-2-mode */
    reg32_t SOC_ADC_ABS_GAIN; /**< AUX_ADC gain in absolute reference mode */
    reg32_t SOC_ADC_REL_GAIN; /**< AUX_ADC gain in relative reference mode */
    reg32_t __reserved13; /**< Reserved */
    reg32_t SOC_ADC_OFFSET_INT; /**< AUX_ADC temperature offsets in absolute reference mode */
    reg32_t SOC_ADC_REF_TRIM_AND_OFFSET_EXT; /**< AUX_ADC reference trim and offset of external reference mode */
    reg32_t AMPCOMP_TH1; /**< amplitude compensation threshold 1 */
    reg32_t AMPCOMP_TH2; /**< amplitude compensation threshold 2 */
    reg32_t AMPCOMP_CTRL1; /**< amplitude compensation control */
    reg32_t ANABYPASS_VALUE2; /**< analog bypass value for OSC */
    reg32_t CONFIG_MISC_ADC; /**< config of IFADC in divide-by-2-mode */
    reg32_t __reserved14; /**< Reserved */
    reg32_t VOLT_TRIM; /**< voltage trim */
    reg32_t OSC_CONF; /**< OSC configuration */
    reg32_t __reserved15; /**< Reserved */
    reg32_t CAP_TRIM; /**< capacitor trim (it says 'capasitor' in the manual - if you know what that is ;-) */
    reg32_t MISC_OTP_DATA_1; /**< misc OSC control */
    reg32_t PWD_CURR_20C; /**< power down current control 20C */
    reg32_t PWD_CURR_35C; /**< power down current control 35C */
    reg32_t PWD_CURR_50C; /**< power down current control 50C */
    reg32_t PWD_CURR_65C; /**< power down current control 65C */
    reg32_t PWD_CURR_80C; /**< power down current control 80C */
    reg32_t PWD_CURR_95C; /**< power down current control 95C */
    reg32_t PWD_CURR_110C; /**< power down current control 110C */
    reg32_t PWD_CURR_125C; /**< power down current control 125C */
} fcfg_regs_t;

/**
 * @brief FCFG register bank
 */
#define FCFG    ((fcfg_regs_t *) (FCFG_BASE))

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X0_CC13X0_FCFG_H */
/** @} */
