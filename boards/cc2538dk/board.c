/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_cc2538dk
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the CC2538DK board
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "ioc.h"
#include "lpm.h"
#include "cc2538-gpio.h"

static void led_init_helper(int gpio_num) {
    gpio_software_control(gpio_num);
    gpio_dir_output(gpio_num);

    /* Enable output without any internal pull resistors: */
    IOC_PXX_OVER[gpio_num] = IOC_OVERRIDE_OE;
}

/**
 * @brief Initialize the SmartRF06's on-board LEDs
 */
void led_init(void)
{
    led_init_helper(LED_RED_GPIO);
    led_init_helper(LED_GREEN_GPIO);
    led_init_helper(LED_YELLOW_GPIO);
    led_init_helper(LED_ORANGE_GPIO);
}

/**
 * @brief Initialize the SmartRF06 board
 */
void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();
}

/** @} */
