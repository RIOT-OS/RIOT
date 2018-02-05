/******************************************************************************
*  Filename:       hw_ints.h
*  Revised:        $Date: 2013-04-29 09:49:55 +0200 (Mon, 29 Apr 2013) $
*  Revision:       $Revision: 9923 $
*
*  Description:    Macros that define the interrupt assignment on Stellaris.
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/


#ifndef __HW_INTS_H__
#define __HW_INTS_H__

// Note: Use the following define if alternate interrupt map is to be used.
//       This map is smaller. The function IntAltMapEnable() must be called
//       to enable The alternate map.
// #define CC2538_USE_ALTERNATE_INTERRUPT_MAP 1

//*****************************************************************************
//
// The following are defines for the fault assignments.
//
//*****************************************************************************
#define FAULT_NMI               2           // NMI fault
#define FAULT_HARD              3           // Hard fault
#define FAULT_MPU               4           // MPU fault
#define FAULT_BUS               5           // Bus fault
#define FAULT_USAGE             6           // Usage fault
#define FAULT_SVCALL            11          // SVCall
#define FAULT_DEBUG             12          // Debug monitor
#define FAULT_PENDSV            14          // PendSV
#define FAULT_SYSTICK           15          // System Tick

//*****************************************************************************
//
// The following are defines for the interrupt assignments.
//
//*****************************************************************************
#define INT_GPIOA               16          // GPIO Port A
#define INT_GPIOB               17          // GPIO Port B
#define INT_GPIOC               18          // GPIO Port C
#define INT_GPIOD               19          // GPIO Port D
// 20 not in use
#define INT_UART0               21          // UART0 Rx and Tx
#define INT_UART1               22          // UART1 Rx and Tx
#define INT_SSI0                23          // SSI0 Rx and Tx
#define INT_I2C0                24          // I2C0 Master and Slave
// 25 - 29 not in use
#define INT_ADC0                30          // ADC0 Sequence 0
// 31 - 33 not in use
#define INT_WATCHDOG            34          // Watchdog timer
#define INT_WATCHDOG0           34          // Watchdog Timer0
#define INT_TIMER0A             35          // Timer 0 subtimer A
#define INT_TIMER0B             36          // Timer 0 subtimer B
#define INT_TIMER1A             37          // Timer 1 subtimer A
#define INT_TIMER1B             38          // Timer 1 subtimer B
#define INT_TIMER2A             39          // Timer 2 subtimer A
#define INT_TIMER2B             40          // Timer 2 subtimer B
#define INT_COMP0               41          // Analog Comparator 0

// 42 - 44 only in use for alternate map
#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
#define INT_RFCORERTX           42           // RFCORE RX/TX
#define INT_RFCOREERR           43           // RFCORE Error
#define INT_ICEPICK             44           // Icepick
#endif // CC2538_USE_ALTERNATE_INTERRUPT_MAP

#define INT_FLASH               45          // FLASH Control

// 46 - 49 only in use for alternate map
#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
#define INT_AES                 46           // AES
#define INT_PKA                 47           // PKA
#define INT_SMTIM               48           // SMTimer
#define INT_MACTIMR             49           // MACTimer
#endif // CC2538_USE_ALTERNATE_INTERRUPT_MAP

#define INT_SSI1                50          // SSI1 Rx and Tx
#define INT_TIMER3A             51          // Timer 3 subtimer A
#define INT_TIMER3B             52          // Timer 3 subtimer B
// 53 - 59 not in use
// 60 only in use for alternate map
#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
#define INT_USB2538             60           // USB new for 2538
#endif // CC2538_USE_ALTERNATE_INTERRUPT_MAP

// 61 not in use
#define INT_UDMA                62          // uDMA controller
#define INT_UDMAERR             63          // uDMA Error

// 64 - 155 not in use
// 156-162 only in use in basic map
#ifndef CC2538_USE_ALTERNATE_INTERRUPT_MAP
#define INT_USB2538            156          // USB new for 2538
#define INT_RFCORERTX          157          // RFCORE RX/TX
#define INT_RFCOREERR          158          // RFCORE Error
#define INT_AES                159          // AES
#define INT_PKA                160          // PKA
#define INT_SMTIM              161          // SMTimer
#define INT_MACTIMR            162          // MACTimer
#endif // not CC2538_USE_ALTERNATE_INTERRUPT_MAP

//*****************************************************************************
//
// The following are defines for the total number of interrupts.
//
//*****************************************************************************

#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
#define NUM_INTERRUPTS          64
#else
#define NUM_INTERRUPTS          163
#endif // CC2538_USE_ALTERNATE_INTERRUPT_MAP

//*****************************************************************************
//
// The following are defines for the total number of priority levels.
//
//*****************************************************************************
#define NUM_PRIORITY            8
#define NUM_PRIORITY_BITS       3

#endif // __HW_INTS_H__
