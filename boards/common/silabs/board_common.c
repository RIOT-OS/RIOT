/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_silabs
 * @{
 *
 * @file
 * @brief       Implementations of the common board features.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board_common.h"

#include "aem.h"
#include "bc.h"
#include "pic.h"

void board_common_init(void)
{
    /* initialize the advanced energy monitor */
#ifdef MODULE_SILABS_AEM
    aem_init();
#endif

    /* initialize the board controller (to enable serial output) */
#ifdef MODULE_SILABS_BC
    bc_init();
#endif

    /* initialize the LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);

    /* initialize the push buttons */
    gpio_init(PB0_PIN, GPIO_IN);
    gpio_init(PB1_PIN, GPIO_IN);

    /* enable power and interrupt controller (for sensors) */
#ifdef MODULE_SILABS_PIC
    pic_init();
#endif
}
