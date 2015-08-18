/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc2538dk
 * @{
 *
 * @file
 * @brief       Board specific implementations for the CC2538DK board
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#include <stdio.h>

#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>

#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"

#include "board.h"
#include "cpu.h"

//#include "ioc.h"
#include "lpm.h"
//#include "cc2538-gpio.h"

extern void (* const g_pfnVectors[])(void);

#if 0
static void led_init_helper(int gpio_num) {
    gpio_software_control(gpio_num);
    gpio_dir_output(gpio_num);

    /* Enable output without any internal pull resistors: */
    IOC_PXX_OVER[gpio_num] = IOC_OVERRIDE_OE;
}
#endif

/**
 * @brief Initialize the SmartRF06's on-board LEDs
 */
void led_init(void)
{
#if 0
    led_init_helper(LED_RED_GPIO);
    led_init_helper(LED_GREEN_GPIO);
    led_init_helper(LED_YELLOW_GPIO);
    led_init_helper(LED_ORANGE_GPIO);
#endif
}

/**
 * @brief Initialize the board
 */
void board_init(void)
{
	//
	// Set vector table base
	//
	MAP_IntVTableBaseSet((unsigned long) &g_pfnVectors[0]);

	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
	MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();
}

/** @} */
