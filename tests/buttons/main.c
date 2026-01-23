/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

#ifndef BTN1_INT_FLANK
#define BTN1_INT_FLANK  GPIO_FALLING
#endif

#ifndef BTN2_INT_FLANK
#define BTN2_INT_FLANK  GPIO_FALLING
#endif

#ifndef BTN3_INT_FLANK
#define BTN3_INT_FLANK  GPIO_FALLING
#endif

#if defined (BTN0_PIN) || defined (BTN1_PIN) || defined (BTN2_PIN) || defined (BTN3_PIN)
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
    if (gpio_init_int(BTN0_PIN, BTN0_MODE, BTN0_INT_FLANK, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN0!");
        return 1;
    }
    ++cnt;
#endif
#ifdef BTN1_PIN
    if (gpio_init_int(BTN1_PIN, BTN1_MODE, BTN1_INT_FLANK, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN1!");
        return 1;
    }
    ++cnt;
#endif
#ifdef BTN2_PIN
    if (gpio_init_int(BTN2_PIN, BTN2_MODE, BTN2_INT_FLANK, cb, (void *)cnt) < 0) {
        puts("[FAILED] init BTN2!");
        return 1;
    }
    ++cnt;
#endif
#ifdef BTN3_PIN
    if (gpio_init_int(BTN3_PIN, BTN3_MODE, BTN3_INT_FLANK, cb, (void *)cnt) < 0) {
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
