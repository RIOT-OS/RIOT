//*****************************************************************************
//
// cpu_usage.c - Routines to determine the CPU utilization.
//
// Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "utils/cpu_usage.h"

//*****************************************************************************
//
//! \addtogroup cpu_usage_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The peripheral identifier for the timer modules that could be used for
// tracking CPU utilization.
//
//*****************************************************************************
static unsigned long g_pulCPUUsageTimerPeriph[4] =
{
    SYSCTL_PERIPH_TIMER0, SYSCTL_PERIPH_TIMER1, SYSCTL_PERIPH_TIMER2,
    SYSCTL_PERIPH_TIMER3
};

//*****************************************************************************
//
// The base address of the timer modules that could be used for tracking CPU
// utilization.
//
//*****************************************************************************
static unsigned long g_pulCPUUsageTimerBase[4] =
{
    TIMER0_BASE, TIMER1_BASE, TIMER2_BASE, TIMER3_BASE
};

//*****************************************************************************
//
// The index of the timer module that will be used for tracking CPU
// utilization.
//
//*****************************************************************************
static unsigned long g_ulCPUUsageTimer;

//*****************************************************************************
//
// The number of processor clock ticks per timing period.
//
//*****************************************************************************
static unsigned long g_ulCPUUsageTicks;

//*****************************************************************************
//
// The value of timer two on the previous timing period.  This is used to
// determine the number of clock ticks counted by the timer during the timing
// period.
//
//*****************************************************************************
static unsigned long g_ulCPUUsagePrevious;

//*****************************************************************************
//
//! Updates the CPU usage for the new timing period.
//!
//! This function, when called at the end of a timing period, will update the
//! CPU usage.
//!
//! \return Returns the CPU usage percentage as a 16.16 fixed-point value.
//
//*****************************************************************************
unsigned long
CPUUsageTick(void)
{
    unsigned long ulValue, ulUsage;

    //
    // Get the current value of the timer.
    //
    ulValue = MAP_TimerValueGet(g_pulCPUUsageTimerBase[g_ulCPUUsageTimer],
                                TIMER_A);

    //
    // Based on the number of clock ticks accumulated by the timer during the
    // previous timing period, compute the CPU usage as a 16.16 fixed-point
    // value.
    //
    ulUsage = ((((g_ulCPUUsagePrevious - ulValue) * 6400) /
                g_ulCPUUsageTicks) * 1024);

    //
    // Save the previous value of the timer.
    //
    g_ulCPUUsagePrevious = ulValue;

    //
    // Return the new CPU usage value.
    //
    return(ulUsage);
}

//*****************************************************************************
//
//! Initializes the CPU usage measurement module.
//!
//! \param ulClockRate is the rate of the clock supplied to the timer module.
//! \param ulRate is the number of times per second that CPUUsageTick() is
//! called.
//! \param ulTimer is the index of the timer module to use.
//!
//! This function prepares the CPU usage measurement module for measuring the
//! CPU usage of the application.
//!
//! \return None.
//
//*****************************************************************************
void
CPUUsageInit(unsigned long ulClockRate, unsigned long ulRate,
             unsigned long ulTimer)
{
    //
    // Check the arguments.
    //
    ASSERT(ulClockRate > ulRate);
    ASSERT(ulTimer < 4);

    //
    // Save the timer index.
    //
    g_ulCPUUsageTimer = ulTimer;

    //
    // Determine the number of system clocks per measurement period.
    //
    g_ulCPUUsageTicks = ulClockRate / ulRate;

    //
    // Set the previous value of the timer to the initial timer value.
    //
    g_ulCPUUsagePrevious = 0xffffffff;

    //
    // Enable peripheral clock gating.
    //
    MAP_SysCtlPeripheralClockGating(true);

    //
    // Enable the third timer while the processor is in run mode, but disable
    // it in sleep mode.  It will therefore count system clocks when the
    // processor is running but not when it is sleeping.
    //
    MAP_SysCtlPeripheralEnable(g_pulCPUUsageTimerPeriph[ulTimer]);
    MAP_SysCtlPeripheralSleepDisable(g_pulCPUUsageTimerPeriph[ulTimer]);

    //
    // Configure the third timer for 32-bit periodic operation.
    //
    MAP_TimerConfigure(g_pulCPUUsageTimerBase[ulTimer], TIMER_CFG_PERIODIC);

    //
    // Set the load value for the third timer to the maximum value.
    //
    MAP_TimerLoadSet(g_pulCPUUsageTimerBase[ulTimer], TIMER_A, 0xffffffff);

    //
    // Enable the third timer.  It will now count the number of system clocks
    // during which the processor is executing code.
    //
    MAP_TimerEnable(g_pulCPUUsageTimerBase[ulTimer], TIMER_A);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
