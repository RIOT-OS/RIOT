/*
 * SPDX-FileCopyrightText: 2005-2008 by Thomas Hillebrandt and Heiko Will
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup         cpu_arm7_common
 * @{
 */

#define I_Bit           0x80
#define F_Bit           0x40

#define SYS32Mode       0x1F
#define IRQ32Mode       0x12
#define FIQ32Mode       0x11

#define INTMode         (FIQ32Mode | IRQ32Mode)

/**
 * @name    IRQ Priority Mapping
 */
/** @{ */
#define HIGHEST_PRIORITY    0x01
#define IRQP_RTIMER         1   /* FIQ_PRIORITY // TODO: investigate problems with rtimer and FIQ */
#define IRQP_TIMER1         1
#define IRQP_WATCHDOG       1
#define IRQP_CLOCK          3
#define IRQP_GPIO           4
#define IRQP_RTC            8
#define LOWEST_PRIORITY     0x0F
/** @} */

#define WDT_INT         0
#define SWI_INT         1
#define ARM_CORE0_INT   2
#define ARM_CORE1_INT   3
#define TIMER0_INT      4
#define TIMER1_INT      5
#define UART0_INT       6
#define UART1_INT       7
#define PWM0_1_INT      8
#define I2C0_INT        9
#define SPI0_INT        10          /* SPI and SSP0 share VIC slot */
#define SSP0_INT        10
#define SSP1_INT        11
#define PLL_INT         12
#define RTC_INT         13
#define EINT0_INT       14
#define EINT1_INT       15
#define EINT2_INT       16
#define EINT3_INT       17
#define ADC0_INT        18
#define I2C1_INT        19
#define BOD_INT         20
#define EMAC_INT        21
#define USB_INT         22
#define CAN_INT         23
#define MCI_INT         24
#define GPDMA_INT       25
#define TIMER2_INT      26
#define TIMER3_INT      27
#define UART2_INT       28
#define UART3_INT       29
#define I2C2_INT        30
#define I2S_INT         31

#define VECT_ADDR_INDEX 0x100
#define VECT_CNTL_INDEX 0x200

bool cpu_install_irq(int IntNumber, void *HandlerAddr, int Priority);

#ifdef __cplusplus
}
#endif

/** @} */
