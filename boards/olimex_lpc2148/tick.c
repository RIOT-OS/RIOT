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

#include "lpc214x.h"
#include "bits.h"
//#include "tick.h"
#include "minimal_dbg_console.h"
#include "VIC.h"

void Timer0_IRQHandler(void) __attribute__((interrupt("IRQ")));

extern void eINT();
extern void dINT();


void driver_timer_load(void)
{
    T0TCR = 0;                  // Disable timer 0.
    T0PR  = 3000;               // Prescaler is set to relevant pclk , counter is incremented every T0PR tact.
    T0CCR = 0;                  // Capture is disabled.
    T0EMR = 0;                  // No external match output.
    T0TC = 0;
    T0MR0 = 1000;
    T0MCR |= BIT0 + BIT1;
    T0TCR = BIT0;               // Enable timer 0.

    dINT();                      // Disable all interrupts
    VICIntEnable = BIT4;          // Enable Interrupthandling for Timer0
    VICVectCntl3 = 4 + BIT5;      // Assign Timer0 to IRQ Slot 3
    VICVectAddr3 = (unsigned int)Timer0_IRQHandler;    // Assign Isr Address
    eINT();
}

int counter = 0;

void Timer0_IRQHandler(void)
{
    extern unsigned int sched_context_switch_request;
    counter++;
    T0IR |= 0xff;											// reset timer1 interrupt flag
    sl_printf("#");

    sched_context_switch_request = 1;

    VICVectAddr = 0;										// acknowledge interrupt (if using VIC IRQ)
}

