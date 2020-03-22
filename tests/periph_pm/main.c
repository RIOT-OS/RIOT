/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Power management peripheral test.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph/pm.h"
#ifdef MODULE_PERIPH_GPIO
#include "board.h"
#include "periph/gpio.h"
#endif
#ifdef MODULE_PM_LAYERED
#ifdef MODULE_PERIPH_RTC
#include "periph/rtc.h"
#endif
#include "pm_layered.h"
#endif
#include "shell.h"

#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_RISING
#endif

#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
static void btn_cb(void *ctx)
{
    (void) ctx;
    puts("BTN0 pressed.");
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   Application entry point.
 */
int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* print test application information */
#ifdef MODULE_PM_LAYERED
    printf("This application allows you to test the CPU power management.\n"
           "The available power modes are 0 - %d. Lower-numbered power modes\n"
           "save more power, but may require an event/interrupt to wake up\n"
           "the CPU. Reset the CPU if needed.\n",
           PM_NUM_MODES - 1);
#else
    puts("This application allows you to test the CPU power management.\n"
         "Layered support is not unavailable for this CPU. Reset the CPU if\n"
         "needed.");
#endif

#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
    puts("using BTN0 as wake-up source");
    gpio_init_int(BTN0_PIN, BTN0_MODE, BTN0_INT_FLANK, btn_cb, NULL);
#endif

    /* run the shell and wait for the user to enter a mode */
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
