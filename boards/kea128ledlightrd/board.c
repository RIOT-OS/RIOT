/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_kea128ledlightrd
 * @{
 *
 * @file
 * @brief       Board specific implementations for the KEA128LEDLIGHTRD
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Anton Gerasimov <anton.gerasimov@here.com>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "tja1042.h"

extern void SystemInit(void);

void board_init(void)
{
    /* initialize clocks */
    SystemInit();

    /* initialize the CPU core */
    cpu_init();

    /* disable NMI on the pin used for LED2 */
    SIM->SOPT0 &= (~SIM_SOPT0_NMIE_MASK);

    /* initialize and turn off the on-board RGB-LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
    gpio_set(LED0_PIN);
    gpio_set(LED1_PIN);
    gpio_set(LED2_PIN);
    gpio_set(LED3_PIN);
}
