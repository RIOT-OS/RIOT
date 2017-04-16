/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3x8e
 * @{
 *
 * @file
 * @brief       CPU specific low-level PWM driver implementation for the SAM3X8E
 *
 * The SAM3 has only support for a single PWM device, so we accept only
 * PWM_DEV(0) for this driver.
 *
 * @author      Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no PWM device is defined */
#if defined(PWM_NUMOF) && defined(PWM_CHAN_NUMOF)

#define PREA_MAX            (10U)

static uint16_t pwm_period;
static uint8_t pwm_chan_mask;

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    uint32_t pwm_clk = freq * res;     /* Desired/real pwm_clock */
    uint32_t diva = 1;                 /* Candidate for 8bit divider */
    uint32_t prea = 0;                 /* Candidate for clock select */

    /* check for valid device and mode (left-aligned PWM only so far) */
    if ((dev != PWM_DEV(0)) || (mode != PWM_LEFT)) {
        return 0;
    }

    /* check if target frequency and resolution is applicable */
    if (pwm_clk > CLOCK_CORECLOCK) {
        return 0;
    }

    /* calculate pre-scalers for targeted frequency and resolution:
     *   clk = CORECLOCK / (2 ^ prea) / diva
     * width prea := [0, 10] and diva [1, 255]
     */
    while ((diva = (CLOCK_CORECLOCK / pwm_clk / (1 << prea))) > 255) {
        ++prea;
    }
    /* make sure PREA does not exceed its limit */
    if (prea > PREA_MAX) {
        return 0;
    }

    /* activate PWM block by enabling it's clock. */
    PMC->PMC_PCER1 = PMC_PCER1_PID36;
    /* disable all channels to allow CPRD updates. */
    PWM->PWM_DIS = 0xff;
    /* configure clock generator */
    PWM->PWM_CLK = PWM_CLK_PREA(prea) | PWM_CLK_DIVA(diva);
    /* remember the used resolution (for cropping inputs later) */
    pwm_period = res - 1;

    /* setup the configured channels */
    pwm_chan_mask = 0;
    for (unsigned i = 0; i < PWM_CHAN_NUMOF; i++) {
        /* configure the use pin */
        gpio_init_mux(pwm_chan[i].pin, GPIO_MUX_B);
        /* and setup the channel */
        pwm_chan_mask |= (1 << pwm_chan[i].hwchan);
        PWM->PWM_CH_NUM[pwm_chan[i].hwchan].PWM_CMR = PWM_CMR_CPRE_CLKA;
        PWM->PWM_CH_NUM[pwm_chan[i].hwchan].PWM_CPRD = pwm_period;
        PWM->PWM_CH_NUM[pwm_chan[i].hwchan].PWM_CDTY = 0;
    }

    /* enable all configured channels */
    PWM->PWM_ENA = pwm_chan_mask;

    /* and return the actual configured frequency */
    return (CLOCK_CORECLOCK / (1 << prea) / diva / res);
}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm == PWM_DEV(0));
    return (uint8_t)PWM_CHAN_NUMOF;
}

/*
 * Update duty-cycle in channel with value.
 * If value is larger than resolution set by pwm_init() it is cropped.
 */
void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert((pwm == PWM_DEV(0)) && (channel < PWM_CHAN_NUMOF));

    /* clip and set new value */
    value = (value > pwm_period) ? pwm_period : value;
    PWM->PWM_CH_NUM[pwm_chan[channel].hwchan].PWM_CDTYUPD = value;
}

void pwm_poweron(pwm_t pwm)
{
    assert(pwm == PWM_DEV(0));
    PMC->PMC_PCER1 = PMC_PCDR1_PID36;
    PWM->PWM_ENA = pwm_chan_mask;
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm == PWM_DEV(0));
    PWM->PWM_ENA = 0;
    PMC->PMC_PCDR1 = PMC_PCDR1_PID36;
}

#endif /* PWM_NUMOF && PWM_CHAN_NUMOF */
