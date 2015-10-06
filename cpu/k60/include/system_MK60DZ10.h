/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef SYSTEM_MK60D10_H_
#define SYSTEM_MK60D10_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief Device specific configuration file for MK60D10 (header file)
 */


/**
 * \brief Current core clock frequency
 *
 * MCGOUTCLK divided by OUTDIV1 clocks the ARM Cortex-M4 core.
 */
extern uint32_t SystemCoreClock;

/**
 * \brief Current system clock frequency
 *
 * MCGOUTCLK divided by OUTDIV1 clocks the crossbar switch and bus masters
 * directly connected to the crossbar. In addition, this clock is used for UART0
 * and UART1.
 */
extern uint32_t SystemSysClock;

/**
 * \brief Current bus clock frequency
 *
 * MCGOUTCLK divided by OUTDIV2 clocks the bus slaves and peripheral (excluding
 * memories).
 */
extern uint32_t SystemBusClock;

/**
 * \brief Current FlexBus clock frequency
 *
 * MCGOUTCLK divided by OUTDIV3 clocks the external FlexBus interface.
 */
extern uint32_t SystemFlexBusClock;

/**
 * \brief Current flash clock frequency
 *
 * MCGOUTCLK divided by OUTDIV4 clocks the flash memory.
 */
extern uint32_t SystemFlashClock;

/**
 * \brief Updates all of the SystemCoreClock variables.
 *
 * It must be called whenever the core clock is changed during program
 * execution. SystemCoreClockUpdate() evaluates the clock register settings and
 * calculates the current core clock.
 */
void SystemCoreClockUpdate(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* #if !defined(SYSTEM_MK60D10_H_) */
