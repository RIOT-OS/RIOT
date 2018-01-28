/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stk3800
 * @{
 *
 * @file
 * @brief       Board specific implementations STK3800 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "periph/gpio.h"

#include "em_dbg.h"
#include "em_gpio.h"

# if AEM_ENABLED
static void aem_init(void)
{
    if (DBG_Connected()) {
        /* enable GPIO clock for configuring SWO pins */
        CMU_ClockEnable(cmuClock_HFPER, true);
        CMU_ClockEnable(cmuClock_GPIO, true);

        /* enable debug peripheral via SWO */
        DBG_SWOEnable(GPIO_ROUTE_SWLOCATION_LOC0);

        /* enable trace in core debug */
        CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

        /* enable PC and IRQ sampling output */
        DWT->CTRL = 0x400113FF;

        /* set TPIU prescaler to 16 */
        TPI->ACPR = 15;

        /* set protocol to NRZ */
        TPI->SPPR = 2;

        /* disable continuous formatting */
        TPI->FFCR = 0x100;

        /* unlock ITM and output data */
        ITM->LAR = 0xC5ACCE55;
        ITM->TCR = 0x10009;
    }
}
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* enable core debug output AEM */
#if AEM_ENABLED
    aem_init();
#endif

    /* enable the board controller, to enable virtual com port */
#if BC_ENABLED
    gpio_init(BC_PIN, GPIO_OUT);
    gpio_set(BC_PIN);
#endif

    /* initialize the LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);

    /* initialize the push buttons */
    gpio_init(PB0_PIN, GPIO_IN);
    gpio_init(PB1_PIN, GPIO_IN);
}
