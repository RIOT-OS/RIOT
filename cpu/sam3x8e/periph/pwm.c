/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_sam3x8e
 * @{
 *
 * @file
 * @brief       CPU specific low-level PWM driver implementation for the SAM3X8E
 *
 * @author      Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 *
 * @}
 */
#include <stdint.h>
#include "board.h"
#include "periph/pwm.h"

/*
 * guard file in case no PWM device is defined,
 */
#if PWM_NUMOF && PWM_0_EN

#define ERR_INIT_MODE           (-1)
#define ERR_INIT_BWTH           (-2)
#define ERR_SET_CHAN            (-1)
#define MCK_DIV_LB_MAX          (10U)

int pwm_init(pwm_t dev, pwm_mode_t mode,
             unsigned int frequency,
             unsigned int resolution)
{

    int32_t retval = ERR_INIT_MODE;    /* Worst/First case */
    uint32_t pwm_clk = 0;              /* Desired/real pwm_clock */
    uint32_t diva = 1;                 /* Candidate for 8bit divider */
    uint32_t prea = 0;                 /* Candidate for clock select */

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            break;
#endif

        default:
            return ERR_INIT_MODE;
    }

    /*
     * Mode check.
     * Only PW_LEFT -which is hw default- supported for now.
     */
    switch (mode) {
        case PWM_LEFT:
            break;

        case PWM_RIGHT:
        case PWM_CENTER:
        default:
            return ERR_INIT_MODE;
    }

    /* Should check if "|log_2 frequency|+|log_2 resolution| <= 32" */
    pwm_clk = frequency * resolution;

    /*
     * The pwm provides 11 prescaled clocks with (MCK/2^prea | prea=[0,10])
     * and a divider (diva) with a denominator range [1,255] in line.
     */
    if (F_CPU < pwm_clk) {  /* Have to cut down resulting frequency. */
        frequency = F_CPU / resolution;
    }
    else {   /* Estimate prescaler and divider. */
        diva = F_CPU / pwm_clk;

        while ((prea < MCK_DIV_LB_MAX) && (~0xff & diva)) {
            prea = prea + 1;
            diva = diva >> 1;
        }

        frequency = F_CPU / ((resolution * diva) << prea);
    }

    retval = frequency;

    /* Activate PWM block by enabling it's clock. */
    PMC->PMC_PCER1 = PMC_PCER1_PID36;

    /* Unlock User Interface */
    PWM_0_DEV->PWM_WPCR  = PWM_ENA_CHID0;

    /* Disable all channels to allow CPRD updates. */
    PWM_0_DEV->PWM_DIS = 0xff;

    /* Step 2. Configure clock generator */
    PWM_0_DEV->PWM_CLK = PWM_CLK_PREA(prea)  | PWM_CLK_DIVA(diva) |
                         (~(PWM_CLK_PREA_Msk | PWM_CLK_DIVA_Msk)  &
                          PWM_0_DEV->PWM_CLK);

    /* +++++++++++ Configure and init channels +++++++++++++++ */

    /* Set clock source, resolution, duty-cycle and enable */
#if PWM_0_CHANNELS > 0
    PWM_0_DEV_CH0->PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM_0_DEV_CH0->PWM_CPRD = resolution - 1;
    PWM_0_DEV_CH0->PWM_CDTY = 0;
    PWM_0_DEV->PWM_ENA = PWM_0_ENA_CH0;
#endif
#if PWM_0_CHANNELS > 1
    PWM_0_DEV_CH1->PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM_0_DEV_CH1->PWM_CPRD = resolution - 1;
    PWM_0_DEV_CH1->PWM_CDTY = 0;
    PWM_0_DEV->PWM_ENA = PWM_0_ENA_CH1;
#endif
#if PWM_0_CHANNELS > 2
    PWM_0_DEV_CH2->PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM_0_DEV_CH2->PWM_CPRD = resolution - 1;
    PWM_0_DEV_CH2->PWM_CDTY = 0;
    PWM_0_DEV->PWM_ENA = PWM_0_ENA_CH2;
#endif
#if PWM_0_CHANNELS > 3
    PWM_0_DEV_CH3->PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM_0_DEV_CH3->PWM_CPRD = resolution - 1;
    PWM_0_DEV_CH3->PWM_CDTY = 0;
    PWM_0_DEV->PWM_ENA = PWM_0_ENA_CH3;
#endif

    /* +++++++++++ Configure and init channels ready++++++++++ */


    /*
     * Disable GPIO and set peripheral A/B ("0/1") for pwm channel pins.
     */
#if PWM_0_CHANNELS > 0
    PWM_0_PORT_CH0->PIO_PDR  |= PWM_0_PIN_CH0;
    PWM_0_PORT_CH0->PIO_ABSR |= PWM_0_PIN_CH0;
#endif
#if PWM_0_CHANNELS > 1
    PWM_0_PORT_CH1->PIO_PDR  |= PWM_0_PIN_CH1;
    PWM_0_PORT_CH1->PIO_ABSR |= PWM_0_PIN_CH1;
#endif
#if PWM_0_CHANNELS > 2
    PWM_0_PORT_CH2->PIO_PDR  |= PWM_0_PIN_CH2;
    PWM_0_PORT_CH2->PIO_ABSR |= PWM_0_PIN_CH2;
#endif
#if PWM_0_CHANNELS > 3
    PWM_0_PORT_CH3->PIO_PDR  |= PWM_0_PIN_CH3;
    PWM_0_PORT_CH3->PIO_ABSR |= PWM_0_PIN_CH3;
#endif

    return retval;
}

/*
 * Update duty-cycle in channel with value.
 * If value is larger than resolution set by pwm_init() it is cropped.
 */
int pwm_set(pwm_t dev, int channel, unsigned int value)
{

    int retval = ERR_SET_CHAN;      /* Worst case */
    uint32_t period = 0;            /* Store pwm period */
    PwmCh_num *chan = (void *)0;    /* Addressed channel. */

    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            break;
#endif

        default:
            return ERR_SET_CHAN;
    }


    switch (channel) {
#if PWM_0_CHANNELS > 0

        case 0:
            chan = PWM_0_DEV_CH0;
            break;
#endif
#if PWM_0_CHANNELS > 1

        case 1:
            chan = PWM_0_DEV_CH1;
            break;
#endif
#if PWM_0_CHANNELS > 2

        case 2:
            chan = PWM_0_DEV_CH2;
            break;
#endif
#if PWM_0_CHANNELS > 3

        case 3:
            chan = PWM_0_DEV_CH3;
            break;
#endif

        default:
            retval = ERR_SET_CHAN;
    }

    if (chan) {
        period = chan->PWM_CPRD & PWM_CPRD_CPRD_Msk;


        if (value < period) {
            chan->PWM_CDTYUPD = value;
        }
        else {   /* Value Out of range. Clip silent as required by interface. */
            chan->PWM_CDTYUPD = period;
        }

        retval = 0;
    }

    return retval;
}

/*
 * Continue operation.
 */
void pwm_start(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PMC->PMC_PCER1 |= PMC_PCER1_PID36;
            break;
#endif
    }
}

/*
 * Stop operation and set output to 0.
 */
void pwm_stop(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PMC->PMC_PCDR1 |= PMC_PCDR1_PID36;
            break;
#endif
    }
}

/*
 * The device is reactivated by by clocking the device block.
 * Operation continues where it has been stopped by poweroff.
 */
void pwm_poweron(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PMC->PMC_PCER1 |= PMC_PCER1_PID36;
            break;
#endif
    }
}

/*
 * The device is set to power saving mode by disabling the clock.
 */
void pwm_poweroff(pwm_t dev)
{
    switch (dev) {
#if PWM_0_EN

        case PWM_0:
            PMC->PMC_PCDR1 |= PMC_PCDR1_PID36;
            break;
#endif
    }
}

#endif /* PWM_NUMOF */
