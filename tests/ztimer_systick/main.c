/*
 * Copyright (C) 2021 Franz Freitag, Justus Krebs, Nick Weiler
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       SysTick Ztimer test application
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "mutex.h"
#include "ztimer.h"
#include "ztimer/periph_systick.h"
#include "periph_conf.h"
#include "board.h"

#define INTERVAL_SEC        2lu
#define INTERVAL_LONG_SEC   30lu

static void test_timer_set(unsigned period, unsigned iterations)
{
    while (iterations--) {
        ztimer_sleep(ZTIMER_SEC, period);
        printf("now: %lu\n", ztimer_now(ZTIMER_SEC));
    }
}

int main(void)
{
    printf("Test setting %lu s intervals\n", INTERVAL_SEC);
    test_timer_set(INTERVAL_SEC, 5);

    printf("Test setting %lu s intervals\n", INTERVAL_LONG_SEC);
    test_timer_set(INTERVAL_LONG_SEC, 5);
    puts("Test passed");
    return 0;
}
