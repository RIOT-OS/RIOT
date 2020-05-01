
/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 Functions to setup the device
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cpu.h"

/**
 * @brief   Optimal wait time in cases where flash pump wakes up from sleep.
 */
#define FPAC1_OPTIMAL_PSLEEPTDIS (0x139)

/**
 * @brief   Trims to be applied when coming from PIN_RESET.
 */
__attribute__ ((weak)) void trim_after_cold_reset(void)
{
    /* Currently no specific trim for Cold Reset */
}

/**
 * @brief   Trims to be applied when coming from POWER_DOWN (also called when
 *          coming from SHUTDOWN and PIN_RESET).
 */
__attribute__ ((weak)) void trim_after_cold_reset_wakeup_from_shutdown_wakeup_from_powerdown(void)
{
    /* Currently no specific trim for Powerdown */
}

/**
 * @brief   Trims to be applied when coming from SHUTDOWN (also called when
 *          coming from PIN_RESET).
 *
 * @param[in] fcfg_rev FCFG1 revision
 */
void trim_after_cold_reset_wakeup_from_shutdown(uint32_t fcfg_rev)
{
    uint32_t ccfg_mode_conf_reg;

    /* Check in CCFG for alternative DCDC setting */
    if ((CCFG->SIZE_AND_DIS_FLAGS & CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING) == 0) {
        /* ADI_3_REFSYS:DCDCCTL5[3]  (=DITHER_EN) = CCFG_MODE_CONF_1[19]   (=ALT_DCDC_DITHER_EN)
         * ADI_3_REFSYS:DCDCCTL5[2:0](=IPEAK    ) = CCFG_MODE_CONF_1[18:16](=ALT_DCDC_IPEAK    )
         *
         * Using a single 4-bit masked write since layout is equal for both
         * source and destination
         */
        ADI3_M4->DCDCCTL5.LOW = 0xF0 | (CCFG->MODE_CONF_1 >>
                                CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_s);
    }

    /* Force DCDC to use RCOSC before starting up XOSC.
     * Clock loss detector does not use XOSC until SCLK_HF actually switches
     * and thus DCDC is not protected from clock loss on XOSC in that time frame.
     * The force must be released when the switch to XOSC has happened. */
    DDI_0_OSC_M16->CTL0.HIGH = DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m |
                               (DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m >> 16);
    /* Dummy read to ensure that the write has propagated */
    DDI_0_OSC->CTL0;

    /* Read the MODE_CONF register in CCFG */
    ccfg_mode_conf_reg = CCFG->MODE_CONF;

    /* First part of trim done after cold reset and wakeup from shutdown:
     *
     * - Adjust the VDDR_TRIM_SLEEP value.
     * - Configure DCDC.
     */
    rom_setup_after_cold_reset_wakeup_from_shutdown_cfg1(ccfg_mode_conf_reg);

    /* Addition to the CC1352 boost mode for HWREV >= 2.0
     * The combination VDDR_EXT_LOAD=0 and VDDS_BOD_LEVEL=1 is defined to select
     * boost mode */
    if (((ccfg_mode_conf_reg & CCFG_MODE_CONF_VDDR_EXT_LOAD) == 0) &&
        ((ccfg_mode_conf_reg & CCFG_MODE_CONF_VDDS_BOD_LEVEL) != 0)) {
        ADI3->DCDCCTL3 = ADI_3_REFSYS_DCDCCTL3_VDDR_BOOST_COMP_BOOST;
    }

    /* Second part of trim done after cold reset and wakeup from shutdown:
     *
     * - Configure XOSC.
     */
    rom_setup_after_cold_reset_wakeup_from_shutdown_cfg2(fcfg_rev, ccfg_mode_conf_reg);

    {
        uint32_t trim_reg;
        uint32_t trim_value;

        /* Propagate the LPM_BIAS trim */
        trim_reg = FCFG->DAC_BIAS_CNF;
        trim_value = (trim_reg & FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_m) >>
                     FCFG1_DAC_BIAS_CNF_LPM_TRIM_IOUT_s;
        ADI_4_AUX->LPMBIAS = (trim_value << ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_s) &
                              ADI_4_AUX_LPMBIAS_LPM_TRIM_IOUT_m;

        /* Set LPM_BIAS_BACKUP_EN according to FCFG1 configuration */
        if (trim_reg & FCFG1_DAC_BIAS_CNF_LPM_BIAS_BACKUP_EN) {
            ADI3_SET->AUX_DEBUG = ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN;
        }
        else {
            ADI3_CLR->AUX_DEBUG = ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN;
        }

        /* Set LPM_BIAS_WIDTH_TRIM according to FCFG1 configuration */
        {
            uint32_t width_trim = (trim_reg & FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_m) >>
                                  FCFG1_DAC_BIAS_CNF_LPM_BIAS_WIDTH_TRIM_s;
            /* Set LPM_BIAS_WIDTH_TRIM = 3
             * Set mask (bits to be written) in [15:8]
             * Set value (in correct bit pos) in [7:0]
             */
            ADI_4_AUX_M8->COMP = (ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_m << 8) |
                                 (width_trim << ADI_4_AUX_COMP_LPM_BIAS_WIDTH_TRIM_s);
        }
    }

    /* Third part of trim done after cold reset and wakeup from shutdown:
     *
     * - Configure HPOSC.
     * - Setup the LF clock.
     */
    rom_setup_after_cold_reset_wakeup_from_shutdown_cfg3(ccfg_mode_conf_reg);

    /* Set AUX into power down active mode */
    aux_sysif_opmode_change(AUX_SYSIF_OPMODEREQ_REQ_PDA);

    /* Disable EFUSE clock */
    FLASH->CFG |= FLASH_CFG_DIS_EFUSECLK;
}

void setup_trim_device(void)
{
    /* Get factory configuration revision, treat undefined revision as 0 */
    uint32_t fcfg_rev = FCFG->FCFG1_REVISION;
    if (fcfg_rev == 0xFFFFFFFF) {
        fcfg_rev = 0;
    }

    /* Enable standby in flash bank */
    FLASH->CFG &= ~FLASH_CFG_DIS_STANDBY;

    if (!(AON_IOC->IOCLATCH & AON_IOC_IOCLATCH_EN)) {
        trim_after_cold_reset_wakeup_from_shutdown_wakeup_from_powerdown();
    }
    else if (!(AON_PMCTL->SLEEPCTL & AON_PMCTL_SLEEPCTL_IO_PAD_SLEEP_DIS)) {
        trim_after_cold_reset_wakeup_from_shutdown(fcfg_rev);
        trim_after_cold_reset_wakeup_from_shutdown_wakeup_from_powerdown();
    }
    else {
        trim_after_cold_reset();
        trim_after_cold_reset_wakeup_from_shutdown(fcfg_rev);
        trim_after_cold_reset_wakeup_from_shutdown_wakeup_from_powerdown();
    }

    /* Set VIMS power domain */
    PRCM->PDCTL1VIMS = 0;

    /* Configure optimal wait time for flash FSM in cases where flash pump
     * wakes up from sleep */
    FLASH->FPAC1 = (FLASH->FPAC1 & ~FLASH_FPAC1_PSLEEPTDIS_m) |
                   (FPAC1_OPTIMAL_PSLEEPTDIS << FLASH_FPAC1_PSLEEPTDIS_s);

    /* Set BOOT_DET bits in AON_PMCTL to 3 if already found to be 1.
     * Note: The BOOT_DET_x_CLR/SET bits must be manually cleared */
    uint32_t boot_det_m = AON_PMCTL_RESETCTL_BOOT_DET_1_m |
                          AON_PMCTL_RESETCTL_BOOT_DET_0_m;
    uint32_t boot_det = (AON_PMCTL->RESETCTL & boot_det_m) >>
                        AON_PMCTL_RESETCTL_BOOT_DET_0_s;
    if (boot_det == 1) {
        uint32_t aon_sys_resetctl = AON_PMCTL->RESETCTL &
                                    ~(AON_PMCTL_RESETCTL_BOOT_DET_1_CLR_m |
                                      AON_PMCTL_RESETCTL_BOOT_DET_0_CLR_m |
                                      AON_PMCTL_RESETCTL_BOOT_DET_1_SET_m |
                                      AON_PMCTL_RESETCTL_BOOT_DET_0_SET_m |
                                      AON_PMCTL_RESETCTL_MCU_WARM_RESET_m);
        AON_PMCTL->RESETCTL = aon_sys_resetctl |
                              AON_PMCTL_RESETCTL_BOOT_DET_1_SET_m;
        AON_PMCTL->RESETCTL = aon_sys_resetctl;
    }

    /* Make sure there are no ongoing VIMS mode change when leaving (There
     * should typically be no wait time here, but need to be sure) */
    while (VIMS->STAT & VIMS_STAT_MODE_CHANGING) {}
}
