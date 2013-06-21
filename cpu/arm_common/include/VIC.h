/* Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will

This file is part of the Micro-mesh SensorWeb Firmware.

Micro-Mesh is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

Micro-Mesh is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Micro-Mesh; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef __ARM_COMMON_H
#define __ARM_COMMON_H

/**
 * @ingroup			arm_common
 * @{
 */

#define I_Bit			0x80
#define F_Bit			0x40

#define SYS32Mode		0x1F
#define IRQ32Mode		0x12
#define FIQ32Mode		0x11

#define INTMode         (FIQ32Mode | IRQ32Mode)


/**
 * @name	IRQ Priority Mapping
 */
//@{
#define HIGHEST_PRIORITY	0x01
#define IRQP_RTIMER			1	// FIQ_PRIORITY // TODO: investigate problems with rtimer and FIQ
#define IRQP_TIMER1			1
#define IRQP_WATCHDOG		1
#define IRQP_CLOCK			3
#define IRQP_GPIO			4
#define IRQP_RTC			8
#define LOWEST_PRIORITY		0x0F
// @}


#define	WDT_INT			0
#define SWI_INT			1
#define ARM_CORE0_INT	2
#define	ARM_CORE1_INT	3
#define	TIMER0_INT		4
#define TIMER1_INT		5
#define UART0_INT		6
#define	UART1_INT		7
#define	PWM0_1_INT		8
#define I2C0_INT		9
#define SPI0_INT		10			/* SPI and SSP0 share VIC slot */
#define SSP0_INT		10
#define	SSP1_INT		11
#define	PLL_INT			12
#define RTC_INT			13
#define EINT0_INT		14
#define EINT1_INT		15
#define EINT2_INT		16
#define EINT3_INT		17
#define	ADC0_INT		18
#define I2C1_INT		19
#define BOD_INT			20
#define EMAC_INT		21
#define USB_INT			22
#define CAN_INT			23
#define MCI_INT			24
#define GPDMA_INT		25
#define TIMER2_INT		26
#define TIMER3_INT		27
#define UART2_INT		28
#define UART3_INT		29
#define I2C2_INT		30
#define I2S_INT			31

#define VECT_ADDR_INDEX	0x100
#define VECT_CNTL_INDEX 0x200

#include <stdbool.h>
#include "cpu.h"

bool cpu_install_irq(int IntNumber, void *HandlerAddr, int Priority);

/** @} */
#endif /*ARMVIC_H_*/
