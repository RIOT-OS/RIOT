/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015-2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_pwm
 *
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"

#ifdef MODULE_PWM_LAYERED
#include "pm_layered.h"
#else
#define pm_block(...)
#define pm_unblock(...)
#endif

/* This mode will be blocked while PWM is active */
#ifndef PWM_PM_BLOCKER
#define PWM_PM_BLOCKER           2
#endif

/**
 * @brief   Keeps track of whether PWM is active for power management
 */
static bool _is_powered_on[PWM_NUMOF];

#define PRESCALER_MAX       (7U)

#ifdef FTM0
static inline FTM_Type *ftm(pwm_t pwm)
{
    return pwm_config[pwm].ftm;
}
#else /* FTM0 */
static inline TPM_Type *tpm(pwm_t pwm)
{
    return pwm_config[pwm].tpm;
}
#endif /* FTM0 */

static void poweron(pwm_t pwm)
{
    if (_is_powered_on[pwm]) {
        return;
    }

    pm_block(PWM_PM_BLOCKER);
    _is_powered_on[pwm] = true;

#ifdef FTM0
    int ftm = pwm_config[pwm].ftm_num;

#ifdef SIM_SCGC6_FTM2_SHIFT
    BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_FTM0_SHIFT + ftm) = 1;
#else
    if (ftm < 2) {
        BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_FTM0_SHIFT + ftm) = 1;
    }
    else if (ftm == 2) {
        BITBAND_REG32(SIM->SCGC3, SIM_SCGC3_FTM2_SHIFT) = 1;
    }
#endif /* SIM_SCGC6_FTM2_SHIFT */

#else /* FTM0 */
    bit_set32(&SIM->SCGC6, SIM_SCGC6_TPM0_SHIFT + pwm_config[pwm].tpm_num);
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

#endif /* FTM0 */
}

uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    uint8_t pre = 0;

    if (pwm >= PWM_NUMOF || ((res * freq) > CLOCK_BUSCLOCK)) {
        return 0;
    }

    /* figure out the clock settings
     * the resulting frequency is calculated by
     *      ticks := BUS_CLK / 2 ^ pre
     *  where `ticks` is `freq * res`
     *  and `pre` must be between [0, 7].
     *
     *  The resulting prescaler yields a timer frequency, which is the closest
     *  possible frequency requested. */
    while ((CLOCK_BUSCLOCK >> pre) > (res * freq)) {
        ++pre;
    }
    /* make sure the calculated prescaler is valid, else return */
    if (pre > PRESCALER_MAX) {
        return 0;
    }

    /* configure the used timer */
    poweron(pwm);

#ifdef FTM0
    /* disable write protect for changing settings */
    ftm(pwm)->MODE = FTM_MODE_WPDIS_MASK;
    /* clear any existing configuration */
    ftm(pwm)->COMBINE = 0;
    ftm(pwm)->CNTIN = 0;
    ftm(pwm)->SWOCTRL = 0;
    /* apply prescaler and set resolution */
    ftm(pwm)->SC = FTM_SC_PS(pre);
    ftm(pwm)->MOD = (res - 1);

    /* set CPWMS bit in the SC register in case of center aligned mode */
    if (mode == PWM_CENTER) {
        BITBAND_REG32(ftm(pwm)->SC, FTM_SC_CPWMS_SHIFT) = 1;
    }

    /* setup the configured channels */
    for (int i = 0; i < (int)pwm_config[pwm].chan_numof; i++) {
        /* configure the used pin */
        gpio_init_port(pwm_config[pwm].chan[i].pin,
                       PORT_PCR_MUX(pwm_config[pwm].chan[i].af));
        /* set the given mode */
        ftm(pwm)->CONTROLS[pwm_config[pwm].chan[i].ftm_chan].CnSC = mode;
    }

    /* and now we start the actual waveform generation */
    ftm(pwm)->SC |= FTM_SC_CLKS(1);

#else /* FTM0 */

    /* apply prescaler and set resolution */
    tpm(pwm)->SC = TPM_SC_PS(pre);
    tpm(pwm)->CNT = 0;
    tpm(pwm)->MOD = (res - 1);

    /* set CPWMS bit in the SC register in case of center aligned mode */
    if (mode == PWM_CENTER) {
        tpm(pwm)->SC |= TPM_SC_CPWMS(1);
    }

    /* setup the configured channels */
    for (int i = 0; i < (int)pwm_config[pwm].chan_numof; i++) {
        /* disable channel before configuring */
        tpm(pwm)->CONTROLS[pwm_config[pwm].chan[i].ftm_chan].CnSC = 0;
        while ((tpm(pwm)->CONTROLS[pwm_config[pwm].chan[i].ftm_chan].CnSC &
                ~(TPM_CnSC_CHF_MASK)) != 0) {}
        /* configure the used pin */
        gpio_init_port(pwm_config[pwm].chan[i].pin,
                       PORT_PCR_MUX(pwm_config[pwm].chan[i].af));
        /* set the given mode */
        tpm(pwm)->CONTROLS[pwm_config[pwm].chan[i].ftm_chan].CnSC = mode;
    }

    /* and now we start the actual waveform generation */
    tpm(pwm)->SC |= TPM_SC_CMOD(1);
#endif /* FTM0 */

    /* finally we need to return the actual applied PWM frequency */
    return (CLOCK_BUSCLOCK >> pre) / res;
}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    return pwm_config[pwm].chan_numof;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert((pwm < PWM_NUMOF) && (channel < pwm_config[pwm].chan_numof));
#ifdef FTM0
    ftm(pwm)->CONTROLS[pwm_config[pwm].chan[channel].ftm_chan].CnV = value;
#else
    tpm(pwm)->CONTROLS[pwm_config[pwm].chan[channel].ftm_chan].CnV = value;
#endif
}

void pwm_poweron(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    poweron(pwm);

    /* enable PWM generation */
#ifdef FTM0
    ftm(pwm)->SC |= FTM_SC_CLKS(1);
#else
    tpm(pwm)->SC |= TPM_SC_CMOD(1);
#endif /* FTM0 */
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);

    if (!_is_powered_on[pwm]) {
        return;
    }

    pm_unblock(PWM_PM_BLOCKER);
    _is_powered_on[pwm] = false;

#ifdef FTM0

    int ftm_num = pwm_config[pwm].ftm_num;

    /* disable PWM generation */
    ftm(pwm)->SC &= ~(FTM_SC_CLKS_MASK);

    /* and power off the peripheral */
#ifdef SIM_SCGC6_FTM2_SHIFT
    BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_FTM0_SHIFT + ftm_num) = 0;
#else
    if (ftm_num < 2) {
        BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_FTM0_SHIFT + ftm_num) = 0;
    }
    else if (ftm_num == 2) {
        BITBAND_REG32(SIM->SCGC3, SIM_SCGC3_FTM2_SHIFT) = 0;
    }
#endif /* SIM_SCGC6_FTM2_SHIFT */

#else /* FTM0 */

    /* disable PWM generation */
    tpm(pwm)->SC &= ~(TPM_SC_CMOD_MASK);

    /* and power off the peripheral */
    bit_clear32(&SIM->SCGC6, SIM_SCGC6_TPM0_SHIFT + pwm_config[pwm].tpm_num);
    SIM->SOPT2 &= ~(SIM_SOPT2_TPMSRC_MASK);

#endif /* FTM0 */
}
