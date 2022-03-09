/* ============================================================================ */
/* Copyright (c) 2020, Texas Instruments Incorporated                           */
/*  All rights reserved.                                                        */
/*                                                                              */
/*  Redistribution and use in source and binary forms, with or without          */
/*  modification, are permitted provided that the following conditions          */
/*  are met:                                                                    */
/*                                                                              */
/*  *  Redistributions of source code must retain the above copyright           */
/*     notice, this list of conditions and the following disclaimer.            */
/*                                                                              */
/*  *  Redistributions in binary form must reproduce the above copyright        */
/*     notice, this list of conditions and the following disclaimer in the      */
/*     documentation and/or other materials provided with the distribution.     */
/*                                                                              */
/*  *  Neither the name of Texas Instruments Incorporated nor the names of      */
/*     its contributors may be used to endorse or promote products derived      */
/*     from this software without specific prior written permission.            */
/*                                                                              */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" */
/*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,       */
/*  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      */
/*  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR            */
/*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,       */
/*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,         */
/*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; */
/*  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,    */
/*  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR     */
/*  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,              */
/*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                          */
/* ============================================================================ */

/********************************************************************
*
* Standard register and bit definitions for the Texas Instruments
* MSP430 microcontroller.
*
* This file supports assembler and C development for
* MSP430x26x devices.
*
* Texas Instruments, Version 1.5
*
* Rev. 1.0, Initial Version
* Rev. 1.1, changed PAREN from sfrb to sfrw
* Rev. 1.2  added TLV in INFO Memory
* Rev. 1.3, added definitions for Interrupt Vectors xxIV
* Rev. 1.4, changed 'void __data20 * volatile' definition
* Rev. 1.5, fixed define: TAG_ADC12_1 to 0x08
*
********************************************************************/

#ifndef __msp430x26x
#define __msp430x26x

#define __MSP430_HAS_MSP430X_CPU__                /* Definition to show that it has MSP430X CPU */

#define __MSP430_HEADER_VERSION__ 1210

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/* PERIPHERAL FILE MAP                                                        */
/*----------------------------------------------------------------------------*/

#define __MSP430_TI_HEADERS__

#include <iomacros.h>


/************************************************************
* STANDARD BITS
************************************************************/

#define BIT0                   (0x0001)
#define BIT1                   (0x0002)
#define BIT2                   (0x0004)
#define BIT3                   (0x0008)
#define BIT4                   (0x0010)
#define BIT5                   (0x0020)
#define BIT6                   (0x0040)
#define BIT7                   (0x0080)
#define BIT8                   (0x0100)
#define BIT9                   (0x0200)
#define BITA                   (0x0400)
#define BITB                   (0x0800)
#define BITC                   (0x1000)
#define BITD                   (0x2000)
#define BITE                   (0x4000)
#define BITF                   (0x8000)

/************************************************************
* STATUS REGISTER BITS
************************************************************/

#define C                      (0x0001)
#define Z                      (0x0002)
#define N                      (0x0004)
#define V                      (0x0100)
#define GIE                    (0x0008)
#define CPUOFF                 (0x0010)
#define OSCOFF                 (0x0020)
#define SCG0                   (0x0040)
#define SCG1                   (0x0080)

/* Low Power Modes coded with Bits 4-7 in SR */

#ifndef __STDC__ /* Begin #defines for assembler */
#define LPM0                   (CPUOFF)
#define LPM1                   (SCG0+CPUOFF)
#define LPM2                   (SCG1+CPUOFF)
#define LPM3                   (SCG1+SCG0+CPUOFF)
#define LPM4                   (SCG1+SCG0+OSCOFF+CPUOFF)
/* End #defines for assembler */

#else /* Begin #defines for C */
#define LPM0_bits              (CPUOFF)
#define LPM1_bits              (SCG0+CPUOFF)
#define LPM2_bits              (SCG1+CPUOFF)
#define LPM3_bits              (SCG1+SCG0+CPUOFF)
#define LPM4_bits              (SCG1+SCG0+OSCOFF+CPUOFF)

#include "in430.h"

#define LPM0      __bis_SR_register(LPM0_bits)         /* Enter Low Power Mode 0 */
#define LPM0_EXIT __bic_SR_register_on_exit(LPM0_bits) /* Exit Low Power Mode 0 */
#define LPM1      __bis_SR_register(LPM1_bits)         /* Enter Low Power Mode 1 */
#define LPM1_EXIT __bic_SR_register_on_exit(LPM1_bits) /* Exit Low Power Mode 1 */
#define LPM2      __bis_SR_register(LPM2_bits)         /* Enter Low Power Mode 2 */
#define LPM2_EXIT __bic_SR_register_on_exit(LPM2_bits) /* Exit Low Power Mode 2 */
#define LPM3      __bis_SR_register(LPM3_bits)         /* Enter Low Power Mode 3 */
#define LPM3_EXIT __bic_SR_register_on_exit(LPM3_bits) /* Exit Low Power Mode 3 */
#define LPM4      __bis_SR_register(LPM4_bits)         /* Enter Low Power Mode 4 */
#define LPM4_EXIT __bic_SR_register_on_exit(LPM4_bits) /* Exit Low Power Mode 4 */
#endif /* End #defines for C */

/************************************************************
* PERIPHERAL FILE MAP
************************************************************/

/************************************************************
* SPECIAL FUNCTION REGISTER ADDRESSES + CONTROL BITS
************************************************************/

sfr_b(IE1);                                   /* Interrupt Enable 1 */
#define WDTIE                  (0x01)         /* Watchdog Interrupt Enable */
#define OFIE                   (0x02)         /* Osc. Fault  Interrupt Enable */
#define NMIIE                  (0x10)         /* NMI Interrupt Enable */
#define ACCVIE                 (0x20)         /* Flash Access Violation Interrupt Enable */

sfr_b(IFG1);                                  /* Interrupt Flag 1 */
#define WDTIFG                 (0x01)         /* Watchdog Interrupt Flag */
#define OFIFG                  (0x02)         /* Osc. Fault Interrupt Flag */
#define PORIFG                 (0x04)         /* Power On Interrupt Flag */
#define RSTIFG                 (0x08)         /* Reset Interrupt Flag */
#define NMIIFG                 (0x10)         /* NMI Interrupt Flag */

sfr_b(IE2);                                   /* Interrupt Enable 2 */
#define UC0IE                  IE2
#define UCA0RXIE               (0x01)
#define UCA0TXIE               (0x02)
#define UCB0RXIE               (0x04)
#define UCB0TXIE               (0x08)

sfr_b(IFG2);                                  /* Interrupt Flag 2 */
#define UC0IFG                 IFG2
#define UCA0RXIFG              (0x01)
#define UCA0TXIFG              (0x02)
#define UCB0RXIFG              (0x04)
#define UCB0TXIFG              (0x08)

sfr_b(UC1IE);                                 /* USCI 1 Interrupt Enable */
#define UCA1RXIE               (0x01)
#define UCA1TXIE               (0x02)
#define UCB1RXIE               (0x04)
#define UCB1TXIE               (0x08)

sfr_b(UC1IFG);                                /* ISCI 1 Interrupt Flags */
#define UCA1RXIFG              (0x01)
#define UCA1TXIFG              (0x02)
#define UCB1RXIFG              (0x04)
#define UCB1TXIFG              (0x08)

/************************************************************
* ADC12
************************************************************/
#define __MSP430_HAS_ADC12__                  /* Definition to show that Module is available */

sfr_w(ADC12CTL0);                             /* ADC12 Control 0 */
sfr_w(ADC12CTL1);                             /* ADC12 Control 1 */
sfr_w(ADC12IFG);                              /* ADC12 Interrupt Flag */
sfr_w(ADC12IE);                               /* ADC12 Interrupt Enable */
sfr_w(ADC12IV);                               /* ADC12 Interrupt Vector Word */

#define ADC12MEM_              (0x0140)       /* ADC12 Conversion Memory */
#ifndef __STDC__
#define ADC12MEM               (ADC12MEM_)    /* ADC12 Conversion Memory (for assembler) */
#else
#define ADC12MEM               ((volatile int*)        ADC12MEM_) /* ADC12 Conversion Memory (for C) */
#endif
sfr_w(ADC12MEM0);                             /* ADC12 Conversion Memory 0 */
sfr_w(ADC12MEM1);                             /* ADC12 Conversion Memory 1 */
sfr_w(ADC12MEM2);                             /* ADC12 Conversion Memory 2 */
sfr_w(ADC12MEM3);                             /* ADC12 Conversion Memory 3 */
sfr_w(ADC12MEM4);                             /* ADC12 Conversion Memory 4 */
sfr_w(ADC12MEM5);                             /* ADC12 Conversion Memory 5 */
sfr_w(ADC12MEM6);                             /* ADC12 Conversion Memory 6 */
sfr_w(ADC12MEM7);                             /* ADC12 Conversion Memory 7 */
sfr_w(ADC12MEM8);                             /* ADC12 Conversion Memory 8 */
sfr_w(ADC12MEM9);                             /* ADC12 Conversion Memory 9 */
sfr_w(ADC12MEM10);                            /* ADC12 Conversion Memory 10 */
sfr_w(ADC12MEM11);                            /* ADC12 Conversion Memory 11 */
sfr_w(ADC12MEM12);                            /* ADC12 Conversion Memory 12 */
sfr_w(ADC12MEM13);                            /* ADC12 Conversion Memory 13 */
sfr_w(ADC12MEM14);                            /* ADC12 Conversion Memory 14 */
sfr_w(ADC12MEM15);                            /* ADC12 Conversion Memory 15 */

#define ADC12MCTL_             (0x0080)       /* ADC12 Memory Control */
#ifndef __STDC__
#define ADC12MCTL              (ADC12MCTL_)   /* ADC12 Memory Control (for assembler) */
#else
#define ADC12MCTL              ((volatile char*)       ADC12MCTL_) /* ADC12 Memory Control (for C) */
#endif
sfr_b(ADC12MCTL0);                            /* ADC12 Memory Control 0 */
sfr_b(ADC12MCTL1);                            /* ADC12 Memory Control 1 */
sfr_b(ADC12MCTL2);                            /* ADC12 Memory Control 2 */
sfr_b(ADC12MCTL3);                            /* ADC12 Memory Control 3 */
sfr_b(ADC12MCTL4);                            /* ADC12 Memory Control 4 */
sfr_b(ADC12MCTL5);                            /* ADC12 Memory Control 5 */
sfr_b(ADC12MCTL6);                            /* ADC12 Memory Control 6 */
sfr_b(ADC12MCTL7);                            /* ADC12 Memory Control 7 */
sfr_b(ADC12MCTL8);                            /* ADC12 Memory Control 8 */
sfr_b(ADC12MCTL9);                            /* ADC12 Memory Control 9 */
sfr_b(ADC12MCTL10);                           /* ADC12 Memory Control 10 */
sfr_b(ADC12MCTL11);                           /* ADC12 Memory Control 11 */
sfr_b(ADC12MCTL12);                           /* ADC12 Memory Control 12 */
sfr_b(ADC12MCTL13);                           /* ADC12 Memory Control 13 */
sfr_b(ADC12MCTL14);                           /* ADC12 Memory Control 14 */
sfr_b(ADC12MCTL15);                           /* ADC12 Memory Control 15 */

/* ADC12CTL0 */
#define ADC12SC                (0x001)        /* ADC12 Start Conversion */
#define ENC                    (0x002)        /* ADC12 Enable Conversion */
#define ADC12TOVIE             (0x004)        /* ADC12 Timer Overflow interrupt enable */
#define ADC12OVIE              (0x008)        /* ADC12 Overflow interrupt enable */
#define ADC12ON                (0x010)        /* ADC12 On/enable */
#define REFON                  (0x020)        /* ADC12 Reference on */
#define REF2_5V                (0x040)        /* ADC12 Ref 0:1.5V / 1:2.5V */
#define MSC                    (0x080)        /* ADC12 Multiple SampleConversion */
#define SHT00                  (0x0100)       /* ADC12 Sample Hold 0 Select 0 */
#define SHT01                  (0x0200)       /* ADC12 Sample Hold 0 Select 1 */
#define SHT02                  (0x0400)       /* ADC12 Sample Hold 0 Select 2 */
#define SHT03                  (0x0800)       /* ADC12 Sample Hold 0 Select 3 */
#define SHT10                  (0x1000)       /* ADC12 Sample Hold 0 Select 0 */
#define SHT11                  (0x2000)       /* ADC12 Sample Hold 1 Select 1 */
#define SHT12                  (0x4000)       /* ADC12 Sample Hold 2 Select 2 */
#define SHT13                  (0x8000)       /* ADC12 Sample Hold 3 Select 3 */
#define MSH                    (0x080)

#define SHT0_0                 (0x0000)       /* ADC12 Sample Hold 0 Select Bit: 0 */
#define SHT0_1                 (0x0100)       /* ADC12 Sample Hold 0 Select Bit: 1 */
#define SHT0_2                 (0x0200)       /* ADC12 Sample Hold 0 Select Bit: 2 */
#define SHT0_3                 (0x0300)       /* ADC12 Sample Hold 0 Select Bit: 3 */
#define SHT0_4                 (0x0400)       /* ADC12 Sample Hold 0 Select Bit: 4 */
#define SHT0_5                 (0x0500)       /* ADC12 Sample Hold 0 Select Bit: 5 */
#define SHT0_6                 (0x0600)       /* ADC12 Sample Hold 0 Select Bit: 6 */
#define SHT0_7                 (0x0700)       /* ADC12 Sample Hold 0 Select Bit: 7 */
#define SHT0_8                 (0x0800)       /* ADC12 Sample Hold 0 Select Bit: 8 */
#define SHT0_9                 (0x0900)       /* ADC12 Sample Hold 0 Select Bit: 9 */
#define SHT0_10                (0x0A00)       /* ADC12 Sample Hold 0 Select Bit: 10 */
#define SHT0_11                (0x0B00)       /* ADC12 Sample Hold 0 Select Bit: 11 */
#define SHT0_12                (0x0C00)       /* ADC12 Sample Hold 0 Select Bit: 12 */
#define SHT0_13                (0x0D00)       /* ADC12 Sample Hold 0 Select Bit: 13 */
#define SHT0_14                (0x0E00)       /* ADC12 Sample Hold 0 Select Bit: 14 */
#define SHT0_15                (0x0F00)       /* ADC12 Sample Hold 0 Select Bit: 15 */

#define SHT1_0                 (0x0000)       /* ADC12 Sample Hold 1 Select Bit: 0 */
#define SHT1_1                 (0x1000)       /* ADC12 Sample Hold 1 Select Bit: 1 */
#define SHT1_2                 (0x2000)       /* ADC12 Sample Hold 1 Select Bit: 2 */
#define SHT1_3                 (0x3000)       /* ADC12 Sample Hold 1 Select Bit: 3 */
#define SHT1_4                 (0x4000)       /* ADC12 Sample Hold 1 Select Bit: 4 */
#define SHT1_5                 (0x5000)       /* ADC12 Sample Hold 1 Select Bit: 5 */
#define SHT1_6                 (0x6000)       /* ADC12 Sample Hold 1 Select Bit: 6 */
#define SHT1_7                 (0x7000)       /* ADC12 Sample Hold 1 Select Bit: 7 */
#define SHT1_8                 (0x8000)       /* ADC12 Sample Hold 1 Select Bit: 8 */
#define SHT1_9                 (0x9000)       /* ADC12 Sample Hold 1 Select Bit: 9 */
#define SHT1_10                (0xA000)       /* ADC12 Sample Hold 1 Select Bit: 10 */
#define SHT1_11                (0xB000)       /* ADC12 Sample Hold 1 Select Bit: 11 */
#define SHT1_12                (0xC000)       /* ADC12 Sample Hold 1 Select Bit: 12 */
#define SHT1_13                (0xD000)       /* ADC12 Sample Hold 1 Select Bit: 13 */
#define SHT1_14                (0xE000)       /* ADC12 Sample Hold 1 Select Bit: 14 */
#define SHT1_15                (0xF000)       /* ADC12 Sample Hold 1 Select Bit: 15 */

/* ADC12CTL1 */
#define ADC12BUSY              (0x0001)       /* ADC12 Busy */
#define CONSEQ0                (0x0002)       /* ADC12 Conversion Sequence Select 0 */
#define CONSEQ1                (0x0004)       /* ADC12 Conversion Sequence Select 1 */
#define ADC12SSEL0             (0x0008)       /* ADC12 Clock Source Select 0 */
#define ADC12SSEL1             (0x0010)       /* ADC12 Clock Source Select 1 */
#define ADC12DIV0              (0x0020)       /* ADC12 Clock Divider Select 0 */
#define ADC12DIV1              (0x0040)       /* ADC12 Clock Divider Select 1 */
#define ADC12DIV2              (0x0080)       /* ADC12 Clock Divider Select 2 */
#define ISSH                   (0x0100)       /* ADC12 Invert Sample Hold Signal */
#define SHP                    (0x0200)       /* ADC12 Sample/Hold Pulse Mode */
#define SHS0                   (0x0400)       /* ADC12 Sample/Hold Source 0 */
#define SHS1                   (0x0800)       /* ADC12 Sample/Hold Source 1 */
#define CSTARTADD0             (0x1000)       /* ADC12 Conversion Start Address 0 */
#define CSTARTADD1             (0x2000)       /* ADC12 Conversion Start Address 1 */
#define CSTARTADD2             (0x4000)       /* ADC12 Conversion Start Address 2 */
#define CSTARTADD3             (0x8000)       /* ADC12 Conversion Start Address 3 */

#define CONSEQ_0               (0x0000)       /* ADC12 Conversion Sequence Select: 0 */
#define CONSEQ_1               (0x0002)       /* ADC12 Conversion Sequence Select: 1 */
#define CONSEQ_2               (0x0004)       /* ADC12 Conversion Sequence Select: 2 */
#define CONSEQ_3               (0x0006)       /* ADC12 Conversion Sequence Select: 3 */
#define ADC12SSEL_0            (0x0000)       /* ADC12 Clock Source Select: 0 */
#define ADC12SSEL_1            (0x0008)       /* ADC12 Clock Source Select: 1 */
#define ADC12SSEL_2            (0x0010)       /* ADC12 Clock Source Select: 2 */
#define ADC12SSEL_3            (0x0018)       /* ADC12 Clock Source Select: 3 */
#define ADC12DIV_0             (0x0000)       /* ADC12 Clock Divider Select: 0 */
#define ADC12DIV_1             (0x0020)       /* ADC12 Clock Divider Select: 1 */
#define ADC12DIV_2             (0x0040)       /* ADC12 Clock Divider Select: 2 */
#define ADC12DIV_3             (0x0060)       /* ADC12 Clock Divider Select: 3 */
#define ADC12DIV_4             (0x0080)       /* ADC12 Clock Divider Select: 4 */
#define ADC12DIV_5             (0x00A0)       /* ADC12 Clock Divider Select: 5 */
#define ADC12DIV_6             (0x00C0)       /* ADC12 Clock Divider Select: 6 */
#define ADC12DIV_7             (0x00E0)       /* ADC12 Clock Divider Select: 7 */
#define SHS_0                  (0x0000)       /* ADC12 Sample/Hold Source: 0 */
#define SHS_1                  (0x0400)       /* ADC12 Sample/Hold Source: 1 */
#define SHS_2                  (0x0800)       /* ADC12 Sample/Hold Source: 2 */
#define SHS_3                  (0x0C00)       /* ADC12 Sample/Hold Source: 3 */
#define CSTARTADD_0            (0x0000)       /* ADC12 Conversion Start Address: 0 */
#define CSTARTADD_1            (0x1000)       /* ADC12 Conversion Start Address: 1 */
#define CSTARTADD_2            (0x2000)       /* ADC12 Conversion Start Address: 2 */
#define CSTARTADD_3            (0x3000)       /* ADC12 Conversion Start Address: 3 */
#define CSTARTADD_4            (0x4000)       /* ADC12 Conversion Start Address: 4 */
#define CSTARTADD_5            (0x5000)       /* ADC12 Conversion Start Address: 5 */
#define CSTARTADD_6            (0x6000)       /* ADC12 Conversion Start Address: 6 */
#define CSTARTADD_7            (0x7000)       /* ADC12 Conversion Start Address: 7 */
#define CSTARTADD_8            (0x8000)       /* ADC12 Conversion Start Address: 8 */
#define CSTARTADD_9            (0x9000)       /* ADC12 Conversion Start Address: 9 */
#define CSTARTADD_10           (0xA000)       /* ADC12 Conversion Start Address: 10 */
#define CSTARTADD_11           (0xB000)       /* ADC12 Conversion Start Address: 11 */
#define CSTARTADD_12           (0xC000)       /* ADC12 Conversion Start Address: 12 */
#define CSTARTADD_13           (0xD000)       /* ADC12 Conversion Start Address: 13 */
#define CSTARTADD_14           (0xE000)       /* ADC12 Conversion Start Address: 14 */
#define CSTARTADD_15           (0xF000)       /* ADC12 Conversion Start Address: 15 */

/* ADC12MCTLx */
#define INCH0                  (0x0001)       /* ADC12 Input Channel Select Bit 0 */
#define INCH1                  (0x0002)       /* ADC12 Input Channel Select Bit 1 */
#define INCH2                  (0x0004)       /* ADC12 Input Channel Select Bit 2 */
#define INCH3                  (0x0008)       /* ADC12 Input Channel Select Bit 3 */
#define SREF0                  (0x0010)       /* ADC12 Select Reference Bit 0 */
#define SREF1                  (0x0020)       /* ADC12 Select Reference Bit 1 */
#define SREF2                  (0x0040)       /* ADC12 Select Reference Bit 2 */
#define EOS                    (0x0080)       /* ADC12 End of Sequence */

#define INCH_0                 (0)            /* ADC12 Input Channel 0 */
#define INCH_1                 (1)            /* ADC12 Input Channel 1 */
#define INCH_2                 (2)            /* ADC12 Input Channel 2 */
#define INCH_3                 (3)            /* ADC12 Input Channel 3 */
#define INCH_4                 (4)            /* ADC12 Input Channel 4 */
#define INCH_5                 (5)            /* ADC12 Input Channel 5 */
#define INCH_6                 (6)            /* ADC12 Input Channel 6 */
#define INCH_7                 (7)            /* ADC12 Input Channel 7 */
#define INCH_8                 (8)            /* ADC12 Input Channel 8 */
#define INCH_9                 (9)            /* ADC12 Input Channel 9 */
#define INCH_10                (10)           /* ADC12 Input Channel 10 */
#define INCH_11                (11)           /* ADC12 Input Channel 11 */
#define INCH_12                (12)           /* ADC12 Input Channel 12 */
#define INCH_13                (13)           /* ADC12 Input Channel 13 */
#define INCH_14                (14)           /* ADC12 Input Channel 14 */
#define INCH_15                (15)           /* ADC12 Input Channel 15 */

#define SREF_0                 (0x0000)       /* ADC12 Select Reference 0 */
#define SREF_1                 (0x0010)       /* ADC12 Select Reference 1 */
#define SREF_2                 (0x0020)       /* ADC12 Select Reference 2 */
#define SREF_3                 (0x0030)       /* ADC12 Select Reference 3 */
#define SREF_4                 (0x0040)       /* ADC12 Select Reference 4 */
#define SREF_5                 (0x0050)       /* ADC12 Select Reference 5 */
#define SREF_6                 (0x0060)       /* ADC12 Select Reference 6 */
#define SREF_7                 (0x0070)       /* ADC12 Select Reference 7 */

/* ADC12IV Definitions */
#define ADC12IV_NONE           (0x0000)       /* No Interrupt pending */
#define ADC12IV_ADC12OVIFG     (0x0002)       /* ADC12OVIFG */
#define ADC12IV_ADC12TOVIFG    (0x0004)       /* ADC12TOVIFG */
#define ADC12IV_ADC12IFG0      (0x0006)       /* ADC12IFG0 */
#define ADC12IV_ADC12IFG1      (0x0008)       /* ADC12IFG1 */
#define ADC12IV_ADC12IFG2      (0x000A)       /* ADC12IFG2 */
#define ADC12IV_ADC12IFG3      (0x000C)       /* ADC12IFG3 */
#define ADC12IV_ADC12IFG4      (0x000E)       /* ADC12IFG4 */
#define ADC12IV_ADC12IFG5      (0x0010)       /* ADC12IFG5 */
#define ADC12IV_ADC12IFG6      (0x0012)       /* ADC12IFG6 */
#define ADC12IV_ADC12IFG7      (0x0014)       /* ADC12IFG7 */
#define ADC12IV_ADC12IFG8      (0x0016)       /* ADC12IFG8 */
#define ADC12IV_ADC12IFG9      (0x0018)       /* ADC12IFG9 */
#define ADC12IV_ADC12IFG10     (0x001A)       /* ADC12IFG10 */
#define ADC12IV_ADC12IFG11     (0x001C)       /* ADC12IFG11 */
#define ADC12IV_ADC12IFG12     (0x001E)       /* ADC12IFG12 */
#define ADC12IV_ADC12IFG13     (0x0020)       /* ADC12IFG13 */
#define ADC12IV_ADC12IFG14     (0x0022)       /* ADC12IFG14 */
#define ADC12IV_ADC12IFG15     (0x0024)       /* ADC12IFG15 */

/************************************************************
* Basic Clock Module
************************************************************/
#define __MSP430_HAS_BC2__                    /* Definition to show that Module is available */

sfr_b(DCOCTL);                                /* DCO Clock Frequency Control */
sfr_b(BCSCTL1);                               /* Basic Clock System Control 1 */
sfr_b(BCSCTL2);                               /* Basic Clock System Control 2 */
sfr_b(BCSCTL3);                               /* Basic Clock System Control 3 */

#define MOD0                   (0x01)         /* Modulation Bit 0 */
#define MOD1                   (0x02)         /* Modulation Bit 1 */
#define MOD2                   (0x04)         /* Modulation Bit 2 */
#define MOD3                   (0x08)         /* Modulation Bit 3 */
#define MOD4                   (0x10)         /* Modulation Bit 4 */
#define DCO0                   (0x20)         /* DCO Select Bit 0 */
#define DCO1                   (0x40)         /* DCO Select Bit 1 */
#define DCO2                   (0x80)         /* DCO Select Bit 2 */

#define RSEL0                  (0x01)         /* Range Select Bit 0 */
#define RSEL1                  (0x02)         /* Range Select Bit 1 */
#define RSEL2                  (0x04)         /* Range Select Bit 2 */
#define RSEL3                  (0x08)         /* Range Select Bit 3 */
#define DIVA0                  (0x10)         /* ACLK Divider 0 */
#define DIVA1                  (0x20)         /* ACLK Divider 1 */
#define XTS                    (0x40)         /* LFXTCLK 0:Low Freq. / 1: High Freq. */
#define XT2OFF                 (0x80)         /* Enable XT2CLK */

#define DIVA_0                 (0x00)         /* ACLK Divider 0: /1 */
#define DIVA_1                 (0x10)         /* ACLK Divider 1: /2 */
#define DIVA_2                 (0x20)         /* ACLK Divider 2: /4 */
#define DIVA_3                 (0x30)         /* ACLK Divider 3: /8 */

#define DCOR                   (0x01)         /* Enable External Resistor : 1 */
#define DIVS0                  (0x02)         /* SMCLK Divider 0 */
#define DIVS1                  (0x04)         /* SMCLK Divider 1 */
#define SELS                   (0x08)         /* SMCLK Source Select 0:DCOCLK / 1:XT2CLK/LFXTCLK */
#define DIVM0                  (0x10)         /* MCLK Divider 0 */
#define DIVM1                  (0x20)         /* MCLK Divider 1 */
#define SELM0                  (0x40)         /* MCLK Source Select 0 */
#define SELM1                  (0x80)         /* MCLK Source Select 1 */

#define DIVS_0                 (0x00)         /* SMCLK Divider 0: /1 */
#define DIVS_1                 (0x02)         /* SMCLK Divider 1: /2 */
#define DIVS_2                 (0x04)         /* SMCLK Divider 2: /4 */
#define DIVS_3                 (0x06)         /* SMCLK Divider 3: /8 */

#define DIVM_0                 (0x00)         /* MCLK Divider 0: /1 */
#define DIVM_1                 (0x10)         /* MCLK Divider 1: /2 */
#define DIVM_2                 (0x20)         /* MCLK Divider 2: /4 */
#define DIVM_3                 (0x30)         /* MCLK Divider 3: /8 */

#define SELM_0                 (0x00)         /* MCLK Source Select 0: DCOCLK */
#define SELM_1                 (0x40)         /* MCLK Source Select 1: DCOCLK */
#define SELM_2                 (0x80)         /* MCLK Source Select 2: XT2CLK/LFXTCLK */
#define SELM_3                 (0xC0)         /* MCLK Source Select 3: LFXTCLK */

#define LFXT1OF                (0x01)         /* Low/high Frequency Oscillator Fault Flag */
#define XT2OF                  (0x02)         /* High frequency oscillator 2 fault flag */
#define XCAP0                  (0x04)         /* XIN/XOUT Cap 0 */
#define XCAP1                  (0x08)         /* XIN/XOUT Cap 1 */
#define LFXT1S0                (0x10)         /* Mode 0 for LFXT1 (XTS = 0) */
#define LFXT1S1                (0x20)         /* Mode 1 for LFXT1 (XTS = 0) */
#define XT2S0                  (0x40)         /* Mode 0 for XT2 */
#define XT2S1                  (0x80)         /* Mode 1 for XT2 */

#define XCAP_0                 (0x00)         /* XIN/XOUT Cap : 0 pF */
#define XCAP_1                 (0x04)         /* XIN/XOUT Cap : 6 pF */
#define XCAP_2                 (0x08)         /* XIN/XOUT Cap : 10 pF */
#define XCAP_3                 (0x0C)         /* XIN/XOUT Cap : 12.5 pF */

#define LFXT1S_0               (0x00)         /* Mode 0 for LFXT1 : Normal operation */
#define LFXT1S_1               (0x10)         /* Mode 1 for LFXT1 : Reserved */
#define LFXT1S_2               (0x20)         /* Mode 2 for LFXT1 : VLO */
#define LFXT1S_3               (0x30)         /* Mode 3 for LFXT1 : Digital input signal */

#define XT2S_0                 (0x00)         /* Mode 0 for XT2 : 0.4 - 1 MHz */
#define XT2S_1                 (0x40)         /* Mode 1 for XT2 : 1 - 4 MHz */
#define XT2S_2                 (0x80)         /* Mode 2 for XT2 : 2 - 16 MHz */
#define XT2S_3                 (0xC0)         /* Mode 3 for XT2 : Digital input signal */

/************************************************************
* Comparator A
************************************************************/
#define __MSP430_HAS_CAPLUS__                 /* Definition to show that Module is available */

sfr_b(CACTL1);                                /* Comparator A Control 1 */
sfr_b(CACTL2);                                /* Comparator A Control 2 */
sfr_b(CAPD);                                  /* Comparator A Port Disable */

#define CAIFG                  (0x01)         /* Comp. A Interrupt Flag */
#define CAIE                   (0x02)         /* Comp. A Interrupt Enable */
#define CAIES                  (0x04)         /* Comp. A Int. Edge Select: 0:rising / 1:falling */
#define CAON                   (0x08)         /* Comp. A enable */
#define CAREF0                 (0x10)         /* Comp. A Internal Reference Select 0 */
#define CAREF1                 (0x20)         /* Comp. A Internal Reference Select 1 */
#define CARSEL                 (0x40)         /* Comp. A Internal Reference Enable */
#define CAEX                   (0x80)         /* Comp. A Exchange Inputs */

#define CAREF_0                (0x00)         /* Comp. A Int. Ref. Select 0 : Off */
#define CAREF_1                (0x10)         /* Comp. A Int. Ref. Select 1 : 0.25*Vcc */
#define CAREF_2                (0x20)         /* Comp. A Int. Ref. Select 2 : 0.5*Vcc */
#define CAREF_3                (0x30)         /* Comp. A Int. Ref. Select 3 : Vt*/

#define CAOUT                  (0x01)         /* Comp. A Output */
#define CAF                    (0x02)         /* Comp. A Enable Output Filter */
#define P2CA0                  (0x04)         /* Comp. A +Terminal Multiplexer */
#define P2CA1                  (0x08)         /* Comp. A -Terminal Multiplexer */
#define P2CA2                  (0x10)         /* Comp. A -Terminal Multiplexer */
#define P2CA3                  (0x20)         /* Comp. A -Terminal Multiplexer */
#define P2CA4                  (0x40)         /* Comp. A +Terminal Multiplexer */
#define CASHORT                (0x80)         /* Comp. A Short + and - Terminals */

#define CAPD0                  (0x01)         /* Comp. A Disable Input Buffer of Port Register .0 */
#define CAPD1                  (0x02)         /* Comp. A Disable Input Buffer of Port Register .1 */
#define CAPD2                  (0x04)         /* Comp. A Disable Input Buffer of Port Register .2 */
#define CAPD3                  (0x08)         /* Comp. A Disable Input Buffer of Port Register .3 */
#define CAPD4                  (0x10)         /* Comp. A Disable Input Buffer of Port Register .4 */
#define CAPD5                  (0x20)         /* Comp. A Disable Input Buffer of Port Register .5 */
#define CAPD6                  (0x40)         /* Comp. A Disable Input Buffer of Port Register .6 */
#define CAPD7                  (0x80)         /* Comp. A Disable Input Buffer of Port Register .7 */

/************************************************************
* DAC12
************************************************************/
#define __MSP430_HAS_DAC12_2__                /* Definition to show that Module is available */

sfr_w(DAC12_0CTL);                            /* DAC12_0 Control */
sfr_w(DAC12_1CTL);                            /* DAC12_1 Control */

#define DAC12GRP               (0x0001)       /* DAC12 group */
#define DAC12ENC               (0x0002)       /* DAC12 enable conversion */
#define DAC12IFG               (0x0004)       /* DAC12 interrupt flag */
#define DAC12IE                (0x0008)       /* DAC12 interrupt enable */
#define DAC12DF                (0x0010)       /* DAC12 data format */
#define DAC12AMP0              (0x0020)       /* DAC12 amplifier bit 0 */
#define DAC12AMP1              (0x0040)       /* DAC12 amplifier bit 1 */
#define DAC12AMP2              (0x0080)       /* DAC12 amplifier bit 2 */
#define DAC12IR                (0x0100)       /* DAC12 input reference and output range */
#define DAC12CALON             (0x0200)       /* DAC12 calibration */
#define DAC12LSEL0             (0x0400)       /* DAC12 load select bit 0 */
#define DAC12LSEL1             (0x0800)       /* DAC12 load select bit 1 */
#define DAC12RES               (0x1000)       /* DAC12 resolution */
#define DAC12SREF0             (0x2000)       /* DAC12 reference bit 0 */
#define DAC12SREF1             (0x4000)       /* DAC12 reference bit 1 */
#define DAC12OPS               (0x8000)       /* DAC12 Operation Amp. */

#define DAC12AMP_0             (0x0000)       /* DAC12 amplifier 0: off,    3-state */
#define DAC12AMP_1             (0x0020)       /* DAC12 amplifier 1: off,    off */
#define DAC12AMP_2             (0x0040)       /* DAC12 amplifier 2: low,    low */
#define DAC12AMP_3             (0x0060)       /* DAC12 amplifier 3: low,    medium */
#define DAC12AMP_4             (0x0080)       /* DAC12 amplifier 4: low,    high */
#define DAC12AMP_5             (0x00A0)       /* DAC12 amplifier 5: medium, medium */
#define DAC12AMP_6             (0x00C0)       /* DAC12 amplifier 6: medium, high */
#define DAC12AMP_7             (0x00E0)       /* DAC12 amplifier 7: high,   high */

#define DAC12LSEL_0            (0x0000)       /* DAC12 load select 0: direct */
#define DAC12LSEL_1            (0x0400)       /* DAC12 load select 1: latched with DAT */
#define DAC12LSEL_2            (0x0800)       /* DAC12 load select 2: latched with pos. Timer_A3.OUT1 */
#define DAC12LSEL_3            (0x0C00)       /* DAC12 load select 3: latched with pos. Timer_B7.OUT1 */

#define DAC12SREF_0            (0x0000)       /* DAC12 reference 0: Vref+ */
#define DAC12SREF_1            (0x2000)       /* DAC12 reference 1: Vref+ */
#define DAC12SREF_2            (0x4000)       /* DAC12 reference 2: Veref+ */
#define DAC12SREF_3            (0x6000)       /* DAC12 reference 3: Veref+ */

sfr_w(DAC12_0DAT);                            /* DAC12_0 Data */
sfr_w(DAC12_1DAT);                            /* DAC12_1 Data */
/************************************************************
* DMA_X
************************************************************/
#define __MSP430_HAS_DMAX_3__                 /* Definition to show that Module is available */

sfr_w(DMACTL0);                               /* DMA Module Control 0 */
#define DMA0TSEL0              (0x0001)       /* DMA channel 0 transfer select bit 0 */
#define DMA0TSEL1              (0x0002)       /* DMA channel 0 transfer select bit 1 */
#define DMA0TSEL2              (0x0004)       /* DMA channel 0 transfer select bit 2 */
#define DMA0TSEL3              (0x0008)       /* DMA channel 0 transfer select bit 3 */
#define DMA1TSEL0              (0x0010)       /* DMA channel 1 transfer select bit 0 */
#define DMA1TSEL1              (0x0020)       /* DMA channel 1 transfer select bit 1 */
#define DMA1TSEL2              (0x0040)       /* DMA channel 1 transfer select bit 2 */
#define DMA1TSEL3              (0x0080)       /* DMA channel 1 transfer select bit 3 */
#define DMA2TSEL0              (0x0100)       /* DMA channel 2 transfer select bit 0 */
#define DMA2TSEL1              (0x0200)       /* DMA channel 2 transfer select bit 1 */
#define DMA2TSEL2              (0x0400)       /* DMA channel 2 transfer select bit 2 */
#define DMA2TSEL3              (0x0800)       /* DMA channel 2 transfer select bit 3 */

#define DMA0TSEL_0             (0x0000)       /* DMA channel 0 transfer select 0:  DMA_REQ (sw)*/
#define DMA0TSEL_1             (0x0001)       /* DMA channel 0 transfer select 1:  Timer_A (TACCR2.IFG) */
#define DMA0TSEL_2             (0x0002)       /* DMA channel 0 transfer select 2:  Timer_B (TBCCR2.IFG) */
#define DMA0TSEL_3             (0x0003)       /* DMA channel 0 transfer select 3:  USCIA0 receive */
#define DMA0TSEL_4             (0x0004)       /* DMA channel 0 transfer select 4:  USCIA0 transmit */
#define DMA0TSEL_5             (0x0005)       /* DMA channel 0 transfer select 5:  DAC12_0CTL.DAC12IFG */
#define DMA0TSEL_6             (0x0006)       /* DMA channel 0 transfer select 6:  ADC12 (ADC12IFG) */
#define DMA0TSEL_7             (0x0007)       /* DMA channel 0 transfer select 7:  Timer_A (TACCR0.IFG) */
#define DMA0TSEL_8             (0x0008)       /* DMA channel 0 transfer select 8:  Timer_B (TBCCR0.IFG) */
#define DMA0TSEL_9             (0x0009)       /* DMA channel 0 transfer select 9:  USCIA1 receive */
#define DMA0TSEL_10            (0x000A)       /* DMA channel 0 transfer select 10: USCIA1 transmit */
#define DMA0TSEL_11            (0x000B)       /* DMA channel 0 transfer select 11: Multiplier ready */
#define DMA0TSEL_12            (0x000C)       /* DMA channel 0 transfer select 12: USCIB0 receive */
#define DMA0TSEL_13            (0x000D)       /* DMA channel 0 transfer select 13: USCIB0 transmit */
#define DMA0TSEL_14            (0x000E)       /* DMA channel 0 transfer select 14: previous DMA channel DMA2IFG */
#define DMA0TSEL_15            (0x000F)       /* DMA channel 0 transfer select 15: ext. Trigger (DMAE0) */

#define DMA1TSEL_0             (0x0000)       /* DMA channel 1 transfer select 0:  DMA_REQ */
#define DMA1TSEL_1             (0x0010)       /* DMA channel 1 transfer select 1:  Timer_A CCRIFG.2 */
#define DMA1TSEL_2             (0x0020)       /* DMA channel 1 transfer select 2:  Timer_B CCRIFG.2 */
#define DMA1TSEL_3             (0x0030)       /* DMA channel 1 transfer select 3:  USCIA0 receive */
#define DMA1TSEL_4             (0x0040)       /* DMA channel 1 transfer select 4:  USCIA0 transmit */
#define DMA1TSEL_5             (0x0050)       /* DMA channel 1 transfer select 5:  DAC12.0IFG */
#define DMA1TSEL_6             (0x0060)       /* DMA channel 1 transfer select 6:  ADC12 (ADC12IFG) */
#define DMA1TSEL_7             (0x0070)       /* DMA channel 1 transfer select 7:  Timer_A (TACCR0.IFG) */
#define DMA1TSEL_8             (0x0080)       /* DMA channel 1 transfer select 8:  Timer_B (TBCCR0.IFG) */
#define DMA1TSEL_9             (0x0090)       /* DMA channel 1 transfer select 9:  USCIA1 receive */
#define DMA1TSEL_10            (0x00A0)       /* DMA channel 1 transfer select 10: USCIA1 transmit */
#define DMA1TSEL_11            (0x00B0)       /* DMA channel 1 transfer select 11: Multiplier ready */
#define DMA1TSEL_12            (0x00C0)       /* DMA channel 1 transfer select 12: USCIB0 receive */
#define DMA1TSEL_13            (0x00D0)       /* DMA channel 1 transfer select 13: USCIB0 transmit */
#define DMA1TSEL_14            (0x00E0)       /* DMA channel 1 transfer select 14: previous DMA channel DMA0IFG */
#define DMA1TSEL_15            (0x00F0)       /* DMA channel 1 transfer select 15: ext. Trigger (DMAE0) */

#define DMA2TSEL_0             (0x0000)       /* DMA channel 2 transfer select 0:  DMA_REQ */
#define DMA2TSEL_1             (0x0100)       /* DMA channel 2 transfer select 1:  Timer_A CCRIFG.2 */
#define DMA2TSEL_2             (0x0200)       /* DMA channel 2 transfer select 2:  Timer_B CCRIFG.2 */
#define DMA2TSEL_3             (0x0300)       /* DMA channel 2 transfer select 3:  USCIA0 receive */
#define DMA2TSEL_4             (0x0400)       /* DMA channel 2 transfer select 4:  USCIA0 transmit */
#define DMA2TSEL_5             (0x0500)       /* DMA channel 2 transfer select 5:  DAC12.0IFG */
#define DMA2TSEL_6             (0x0600)       /* DMA channel 2 transfer select 6:  ADC12 (ADC12IFG) */
#define DMA2TSEL_7             (0x0700)       /* DMA channel 2 transfer select 7:  Timer_A (TACCR0.IFG) */
#define DMA2TSEL_8             (0x0800)       /* DMA channel 2 transfer select 8:  Timer_B (TBCCR0.IFG) */
#define DMA2TSEL_9             (0x0900)       /* DMA channel 2 transfer select 9:  USCIA1 receive */
#define DMA2TSEL_10            (0x0A00)       /* DMA channel 2 transfer select 10: USCIA1 transmit */
#define DMA2TSEL_11            (0x0B00)       /* DMA channel 2 transfer select 11: Multiplier ready */
#define DMA2TSEL_12            (0x0C00)       /* DMA channel 2 transfer select 12: USCIB0 receive */
#define DMA2TSEL_13            (0x0D00)       /* DMA channel 2 transfer select 13: USCIB0 transmit */
#define DMA2TSEL_14            (0x0E00)       /* DMA channel 2 transfer select 14: previous DMA channel DMA1IFG */
#define DMA2TSEL_15            (0x0F00)       /* DMA channel 2 transfer select 15: ext. Trigger (DMAE0) */

sfr_w(DMACTL1);                               /* DMA Module Control 1 */
#define ENNMI                  (0x0001)       /* Enable NMI interruption of DMA */
#define ROUNDROBIN             (0x0002)       /* Round-Robin DMA channel priorities */
#define DMAONFETCH             (0x0004)       /* DMA transfer on instruction fetch */

sfr_w(DMAIV);                                 /* DMA Interrupt Vector Word */
sfr_w(DMA0CTL);                               /* DMA Channel 0 Control */
sfr_w(DMA1CTL);                               /* DMA Channel 1 Control */
sfr_w(DMA2CTL);                               /* DMA Channel 2 Control */

#define DMAREQ                 (0x0001)       /* Initiate DMA transfer with DMATSEL */
#define DMAABORT               (0x0002)       /* DMA transfer aborted by NMI */
#define DMAIE                  (0x0004)       /* DMA interrupt enable */
#define DMAIFG                 (0x0008)       /* DMA interrupt flag */
#define DMAEN                  (0x0010)       /* DMA enable */
#define DMALEVEL               (0x0020)       /* DMA level sensitive trigger select */
#define DMASRCBYTE             (0x0040)       /* DMA source byte */
#define DMADSTBYTE             (0x0080)       /* DMA destination byte */
#define DMASRCINCR0            (0x0100)       /* DMA source increment bit 0 */
#define DMASRCINCR1            (0x0200)       /* DMA source increment bit 1 */
#define DMADSTINCR0            (0x0400)       /* DMA destination increment bit 0 */
#define DMADSTINCR1            (0x0800)       /* DMA destination increment bit 1 */
#define DMADT0                 (0x1000)       /* DMA transfer mode bit 0 */
#define DMADT1                 (0x2000)       /* DMA transfer mode bit 1 */
#define DMADT2                 (0x4000)       /* DMA transfer mode bit 2 */

#define DMASWDW                (0x0000)       /* DMA transfer: source word to destination word */
#define DMASBDW                (0x0040)       /* DMA transfer: source byte to destination word */
#define DMASWDB                (0x0080)       /* DMA transfer: source word to destination byte */
#define DMASBDB                (0x00C0)       /* DMA transfer: source byte to destination byte */

#define DMASRCINCR_0           (0x0000)       /* DMA source increment 0: source address unchanged */
#define DMASRCINCR_1           (0x0100)       /* DMA source increment 1: source address unchanged */
#define DMASRCINCR_2           (0x0200)       /* DMA source increment 2: source address decremented */
#define DMASRCINCR_3           (0x0300)       /* DMA source increment 3: source address incremented */

#define DMADSTINCR_0           (0x0000)       /* DMA destination increment 0: destination address unchanged */
#define DMADSTINCR_1           (0x0400)       /* DMA destination increment 1: destination address unchanged */
#define DMADSTINCR_2           (0x0800)       /* DMA destination increment 2: destination address decremented */
#define DMADSTINCR_3           (0x0C00)       /* DMA destination increment 3: destination address incremented */

#define DMADT_0                (0x0000)       /* DMA transfer mode 0: single */
#define DMADT_1                (0x1000)       /* DMA transfer mode 1: block */
#define DMADT_2                (0x2000)       /* DMA transfer mode 2: interleaved */
#define DMADT_3                (0x3000)       /* DMA transfer mode 3: interleaved */
#define DMADT_4                (0x4000)       /* DMA transfer mode 4: single, repeat */
#define DMADT_5                (0x5000)       /* DMA transfer mode 5: block, repeat */
#define DMADT_6                (0x6000)       /* DMA transfer mode 6: interleaved, repeat */
#define DMADT_7                (0x7000)       /* DMA transfer mode 7: interleaved, repeat */

sfr_l(DMA0SA);                                /* DMA Channel 0 Source Address */
sfr_w(DMA0SAL);                               /* DMA Channel 0 Source Address */
sfr_l(DMA0DA);                                /* DMA Channel 0 Destination Address */
sfr_w(DMA0DAL);                               /* DMA Channel 0 Destination Address */
sfr_w(DMA0SZ);                                /* DMA Channel 0 Transfer Size */
sfr_l(DMA1SA);                                /* DMA Channel 1 Source Address */
sfr_w(DMA1SAL);                               /* DMA Channel 1 Source Address */
sfr_l(DMA1DA);                                /* DMA Channel 1 Destination Address */
sfr_w(DMA1DAL);                               /* DMA Channel 1 Destination Address */
sfr_w(DMA1SZ);                                /* DMA Channel 1 Transfer Size */
sfr_l(DMA2SA);                                /* DMA Channel 2 Source Address */
sfr_w(DMA2SAL);                               /* DMA Channel 2 Source Address */
sfr_l(DMA2DA);                                /* DMA Channel 2 Destination Address */
sfr_w(DMA2DAL);                               /* DMA Channel 2 Destination Address */
sfr_w(DMA2SZ);                                /* DMA Channel 2 Transfer Size */

/* DMAIV Definitions */
#define DMAIV_NONE             (0x0000)       /* No Interrupt pending */
#define DMAIV_DMA0IFG          (0x0002)       /* DMA0IFG */
#define DMAIV_DMA1IFG          (0x0004)       /* DMA1IFG */
#define DMAIV_DMA2IFG          (0x0006)       /* DMA2IFG */

/*************************************************************
* Flash Memory
*************************************************************/
#define __MSP430_HAS_FLASH2__                 /* Definition to show that Module is available */

sfr_w(FCTL1);                                 /* FLASH Control 1 */
sfr_w(FCTL2);                                 /* FLASH Control 2 */
sfr_w(FCTL3);                                 /* FLASH Control 3 */
sfr_w(FCTL4);                                 /* FLASH Control 4 */

#define FRKEY                  (0x9600)       /* Flash key returned by read */
#define FWKEY                  (0xA500)       /* Flash key for write */
#define FXKEY                  (0x3300)       /* for use with XOR instruction */

#define ERASE                  (0x0002)       /* Enable bit for Flash segment erase */
#define MERAS                  (0x0004)       /* Enable bit for Flash mass erase */
#define EEI                    (0x0008)       /* Enable Erase Interrupts */
#define EEIEX                  (0x0010)       /* Enable Emergency Interrupt Exit */
#define WRT                    (0x0040)       /* Enable bit for Flash write */
#define BLKWRT                 (0x0080)       /* Enable bit for Flash segment write */
#define SEGWRT                 (0x0080)       /* old definition */ /* Enable bit for Flash segment write */

#define FN0                    (0x0001)       /* Divide Flash clock by 1 to 64 using FN0 to FN5 according to: */
#define FN1                    (0x0002)       /*  32*FN5 + 16*FN4 + 8*FN3 + 4*FN2 + 2*FN1 + FN0 + 1 */
#ifndef FN2
#define FN2                    (0x0004)
#endif
#ifndef FN3
#define FN3                    (0x0008)
#endif
#ifndef FN4
#define FN4                    (0x0010)
#endif
#define FN5                    (0x0020)
#define FSSEL0                 (0x0040)       /* Flash clock select 0 */        /* to distinguish from USART SSELx */
#define FSSEL1                 (0x0080)       /* Flash clock select 1 */

#define FSSEL_0                (0x0000)       /* Flash clock select: 0 - ACLK */
#define FSSEL_1                (0x0040)       /* Flash clock select: 1 - MCLK */
#define FSSEL_2                (0x0080)       /* Flash clock select: 2 - SMCLK */
#define FSSEL_3                (0x00C0)       /* Flash clock select: 3 - SMCLK */

#define BUSY                   (0x0001)       /* Flash busy: 1 */
#define KEYV                   (0x0002)       /* Flash Key violation flag */
#define ACCVIFG                (0x0004)       /* Flash Access violation flag */
#define WAIT                   (0x0008)       /* Wait flag for segment write */
#define LOCK                   (0x0010)       /* Lock bit: 1 - Flash is locked (read only) */
#define EMEX                   (0x0020)       /* Flash Emergency Exit */
#define LOCKA                  (0x0040)       /* Segment A Lock bit: read = 1 - Segment is locked (read only) */
#define FAIL                   (0x0080)       /* Last Program or Erase failed */

#define MGR0                   (0x0010)       /* Marginal read 0 mode. */
#define MGR1                   (0x0020)       /* Marginal read 1 mode. */

/************************************************************
* HARDWARE MULTIPLIER
************************************************************/
#define __MSP430_HAS_MPY__                    /* Definition to show that Module is available */

sfr_w(MPY);                                   /* Multiply Unsigned/Operand 1 */
sfr_w(MPYS);                                  /* Multiply Signed/Operand 1 */
sfr_w(MAC);                                   /* Multiply Unsigned and Accumulate/Operand 1 */
sfr_w(MACS);                                  /* Multiply Signed and Accumulate/Operand 1 */
sfr_w(OP2);                                   /* Operand 2 */
sfr_w(RESLO);                                 /* Result Low Word */
sfr_w(RESHI);                                 /* Result High Word */
sfr_w(SUMEXT);                                /* Sum Extend */

/************************************************************
* DIGITAL I/O Port1/2 Pull up / Pull down Resistors
************************************************************/
#define __MSP430_HAS_PORT1_R__                /* Definition to show that Module is available */
#define __MSP430_HAS_PORT2_R__                /* Definition to show that Module is available */

#define __MSP430_HAS_P1SEL__                  /* Define for DriverLib */
#define __MSP430_HAS_P2SEL__                  /* Define for DriverLib */

sfr_b(P1IN);                                  /* Port 1 Input */
sfr_b(P1OUT);                                 /* Port 1 Output */
sfr_b(P1DIR);                                 /* Port 1 Direction */
sfr_b(P1IFG);                                 /* Port 1 Interrupt Flag */
sfr_b(P1IES);                                 /* Port 1 Interrupt Edge Select */
sfr_b(P1IE);                                  /* Port 1 Interrupt Enable */
sfr_b(P1SEL);                                 /* Port 1 Selection */
sfr_b(P1REN);                                 /* Port 1 Resistor Enable */

sfr_b(P2IN);                                  /* Port 2 Input */
sfr_b(P2OUT);                                 /* Port 2 Output */
sfr_b(P2DIR);                                 /* Port 2 Direction */
sfr_b(P2IFG);                                 /* Port 2 Interrupt Flag */
sfr_b(P2IES);                                 /* Port 2 Interrupt Edge Select */
sfr_b(P2IE);                                  /* Port 2 Interrupt Enable */
sfr_b(P2SEL);                                 /* Port 2 Selection */
sfr_b(P2REN);                                 /* Port 2 Resistor Enable */

/************************************************************
* DIGITAL I/O Port3/4 Pull up / Pull down Resistors
************************************************************/
#define __MSP430_HAS_PORT3_R__                /* Definition to show that Module is available */
#define __MSP430_HAS_PORT4_R__                /* Definition to show that Module is available */

#define __MSP430_HAS_P3SEL__                  /* Define for DriverLib */
#define __MSP430_HAS_P4SEL__                  /* Define for DriverLib */

sfr_b(P3IN);                                  /* Port 3 Input */
sfr_b(P3OUT);                                 /* Port 3 Output */
sfr_b(P3DIR);                                 /* Port 3 Direction */
sfr_b(P3SEL);                                 /* Port 3 Selection */
sfr_b(P3REN);                                 /* Port 3 Resistor Enable */

sfr_b(P4IN);                                  /* Port 4 Input */
sfr_b(P4OUT);                                 /* Port 4 Output */
sfr_b(P4DIR);                                 /* Port 4 Direction */
sfr_b(P4SEL);                                 /* Port 4 Selection */
sfr_b(P4REN);                                 /* Port 4 Resistor Enable */

/************************************************************
* DIGITAL I/O Port5/6 Pull up / Pull down Resistors
************************************************************/
#define __MSP430_HAS_PORT5_R__                /* Definition to show that Module is available */
#define __MSP430_HAS_PORT6_R__                /* Definition to show that Module is available */

#define __MSP430_HAS_P5SEL__                  /* Define for DriverLib */
#define __MSP430_HAS_P6SEL__                  /* Define for DriverLib */

sfr_b(P5IN);                                  /* Port 5 Input */
sfr_b(P5OUT);                                 /* Port 5 Output */
sfr_b(P5DIR);                                 /* Port 5 Direction */
sfr_b(P5SEL);                                 /* Port 5 Selection */
sfr_b(P5REN);                                 /* Port 5 Resistor Enable */

sfr_b(P6IN);                                  /* Port 6 Input */
sfr_b(P6OUT);                                 /* Port 6 Output */
sfr_b(P6DIR);                                 /* Port 6 Direction */
sfr_b(P6SEL);                                 /* Port 6 Selection */
sfr_b(P6REN);                                 /* Port 6 Resistor Enable */

/************************************************************
* DIGITAL I/O Port7/8 Pull up / Pull down Resistors
************************************************************/
#define __MSP430_HAS_PORT7_R__                /* Definition to show that Module is available */
#define __MSP430_HAS_PORT8_R__                /* Definition to show that Module is available */
#define __MSP430_HAS_PORTA_R__                /* Definition to show that Module is available */

#define __MSP430_HAS_P7SEL__                  /* Define for DriverLib */
#define __MSP430_HAS_P8SEL__                  /* Define for DriverLib */
#define __MSP430_HAS_PASEL__                  /* Define for DriverLib */

sfr_b(P7IN);                                  /* Port 7 Input */
sfr_b(P7OUT);                                 /* Port 7 Output */
sfr_b(P7DIR);                                 /* Port 7 Direction */
sfr_b(P7SEL);                                 /* Port 7 Selection */
sfr_b(P7REN);                                 /* Port 7 Resistor Enable */

sfr_b(P8IN);                                  /* Port 8 Input */
sfr_b(P8OUT);                                 /* Port 8 Output */
sfr_b(P8DIR);                                 /* Port 8 Direction */
sfr_b(P8SEL);                                 /* Port 8 Selection */
sfr_b(P8REN);                                 /* Port 8 Resistor Enable */

sfr_w(PAIN);                                  /* Port A Input */
sfr_w(PAOUT);                                 /* Port A Output */
sfr_w(PADIR);                                 /* Port A Direction */
sfr_w(PASEL);                                 /* Port A Selection */
sfr_w(PAREN);                                 /* Port A Resistor Enable */

/************************************************************
* Brown-Out, Supply Voltage Supervision (SVS)
************************************************************/
#define __MSP430_HAS_SVS__                    /* Definition to show that Module is available */

sfr_b(SVSCTL);                                /* SVS Control */
#define SVSFG                  (0x01)         /* SVS Flag */
#define SVSOP                  (0x02)         /* SVS output (read only) */
#define SVSON                  (0x04)         /* Switches the SVS on/off */
#define PORON                  (0x08)         /* Enable POR Generation if Low Voltage */
#define VLD0                   (0x10)
#define VLD1                   (0x20)
#define VLD2                   (0x40)
#define VLD3                   (0x80)

#define VLDON                  (0x10)
#define VLDOFF                 (0x00)
#define VLD_1_8V               (0x10)

/************************************************************
* Timer A3
************************************************************/
#define __MSP430_HAS_TA3__                    /* Definition to show that Module is available */

sfr_w(TAIV);                                  /* Timer A Interrupt Vector Word */
sfr_w(TACTL);                                 /* Timer A Control */
sfr_w(TACCTL0);                               /* Timer A Capture/Compare Control 0 */
sfr_w(TACCTL1);                               /* Timer A Capture/Compare Control 1 */
sfr_w(TACCTL2);                               /* Timer A Capture/Compare Control 2 */
sfr_w(TAR);                                   /* Timer A Counter Register */
sfr_w(TACCR0);                                /* Timer A Capture/Compare 0 */
sfr_w(TACCR1);                                /* Timer A Capture/Compare 1 */
sfr_w(TACCR2);                                /* Timer A Capture/Compare 2 */

/* Alternate register names */
#define CCTL0                  TACCTL0        /* Timer A Capture/Compare Control 0 */
#define CCTL1                  TACCTL1        /* Timer A Capture/Compare Control 1 */
#define CCTL2                  TACCTL2        /* Timer A Capture/Compare Control 2 */
#define CCR0                   TACCR0         /* Timer A Capture/Compare 0 */
#define CCR1                   TACCR1         /* Timer A Capture/Compare 1 */
#define CCR2                   TACCR2         /* Timer A Capture/Compare 2 */
#define CCTL0_                 TACCTL0_       /* Timer A Capture/Compare Control 0 */
#define CCTL1_                 TACCTL1_       /* Timer A Capture/Compare Control 1 */
#define CCTL2_                 TACCTL2_       /* Timer A Capture/Compare Control 2 */
#define CCR0_                  TACCR0_        /* Timer A Capture/Compare 0 */
#define CCR1_                  TACCR1_        /* Timer A Capture/Compare 1 */
#define CCR2_                  TACCR2_        /* Timer A Capture/Compare 2 */
/* Alternate register names - 5xx style */
#define TA0IV                  TAIV           /* Timer A Interrupt Vector Word */
#define TA0CTL                 TACTL          /* Timer A Control */
#define TA0CCTL0               TACCTL0        /* Timer A Capture/Compare Control 0 */
#define TA0CCTL1               TACCTL1        /* Timer A Capture/Compare Control 1 */
#define TA0CCTL2               TACCTL2        /* Timer A Capture/Compare Control 2 */
#define TA0R                   TAR            /* Timer A Counter Register */
#define TA0CCR0                TACCR0         /* Timer A Capture/Compare 0 */
#define TA0CCR1                TACCR1         /* Timer A Capture/Compare 1 */
#define TA0CCR2                TACCR2         /* Timer A Capture/Compare 2 */
#define TA0IV_                 TAIV_          /* Timer A Interrupt Vector Word */
#define TA0CTL_                TACTL_         /* Timer A Control */
#define TA0CCTL0_              TACCTL0_       /* Timer A Capture/Compare Control 0 */
#define TA0CCTL1_              TACCTL1_       /* Timer A Capture/Compare Control 1 */
#define TA0CCTL2_              TACCTL2_       /* Timer A Capture/Compare Control 2 */
#define TA0R_                  TAR_           /* Timer A Counter Register */
#define TA0CCR0_               TACCR0_        /* Timer A Capture/Compare 0 */
#define TA0CCR1_               TACCR1_        /* Timer A Capture/Compare 1 */
#define TA0CCR2_               TACCR2_        /* Timer A Capture/Compare 2 */
#define TIMER0_A1_VECTOR       TIMERA1_VECTOR /* Int. Vector: Timer A CC1-2, TA */
#define TIMER0_A0_VECTOR       TIMERA0_VECTOR /* Int. Vector: Timer A CC0 */

#define TASSEL1                (0x0200)       /* Timer A clock source select 1 */
#define TASSEL0                (0x0100)       /* Timer A clock source select 0 */
#define ID1                    (0x0080)       /* Timer A clock input divider 1 */
#define ID0                    (0x0040)       /* Timer A clock input divider 0 */
#define MC1                    (0x0020)       /* Timer A mode control 1 */
#define MC0                    (0x0010)       /* Timer A mode control 0 */
#define TACLR                  (0x0004)       /* Timer A counter clear */
#define TAIE                   (0x0002)       /* Timer A counter interrupt enable */
#define TAIFG                  (0x0001)       /* Timer A counter interrupt flag */

#define MC_0                   (0x0000)       /* Timer A mode control: 0 - Stop */
#define MC_1                   (0x0010)       /* Timer A mode control: 1 - Up to CCR0 */
#define MC_2                   (0x0020)       /* Timer A mode control: 2 - Continous up */
#define MC_3                   (0x0030)       /* Timer A mode control: 3 - Up/Down */
#define ID_0                   (0x0000)       /* Timer A input divider: 0 - /1 */
#define ID_1                   (0x0040)       /* Timer A input divider: 1 - /2 */
#define ID_2                   (0x0080)       /* Timer A input divider: 2 - /4 */
#define ID_3                   (0x00C0)       /* Timer A input divider: 3 - /8 */
#define TASSEL_0               (0x0000)       /* Timer A clock source select: 0 - TACLK */
#define TASSEL_1               (0x0100)       /* Timer A clock source select: 1 - ACLK  */
#define TASSEL_2               (0x0200)       /* Timer A clock source select: 2 - SMCLK */
#define TASSEL_3               (0x0300)       /* Timer A clock source select: 3 - INCLK */

#define CM1                    (0x8000)       /* Capture mode 1 */
#define CM0                    (0x4000)       /* Capture mode 0 */
#define CCIS1                  (0x2000)       /* Capture input select 1 */
#define CCIS0                  (0x1000)       /* Capture input select 0 */
#define SCS                    (0x0800)       /* Capture sychronize */
#define SCCI                   (0x0400)       /* Latched capture signal (read) */
#define CAP                    (0x0100)       /* Capture mode: 1 /Compare mode : 0 */
#define OUTMOD2                (0x0080)       /* Output mode 2 */
#define OUTMOD1                (0x0040)       /* Output mode 1 */
#define OUTMOD0                (0x0020)       /* Output mode 0 */
#define CCIE                   (0x0010)       /* Capture/compare interrupt enable */
#define CCI                    (0x0008)       /* Capture input signal (read) */
#define OUT                    (0x0004)       /* PWM Output signal if output mode 0 */
#define COV                    (0x0002)       /* Capture/compare overflow flag */
#define CCIFG                  (0x0001)       /* Capture/compare interrupt flag */

#define OUTMOD_0               (0x0000)       /* PWM output mode: 0 - output only */
#define OUTMOD_1               (0x0020)       /* PWM output mode: 1 - set */
#define OUTMOD_2               (0x0040)       /* PWM output mode: 2 - PWM toggle/reset */
#define OUTMOD_3               (0x0060)       /* PWM output mode: 3 - PWM set/reset */
#define OUTMOD_4               (0x0080)       /* PWM output mode: 4 - toggle */
#define OUTMOD_5               (0x00A0)       /* PWM output mode: 5 - Reset */
#define OUTMOD_6               (0x00C0)       /* PWM output mode: 6 - PWM toggle/set */
#define OUTMOD_7               (0x00E0)       /* PWM output mode: 7 - PWM reset/set */
#define CCIS_0                 (0x0000)       /* Capture input select: 0 - CCIxA */
#define CCIS_1                 (0x1000)       /* Capture input select: 1 - CCIxB */
#define CCIS_2                 (0x2000)       /* Capture input select: 2 - GND */
#define CCIS_3                 (0x3000)       /* Capture input select: 3 - Vcc */
#define CM_0                   (0x0000)       /* Capture mode: 0 - disabled */
#define CM_1                   (0x4000)       /* Capture mode: 1 - pos. edge */
#define CM_2                   (0x8000)       /* Capture mode: 1 - neg. edge */
#define CM_3                   (0xC000)       /* Capture mode: 1 - both edges */

/* TA3IV Definitions */
#define TAIV_NONE              (0x0000)       /* No Interrupt pending */
#define TAIV_TACCR1            (0x0002)       /* TACCR1_CCIFG */
#define TAIV_TACCR2            (0x0004)       /* TACCR2_CCIFG */
#define TAIV_6                 (0x0006)       /* Reserved */
#define TAIV_8                 (0x0008)       /* Reserved */
#define TAIV_TAIFG             (0x000A)       /* TAIFG */

/* Alternate register names - 5xx style */
#define TA0IV_NONE             (0x0000)       /* No Interrupt pending */
#define TA0IV_TACCR1           (0x0002)       /* TA0CCR1_CCIFG */
#define TA0IV_TACCR2           (0x0004)       /* TA0CCR2_CCIFG */
#define TA0IV_6                (0x0006)       /* Reserved */
#define TA0IV_8                (0x0008)       /* Reserved */
#define TA0IV_TAIFG            (0x000A)       /* TA0IFG */

/************************************************************
* Timer B7
************************************************************/
#define __MSP430_HAS_TB7__                    /* Definition to show that Module is available */

sfr_w(TBIV);                                  /* Timer B Interrupt Vector Word */
sfr_w(TBCTL);                                 /* Timer B Control */
sfr_w(TBCCTL0);                               /* Timer B Capture/Compare Control 0 */
sfr_w(TBCCTL1);                               /* Timer B Capture/Compare Control 1 */
sfr_w(TBCCTL2);                               /* Timer B Capture/Compare Control 2 */
sfr_w(TBCCTL3);                               /* Timer B Capture/Compare Control 3 */
sfr_w(TBCCTL4);                               /* Timer B Capture/Compare Control 4 */
sfr_w(TBCCTL5);                               /* Timer B Capture/Compare Control 5 */
sfr_w(TBCCTL6);                               /* Timer B Capture/Compare Control 6 */
sfr_w(TBR);                                   /* Timer B Counter Register */
sfr_w(TBCCR0);                                /* Timer B Capture/Compare 0 */
sfr_w(TBCCR1);                                /* Timer B Capture/Compare 1 */
sfr_w(TBCCR2);                                /* Timer B Capture/Compare 2 */
sfr_w(TBCCR3);                                /* Timer B Capture/Compare 3 */
sfr_w(TBCCR4);                                /* Timer B Capture/Compare 4 */
sfr_w(TBCCR5);                                /* Timer B Capture/Compare 5 */
sfr_w(TBCCR6);                                /* Timer B Capture/Compare 6 */

/* Alternate register names - 5xx style */
#define TB0IV                  TBIV           /* Timer B Interrupt Vector Word */
#define TB0CTL                 TBCTL          /* Timer B Control */
#define TB0CCTL0               TBCCTL0        /* Timer B Capture/Compare Control 0 */
#define TB0CCTL1               TBCCTL1        /* Timer B Capture/Compare Control 1 */
#define TB0CCTL2               TBCCTL2        /* Timer B Capture/Compare Control 2 */
#define TB0CCTL3               TBCCTL3        /* Timer B Capture/Compare Control 3 */
#define TB0CCTL4               TBCCTL4        /* Timer B Capture/Compare Control 4 */
#define TB0CCTL5               TBCCTL5        /* Timer B Capture/Compare Control 5 */
#define TB0CCTL6               TBCCTL6        /* Timer B Capture/Compare Control 6 */
#define TB0R                   TBR            /* Timer B Counter Register */
#define TB0CCR0                TBCCR0         /* Timer B Capture/Compare 0 */
#define TB0CCR1                TBCCR1         /* Timer B Capture/Compare 1 */
#define TB0CCR2                TBCCR2         /* Timer B Capture/Compare 2 */
#define TB0CCR3                TBCCR3         /* Timer B Capture/Compare 3 */
#define TB0CCR4                TBCCR4         /* Timer B Capture/Compare 4 */
#define TB0CCR5                TBCCR5         /* Timer B Capture/Compare 5 */
#define TB0CCR6                TBCCR6         /* Timer B Capture/Compare 6 */
#define TB0IV_                 TBIV_          /* Timer B Interrupt Vector Word */
#define TB0CTL_                TBCTL_         /* Timer B Control */
#define TB0CCTL0_              TBCCTL0_       /* Timer B Capture/Compare Control 0 */
#define TB0CCTL1_              TBCCTL1_       /* Timer B Capture/Compare Control 1 */
#define TB0CCTL2_              TBCCTL2_       /* Timer B Capture/Compare Control 2 */
#define TB0CCTL3_              TBCCTL3_       /* Timer B Capture/Compare Control 3 */
#define TB0CCTL4_              TBCCTL4_       /* Timer B Capture/Compare Control 4 */
#define TB0CCTL5_              TBCCTL5_       /* Timer B Capture/Compare Control 5 */
#define TB0CCTL6_              TBCCTL6_       /* Timer B Capture/Compare Control 6 */
#define TB0R_                  TBR_           /* Timer B Counter Register */
#define TB0CCR0_               TBCCR0_        /* Timer B Capture/Compare 0 */
#define TB0CCR1_               TBCCR1_        /* Timer B Capture/Compare 1 */
#define TB0CCR2_               TBCCR2_        /* Timer B Capture/Compare 2 */
#define TB0CCR3_               TBCCR3_        /* Timer B Capture/Compare 3 */
#define TB0CCR4_               TBCCR4_        /* Timer B Capture/Compare 4 */
#define TB0CCR5_               TBCCR5_        /* Timer B Capture/Compare 5 */
#define TB0CCR6_               TBCCR6_        /* Timer B Capture/Compare 6 */
#define TIMER0_B1_VECTOR       TIMERB1_VECTOR /* Int. Vector: Timer B CC1-6, TB */
#define TIMER0_B0_VECTOR       TIMERB0_VECTOR /* Int. Vector: Timer B CC0 */

#define TBCLGRP1               (0x4000)       /* Timer B Compare latch load group 1 */
#define TBCLGRP0               (0x2000)       /* Timer B Compare latch load group 0 */
#define CNTL1                  (0x1000)       /* Counter lenght 1 */
#define CNTL0                  (0x0800)       /* Counter lenght 0 */
#define TBSSEL1                (0x0200)       /* Clock source 1 */
#define TBSSEL0                (0x0100)       /* Clock source 0 */
#define TBCLR                  (0x0004)       /* Timer B counter clear */
#define TBIE                   (0x0002)       /* Timer B interrupt enable */
#define TBIFG                  (0x0001)       /* Timer B interrupt flag */

#define SHR1                   (0x4000)       /* Timer B Compare latch load group 1 */
#define SHR0                   (0x2000)       /* Timer B Compare latch load group 0 */

#define TBSSEL_0               (0x0000)       /* Clock Source: TBCLK */
#define TBSSEL_1               (0x0100)       /* Clock Source: ACLK  */
#define TBSSEL_2               (0x0200)       /* Clock Source: SMCLK */
#define TBSSEL_3               (0x0300)       /* Clock Source: INCLK */
#define CNTL_0                 (0x0000)       /* Counter lenght: 16 bit */
#define CNTL_1                 (0x0800)       /* Counter lenght: 12 bit */
#define CNTL_2                 (0x1000)       /* Counter lenght: 10 bit */
#define CNTL_3                 (0x1800)       /* Counter lenght:  8 bit */
#define SHR_0                  (0x0000)       /* Timer B Group: 0 - individually */
#define SHR_1                  (0x2000)       /* Timer B Group: 1 - 3 groups (1-2, 3-4, 5-6) */
#define SHR_2                  (0x4000)       /* Timer B Group: 2 - 2 groups (1-3, 4-6)*/
#define SHR_3                  (0x6000)       /* Timer B Group: 3 - 1 group (all) */
#define TBCLGRP_0              (0x0000)       /* Timer B Group: 0 - individually */
#define TBCLGRP_1              (0x2000)       /* Timer B Group: 1 - 3 groups (1-2, 3-4, 5-6) */
#define TBCLGRP_2              (0x4000)       /* Timer B Group: 2 - 2 groups (1-3, 4-6)*/
#define TBCLGRP_3              (0x6000)       /* Timer B Group: 3 - 1 group (all) */

/* Additional Timer B Control Register bits are defined in Timer A */
#define CLLD1                  (0x0400)       /* Compare latch load source 1 */
#define CLLD0                  (0x0200)       /* Compare latch load source 0 */

#define SLSHR1                 (0x0400)       /* Compare latch load source 1 */
#define SLSHR0                 (0x0200)       /* Compare latch load source 0 */

#define SLSHR_0                (0x0000)       /* Compare latch load sourec : 0 - immediate */
#define SLSHR_1                (0x0200)       /* Compare latch load sourec : 1 - TBR counts to 0 */
#define SLSHR_2                (0x0400)       /* Compare latch load sourec : 2 - up/down */
#define SLSHR_3                (0x0600)       /* Compare latch load sourec : 3 - TBR counts to TBCTL0 */

#define CLLD_0                 (0x0000)       /* Compare latch load sourec : 0 - immediate */
#define CLLD_1                 (0x0200)       /* Compare latch load sourec : 1 - TBR counts to 0 */
#define CLLD_2                 (0x0400)       /* Compare latch load sourec : 2 - up/down */
#define CLLD_3                 (0x0600)       /* Compare latch load sourec : 3 - TBR counts to TBCTL0 */

/* TB7IV Definitions */
#define TBIV_NONE              (0x0000)       /* No Interrupt pending */
#define TBIV_TBCCR1            (0x0002)       /* TBCCR1_CCIFG */
#define TBIV_TBCCR2            (0x0004)       /* TBCCR2_CCIFG */
#define TBIV_TBCCR3            (0x0006)       /* TBCCR3_CCIFG */
#define TBIV_TBCCR4            (0x0008)       /* TBCCR4_CCIFG */
#define TBIV_TBCCR5            (0x000A)       /* TBCCR3_CCIFG */
#define TBIV_TBCCR6            (0x000C)       /* TBCCR4_CCIFG */
#define TBIV_TBIFG             (0x000E)       /* TBIFG */

/* Alternate register names - 5xx style */
#define TB0IV_NONE             (0x0000)       /* No Interrupt pending */
#define TB0IV_TBCCR1           (0x0002)       /* TB0CCR1_CCIFG */
#define TB0IV_TBCCR2           (0x0004)       /* TB0CCR2_CCIFG */
#define TB0IV_TBCCR3           (0x0006)       /* TB0CCR3_CCIFG */
#define TB0IV_TBCCR4           (0x0008)       /* TB0CCR4_CCIFG */
#define TB0IV_TBCCR5           (0x000A)       /* TB0CCR3_CCIFG */
#define TB0IV_TBCCR6           (0x000C)       /* TB0CCR4_CCIFG */
#define TB0IV_TBIFG            (0x000E)       /* TB0IFG */

/************************************************************
* USCI
************************************************************/
#define __MSP430_HAS_USCI__                   /* Definition to show that Module is available */
#define __MSP430_HAS_USCI_AB0__                /* Definition to show that Module is available */
#define __MSP430_HAS_USCI_AB1__                /* Definition to show that Module is available */

sfr_b(UCA0CTL0);                              /* USCI A0 Control Register 0 */
sfr_b(UCA0CTL1);                              /* USCI A0 Control Register 1 */
sfr_b(UCA0BR0);                               /* USCI A0 Baud Rate 0 */
sfr_b(UCA0BR1);                               /* USCI A0 Baud Rate 1 */
sfr_b(UCA0MCTL);                              /* USCI A0 Modulation Control */
sfr_b(UCA0STAT);                              /* USCI A0 Status Register */
sfr_b(UCA0RXBUF);                             /* USCI A0 Receive Buffer */
sfr_b(UCA0TXBUF);                             /* USCI A0 Transmit Buffer */
sfr_b(UCA0ABCTL);                             /* USCI A0 LIN Control */
sfr_b(UCA0IRTCTL);                            /* USCI A0 IrDA Transmit Control */
sfr_b(UCA0IRRCTL);                            /* USCI A0 IrDA Receive Control */



sfr_b(UCB0CTL0);                              /* USCI B0 Control Register 0 */
sfr_b(UCB0CTL1);                              /* USCI B0 Control Register 1 */
sfr_b(UCB0BR0);                               /* USCI B0 Baud Rate 0 */
sfr_b(UCB0BR1);                               /* USCI B0 Baud Rate 1 */
sfr_b(UCB0I2CIE);                             /* USCI B0 I2C Interrupt Enable Register */
sfr_b(UCB0STAT);                              /* USCI B0 Status Register */
sfr_b(UCB0RXBUF);                             /* USCI B0 Receive Buffer */
sfr_b(UCB0TXBUF);                             /* USCI B0 Transmit Buffer */
sfr_w(UCB0I2COA);                             /* USCI B0 I2C Own Address */
sfr_w(UCB0I2CSA);                             /* USCI B0 I2C Slave Address */

sfr_b(UCA1CTL0);                              /* USCI A1 Control Register 0 */
sfr_b(UCA1CTL1);                              /* USCI A1 Control Register 1 */
sfr_b(UCA1BR0);                               /* USCI A1 Baud Rate 0 */
sfr_b(UCA1BR1);                               /* USCI A1 Baud Rate 1 */
sfr_b(UCA1MCTL);                              /* USCI A1 Modulation Control */
sfr_b(UCA1STAT);                              /* USCI A1 Status Register */
sfr_b(UCA1RXBUF);                             /* USCI A1 Receive Buffer */
sfr_b(UCA1TXBUF);                             /* USCI A1 Transmit Buffer */
sfr_b(UCA1ABCTL);                             /* USCI A1 LIN Control */
sfr_b(UCA1IRTCTL);                            /* USCI A1 IrDA Transmit Control */
sfr_b(UCA1IRRCTL);                            /* USCI A1 IrDA Receive Control */



sfr_b(UCB1CTL0);                              /* USCI B1 Control Register 0 */
sfr_b(UCB1CTL1);                              /* USCI B1 Control Register 1 */
sfr_b(UCB1BR0);                               /* USCI B1 Baud Rate 0 */
sfr_b(UCB1BR1);                               /* USCI B1 Baud Rate 1 */
sfr_b(UCB1I2CIE);                             /* USCI B1 I2C Interrupt Enable Register */
sfr_b(UCB1STAT);                              /* USCI B1 Status Register */
sfr_b(UCB1RXBUF);                             /* USCI B1 Receive Buffer */
sfr_b(UCB1TXBUF);                             /* USCI B1 Transmit Buffer */
sfr_w(UCB1I2COA);                             /* USCI B1 I2C Own Address */
sfr_w(UCB1I2CSA);                             /* USCI B1 I2C Slave Address */

// UART-Mode Bits
#define UCPEN                  (0x80)         /* Async. Mode: Parity enable */
#define UCPAR                  (0x40)         /* Async. Mode: Parity     0:odd / 1:even */
#define UCMSB                  (0x20)         /* Async. Mode: MSB first  0:LSB / 1:MSB */
#define UC7BIT                 (0x10)         /* Async. Mode: Data Bits  0:8-bits / 1:7-bits */
#define UCSPB                  (0x08)         /* Async. Mode: Stop Bits  0:one / 1: two */
#define UCMODE1                (0x04)         /* Async. Mode: USCI Mode 1 */
#define UCMODE0                (0x02)         /* Async. Mode: USCI Mode 0 */
#define UCSYNC                 (0x01)         /* Sync-Mode  0:UART-Mode / 1:SPI-Mode */

// SPI-Mode Bits
#define UCCKPH                 (0x80)         /* Sync. Mode: Clock Phase */
#define UCCKPL                 (0x40)         /* Sync. Mode: Clock Polarity */
#define UCMST                  (0x08)         /* Sync. Mode: Master Select */

// I2C-Mode Bits
#define UCA10                  (0x80)         /* 10-bit Address Mode */
#define UCSLA10                (0x40)         /* 10-bit Slave Address Mode */
#define UCMM                   (0x20)         /* Multi-Master Environment */
//#define res               (0x10)    /* reserved */
#define UCMODE_0               (0x00)         /* Sync. Mode: USCI Mode: 0 */
#define UCMODE_1               (0x02)         /* Sync. Mode: USCI Mode: 1 */
#define UCMODE_2               (0x04)         /* Sync. Mode: USCI Mode: 2 */
#define UCMODE_3               (0x06)         /* Sync. Mode: USCI Mode: 3 */

// UART-Mode Bits
#define UCSSEL1                (0x80)         /* USCI 0 Clock Source Select 1 */
#define UCSSEL0                (0x40)         /* USCI 0 Clock Source Select 0 */
#define UCRXEIE                (0x20)         /* RX Error interrupt enable */
#define UCBRKIE                (0x10)         /* Break interrupt enable */
#define UCDORM                 (0x08)         /* Dormant (Sleep) Mode */
#define UCTXADDR               (0x04)         /* Send next Data as Address */
#define UCTXBRK                (0x02)         /* Send next Data as Break */
#define UCSWRST                (0x01)         /* USCI Software Reset */

// SPI-Mode Bits
//#define res               (0x20)    /* reserved */
//#define res               (0x10)    /* reserved */
//#define res               (0x08)    /* reserved */
//#define res               (0x04)    /* reserved */
//#define res               (0x02)    /* reserved */

// I2C-Mode Bits
//#define res               (0x20)    /* reserved */
#define UCTR                   (0x10)         /* Transmit/Receive Select/Flag */
#define UCTXNACK               (0x08)         /* Transmit NACK */
#define UCTXSTP                (0x04)         /* Transmit STOP */
#define UCTXSTT                (0x02)         /* Transmit START */
#define UCSSEL_0               (0x00)         /* USCI 0 Clock Source: 0 */
#define UCSSEL_1               (0x40)         /* USCI 0 Clock Source: 1 */
#define UCSSEL_2               (0x80)         /* USCI 0 Clock Source: 2 */
#define UCSSEL_3               (0xC0)         /* USCI 0 Clock Source: 3 */

#define UCBRF3                 (0x80)         /* USCI First Stage Modulation Select 3 */
#define UCBRF2                 (0x40)         /* USCI First Stage Modulation Select 2 */
#define UCBRF1                 (0x20)         /* USCI First Stage Modulation Select 1 */
#define UCBRF0                 (0x10)         /* USCI First Stage Modulation Select 0 */
#define UCBRS2                 (0x08)         /* USCI Second Stage Modulation Select 2 */
#define UCBRS1                 (0x04)         /* USCI Second Stage Modulation Select 1 */
#define UCBRS0                 (0x02)         /* USCI Second Stage Modulation Select 0 */
#define UCOS16                 (0x01)         /* USCI 16-times Oversampling enable */

#define UCBRF_0                (0x00)         /* USCI First Stage Modulation: 0 */
#define UCBRF_1                (0x10)         /* USCI First Stage Modulation: 1 */
#define UCBRF_2                (0x20)         /* USCI First Stage Modulation: 2 */
#define UCBRF_3                (0x30)         /* USCI First Stage Modulation: 3 */
#define UCBRF_4                (0x40)         /* USCI First Stage Modulation: 4 */
#define UCBRF_5                (0x50)         /* USCI First Stage Modulation: 5 */
#define UCBRF_6                (0x60)         /* USCI First Stage Modulation: 6 */
#define UCBRF_7                (0x70)         /* USCI First Stage Modulation: 7 */
#define UCBRF_8                (0x80)         /* USCI First Stage Modulation: 8 */
#define UCBRF_9                (0x90)         /* USCI First Stage Modulation: 9 */
#define UCBRF_10               (0xA0)         /* USCI First Stage Modulation: A */
#define UCBRF_11               (0xB0)         /* USCI First Stage Modulation: B */
#define UCBRF_12               (0xC0)         /* USCI First Stage Modulation: C */
#define UCBRF_13               (0xD0)         /* USCI First Stage Modulation: D */
#define UCBRF_14               (0xE0)         /* USCI First Stage Modulation: E */
#define UCBRF_15               (0xF0)         /* USCI First Stage Modulation: F */

#define UCBRS_0                (0x00)         /* USCI Second Stage Modulation: 0 */
#define UCBRS_1                (0x02)         /* USCI Second Stage Modulation: 1 */
#define UCBRS_2                (0x04)         /* USCI Second Stage Modulation: 2 */
#define UCBRS_3                (0x06)         /* USCI Second Stage Modulation: 3 */
#define UCBRS_4                (0x08)         /* USCI Second Stage Modulation: 4 */
#define UCBRS_5                (0x0A)         /* USCI Second Stage Modulation: 5 */
#define UCBRS_6                (0x0C)         /* USCI Second Stage Modulation: 6 */
#define UCBRS_7                (0x0E)         /* USCI Second Stage Modulation: 7 */

#define UCLISTEN               (0x80)         /* USCI Listen mode */
#define UCFE                   (0x40)         /* USCI Frame Error Flag */
#define UCOE                   (0x20)         /* USCI Overrun Error Flag */
#define UCPE                   (0x10)         /* USCI Parity Error Flag */
#define UCBRK                  (0x08)         /* USCI Break received */
#define UCRXERR                (0x04)         /* USCI RX Error Flag */
#define UCADDR                 (0x02)         /* USCI Address received Flag */
#define UCBUSY                 (0x01)         /* USCI Busy Flag */
#define UCIDLE                 (0x02)         /* USCI Idle line detected Flag */

//#define res               (0x80)    /* reserved */
//#define res               (0x40)    /* reserved */
//#define res               (0x20)    /* reserved */
//#define res               (0x10)    /* reserved */
#define UCNACKIE               (0x08)         /* NACK Condition interrupt enable */
#define UCSTPIE                (0x04)         /* STOP Condition interrupt enable */
#define UCSTTIE                (0x02)         /* START Condition interrupt enable */
#define UCALIE                 (0x01)         /* Arbitration Lost interrupt enable */

#define UCSCLLOW               (0x40)         /* SCL low */
#define UCGC                   (0x20)         /* General Call address received Flag */
#define UCBBUSY                (0x10)         /* Bus Busy Flag */
#define UCNACKIFG              (0x08)         /* NAK Condition interrupt Flag */
#define UCSTPIFG               (0x04)         /* STOP Condition interrupt Flag */
#define UCSTTIFG               (0x02)         /* START Condition interrupt Flag */
#define UCALIFG                (0x01)         /* Arbitration Lost interrupt Flag */

#define UCIRTXPL5              (0x80)         /* IRDA Transmit Pulse Length 5 */
#define UCIRTXPL4              (0x40)         /* IRDA Transmit Pulse Length 4 */
#define UCIRTXPL3              (0x20)         /* IRDA Transmit Pulse Length 3 */
#define UCIRTXPL2              (0x10)         /* IRDA Transmit Pulse Length 2 */
#define UCIRTXPL1              (0x08)         /* IRDA Transmit Pulse Length 1 */
#define UCIRTXPL0              (0x04)         /* IRDA Transmit Pulse Length 0 */
#define UCIRTXCLK              (0x02)         /* IRDA Transmit Pulse Clock Select */
#define UCIREN                 (0x01)         /* IRDA Encoder/Decoder enable */

#define UCIRRXFL5              (0x80)         /* IRDA Receive Filter Length 5 */
#define UCIRRXFL4              (0x40)         /* IRDA Receive Filter Length 4 */
#define UCIRRXFL3              (0x20)         /* IRDA Receive Filter Length 3 */
#define UCIRRXFL2              (0x10)         /* IRDA Receive Filter Length 2 */
#define UCIRRXFL1              (0x08)         /* IRDA Receive Filter Length 1 */
#define UCIRRXFL0              (0x04)         /* IRDA Receive Filter Length 0 */
#define UCIRRXPL               (0x02)         /* IRDA Receive Input Polarity */
#define UCIRRXFE               (0x01)         /* IRDA Receive Filter enable */

//#define res               (0x80)    /* reserved */
//#define res               (0x40)    /* reserved */
#define UCDELIM1               (0x20)         /* Break Sync Delimiter 1 */
#define UCDELIM0               (0x10)         /* Break Sync Delimiter 0 */
#define UCSTOE                 (0x08)         /* Sync-Field Timeout error */
#define UCBTOE                 (0x04)         /* Break Timeout error */
//#define res               (0x02)    /* reserved */
#define UCABDEN                (0x01)         /* Auto Baud Rate detect enable */

#define UCGCEN                 (0x8000)       /* I2C General Call enable */
#define UCOA9                  (0x0200)       /* I2C Own Address 9 */
#define UCOA8                  (0x0100)       /* I2C Own Address 8 */
#define UCOA7                  (0x0080)       /* I2C Own Address 7 */
#define UCOA6                  (0x0040)       /* I2C Own Address 6 */
#define UCOA5                  (0x0020)       /* I2C Own Address 5 */
#define UCOA4                  (0x0010)       /* I2C Own Address 4 */
#define UCOA3                  (0x0008)       /* I2C Own Address 3 */
#define UCOA2                  (0x0004)       /* I2C Own Address 2 */
#define UCOA1                  (0x0002)       /* I2C Own Address 1 */
#define UCOA0                  (0x0001)       /* I2C Own Address 0 */

#define UCSA9                  (0x0200)       /* I2C Slave Address 9 */
#define UCSA8                  (0x0100)       /* I2C Slave Address 8 */
#define UCSA7                  (0x0080)       /* I2C Slave Address 7 */
#define UCSA6                  (0x0040)       /* I2C Slave Address 6 */
#define UCSA5                  (0x0020)       /* I2C Slave Address 5 */
#define UCSA4                  (0x0010)       /* I2C Slave Address 4 */
#define UCSA3                  (0x0008)       /* I2C Slave Address 3 */
#define UCSA2                  (0x0004)       /* I2C Slave Address 2 */
#define UCSA1                  (0x0002)       /* I2C Slave Address 1 */
#define UCSA0                  (0x0001)       /* I2C Slave Address 0 */

/************************************************************
* WATCHDOG TIMER
************************************************************/
#define __MSP430_HAS_WDT__                    /* Definition to show that Module is available */

sfr_w(WDTCTL);                                /* Watchdog Timer Control */
/* The bit names have been prefixed with "WDT" */
#define WDTIS0                 (0x0001)
#define WDTIS1                 (0x0002)
#define WDTSSEL                (0x0004)
#define WDTCNTCL               (0x0008)
#define WDTTMSEL               (0x0010)
#define WDTNMI                 (0x0020)
#define WDTNMIES               (0x0040)
#define WDTHOLD                (0x0080)

#define WDTPW                  (0x5A00)

/* WDT-interval times [1ms] coded with Bits 0-2 */
/* WDT is clocked by fSMCLK (assumed 1MHz) */
#define WDT_MDLY_32         (WDTPW+WDTTMSEL+WDTCNTCL)                         /* 32ms interval (default) */
#define WDT_MDLY_8          (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS0)                  /* 8ms     " */
#define WDT_MDLY_0_5        (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1)                  /* 0.5ms   " */
#define WDT_MDLY_0_064      (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1+WDTIS0)           /* 0.064ms " */
/* WDT is clocked by fACLK (assumed 32KHz) */
#define WDT_ADLY_1000       (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL)                 /* 1000ms  " */
#define WDT_ADLY_250        (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS0)          /* 250ms   " */
#define WDT_ADLY_16         (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1)          /* 16ms    " */
#define WDT_ADLY_1_9        (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1+WDTIS0)   /* 1.9ms   " */
/* Watchdog mode -> reset after expired time */
/* WDT is clocked by fSMCLK (assumed 1MHz) */
#define WDT_MRST_32         (WDTPW+WDTCNTCL)                                  /* 32ms interval (default) */
#define WDT_MRST_8          (WDTPW+WDTCNTCL+WDTIS0)                           /* 8ms     " */
#define WDT_MRST_0_5        (WDTPW+WDTCNTCL+WDTIS1)                           /* 0.5ms   " */
#define WDT_MRST_0_064      (WDTPW+WDTCNTCL+WDTIS1+WDTIS0)                    /* 0.064ms " */
/* WDT is clocked by fACLK (assumed 32KHz) */
#define WDT_ARST_1000       (WDTPW+WDTCNTCL+WDTSSEL)                          /* 1000ms  " */
#define WDT_ARST_250        (WDTPW+WDTCNTCL+WDTSSEL+WDTIS0)                   /* 250ms   " */
#define WDT_ARST_16         (WDTPW+WDTCNTCL+WDTSSEL+WDTIS1)                   /* 16ms    " */
#define WDT_ARST_1_9        (WDTPW+WDTCNTCL+WDTSSEL+WDTIS1+WDTIS0)            /* 1.9ms   " */

/* INTERRUPT CONTROL */
/* These two bits are defined in the Special Function Registers */
/* #define WDTIE               0x01 */
/* #define WDTIFG              0x01 */

/************************************************************
* Calibration Data in Info Mem
************************************************************/

/* TLV Calibration Data Structure */
#define TAG_DCO_30             (0x01)         /* Tag for DCO30  Calibration Data */
#define TAG_ADC12_1            (0x08)         /* Tag for ADC12_1 Calibration Data */
#define TAG_EMPTY              (0xFE)         /* Tag for Empty Data Field in Calibration Data */

#ifndef __DisableCalData
sfr_w(TLV_CHECKSUM);                          /* TLV CHECK SUM */
sfr_b(TLV_DCO_30_TAG);                        /* TLV TAG_DCO30 TAG */
sfr_b(TLV_DCO_30_LEN);                        /* TLV TAG_DCO30 LEN */
sfr_b(TLV_ADC12_1_TAG);                       /* TLV ADC12_1 TAG */
sfr_b(TLV_ADC12_1_LEN);                       /* TLV ADC12_1 LEN */
#endif

#define CAL_ADC_25T85          (0x0010)       /* Index for 2.5V/85Deg Cal. Value */
#define CAL_ADC_25T30          (0x000E)       /* Index for 2.5V/30Deg Cal. Value */
#define CAL_ADC_25VREF_FACTOR  (0x000C)       /* Index for 2.5V Ref. Factor */
#define CAL_ADC_15T85          (0x000A)       /* Index for 1.5V/85Deg Cal. Value */
#define CAL_ADC_15T30          (0x0008)       /* Index for 1.5V/30Deg Cal. Value */
#define CAL_ADC_15VREF_FACTOR  (0x0006)       /* Index for ADC 1.5V Ref. Factor */
#define CAL_ADC_OFFSET         (0x0004)       /* Index for ADC Offset */
#define CAL_ADC_GAIN_FACTOR    (0x0002)       /* Index for ADC Gain Factor */

#define CAL_DCO_16MHZ          (0x0002)       /* Index for DCOCTL  Calibration Data for 16MHz */
#define CAL_BC1_16MHZ          (0x0003)       /* Index for BCSCTL1 Calibration Data for 16MHz */
#define CAL_DCO_12MHZ          (0x0004)       /* Index for DCOCTL  Calibration Data for 12MHz */
#define CAL_BC1_12MHZ          (0x0005)       /* Index for BCSCTL1 Calibration Data for 12MHz */
#define CAL_DCO_8MHZ           (0x0006)       /* Index for DCOCTL  Calibration Data for 8MHz */
#define CAL_BC1_8MHZ           (0x0007)       /* Index for BCSCTL1 Calibration Data for 8MHz */
#define CAL_DCO_1MHZ           (0x0008)       /* Index for DCOCTL  Calibration Data for 1MHz */
#define CAL_BC1_1MHZ           (0x0009)       /* Index for BCSCTL1 Calibration Data for 1MHz */


/************************************************************
* Calibration Data in Info Mem
************************************************************/

#ifndef __DisableCalData

sfr_b(CALDCO_16MHZ);                          /* DCOCTL  Calibration Data for 16MHz */
sfr_b(CALBC1_16MHZ);                          /* BCSCTL1 Calibration Data for 16MHz */
sfr_b(CALDCO_12MHZ);                          /* DCOCTL  Calibration Data for 12MHz */
sfr_b(CALBC1_12MHZ);                          /* BCSCTL1 Calibration Data for 12MHz */
sfr_b(CALDCO_8MHZ);                           /* DCOCTL  Calibration Data for 8MHz */
sfr_b(CALBC1_8MHZ);                           /* BCSCTL1 Calibration Data for 8MHz */
sfr_b(CALDCO_1MHZ);                           /* DCOCTL  Calibration Data for 1MHz */
sfr_b(CALBC1_1MHZ);                           /* BCSCTL1 Calibration Data for 1MHz */

#endif /* #ifndef __DisableCalData */

/************************************************************
* Interrupt Vectors (offset from 0xFFC0)
************************************************************/

#define RESERVED0_VECTOR        ( 1)                     /* 0xFFC0 Reserved Int. Vector 0 */
#define RESERVED1_VECTOR        ( 2)                     /* 0xFFC2 Reserved Int. Vector 1 */
#define RESERVED2_VECTOR        ( 3)                     /* 0xFFC4 Reserved Int. Vector 2 */
#define RESERVED3_VECTOR        ( 4)                     /* 0xFFC6 Reserved Int. Vector 3 */
#define RESERVED4_VECTOR        ( 5)                     /* 0xFFC8 Reserved Int. Vector 4 */
#define RESERVED5_VECTOR        ( 6)                     /* 0xFFCA Reserved Int. Vector 5 */
#define RESERVED6_VECTOR        ( 7)                     /* 0xFFCC Reserved Int. Vector 6 */
#define RESERVED7_VECTOR        ( 8)                     /* 0xFFCE Reserved Int. Vector 7 */
#define RESERVED8_VECTOR        ( 9)                     /* 0xFFD0 Reserved Int. Vector 8 */
#define RESERVED9_VECTOR        (10)                     /* 0xFFD2 Reserved Int. Vector 9 */
#define RESERVED10_VECTOR       (11)                     /* 0xFFD4 Reserved Int. Vector 10 */
#define RESERVED11_VECTOR       (12)                     /* 0xFFD6 Reserved Int. Vector 11 */
#define RESERVED12_VECTOR       (13)                     /* 0xFFD8 Reserved Int. Vector 12 */
#define RESERVED13_VECTOR       (14)                     /* 0xFFDA Reserved Int. Vector 13 */
#define DAC12_VECTOR            (15)                     /* 0xFFDC DAC12 */
#define DMA_VECTOR              (16)                     /* 0xFFDE DMA */
#define USCIAB1TX_VECTOR        (17)                     /* 0xFFE0 USCI A1/B1 Transmit */
#define USCIAB1RX_VECTOR        (18)                     /* 0xFFE2 USCI A1/B1 Receive */
#define PORT1_VECTOR            (19)                     /* 0xFFE4 Port 1 */
#define PORT2_VECTOR            (20)                     /* 0xFFE6 Port 2 */
#define RESERVED20_VECTOR       (21)                     /* 0xFFE8 Reserved Int. Vector 20 */
#define ADC12_VECTOR            (22)                     /* 0xFFEA ADC */
#define USCIAB0TX_VECTOR        (23)                     /* 0xFFEC USCI A0/B0 Transmit */
#define USCIAB0RX_VECTOR        (24)                     /* 0xFFEE USCI A0/B0 Receive */
#define TIMERA1_VECTOR          (25)                     /* 0xFFF0 Timer A CC1-2, TA */
#define TIMERA0_VECTOR          (26)                     /* 0xFFF2 Timer A CC0 */
#define WDT_VECTOR              (27)                     /* 0xFFF4 Watchdog Timer */
#define COMPARATORA_VECTOR      (28)                     /* 0xFFF6 Comparator A */
#define TIMERB1_VECTOR          (29)                     /* 0xFFF8 Timer B CC1-6, TB */
#define TIMERB0_VECTOR          (30)                     /* 0xFFFA Timer B CC0 */
#define NMI_VECTOR              (31)                     /* 0xFFFC Non-maskable */
#define RESET_VECTOR            ("reset")                /* 0xFFFE Reset [Highest Priority] */

/************************************************************
* End of Modules
************************************************************/

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* #ifndef __msp430x26x */

