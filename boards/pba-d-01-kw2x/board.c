/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_pba-d-01
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Phytec PBA-D-01
 *              evaluation board for PWA-A-002 Module
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "board.h"

static void leds_init(void);

void board_init(void)
{
    leds_init();
    cpu_init();
}

/**
 * @brief Initialize the boards on-board RGB-LED
 *
 */
static void leds_init(void)
{
    /* enable clock */
    LED_B_PORT_CLKEN();
    LED_G_PORT_CLKEN();
    LED_R_PORT_CLKEN();
    /* configure pins as gpio */
    LED_B_PORT_BASE->PCR[LED_B_PIN] = PORT_PCR_MUX(1);
    LED_G_PORT_BASE->PCR[LED_G_PIN] = PORT_PCR_MUX(1);
    LED_R_PORT_BASE->PCR[LED_R_PIN] = PORT_PCR_MUX(1);
    LED_B_GPIO_BASE->PDDR |= (1 << LED_B_PIN);
    LED_G_GPIO_BASE->PDDR |= (1 << LED_G_PIN);
    LED_R_GPIO_BASE->PDDR |= (1 << LED_R_PIN);
    /* turn all LEDs off */
    LED_B_GPIO_BASE->PSOR |= (1 << LED_B_PIN);
    LED_G_GPIO_BASE->PSOR |= (1 << LED_G_PIN);
    LED_R_GPIO_BASE->PSOR |= (1 << LED_R_PIN);
}
