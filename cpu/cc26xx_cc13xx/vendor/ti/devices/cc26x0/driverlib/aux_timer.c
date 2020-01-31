/******************************************************************************
*  Filename:       aux_timer.c
*  Revised:        2017-06-05 12:13:49 +0200 (Mon, 05 Jun 2017)
*  Revision:       49096
*
*  Description:    Driver for the AUX Timer Module
*
*  Copyright (c) 2015 - 2017, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#include "aux_timer.h"

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  AUXTimerConfigure
    #define AUXTimerConfigure               NOROM_AUXTimerConfigure
    #undef  AUXTimerStart
    #define AUXTimerStart                   NOROM_AUXTimerStart
    #undef  AUXTimerStop
    #define AUXTimerStop                    NOROM_AUXTimerStop
    #undef  AUXTimerPrescaleSet
    #define AUXTimerPrescaleSet             NOROM_AUXTimerPrescaleSet
    #undef  AUXTimerPrescaleGet
    #define AUXTimerPrescaleGet             NOROM_AUXTimerPrescaleGet
#endif

//*****************************************************************************
//
// Configure AUX timer
//
//*****************************************************************************
void
AUXTimerConfigure(uint32_t ui32Timer, uint32_t ui32Config)
{
    uint32_t ui32Val;

    // Check the arguments.
    ASSERT((ui32Timer == AUX_TIMER_0) || (ui32Timer == AUX_TIMER_1) ||
           (ui32Timer == AUX_TIMER_BOTH));

    // Configure Timer 0.
    if(ui32Timer & AUX_TIMER_0)
    {
        // Stop timer 0.
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CTL) = 0;

        // Set mode.
        ui32Val = HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CFG);
        ui32Val &= ~(AUX_TIMER_T0CFG_MODE_M | AUX_TIMER_T0CFG_RELOAD_M);
        ui32Val |= (ui32Config & (AUX_TIMER_T0CFG_MODE_M |
                                  AUX_TIMER_T0CFG_RELOAD_M));
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CFG) = ui32Val;

        // If edge counter, set rising/falling edge and tick source.
        if(ui32Config & AUX_TIMER_T0CFG_MODE_M)
        {
            ui32Val = HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CFG);
            ui32Val &= ~(AUX_TIMER_T0CFG_TICK_SRC_POL_M |
                         AUX_TIMER_T0CFG_TICK_SRC_M);

            // Set edge polarity.
            if(ui32Config & AUX_TIMER_CFG_FALLING_EDGE)
            {
                ui32Val |= AUX_TIMER_T0CFG_TICK_SRC_POL;
            }

            // Set tick source.
            ui32Val |= (ui32Config & AUX_TIMER_T0CFG_TICK_SRC_M);
            HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CFG) = ui32Val;
        }
    }

    // Configure Timer 1.
    if(ui32Timer & AUX_TIMER_1)
    {
        // Stop timer 1.
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CTL) = 0;

        // Set mode.
        ui32Val = HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CFG);
        ui32Val &= ~(AUX_TIMER_T1CFG_MODE_M | AUX_TIMER_T1CFG_RELOAD_M);
        ui32Val |= ((ui32Config) & (AUX_TIMER_T1CFG_MODE_M |
                                    AUX_TIMER_T1CFG_RELOAD_M));
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CFG) = ui32Val;

        // If edge counter, set rising/falling edge and tick source.
        if(ui32Config & AUX_TIMER_T1CFG_MODE)
        {
            ui32Val = HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CFG);
            ui32Val &= ~(AUX_TIMER_T1CFG_TICK_SRC_POL_M |
                         AUX_TIMER_T1CFG_TICK_SRC_M);

            // Set edge polarity.
            if(ui32Config & AUX_TIMER_CFG_FALLING_EDGE)
            {
                ui32Val |= AUX_TIMER_T1CFG_TICK_SRC_POL;
            }

            // Set tick source.
            ui32Val |= (ui32Config & AUX_TIMER_T1CFG_TICK_SRC_M);
            HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CFG) = ui32Val;
        }
    }
}

//*****************************************************************************
//
// Start AUX timer
//
//*****************************************************************************
void
AUXTimerStart(uint32_t ui32Timer)
{
    // Check the arguments.
    ASSERT((ui32Timer == AUX_TIMER_0) ||
           (ui32Timer == AUX_TIMER_1) ||
           (ui32Timer == AUX_TIMER_BOTH));

    if(ui32Timer & AUX_TIMER_0)
    {
        // Start timer 0.
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CTL) = AUX_TIMER_T0CTL_EN;
    }
    if(ui32Timer & AUX_TIMER_1)
    {
        // Start timer 1.
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CTL) = AUX_TIMER_T1CTL_EN;
    }
}

//*****************************************************************************
//
// Stop AUX timer
//
//*****************************************************************************
void
AUXTimerStop(uint32_t ui32Timer)
{
    // Check the arguments.
    ASSERT((ui32Timer == AUX_TIMER_0) ||
           (ui32Timer == AUX_TIMER_1) ||
           (ui32Timer == AUX_TIMER_BOTH));

    if(ui32Timer & AUX_TIMER_0)
    {
        // Stop timer 0.
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CTL) = 0;
    }
    if(ui32Timer & AUX_TIMER_1)
    {
        // Stop timer 1.
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CTL) = 0;
    }
}

//*****************************************************************************
//
// Set AUX timer prescale value
//
//*****************************************************************************
void
AUXTimerPrescaleSet(uint32_t ui32Timer, uint32_t ui32PrescaleDiv)
{
    uint32_t ui32Val;

    // Check the arguments.
    ASSERT((ui32Timer == AUX_TIMER_0) || (ui32Timer == AUX_TIMER_1) ||
           (ui32Timer == AUX_TIMER_BOTH));
    ASSERT(ui32PrescaleDiv <= AUX_TIMER_PRESCALE_DIV_32768);

    if(ui32Timer & AUX_TIMER_0)
    {
        // Set timer 0 prescale value.
        ui32Val = HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CFG);
        ui32Val &= ~AUX_TIMER_T0CFG_PRE_M;
        ui32Val |=  ui32PrescaleDiv << AUX_TIMER_T0CFG_PRE_S;
        HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CFG) = ui32Val;
    }
    if(ui32Timer & AUX_TIMER_1)
    {
        // Set timer 1 prescale value.
        ui32Val = HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T1CFG);
        ui32Val &= ~AUX_TIMER_T1CFG_PRE_M;
        ui32Val |=  ui32PrescaleDiv << AUX_TIMER_T1CFG_PRE_S;
        HWREG(AUX_TIMER_BASE  + AUX_TIMER_O_T1CFG) = ui32Val;
    }
}

//*****************************************************************************
//
// Get AUX timer prescale value
//
//*****************************************************************************
uint32_t
AUXTimerPrescaleGet(uint32_t ui32Timer)
{
    uint32_t ui32Val;
    uint32_t ui32PrescaleDiv;

    // Check the arguments.
    ASSERT((ui32Timer == AUX_TIMER_0) || (ui32Timer == AUX_TIMER_1));

    ui32Val = (HWREG(AUX_TIMER_BASE + AUX_TIMER_O_T0CFG));
    if(ui32Timer & AUX_TIMER_0)
    {
        // Get timer 0 prescale value.
        ui32PrescaleDiv =
            (ui32Val & AUX_TIMER_T0CFG_PRE_M) >> AUX_TIMER_T0CFG_PRE_S;
    }
    else
    {
        // Get timer 1 prescale value.
        ui32PrescaleDiv =
            (ui32Val & AUX_TIMER_T1CFG_PRE_M) >> AUX_TIMER_T1CFG_PRE_S;
    }

    return(ui32PrescaleDiv);
}
