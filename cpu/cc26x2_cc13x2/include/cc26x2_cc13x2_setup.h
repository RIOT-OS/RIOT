/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 Device setup functions
 */

#ifndef CC26X2_CC13X2_SETUP_H
#define CC26X2_CC13X2_SETUP_H

#include <cc26xx_cc13xx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Setup API address
 */
#define ROM_API_SETUP ((uint32_t *) (ROM_API_TABLE[28]))

/**
 * @brief   Setup API ROM functions
 * @{
 */
#define rom_setup_after_cold_reset_wakeup_from_shutdown_cfg1 \
    ((void (*)(uint32_t mode_conf))ROM_API_SETUP[0])

#define rom_setup_after_cold_reset_wakeup_from_shutdown_cfg2 \
    ((void (*)(uint32_t rev, uint32_t mode_conf))ROM_API_SETUP[1])

#define rom_setup_after_cold_reset_wakeup_from_shutdown_cfg3 \
    ((void (*)(uint32_t mode_conf))ROM_API_SETUP[2])

#define rom_setup_get_trim_for_adc_sh_mode_en \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[3])

#define rom_setup_get_trim_for_adc_sh_vbuf_en \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[4])

#define rom_setup_get_trim_for_ampcomp_ctrl \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[5])

#define rom_setup_get_trim_for_ampcomp_th1 \
    ((uint32_t (*)(void))ROM_API_SETUP[6])

#define rom_setup_get_trim_for_ampcomp_th2 \
    ((uint32_t (*)(void))ROM_API_SETUP[7])

#define rom_setup_get_trim_for_anabypass_value1 \
    ((uint32_t (*)(uint32_t mode_conf))ROM_API_SETUP[8])

#define rom_setup_get_trim_for_dblr_loop_filter_reset_voltage \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[9])

#define rom_setup_get_trim_for_radc_ext_cfg \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[10])

#define rom_setup_get_trim_for_rc_osc_lf_ibias_trim \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[11])

#define rom_setup_get_trim_for_rc_osc_lf_rtune_ctune_trim \
    ((uint32_t (*)(void))ROM_API_SETUP[12])

#define rom_setup_get_trim_for_xosc_hf_ctl \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[13])

#define rom_setup_get_trim_for_xosc_hf_fast_start \
    ((uint32_t (*)(void))ROM_API_SETUP[14])

#define rom_setup_get_trim_for_xosc_hf_ibiastherm \
    ((uint32_t (*)(void))ROM_API_SETUP[15])

#define rom_setup_get_trim_for_xosc_lf_regulator_and_cmirrwr_ratio \
    ((uint32_t (*)(uint32_t rev))ROM_API_SETUP[16])

#define rom_setup_set_aon_rtc_sub_sec_inc \
    ((void (*)(uint32_t subsecinc))ROM_API_SETUP[17])

#define rom_setup_set_cache_mode_according_to_ccfg_setting \
    ((void (*)(void))ROM_API_SETUP[18])

#define rom_setup_step_vddr_trim_to \
    ((void (*)(uint32_t tocode))ROM_API_SETUP[19])
/** @} */

/**
 * @brief   Performs the necessary trim of the device which is not done in ROM
 *          boot code.
 *
 * The following is handled by this function:
 *
 * - Checks if the driverlib variant used by the application is supported by the
 *   device. Execution is halted in case of unsupported driverlib variant.
 * - Configures VIMS cache mode based on setting in CCFG.
 * - Configures functionalities like DCDC and XOSC dependent on startup modes
 *   like cold reset, wakeup from shutdown and wakeup from from powerdown.
 * - Configures VIMS power domain control.
 * - Configures optimal wait time for flash FSM in cases where flash pump wakes
 *   up from sleep.
 *
 * @note It does no damage to execute this function again. It only consumes
 * time.
 */
void setup_trim_device(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
#endif /* CC26X2_CC13X2_SETUP_H */
