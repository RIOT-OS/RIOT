/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_spark-core
 * @{
 *
 * @file
 * @brief       Board specific implementations for the spark-core board
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
    gpio_set(LED1_PIN);
    gpio_set(LED2_PIN);
    gpio_set(LED3_PIN);

    /* initialize the CPU */
    cpu_init();
    /* disable systick interrupt, set by the spark bootloader */
    SysTick->CTRL = 0;
    /* signal to spark bootloader: do not enable IWDG! set Stop Mode Flag! */
    BKP->DR9 = 0xAFFF;
    /* configure the RIOT vector table location to internal flash + spark bootloader offset */
    SCB->VTOR = LOCATION_VTABLE;
}
