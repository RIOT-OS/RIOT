/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-FileCopyrightText: 2015-2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
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
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"

/* This driver uses the FlexTimer module (FTM) for generating PWM signals, but
 * not all Kinetis CPUs have such a module. This preprocessor condition prevents
 * compilation errors when the CPU header lacks the register definitions for the
 * FTM */
#ifdef FTM0

#define PRESCALER_MAX       (7U)

static inline FTM_Type *ftm(pwm_t pwm)
{
    return pwm_config[pwm].ftm;
}

static void poweron(pwm_t pwm)
{
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
#endif
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
        /* and reset the PWM to 0% duty cycle */
        ftm(pwm)->CONTROLS[pwm_config[pwm].chan[i].ftm_chan].CnV = 0;
    }

    /* and now we start the actual waveform generation */
    ftm(pwm)->SC |= FTM_SC_CLKS(1);

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
    ftm(pwm)->CONTROLS[pwm_config[pwm].chan[channel].ftm_chan].CnV = value;
}

void pwm_poweron(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    poweron(pwm);
    ftm(pwm)->SC |= FTM_SC_CLKS(1);
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    int ftm_num = pwm_config[pwm].ftm_num;

    /* disable PWM generation */
    ftm(pwm)->SC &= ~(FTM_SC_CLKS_MASK);

    /* and power of the peripheral */
#ifdef SIM_SCGC6_FTM2_SHIFT
    BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_FTM0_SHIFT + ftm_num) = 0;
#else
    if (ftm_num < 2) {
        BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_FTM0_SHIFT + ftm_num) = 0;
    }
    else if (ftm_num == 2) {
        BITBAND_REG32(SIM->SCGC3, SIM_SCGC3_FTM2_SHIFT) = 0;
    }
#endif
}

#endif /* defined(FTM0) */
