/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_ek-lm4f120xl
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Stellaris Launchpad LM4F120 board
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include "board.h"

void board_init(void)
{
	SetupFPU();
	SetupClock(CLK40);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 2);

    /* initialize the CPU */
    cpu_init();
}

void SetupFPU(void)
{
	ROM_FPUEnable();
	ROM_FPULazyStackingEnable();
}

void SetupClock(int clk)
{
	switch(clk){
		case CLK80:
			ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		case CLK50:
			ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		case CLK40:
			ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		case CLK16:
			ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		default:
			ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
	}
}


