/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Blinky application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"
#include "periph/gpio.h"

static void delay(void)
{
    // if (IS_USED(MODULE_ZTIMER)) {
        // ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
    // }
    // else {
    //     /*
    //      * As fallback for freshly ported boards with no timer drivers written
    //      * yet, we just use the CPU to delay execution and assume that roughly
    //      * 20 CPU cycles are spend per loop iteration.
    //      *
    //      * Note that the volatile qualifier disables compiler optimizations for
    //      * all accesses to the counter variable. Without volatile, modern
    //      * compilers would detect that the loop is only wasting CPU cycles and
    //      * optimize it out - but here the wasting of CPU cycles is desired.
    //      */
        uint32_t loops = coreclk() / 20;
        for (volatile uint32_t i = 0; i < loops; i++) { }
    // }
}

int main(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    while (1) {
        delay();
#ifdef LED0_TOGGLE
        LED0_TOGGLE;
#else
        puts("Blink! (No LED present or configured...)");
#endif
    }

    return 0;
}
