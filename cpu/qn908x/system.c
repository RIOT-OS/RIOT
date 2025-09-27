/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_qn908x
 * @{
 *
 * @file
 * @brief       CMSIS system setup wrapper functions for NXP QN908x
 *
 * The system_QN908XC.h header is used by several vendor headers (including
 * QN908XC.h which defines all the registers and some drivers). The
 * system_QN908XC.c file in the vendor SDK implements the system initialization
 * via the SystemInit() function and provides the current system clock
 * frequency, however part of the system initialization is more appropriate in
 * the board module (in `board_init`) or in the cpu module (in `cpu_init`) and
 * some values like the XTAL or BUCK configuration depend on the actual board
 * used while in the SDK code they are set to the QN9080DK developer module.
 * Because of this we can't include that source file here.
 *
 * This file implements the minimum required to make some of the drivers in the
 * vendor code work by providing the global SystemCoreClock variable and a
 * function to update its value with the current clock configuration.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"

#include "vendor/drivers/fsl_clock.h"

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

void SystemInit(void)
{
    /* Do nothing here. The system initialization is done in board_init() and
     * cpu_init() as needed. This function shouldn't be called anyway.
     */
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = CLOCK_GetFreq(kCLOCK_CoreSysClk);
}
