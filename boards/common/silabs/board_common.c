/*
 * SPDX-FileCopyrightText: 2018 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#ifdef MODULE_SILABS_AEM
#include "aem.h"
#endif
#ifdef MODULE_SILABS_BC
#include "bc.h"
#endif
#ifdef MODULE_SILABS_PIC
#include "pic.h"
#endif

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

    /* initialize the push buttons */
    gpio_init(PB0_PIN, GPIO_IN);
    gpio_init(PB1_PIN, GPIO_IN);

    /* enable power and interrupt controller (for sensors) */
#ifdef MODULE_SILABS_PIC
    pic_init();
#endif
}
