/*
 * Copyright (C) 2014 Freie Universität Berlin
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
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 2);

    /* initialize the CPU */
    cpu_init();
}

void SetupFPU(void)
{
	FPUEnable();
	FPULazyStackingEnable();
}

void SetupClock(int clk)
{
	switch(clk){
		case CLK80:
			SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		case CLK50:
			SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		case CLK40:
			SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		case CLK16:
			SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
		default:
			SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
			break;
	}
}


