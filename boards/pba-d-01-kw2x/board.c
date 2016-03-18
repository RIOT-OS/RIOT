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
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    /* initialize the CPU core */
    cpu_init();
}
