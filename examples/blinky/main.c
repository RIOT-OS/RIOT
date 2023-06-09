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

#include "wait.h"
#include "board.h"
#include "periph_conf.h"

int main(void)
{
    while (1) {
        /* it is fine if the time between blinking is inaccurate */
        wait_at_least_ms(500);
#ifdef LED0_TOGGLE
        LED0_TOGGLE;
#else
        puts("Blink! (No LED present or configured...)");
#endif
    }

    return 0;
}
