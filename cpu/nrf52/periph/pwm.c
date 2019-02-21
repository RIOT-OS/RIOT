/*
 * Copyright (C) 2016-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral PWM interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include "periph/pwm.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Define masks to get the MODE register value and the polarity from
 *          the given pwm_mode_t value
 * @{
 */
#define POL_MASK        (0x8000)
#define MAX_RES         (0x7fff)
#define PIN_CNF_SET     (GPIO_PIN_CNF_DIR_Output | GPIO_PIN_CNF_INPUT_Msk)
#define MAX_PRESCALER   (7U)

/** @} */

/**
 * @brief   When calculating the actual frequency, we allow for a deviation of
 *          ~15.5% (val +- (val / 8))
 */
#define F_DEV           (8)

/**
 * @brief   Allocate some memory for the PWM sequences
 */
static uint16_t pwm_seq[PWM_NUMOF][PWM_CHANNELS];

static inline NRF_PWM_Type *dev(pwm_t pwm)
{
    return pwm_config[pwm].dev;
}

/**
 * @note Center mode is not supported. Use left or right aligned PWM modes.
 */
uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    /* check if given device is valid */
    if ((pwm >= PWM_NUMOF) || (res > MAX_RES)) {
        return 0;
    }

    /* check if pwm mode is supported */
    if ((mode != PWM_RIGHT) && (mode != PWM_LEFT)) {
        return 0;
    }

    /* make sure the device is stopped */
    dev(pwm)->TASKS_STOP = 1;
    dev(pwm)->ENABLE = 0;

    /* calculate the needed frequency, for center modes we need double */
    uint32_t real_clk;
    uint32_t clk = (freq * res);
    /* match to best fitting prescaler */
    for (uint8_t i = 0; i < (MAX_PRESCALER + 1); i++) {
        real_clk = (PERIPH_CLOCK >> i);
        if (((real_clk - (real_clk / F_DEV)) < clk) &&
            ((real_clk + (real_clk / F_DEV)) > clk)) {
            dev(pwm)->PRESCALER = i;
            break;
        }
        if (i == MAX_PRESCALER) {
            return 0;
        }
    }
    real_clk /= res;

    /* pin configuration */
    for (unsigned i = 0; i < PWM_CHANNELS; i++) {
        if (pwm_config[pwm].pin[i] != GPIO_UNDEF) {
            NRF_P0->PIN_CNF[pwm_config[pwm].pin[i]] = PIN_CNF_SET;
        }
        /* either left aligned pol or inverted duty cycle */
        pwm_seq[pwm][i] = (POL_MASK & mode) ? POL_MASK : res;
        dev(pwm)->PSEL.OUT[i] = pwm_config[pwm].pin[i];
        DEBUG("set PIN[%i] to %i with 0x%x\n",
              (int)i, (int)pwm_config[pwm].pin[i], pwm_seq[pwm][i]);
    }

    /* enable the device */
    dev(pwm)->ENABLE = 1;

    /* finally get the actual selected frequency */
    DEBUG("set real f to %i\n", (int)real_clk);
    dev(pwm)->COUNTERTOP = (res - 1);

    /* select PWM mode */
    dev(pwm)->MODE = PWM_MODE_UPDOWN_Up;
    dev(pwm)->LOOP = PWM_LOOP_CNT_Disabled;
    dev(pwm)->DECODER = PWM_DECODER_LOAD_Individual;

    DEBUG("MODE: 0x%08x\n", (int)dev(pwm)->MODE);
    DEBUG("LOOP: 0x%08x\n", (int)dev(pwm)->LOOP);
    DEBUG("DECODER: 0x%08x\n", (int)dev(pwm)->DECODER);

    /* setup the sequence */
    dev(pwm)->SEQ[0].PTR = (uint32_t)pwm_seq[pwm];
    dev(pwm)->SEQ[0].CNT = pwm_channels(pwm);
    dev(pwm)->SEQ[0].REFRESH = 0;
    dev(pwm)->SEQ[0].ENDDELAY = 0;

    DEBUG("ptr: 0x%08x\n", (int)dev(pwm)->SEQ[0].PTR);
    DEBUG("cnt: 0x%08x\n", (int)dev(pwm)->SEQ[0].CNT);
    DEBUG("refresh: 0x%08x\n", (int)dev(pwm)->SEQ[0].REFRESH);
    DEBUG("enddelay: 0x%08x\n", (int)dev(pwm)->SEQ[0].ENDDELAY);

    /* start sequence */
    dev(pwm)->TASKS_SEQSTART[0] = 1;

    DEBUG("PWM started\n");

    return real_clk;
}

uint8_t pwm_channels(pwm_t pwm)
{
    uint8_t channels = 0;

    while ((channels < PWM_CHANNELS) &&
           (pwm_config[pwm].pin[channels] != GPIO_UNDEF)) {
        ++channels;
    }

    return channels;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    if ((channel >= PWM_CHANNELS) || (value > MAX_RES)) {
        return;
    }

    /* left aligned */
    if (pwm_seq[pwm][channel] & POL_MASK) {
        pwm_seq[pwm][channel] = value | POL_MASK;
    }
    /* right aligned */
    else {
        pwm_seq[pwm][channel] = dev(pwm)->COUNTERTOP - value;
    }

    dev(pwm)->TASKS_SEQSTART[0] = 1;
}

void pwm_stop(pwm_t pwm)
{
    DEBUG("STOPPING PWM %i\n", (int)pwm);
    dev(pwm)->TASKS_STOP = 1;
    dev(pwm)->ENABLE = 0;
}

void pwm_start(pwm_t pwm)
{
    DEBUG("STARTING PWM %i\n", (int)pwm);
    dev(pwm)->ENABLE = 1;
    dev(pwm)->TASKS_SEQSTART[0] = 1;
}

void pwm_poweron(pwm_t pwm)
{
    pwm_start(pwm);
}

void pwm_poweroff(pwm_t pwm)
{
    pwm_stop(pwm);
}
