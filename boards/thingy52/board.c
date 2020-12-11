/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_thingy52
 * @{
 *
 * @file
 * @brief       Board initialization for the Thingy:52
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#ifdef MODULE_PWM_DAC
#include "pwm_dac.h"
#endif

#include "periph/gpio.h"

void board_init(void)
{
    /* LP22HB and HTS221 can only be powered with VREG when VDD_PWR_CTRL_PIN
       is set. */
#if defined(MODULE_LPS22HB) || defined(MODULE_HTS221)
    gpio_init(VDD_PWR_CTRL_PIN, GPIO_OUT);
    gpio_set(VDD_PWR_CTRL_PIN);
#endif

    /* initialize the CPU */
    cpu_init();

#ifdef MODULE_PWM_DAC
    /*
     * There isn't really a band-pass installed, thus the frequency needs to be
     * high enough to not come out of the speaker, or at least if it does be
     * filtered out in the human ear low-pass. That's a strange hardware choice
     * for something where no real DAC is wired to the output, but that's how
     * it is.
     *
     * As a consequence, we're stuck with 6-bit dynamics.
     * */
    dac_t pwmdac = pwm_dac_init(PWM_DEV(0), PWM_LEFT, 250000, 1 << 6);

    /* Validate hardcoded value from board.h */
    assert(pwmdac == DAC_LINE(0));
#endif

    /* Initialize speaker pin and keep it off initially */
    gpio_init(SPK_PWR_CTRL_PIN, GPIO_OUT);
    gpio_clear(SPK_PWR_CTRL_PIN);
}

void board_speaker_on(void)
{
    gpio_set(SPK_PWR_CTRL_PIN);
}

void board_speaker_off(void)
{
    gpio_clear(SPK_PWR_CTRL_PIN);
}
