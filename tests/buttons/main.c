/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for the on-board button macros
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "board.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define TEST_FLANK      GPIO_FALLING

#ifdef BTN0_PIN /* assuming that first button is always BTN0 */
static void cb(void *arg)
{
    printf("Pressed BTN%d\n", (int)arg);
}
#endif

int main(void)
{
    int cnt = 0;
    /* get the number of available buttons and init interrupt handler */
#ifdef BTN0_PIN
    if (gpio_init_int(BTN0_PIN, BTN0_MODE, TEST_FLANK, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN0!");
        return 1;
    }
    ++cnt;
#endif
#ifdef BTN1_PIN
    if (gpio_init_int(BTN1_PIN, BTN1_MODE, TEST_FLANK, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN1!");
        return 1;
    }
    ++cnt;
#endif
#ifdef BTN2_PIN
    if (gpio_init_int(BTN2_PIN, BTN2_MODE, TEST_FLANK, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN2!");
        return 1;
    }
    ++cnt;
#endif
#ifdef BTN3_PIN
    if (gpio_init_int(BTN3_PIN, BTN3_MODE, TEST_FLANK, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN3!");
        return 1;
    }
    ++cnt;
#endif

    puts("On-board button test\n");
    /* cppcheck-suppress knownConditionTrueFalse
     * (reason: board-dependent ifdefs) */
    if (cnt == 0) {
        puts("[FAILED] no buttons available!");
        return 2;
    }
    printf(" -- Available buttons: %i\n\n", cnt);
    puts(" -- Try pressing buttons to test.\n");
    puts("[SUCCESS]");
    return 0;
}
