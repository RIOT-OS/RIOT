/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_microbit_v2
 * @{
 *
 * @file
 * @brief       Board initialization code for the BBC micro:bit v2
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

#ifdef MODULE_PWM_DAC
#include "pwm_dac.h"
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#ifdef MODULE_PWM_DAC
    /*
     * There isn't really a low-pass installed, thus the frequency needs to be
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

    /* initialize the on board buttons */
    gpio_init(BTN0_PIN, BTN0_MODE);
    gpio_init(BTN1_PIN, BTN1_MODE);
}
