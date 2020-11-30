/*
** ###################################################################
**     Processors:          QN9080C
**                          QN9083C
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          MCUXpresso Compiler
**
**     Reference manual:    QN908X User manual Rev.1.0 21 Mar 2017
**     Version:             rev. 1.0, 2017-03-27
**     Build:               b170328
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2017 NXP
**     All rights reserved.
**     
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2017-03-27)
**         Initial version.
**
** ###################################################################
*/

/*!
 * @file QN908XC
 * @version 1.0
 * @date 2017-03-27
 * @brief Device specific configuration file for QN908XC (header file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#ifndef _SYSTEM_QN908XC_H_
#define _SYSTEM_QN908XC_H_                       /**< Symbol preventing repeated inclusion */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CLK_XTAL_16MHZ 16000000u /* 16 MHz XTAL clock */
#define CLK_XTAL_32MHZ 32000000u /* 32 MHz XTAL clock */
#define CLK_OSC_32MHZ 32000000u  /* OSC 32 MHz */
#define CLK_XTAL_32KHZ 32768u    /* XTAL 32 kHz */
#define CLK_RCO_32KHZ 32000u     /* RCO 32 kHz */

#define DEFAULT_SYSTEM_CLOCK 16000000u /* Default System clock value */

/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
extern uint32_t SystemCoreClock;

/**
 * @brief Setup the microcontroller system.
 *
 * Typically this function configures the oscillator (PLL) that is part of the
 * microcontroller device. For systems with variable clock speed it also updates
 * the variable SystemCoreClock. SystemInit is called from startup_device file.
 */
void SystemInit (void);

/**
 * @brief Updates the SystemCoreClock variable.
 *
 * It must be called whenever the core clock is changed during program
 * execution. SystemCoreClockUpdate() evaluates the clock register settings and calculates
 * the current core clock.
 */
void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif  /* _SYSTEM_QN908XC_H_ */
