/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32F3Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "onboard_led.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    onboard_led_init();

    /* initialize the CPU */
    cpu_init();
}
