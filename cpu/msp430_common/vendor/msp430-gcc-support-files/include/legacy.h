//*****************************************************************************
//
// Copyright (C) 2012 - 2015 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************


/************************************************************
* ADC
************************************************************/

#ifdef __MSP430_HAS_ADC__

#define ADCIV_ADCHIIFG          ADCIV__ADCHIIFG
#define ADCIV_ADCIFG            ADCIV__ADCIFG0
#define ADCIV_ADCINIFG          ADCIV__ADCINIFG
#define ADCIV_ADCLOIFG          ADCIV__ADCLOIFG
#define ADCIV_ADCOVIFG          ADCIV__ADCOVIFG
#define ADCIV_ADCTOVIFG         ADCIV__ADCTOVIFG
#define ADCIV_NONE              ADCIV__NONE

#endif

/************************************************************
* ADC12_B
************************************************************/
#ifdef __MSP430_HAS_ADC12_B__

#define ADC12_VECTOR         ADC12_B_VECTOR

#define ADC12IV_NONE         ADC12IV__NONE
#define ADC12IV_ADC12HIIFG   ADC12IV__ADC12HIIFG
#define ADC12IV_ADC12IFG0    ADC12IV__ADC12IFG0
#define ADC12IV_ADC12IFG1    ADC12IV__ADC12IFG1
#define ADC12IV_ADC12IFG2    ADC12IV__ADC12IFG2
#define ADC12IV_ADC12IFG3    ADC12IV__ADC12IFG3
#define ADC12IV_ADC12IFG4    ADC12IV__ADC12IFG4
#define ADC12IV_ADC12IFG5    ADC12IV__ADC12IFG5
#define ADC12IV_ADC12IFG6    ADC12IV__ADC12IFG6
#define ADC12IV_ADC12IFG7    ADC12IV__ADC12IFG7
#define ADC12IV_ADC12IFG8    ADC12IV__ADC12IFG8
#define ADC12IV_ADC12IFG9    ADC12IV__ADC12IFG9
#define ADC12IV_ADC12IFG10   ADC12IV__ADC12IFG10
#define ADC12IV_ADC12IFG11   ADC12IV__ADC12IFG11
#define ADC12IV_ADC12IFG12   ADC12IV__ADC12IFG12
#define ADC12IV_ADC12IFG13   ADC12IV__ADC12IFG13
#define ADC12IV_ADC12IFG14   ADC12IV__ADC12IFG14
#define ADC12IV_ADC12IFG15   ADC12IV__ADC12IFG15
#define ADC12IV_ADC12IFG16   ADC12IV__ADC12IFG16
#define ADC12IV_ADC12IFG17   ADC12IV__ADC12IFG17
#define ADC12IV_ADC12IFG18   ADC12IV__ADC12IFG18
#define ADC12IV_ADC12IFG19   ADC12IV__ADC12IFG19
#define ADC12IV_ADC12IFG20   ADC12IV__ADC12IFG20
#define ADC12IV_ADC12IFG21   ADC12IV__ADC12IFG21
#define ADC12IV_ADC12IFG22   ADC12IV__ADC12IFG22
#define ADC12IV_ADC12IFG23   ADC12IV__ADC12IFG23
#define ADC12IV_ADC12IFG24   ADC12IV__ADC12IFG24
#define ADC12IV_ADC12IFG25   ADC12IV__ADC12IFG25
#define ADC12IV_ADC12IFG26   ADC12IV__ADC12IFG26
#define ADC12IV_ADC12IFG27   ADC12IV__ADC12IFG27
#define ADC12IV_ADC12IFG28   ADC12IV__ADC12IFG28
#define ADC12IV_ADC12IFG29   ADC12IV__ADC12IFG29
#define ADC12IV_ADC12IFG30   ADC12IV__ADC12IFG30
#define ADC12IV_ADC12IFG31   ADC12IV__ADC12IFG31
#define ADC12IV_ADC12INIFG   ADC12IV__ADC12INIFG
#define ADC12IV_ADC12LOIFG   ADC12IV__ADC12LOIFG
#define ADC12IV_ADC12OVIFG   ADC12IV__ADC12OVIFG
#define ADC12IV_ADC12RDYIFG  ADC12IV__ADC12RDYIFG
#define ADC12IV_ADC12TOVIFG  ADC12IV__ADC12TOVIFG

#endif

/************************************************************
* CAP TOUCH
************************************************************/

#ifdef __MSP430_HAS_CAPTIO0__
#define __MSP430_HAS_CAP_TOUCH_IO_0__
#define CAP_TOUCH_0_BASE    CAPTIO0_BASE
#define __MSP430_BASEADDRESS_CAP_TOUCH_IO_0__ CAPTIO0_BASE
#define CAPSIO0CTL          CAPTIO0CTL
#endif
#ifdef __MSP430_HAS_CAPTIO1__
#define __MSP430_HAS_CAP_TOUCH_IO_1__
#define __MSP430_BASEADDRESS_CAP_TOUCH_IO_1__ CAPTIO1_BASE
#define CAP_TOUCH_1_BASE    CAPTIO1_BASE
#define CAPSIO1CTL          CAPTIO1CTL
#endif

#ifdef __MSP430_HAS_CAPTIOx__

#define CAPSIOPISEL0            CAPTIOPISEL0
#define CAPSIOPISEL1            CAPTIOPISEL1
#define CAPSIOPISEL2            CAPTIOPISEL2
#define CAPSIOPOSEL0            CAPTIOPOSEL0
#define CAPSIOPOSEL1            CAPTIOPOSEL1
#define CAPSIOPOSEL2            CAPTIOPOSEL2
#define CAPSIOPOSEL3            CAPTIOPOSEL3
#define CAPSIOEN                CAPTIOEN
#define CAPSIO                  CAPTIO

#endif

/************************************************************
* CLOCK SYSTEM
************************************************************/

#ifdef __MSP430_HAS_CS__

#ifdef __MSP430FR2XX_FR4XX_FAMILY__

#define DCOFTRIM_0              (0x0000)    /* DCO frequency trim: 0 */
#define DCOFTRIM_1              (0x0010)    /* DCO frequency trim: 1 */
#define DCOFTRIM_2              (0x0020)    /* DCO frequency trim: 2 */
#define DCOFTRIM_3              (0x0030)    /* DCO frequency trim: 3 */
#define DCOFTRIM_4              (0x0040)    /* DCO frequency trim: 4 */
#define DCOFTRIM_5              (0x0050)    /* DCO frequency trim: 5 */
#define DCOFTRIM_6              (0x0060)    /* DCO frequency trim: 6 */
#define DCOFTRIM_7              (0x0070)    /* DCO frequency trim: 7 */

#endif

#define SELM__LFMODOSC          SELM__LFMODCLK
#define SELM__MODOSC            SELM__MODCLK
#define SELS__LFMODOSC          SELS__LFMODCLK
#define SELS__MODOSC            SELS__MODCLK
#define SELA__LFMODOSC          SELA__LFMODCLK

#define SELA_7                  SELA
#define SELM_7                  SELM
#define SELS_7                  SELS

#ifndef MODCLKREQEN
#define MODCLKREQEN             MODOSCREQEN
#define MODCLKREQEN_L           MODOSCREQEN_L
#endif

#endif

/************************************************************
* COMP_E
************************************************************/

#ifdef __MSP430_HAS_COMP_E__

#define CEIV_CEIFG              CEIV__CEIFG
#define CEIV_CEIIFG             CEIV__CEIIFG
#define CEIV_CERDYIFG           CEIV__CERDYIFG
#define CEIV_NONE               CEIV__NONE

#define CEPWRMD_3               CEPWRMD

#define CEREF0_0                (0x0000)       /* Comp. E Int. Ref.0 Select 0 : 1/32 */
#define CEREF0_1                (0x0001)       /* Comp. E Int. Ref.0 Select 1 : 2/32 */
#define CEREF0_2                (0x0002)       /* Comp. E Int. Ref.0 Select 2 : 3/32 */
#define CEREF0_3                (0x0003)       /* Comp. E Int. Ref.0 Select 3 : 4/32 */
#define CEREF0_4                (0x0004)       /* Comp. E Int. Ref.0 Select 4 : 5/32 */
#define CEREF0_5                (0x0005)       /* Comp. E Int. Ref.0 Select 5 : 6/32 */
#define CEREF0_6                (0x0006)       /* Comp. E Int. Ref.0 Select 6 : 7/32 */
#define CEREF0_7                (0x0007)       /* Comp. E Int. Ref.0 Select 7 : 8/32 */
#define CEREF0_8                (0x0008)       /* Comp. E Int. Ref.0 Select 0 : 9/32 */
#define CEREF0_9                (0x0009)       /* Comp. E Int. Ref.0 Select 1 : 10/32 */
#define CEREF0_10               (0x000A)       /* Comp. E Int. Ref.0 Select 2 : 11/32 */
#define CEREF0_11               (0x000B)       /* Comp. E Int. Ref.0 Select 3 : 12/32 */
#define CEREF0_12               (0x000C)       /* Comp. E Int. Ref.0 Select 4 : 13/32 */
#define CEREF0_13               (0x000D)       /* Comp. E Int. Ref.0 Select 5 : 14/32 */
#define CEREF0_14               (0x000E)       /* Comp. E Int. Ref.0 Select 6 : 15/32 */
#define CEREF0_15               (0x000F)       /* Comp. E Int. Ref.0 Select 7 : 16/32 */
#define CEREF0_16               (0x0010)       /* Comp. E Int. Ref.0 Select 0 : 17/32 */
#define CEREF0_17               (0x0011)       /* Comp. E Int. Ref.0 Select 1 : 18/32 */
#define CEREF0_18               (0x0012)       /* Comp. E Int. Ref.0 Select 2 : 19/32 */
#define CEREF0_19               (0x0013)       /* Comp. E Int. Ref.0 Select 3 : 20/32 */
#define CEREF0_20               (0x0014)       /* Comp. E Int. Ref.0 Select 4 : 21/32 */
#define CEREF0_21               (0x0015)       /* Comp. E Int. Ref.0 Select 5 : 22/32 */
#define CEREF0_22               (0x0016)       /* Comp. E Int. Ref.0 Select 6 : 23/32 */
#define CEREF0_23               (0x0017)       /* Comp. E Int. Ref.0 Select 7 : 24/32 */
#define CEREF0_24               (0x0018)       /* Comp. E Int. Ref.0 Select 0 : 25/32 */
#define CEREF0_25               (0x0019)       /* Comp. E Int. Ref.0 Select 1 : 26/32 */
#define CEREF0_26               (0x001A)       /* Comp. E Int. Ref.0 Select 2 : 27/32 */
#define CEREF0_27               (0x001B)       /* Comp. E Int. Ref.0 Select 3 : 28/32 */
#define CEREF0_28               (0x001C)       /* Comp. E Int. Ref.0 Select 4 : 29/32 */
#define CEREF0_29               (0x001D)       /* Comp. E Int. Ref.0 Select 5 : 30/32 */
#define CEREF0_30               (0x001E)       /* Comp. E Int. Ref.0 Select 6 : 31/32 */
#define CEREF0_31               (0x001F)       /* Comp. E Int. Ref.0 Select 7 : 32/32 */
#define CEREF1_0                (0x0000)       /* Comp. E Int. Ref.1 Select 0 : 1/32 */

#define CEREF1_1                (0x0100)       /* Comp. E Int. Ref.1 Select 1 : 2/32 */
#define CEREF1_2                (0x0200)       /* Comp. E Int. Ref.1 Select 2 : 3/32 */
#define CEREF1_3                (0x0300)       /* Comp. E Int. Ref.1 Select 3 : 4/32 */
#define CEREF1_4                (0x0400)       /* Comp. E Int. Ref.1 Select 4 : 5/32 */
#define CEREF1_5                (0x0500)       /* Comp. E Int. Ref.1 Select 5 : 6/32 */
#define CEREF1_6                (0x0600)       /* Comp. E Int. Ref.1 Select 6 : 7/32 */
#define CEREF1_7                (0x0700)       /* Comp. E Int. Ref.1 Select 7 : 8/32 */
#define CEREF1_8                (0x0800)       /* Comp. E Int. Ref.1 Select 0 : 9/32 */
#define CEREF1_9                (0x0900)       /* Comp. E Int. Ref.1 Select 1 : 10/32 */
#define CEREF1_10               (0x0A00)       /* Comp. E Int. Ref.1 Select 2 : 11/32 */
#define CEREF1_11               (0x0B00)       /* Comp. E Int. Ref.1 Select 3 : 12/32 */
#define CEREF1_12               (0x0C00)       /* Comp. E Int. Ref.1 Select 4 : 13/32 */
#define CEREF1_13               (0x0D00)       /* Comp. E Int. Ref.1 Select 5 : 14/32 */
#define CEREF1_14               (0x0E00)       /* Comp. E Int. Ref.1 Select 6 : 15/32 */
#define CEREF1_15               (0x0F00)       /* Comp. E Int. Ref.1 Select 7 : 16/32 */
#define CEREF1_16               (0x1000)       /* Comp. E Int. Ref.1 Select 0 : 17/32 */
#define CEREF1_17               (0x1100)       /* Comp. E Int. Ref.1 Select 1 : 18/32 */
#define CEREF1_18               (0x1200)       /* Comp. E Int. Ref.1 Select 2 : 19/32 */
#define CEREF1_19               (0x1300)       /* Comp. E Int. Ref.1 Select 3 : 20/32 */
#define CEREF1_20               (0x1400)       /* Comp. E Int. Ref.1 Select 4 : 21/32 */
#define CEREF1_21               (0x1500)       /* Comp. E Int. Ref.1 Select 5 : 22/32 */
#define CEREF1_22               (0x1600)       /* Comp. E Int. Ref.1 Select 6 : 23/32 */
#define CEREF1_23               (0x1700)       /* Comp. E Int. Ref.1 Select 7 : 24/32 */
#define CEREF1_24               (0x1800)       /* Comp. E Int. Ref.1 Select 0 : 25/32 */
#define CEREF1_25               (0x1900)       /* Comp. E Int. Ref.1 Select 1 : 26/32 */
#define CEREF1_26               (0x1A00)       /* Comp. E Int. Ref.1 Select 2 : 27/32 */
#define CEREF1_27               (0x1B00)       /* Comp. E Int. Ref.1 Select 3 : 28/32 */
#define CEREF1_28               (0x1C00)       /* Comp. E Int. Ref.1 Select 4 : 29/32 */
#define CEREF1_29               (0x1D00)       /* Comp. E Int. Ref.1 Select 5 : 30/32 */
#define CEREF1_30               (0x1E00)       /* Comp. E Int. Ref.1 Select 6 : 31/32 */
#define CEREF1_31               (0x1F00)       /* Comp. E Int. Ref.1 Select 7 : 32/32 */

#endif

/************************************************************
* CRC32
************************************************************/

#ifdef __MSP430_HAS_CRC32__

#define CRC32DIB0               CRC32DIW0_L
#define CRC32DIRBB0             CRC32DIRBW0_H
#define CRC32RESB0              CRC32INIRESW0_L
#define CRC32RESB1              CRC32INIRESW0_H
#define CRC32RESB2              CRC32INIRESW1_L
#define CRC32RESB3              CRC32INIRESW1_H
#define CRC32RESRB3             CRC32RESRW1_L
#define CRC32RESRB2             CRC32RESRW1_H
#define CRC32RESRB1             CRC32RESRW0_L
#define CRC32RESRB0             CRC32RESRW0_H
#define CRC16DIB0               CRC16DIW0_L
#define CRC16DIRBB0             CRC16DIRBW0_L
#define CRC16INIRESB1           CRC16INIRESW0_H
#define CRC16INIRESB0           CRC16INIRESW0_L
#define CRC16RESRB1             CRC16RESRW0_L
#define CRC16RESRB0             CRC16RESRW0_H

#endif

/************************************************************
* DIO
************************************************************/

#ifdef __MSP430_HAS_DIO__

#ifdef __MSP430_HAS_PORT1_R__
#define P1IV_NONE       P1IV__NONE
#define P1IV_P1IFG0     P1IV__P1IFG0
#define P1IV_P1IFG1     P1IV__P1IFG1
#define P1IV_P1IFG2     P1IV__P1IFG2
#define P1IV_P1IFG3     P1IV__P1IFG3
#define P1IV_P1IFG4     P1IV__P1IFG4
#define P1IV_P1IFG5     P1IV__P1IFG5
#define P1IV_P1IFG6     P1IV__P1IFG6
#define P1IV_P1IFG7     P1IV__P1IFG7
#endif
#ifdef __MSP430_HAS_PORT2_R__
#define P2IV_NONE       P2IV__NONE
#define P2IV_P2IFG0     P2IV__P2IFG0
#define P2IV_P2IFG1     P2IV__P2IFG1
#define P2IV_P2IFG2     P2IV__P2IFG2
#define P2IV_P2IFG3     P2IV__P2IFG3
#define P2IV_P2IFG4     P2IV__P2IFG4
#define P2IV_P2IFG5     P2IV__P2IFG5
#define P2IV_P2IFG6     P2IV__P2IFG6
#define P2IV_P2IFG7     P2IV__P2IFG7
#endif
#ifdef __MSP430_HAS_PORT3_R__
#define P3IV_NONE       P3IV__NONE
#define P3IV_P3IFG0     P3IV__P3IFG0
#define P3IV_P3IFG1     P3IV__P3IFG1
#define P3IV_P3IFG2     P3IV__P3IFG2
#define P3IV_P3IFG3     P3IV__P3IFG3
#define P3IV_P3IFG4     P3IV__P3IFG4
#define P3IV_P3IFG5     P3IV__P3IFG5
#define P3IV_P3IFG6     P3IV__P3IFG6
#define P3IV_P3IFG7     P3IV__P3IFG7
#endif
#ifdef __MSP430_HAS_PORT4_R__
#define P4IV_NONE       P4IV__NONE
#define P4IV_P4IFG0     P4IV__P4IFG0
#define P4IV_P4IFG1     P4IV__P4IFG1
#define P4IV_P4IFG2     P4IV__P4IFG2
#define P4IV_P4IFG3     P4IV__P4IFG3
#define P4IV_P4IFG4     P4IV__P4IFG4
#define P4IV_P4IFG5     P4IV__P4IFG5
#define P4IV_P4IFG6     P4IV__P4IFG6
#define P4IV_P4IFG7     P4IV__P4IFG7
#endif
#ifdef __MSP430_HAS_PORT5_R__
#define P5IV_NONE       P5IV__NONE
#define P5IV_P5IFG0     P5IV__P5IFG0
#define P5IV_P5IFG1     P5IV__P5IFG1
#define P5IV_P5IFG2     P5IV__P5IFG2
#define P5IV_P5IFG3     P5IV__P5IFG3
#define P5IV_P5IFG4     P5IV__P5IFG4
#define P5IV_P5IFG5     P5IV__P5IFG5
#define P5IV_P5IFG6     P5IV__P5IFG6
#define P5IV_P5IFG7     P5IV__P5IFG7
#endif
#ifdef __MSP430_HAS_PORT6_R__
#define P6IV_NONE       P6IV__NONE
#define P6IV_P6IFG0     P6IV__P6IFG0
#define P6IV_P6IFG1     P6IV__P6IFG1
#define P6IV_P6IFG2     P6IV__P6IFG2
#define P6IV_P6IFG3     P6IV__P6IFG3
#define P6IV_P6IFG4     P6IV__P6IFG4
#define P6IV_P6IFG5     P6IV__P6IFG5
#define P6IV_P6IFG6     P6IV__P6IFG6
#define P6IV_P6IFG7     P6IV__P6IFG7
#endif
#ifdef __MSP430_HAS_PORT7_R__
#define P7IV_NONE       P7IV__NONE
#define P7IV_P7IFG0     P7IV__P7IFG0
#define P7IV_P7IFG1     P7IV__P7IFG1
#define P7IV_P7IFG2     P7IV__P7IFG2
#define P7IV_P7IFG3     P7IV__P7IFG3
#define P7IV_P7IFG4     P7IV__P7IFG4
#define P7IV_P7IFG5     P7IV__P7IFG5
#define P7IV_P7IFG6     P7IV__P7IFG6
#define P7IV_P7IFG7     P7IV__P7IFG7
#endif
#ifdef __MSP430_HAS_PORT8_R__
#define P8IV_NONE       P8IV__NONE
#define P8IV_P8IFG0     P8IV__P8IFG0
#define P8IV_P8IFG1     P8IV__P8IFG1
#define P8IV_P8IFG2     P8IV__P8IFG2
#define P8IV_P8IFG3     P8IV__P8IFG3
#define P8IV_P8IFG4     P8IV__P8IFG4
#define P8IV_P8IFG5     P8IV__P8IFG5
#define P8IV_P8IFG6     P8IV__P8IFG6
#define P8IV_P8IFG7     P8IV__P8IFG7
#endif
#endif

/************************************************************
* DMA
************************************************************/

#ifdef __MSP430_HAS_DMA__

#define __MSP430_HAS_DMAX_6__
#define __MSP430_BASEADDRESS_DMAX_6__ DMA_BASE
    
#define DMAIV_NONE              DMAIV__NONE
#define DMAIV_DMA0IFG           DMAIV__DMA0IFG
#define DMAIV_DMA1IFG           DMAIV__DMA1IFG
#define DMAIV_DMA2IFG           DMAIV__DMA2IFG
#define DMAIV_DMA3IFG           DMAIV__DMA3IFG
#define DMAIV_DMA4IFG           DMAIV__DMA4IFG
#define DMAIV_DMA5IFG           DMAIV__DMA5IFG

#define DMASBDB                 DMASRCBYTE | DMADSTBYTE
#define DMASBDW                 DMASRCBYTE
#define DMASWDB                 DMADSTBYTE
#define DMASWDW                 (0x0000)

#ifdef DMA0CTL
#define DMA0SAL                 DMA0SA_L
#define DMA0SAH                 DMA0SA_H
#define DMA0DAL                 DMA0DA_L
#define DMA0DAH                 DMA0DA_H
#endif
#ifdef DMA1CTL
#define DMA1SAL                 DMA1SA_L
#define DMA1SAH                 DMA1SA_H
#define DMA1DAL                 DMA1DA_L
#define DMA1DAH                 DMA1DA_H
#endif
#ifdef DMA2CTL
#define DMA2SAL                 DMA2SA_L
#define DMA2SAH                 DMA2SA_H
#define DMA2DAL                 DMA2DA_L
#define DMA2DAH                 DMA2DA_H
#endif

#endif

/************************************************************
* ECOMP0
************************************************************/

#ifdef __MSP430_HAS_ECOMP0__
#define ECOMP_BASE              ECOMP0_BASE

#ifdef OFS_CP0CTL0
#define CPCTL0                  CP0CTL0
#define CPCTL0_L                CP0CTL0_L
#define CPCTL0_H                CP0CTL0_H
#define OFS_CPCTL0              OFS_CP0CTL0
#define OFS_CPCTL0_L            OFS_CP0CTL0
#define OFS_CPCTL0_H            OFS_CP0CTL0+1
#endif
#ifdef OFS_CP0CTL1
#define CPCTL1                  CP0CTL1
#define CPCTL1_L                CP0CTL1_L
#define CPCTL1_H                CP0CTL1_H
#define OFS_CPCTL1              OFS_CP0CTL1
#define OFS_CPCTL1_L            OFS_CP0CTL1
#define OFS_CPCTL1_H            OFS_CP0CTL1+1
#endif
#ifdef OFS_CP0INT
#define CPINT                   CP0INT
#define CPINT_L                 CP0INT_L
#define CPINT_H                 CP0INT_H
#define OFS_CPINT               OFS_CP0INT
#define OFS_CPINT_L             OFS_CP0INT
#define OFS_CPINT_H             OFS_CP0INT+1
#endif
#ifdef OFS_CP0IV
// not provided due to possible invalid re-define
//#define CPIV                    CP0IV
//#define CPIV_L                  CP0IV_L
//#define CPIV_H                  CP0IV_H
#define OFS_CPIV                OFS_CP0IV
#define OFS_CPIV_L              OFS_CP0IV_L
#define OFS_CPIV_H              OFS_CP0IV+1
#endif
#ifdef OFS_CP0DACCTL
#define CPDACCTL                CP0DACCTL
#define CPDACCTL_L              CP0DACCTL_L
#define CPDACCTL_H              CP0DACCTL_H
#define OFS_CPDACCTL            OFS_CP0DACCTL
#define OFS_CPDACCTL_L          OFS_CP0DACCTL
#define OFS_CPDACCTL_H          OFS_CP0DACCTL+1
#endif
#ifdef OFS_CP0DACDATA
#define CPDACDATA               CP0DACDATA
#define CPDACDATA_L             CP0DACDATA_L
#define CPDACDATA_H             CP0DACDATA_H
#define OFS_CPDACDATA           OFS_CP0DACDATA
#define OFS_CPDACDATA_L         OFS_CP0DACDATA
#define OFS_CPDACDATA_H         OFS_CP0DACDATA+1
#endif

#endif

/************************************************************
* FRCTL
************************************************************/

#ifdef __MSP430_HAS_FRCTL__

#define __MSP430_HAS_FRAM__
#define __MSP430_HAS_GC__
#define __MSP430_BASEADDRESS_FRAM__ FRCTL_BASE
#define FRAM_BASE               FRCTL_BASE
    
#define FWPW                    FRCTLPW
#define FRPW                    (0x9600)       /* FRAM password returned by read */
#define FXPW                    (0x3300)       /* for use with XOR instruction */

#endif

/************************************************************
* LCD_C
************************************************************/

#ifdef __MSP430_HAS_LCD_C__

#define OFS_LCDBM1              OFS_LCDM33_LCDBM1
#define OFS_LCDBM2              OFS_LCDM34_LCDBM2
#define OFS_LCDBM3              OFS_LCDM35_LCDBM3
#define OFS_LCDBM4              OFS_LCDM36_LCDBM4
#define OFS_LCDBM5              OFS_LCDM37_LCDBM5
#define OFS_LCDBM6              OFS_LCDM38_LCDBM6
#define OFS_LCDBM7              OFS_LCDM39_LCDBM7
#define OFS_LCDBM8              OFS_LCDM40_LCDBM8
#define OFS_LCDBM9              OFS_LCDM41_LCDBM9
#define OFS_LCDBM10             OFS_LCDM42_LCDBM10
#define OFS_LCDBM11             OFS_LCDM43_LCDBM11
#define OFS_LCDBM12             OFS_LCDM44_LCDBM12
#define OFS_LCDBM13             OFS_LCDM45_LCDBM13
#define OFS_LCDBM14             OFS_LCDM46_LCDBM14
#define OFS_LCDBM15             OFS_LCDM47_LCDBM15
#define OFS_LCDBM16             OFS_LCDM48_LCDBM16
#define OFS_LCDBM17             OFS_LCDM49_LCDBM17
#define OFS_LCDBM18             OFS_LCDM50_LCDBM18
#define OFS_LCDBM19             OFS_LCDM51_LCDBM19
#define OFS_LCDBM20             OFS_LCDM52_LCDBM20 

#endif

/************************************************************
* LEA
************************************************************/

#ifdef __MSP430_HAS_LEA__

#define __MSP430_HAS_LEA_SC__
#define __MSP430_BASEADDRESS_LEA_SC__ LEA_BASE
#define LEA_SC_BASE LEA_BASE

#define LEASCCAP                LEACAP
#define LEASCCAPL               LEACAPL
#define LEASCCAPH               LEACAPH
#define LEASCCNF0               LEACNF0
#define LEASCCNF0L              LEACNF0L
#define LEASCCNF0H              LEACNF0H
#define LEASCCNF1               LEACNF1
#define LEASCCNF1L              LEACNF1L
#define LEASCCNF1H              LEACNF1H
#define LEASCCNF2               LEACNF2
#define LEASCCNF2L              LEACNF2L
#define LEASCCNF2H              LEACNF2H
#define LEASCMB                 LEAMB
#define LEASCMBL                LEAMBL
#define LEASCMBH                LEAMBH
#define LEASCMT                 LEAMT
#define LEASCMTL                LEAMTL
#define LEASCMTH                LEAMTH
#define LEASCCMA                LEACMA
#define LEASCCMAL               LEACMAL
#define LEASCCMAH               LEACMAH
#define LEASCCMCTL              LEACMCTL
#define LEASCCMCTLL             LEACMCTLL
#define LEASCCMCTLH             LEACMCTLH
#define LEASCCMDSTAT            LEACMDSTAT
#define LEASCCMDSTATL           LEACMDSTATL
#define LEASCCMDSTATH           LEACMDSTATH
#define LEASCS1STAT             LEAS1STAT
#define LEASCS1STATL            LEAS1STATL
#define LEASCS1STATH            LEAS1STATH
#define LEASCS0STAT             LEAS0STAT
#define LEASCS0STATL            LEAS0STATL
#define LEASCS0STATH            LEAS0STATH
#define LEASCDSTSTAT            LEADSTSTAT
#define LEASCDSTSTATL           LEADSTSTATL
#define LEASCDSTSTATH           LEADSTSTATH
#define LEASCPMCTL              LEAPMCTL
#define LEASCPMCTLL             LEAPMCTLL
#define LEASCPMCTLH             LEAPMCTLH
#define LEASCPMDST              LEAPMDST
#define LEASCPMDSTL             LEAPMDSTL
#define LEASCPMDSTH             LEAPMDSTH
#define LEASCPMS1               LEAPMS1
#define LEASCPMS1L              LEAPMS1L
#define LEASCPMS1H              LEAPMS1H
#define LEASCPMS0               LEAPMS0
#define LEASCPMS0L              LEAPMS0L
#define LEASCPMS0H              LEAPMS0H
#define LEASCPMCB               LEAPMCB
#define LEASCPMCBL              LEAPMCBL
#define LEASCPMCBH              LEAPMCBH
#define LEASCIFGSET             LEAIFGSET
#define LEASCIFGSETL            LEAIFGSETL
#define LEASCIFGSETH            LEAIFGSETH
#define LEASCIE                 LEAIE
#define LEASCIEL                LEAIEL
#define LEASCIEH                LEAIEH
#define LEASCIFG                LEAIFG
#define LEASCIFGL               LEAIFGL
#define LEASCIFGH               LEAIFGH
#define LEASCIV                 LEAIV
#define LEASCIVL                LEAIVL
#define LEASCIVH                LEAIVH

#define OFS_LEASCCAP            OFS_LEACAP
#define OFS_LEASCCAP_L          OFS_LEACAP_L
#define OFS_LEASCCAP_H          OFS_LEACAP_H
#define OFS_LEASCCNF0           OFS_LEACNF0
#define OFS_LEASCCNF0_L         OFS_LEACNF0_L
#define OFS_LEASCCNF0_H         OFS_LEACNF0_H
#define OFS_LEASCCNF1           OFS_LEACNF1
#define OFS_LEASCCNF1_L         OFS_LEACNF1_L
#define OFS_LEASCCNF1_H         OFS_LEACNF1_H
#define OFS_LEASCCNF2           OFS_LEACNF2
#define OFS_LEASCCNF2_L         OFS_LEACNF2_L
#define OFS_LEASCCNF2_H         OFS_LEACNF2_H
#define OFS_LEASCMB             OFS_LEAMB
#define OFS_LEASCMB_L           OFS_LEAMB_L
#define OFS_LEASCMB_H           OFS_LEAMB_H
#define OFS_LEASCMT             OFS_LEAMT
#define OFS_LEASCMT_L           OFS_LEAMT_L
#define OFS_LEASCMT_H           OFS_LEAMT_H
#define OFS_LEASCCMA            OFS_LEACMA
#define OFS_LEASCCMA_L          OFS_LEACMA_L
#define OFS_LEASCCMA_H          OFS_LEACMA_H
#define OFS_LEASCCMCTL          OFS_LEACMCTL
#define OFS_LEASCCMCTL_L        OFS_LEACMCTL_L
#define OFS_LEASCCMCTL_H        OFS_LEACMCTL_H
#define OFS_LEASCCMDSTAT        OFS_LEACMDSTAT
#define OFS_LEASCCMDSTAT_L      OFS_LEACMDSTAT_L
#define OFS_LEASCCMDSTAT_H      OFS_LEACMDSTAT_H
#define OFS_LEASCS1STAT         OFS_LEAS1STAT
#define OFS_LEASCS1STAT_L       OFS_LEAS1STAT_L
#define OFS_LEASCS1STAT_H       OFS_LEAS1STAT_H
#define OFS_LEASCS0STAT         OFS_LEAS0STAT
#define OFS_LEASCS0STAT_L       OFS_LEAS0STAT_L
#define OFS_LEASCS0STAT_H       OFS_LEAS0STAT_H
#define OFS_LEASCDSTSTAT        OFS_LEADSTSTAT
#define OFS_LEASCDSTSTAT_L      OFS_LEADSTSTAT_L
#define OFS_LEASCDSTSTAT_H      OFS_LEADSTSTAT_H
#define OFS_LEASCPMCTL          OFS_LEAPMCTL
#define OFS_LEASCPMCTL_L        OFS_LEAPMCTL_L
#define OFS_LEASCPMCTL_H        OFS_LEAPMCTL_H
#define OFS_LEASCPMDST          OFS_LEAPMDST
#define OFS_LEASCPMDST_L        OFS_LEAPMDST_L
#define OFS_LEASCPMDST_H        OFS_LEAPMDST_H
#define OFS_LEASCPMS1           OFS_LEAPMS1
#define OFS_LEASCPMS1_L         OFS_LEAPMS1_L
#define OFS_LEASCPMS1_H         OFS_LEAPMS1_H
#define OFS_LEASCPMS0           OFS_LEAPMS0
#define OFS_LEASCPMS0_L         OFS_LEAPMS0_L
#define OFS_LEASCPMS0_H         OFS_LEAPMS0_H
#define OFS_LEASCPMCB           OFS_LEAPMCB
#define OFS_LEASCPMCB_L         OFS_LEAPMCB_L
#define OFS_LEASCPMCB_H         OFS_LEAPMCB_H
#define OFS_LEASCIFGSET         OFS_LEAIFGSET
#define OFS_LEASCIFGSET_L       OFS_LEAIFGSET_L
#define OFS_LEASCIFGSET_H       OFS_LEAIFGSET_H
#define OFS_LEASCIE             OFS_LEAIE
#define OFS_LEASCIE_L           OFS_LEAIE_L
#define OFS_LEASCIE_H           OFS_LEAIE_H
#define OFS_LEASCIFG            OFS_LEAIFG
#define OFS_LEASCIFG_L          OFS_LEAIFG_L
#define OFS_LEASCIFG_H          OFS_LEAIFG_H
#define OFS_LEASCIV             OFS_LEAIV
#define OFS_LEASCIV_L           OFS_LEAIV_L
#define OFS_LEASCIV_H           OFS_LEAIV_H

#define LEASCSWRST              LEASWRST
#define LEASCFTHOLD             LEAFTHOLD
#define LEASCFTHOLD_0           LEAFTHOLD_0
#define LEASCFTHOLD_1           LEAFTHOLD_1
#define LEASCLPR                LEALPR
#define LEASCLPR_0              LEALPR_0
#define LEASCLPR_1              LEALPR_1
#define LEASCILPM               LEAILPM
#define LEASCILPM_0             LEAILPM_0
#define LEASCILPM_1             LEAILPM_1
#define LEASCILB                LEAILB
#define LEASCTIMFLTE            LEATIMFLTE
#define LEASCTIMFLT_0           LEATIMFLT_0
#define LEASCTIMFLTE_1          LEATIMFLTE_1
#define LEASCCFLT               LEACFLT
#define LEASCCFLT_0             LEACFLT_0
#define LEASCCFLT_1             LEACFLT_1
#define LEASCMEMFLTE            LEAMEMFLTE
#define LEASCMEMFLTE_0          LEAMEMFLTE_0
#define LEASCMEMFLTE_1          LEAMEMFLTE_1
#define LEASCDONES              LEADONES
#define LEASCFREES              LEAFREES
#define LEASCTIMFLTS            LEATIMFLTS
#define LEASCCFLTS              LEACFLTS
#define LEASCCFLTS_0            LEACFLTS_0
#define LEASCCFLTS_1            LEACFLTS_1
#define LEASCMEMFLTS            LEAMEMFLTS
#define LEASCMEMFLTS_0          LEAMEMFLTS_0
#define LEASCMEMFLTS_1          LEAMEMFLTS_1
#define LEASCTRST               LEATRST
#define LEASCTEN                LEATEN
#define LEASCTISEL0             LEATISEL0
#define LEASCTISEL1             LEATISEL1
#define LEASCTISEL2             LEATISEL2
#define LEASCTISEL3             LEATISEL3
#define LEASCTISEL              LEATISEL
#define LEASCTISEL_0            LEATISEL_0
#define LEASCTISEL_1            LEATISEL_1
#define LEASCTISEL_2            LEATISEL_2
#define LEASCTISEL_3            LEATISEL_3
#define LEASCTISEL_4            LEATISEL_4
#define LEASCTISEL_5            LEATISEL_5
#define LEASCTISEL_6            LEATISEL_6
#define LEASCTISEL_7            LEATISEL_7
#define LEASCTISEL_8            LEATISEL_8
#define LEASCTISEL_9            LEATISEL_9
#define LEASCTISEL_10           LEATISEL_10
#define LEASCTISEL_11           LEATISEL_11
#define LEASCTISEL_12           LEATISEL_12
#define LEASCTISEL_13           LEATISEL_13
#define LEASCTISEL_14           LEATISEL_14
#define LEASCTISEL_15           LEATISEL_15

#define LEASCBUSY               LEABUSY
#define LEASCBUSY_0             LEABUSY_0
#define LEASCBUSY_1             LEABUSY_1
#define LEASCBUSY__READY        LEABUSY__READY
#define LEASCBUSY__BUSY         LEABUSY__BUSY
#define LEASCMODE               LEAMODE
#define LEASCMODE0              LEAMODE0
#define LEASCMODE1              LEAMODE1
#define LEASCMODE2              LEAMODE2
#define LEASCMODE3              LEAMODE3
#define LEASCMODE_0             LEAMODE_0
#define LEASCMODE_1             LEAMODE_1
#define LEASCMODE_2             LEAMODE_2
#define LEASCMODE_3             LEAMODE_3
#define LEASCMODE_4             LEAMODE_4
#define LEASCMODE_5             LEAMODE_5
#define LEASCMODE_6             LEAMODE_6
#define LEASCMODE_7             LEAMODE_7
#define LEASCMODE__OFF          LEAMODE__OFF
#define LEASCMODE__READY        LEAMODE__READY
#define LEASCMODE__RUNS         LEAMODE__RUNS
#define LEASCMODE__RUNR         LEAMODE__RUNR
#define LEASCMODE__RUNA         LEAMODE__RUNA
#define LEASCMODE__NOTIFY       LEAMODE__NOTIFY
#define LEASCMODE__SLEEP        LEAMODE__SLEEP
#define LEASCMODE__RUNL         LEAMODE__RUNL
#define LEASCPWST               LEAPWST
#define LEASCPWST0              LEAPWST0
#define LEASCPWST1              LEAPWST1
#define LEASCPWST2              LEAPWST2
#define LEASCPWST3              LEAPWST3
#define LEASCASST               LEAASST
#define LEASCASST0              LEAASST0
#define LEASCASST1              LEAASST1
#define LEASCASST2              LEAASST2
#define LEASCASST3              LEAASST3
#define LEASCDONEC              LEADONEC
#define LEASCFREEC              LEAFREEC
#define LEASCTIMFLTC            LEATIMFLTC
#define LEASCCFLTC              LEACFLTC
#define LEASCCFLTC_0            LEACFLTC_0
#define LEASCCFLTC_1            LEACFLTC_1
#define LEASCMEMFLTC            LEAMEMFLTC
#define LEASCMEMFLTC_0          LEAMEMFLTC_0
#define LEASCMEMFLTC_1          LEAMEMFLTC_1
#define LEASCRDSTAT             LEARDSTAT
#define LEASCRDSTAT0            LEARDSTAT0
#define LEASCRDSTAT1            LEARDSTAT1
#define LEASCRDSTAT2            LEARDSTAT2
#define LEASCRDSTAT3            LEARDSTAT3
#define LEASCWRSTAT             LEAWRSTAT
#define LEASCWRSTAT0            LEAWRSTAT0
#define LEASCWRSTAT1            LEAWRSTAT1
#define LEASCWRSTAT2            LEAWRSTAT2
#define LEASCWRSTAT3            LEAWRSTAT3

#define LEASCSPTR               LEASPTR
#define LEASCSPTR0              LEASPTR0
#define LEASCSPTR1              LEASPTR1
#define LEASCSPTR2              LEASPTR2
#define LEASCSPTR3              LEASPTR3
#define LEASCSPTR4              LEASPTR4
#define LEASCSPTR5              LEASPTR5
#define LEASCSPTR6              LEASPTR6
#define LEASCSPTR7              LEASPTR7
#define LEASCSPTR8              LEASPTR8
#define LEASCSPTR9              LEASPTR9
#define LEASCSPTR10             LEASPTR10
#define LEASCSPTR11             LEASPTR11
#define LEASCSPTR12             LEASPTR12
#define LEASCSPTR13             LEASPTR13
#define LEASCSPTR14             LEASPTR14
#define LEASCSPTR15             LEASPTR15

#define LEASCMB0                LEAMB0
#define LEASCMB1                LEAMB1
#define LEASCMB2                LEAMB2
#define LEASCMB3                LEAMB3
#define LEASCMB4                LEAMB4
#define LEASCMB5                LEAMB5
#define LEASCMB6                LEAMB6
#define LEASCMB7                LEAMB7
#define LEASCMB8                LEAMB8
#define LEASCMB9                LEAMB9
#define LEASCMB10               LEAMB10
#define LEASCMB11               LEAMB11
#define LEASCMB12               LEAMB12
#define LEASCMB13               LEAMB13
#define LEASCMB14               LEAMB14
#define LEASCMB15               LEAMB15

#define LEASCMT0                LEAMT0
#define LEASCMT1                LEAMT1
#define LEASCMT2                LEAMT2
#define LEASCMT3                LEAMT3
#define LEASCMT4                LEAMT4
#define LEASCMT5                LEAMT5
#define LEASCMT6                LEAMT6
#define LEASCMT7                LEAMT7
#define LEASCMT8                LEAMT8
#define LEASCMT9                LEAMT9
#define LEASCMT10               LEAMT10
#define LEASCMT11               LEAMT11
#define LEASCMT12               LEAMT12
#define LEASCMT13               LEAMT13
#define LEASCMT14               LEAMT14
#define LEASCMT15               LEAMT15

#define LEASCCMDP               LEACMDP
#define LEASCCMDP0              LEACMDP0
#define LEASCCMDP1              LEACMDP1
#define LEASCCMDP2              LEACMDP2
#define LEASCCMDP3              LEACMDP3
#define LEASCCMDP4              LEACMDP4
#define LEASCCMDP5              LEACMDP5
#define LEASCCMDP6              LEACMDP6
#define LEASCCMDP7              LEACMDP7
#define LEASCCMDP8              LEACMDP8
#define LEASCCMDP9              LEACMDP9
#define LEASCCMDP10             LEACMDP10
#define LEASCCMDP11             LEACMDP11
#define LEASCCMDP12             LEACMDP12
#define LEASCCMDP13             LEACMDP13
#define LEASCCMDP14             LEACMDP14
#define LEASCCMDP15             LEACMDP15
#define LEASCCMDP16             LEACMDP16
#define LEASCCMDP17             LEACMDP17
#define LEASCCMDP18             LEACMDP18
#define LEASCCMDP19             LEACMDP19
#define LEASCCMDP20             LEACMDP20
#define LEASCCMDP21             LEACMDP21
#define LEASCCMDP22             LEACMDP22
#define LEASCCMDP23             LEACMDP23
#define LEASCCMDP24             LEACMDP24
#define LEASCCMDP25             LEACMDP25
#define LEASCCMDP26             LEACMDP26
#define LEASCCMDP27             LEACMDP27
#define LEASCCMDP28             LEACMDP28
#define LEASCCMDP29             LEACMDP29
#define LEASCCMDP30             LEACMDP30
#define LEASCCMDP31             LEACMDP31

#define LEASCCMAE               LEACMAE
#define LEASCCMAE_0             LEACMAE_0
#define LEASCCMAE_1             LEACMAE_1
#define LEASCINC                LEAINC
#define LEASCDEC                LEADEC
#define LEASCCROFF              LEACROFF
#define LEASCCROFF0             LEACROFF0
#define LEASCCROFF1             LEACROFF1
#define LEASCCROFF_0            LEACROFF_0
#define LEASCCROFF_1            LEACROFF_1
#define LEASCCROFF_2            LEACROFF_2
#define LEASCCROFF_3            LEACROFF_3

#define LEASCCRACTION           LEACRACTION
#define LEASCCMAP               LEACMAP
#define LEASCCMAP0              LEACMAP0
#define LEASCCMAP1              LEACMAP1
#define LEASCCMAP2              LEACMAP2
#define LEASCCMAP3              LEACMAP3
#define LEASCCMAP4              LEACMAP4
#define LEASCCMAP5              LEACMAP5
#define LEASCCMAP6              LEACMAP6
#define LEASCCMAP7              LEACMAP7
#define LEASCCMAP8              LEACMAP8
#define LEASCCMAP9              LEACMAP9
#define LEASCCMAP10             LEACMAP10
#define LEASCCMAP11             LEACMAP11
#define LEASCCMAP12             LEACMAP12
#define LEASCCMAP13             LEACMAP13
#define LEASCCMAP14             LEACMAP14
#define LEASCCMAP15             LEACMAP15

#define LEASCITFLG              LEAITFLG
#define LEASCITFLG0             LEAITFLG0
#define LEASCITFLG1             LEAITFLG1
#define LEASCITFLG_0            LEAITFLG_0
#define LEASCITFLG_1            LEAITFLG_1
#define LEASCITFLG_2            LEAITFLG_2
#define LEASCITFLG_3            LEAITFLG_3
#define LEASCCMD                LEACMD
#define LEASCCMD0               LEACMD0
#define LEASCCMD1               LEACMD1
#define LEASCCMD2               LEACMD2
#define LEASCCMD3               LEACMD3
#define LEASCCMD4               LEACMD4
#define LEASCCMD5               LEACMD5
#define LEASCCMD6               LEACMD6
#define LEASCCMD7               LEACMD7
#define LEASCCMD_0              LEACMD_0
#define LEASCCMD_2              LEACMD_2
#define LEASCCMD_4              LEACMD_4
#define LEASCCMD_6              LEACMD_6
#define LEASCCMD_8              LEACMD_8
#define LEASCCMD_10             LEACMD_10
#define LEASCCMD_12             LEACMD_12
#define LEASCCMD_13             LEACMD_13
#define LEASCCMD_15             LEACMD_15
#define LEASCCMD_16             LEACMD_16
#define LEASCCMD_17             LEACMD_17
#define LEASCCMD_18             LEACMD_18
#define LEASCCMD_19             LEACMD_19
#define LEASCCMD_21             LEACMD_21
#define LEASCCMD_22             LEACMD_22
#define LEASCCMD_23             LEACMD_23
#define LEASCCMD_24             LEACMD_24
#define LEASCCMD_25             LEACMD_25
#define LEASCCMD_26             LEACMD_26
#define LEASCCMD_27             LEACMD_27
#define LEASCCMD_28             LEACMD_28
#define LEASCCMD_29             LEACMD_29
#define LEASCCMD_30             LEACMD_30
#define LEASCCMD_31             LEACMD_31
#define LEASCCMD_32             LEACMD_32
#define LEASCCMD_33             LEACMD_33
#define LEASCCMD_34             LEACMD_34
#define LEASCCMD_36             LEACMD_36
#define LEASCCMD_39             LEACMD_39
#define LEASCCMD_41             LEACMD_41
#define LEASCCMD_43             LEACMD_43
#define LEASCCMD_45             LEACMD_45
#define LEASCCMD_47             LEACMD_47
#define LEASCCMD_48             LEACMD_48
#define LEASCCMD_50             LEACMD_50
#define LEASCCMD_52             LEACMD_52
#define LEASCCMD_53             LEACMD_53
#define LEASCCMD_54             LEACMD_54
#define LEASCCMD_55             LEACMD_55
#define LEASCCMD_56             LEACMD_56
#define LEASCCMD_58             LEACMD_58
#define LEASCCMD_59             LEACMD_59
#define LEASCCMD_64             LEACMD_64
#define LEASCCMD_65             LEACMD_65
#define LEASCCMD_66             LEACMD_66
#define LEASCCMD_67             LEACMD_67
#define LEASCCMD_68             LEACMD_68
#define LEASCCMD_69             LEACMD_69
#define LEASCCMD_70             LEACMD_70
#define LEASCCMD_71             LEACMD_71
#define LEASCCMD_73             LEACMD_73
#define LEASCCMD_75             LEACMD_75
#define LEASCCMD_76             LEACMD_76
#define LEASCCMD_77             LEACMD_77
#define LEASCCMD_78             LEACMD_78
#define LEASCCMD_79             LEACMD_79
#define LEASCCMD__SUSPEND                LEACMD__SUSPEND
#define LEASCCMD__RESUME                 LEACMD__RESUME
#define LEASCCMD__FFTCOMPLEXFIXEDSCALING LEACMD__FFTCOMPLEXFIXEDSCALING
#define LEASCCMD__FIR                    LEACMD__FIR
#define LEASCCMD__POLYNOMIAL             LEACMD__POLYNOMIAL
#define LEASCCMD__FFT                    LEACMD__FFT
#define LEASCCMD__POLYNOMIALLONG         LEACMD__POLYNOMIALLONG
#define LEASCCMD__MPYMATRIXROW           LEACMD__MPYMATRIXROW
#define LEASCCMD__MPYMATRIX              LEACMD__MPYMATRIX
#define LEASCCMD__ADDMATRIX              LEACMD__ADDMATRIX
#define LEASCCMD__MAXMATRIX              LEACMD__MAXMATRIX
#define LEASCCMD__MINMATRIX              LEACMD__MINMATRIX
#define LEASCCMD__IIRBQ1                 LEACMD__IIRBQ1
#define LEASCCMD__MAC                    LEACMD__MAC
#define LEASCCMD__DEINTERLEAVEEVENEVEN   LEACMD__DEINTERLEAVEEVENEVEN
#define LEASCCMD__DEINTERLEAVEEVENODD    LEACMD__DEINTERLEAVEEVENODD
#define LEASCCMD__DEINTERLEAVEODDEVEN    LEACMD__DEINTERLEAVEODDEVEN
#define LEASCCMD__DEINTERLEAVEODDODD     LEACMD__DEINTERLEAVEODDODD
#define LEASCCMD__MACCOMPLEXMATRIX       LEACMD__MACCOMPLEXMATRIX
#define LEASCCMD__MACCOMPLEXCONJUGATEMATRIX LEACMD__MACCOMPLEXCONJUGATEMATRIX
#define LEASCCMD__SUBMATRIX              LEACMD__SUBMATRIX
#define LEASCCMD__MPYLONGMATRIX          LEACMD__MPYLONGMATRIX
#define LEASCCMD__MPYCOMPLEXMATRIX       LEACMD__MPYCOMPLEXMATRIX
#define LEASCCMD__ADDLONGMATRIX          LEACMD__ADDLONGMATRIX
#define LEASCCMD__MOVELONGLIST           LEACMD__MOVELONGLIST
#define LEASCCMD__BITREVERSECOMPLEXEVEN  LEACMD__BITREVERSECOMPLEXEVEN
#define LEASCCMD__BITREVERSECOMPLEXODD   LEACMD__BITREVERSECOMPLEXODD
#define LEASCCMD__IIRBQ2EXTENDED         LEACMD__IIRBQ2EXTENDED
#define LEASCCMD__FFTCOMPLEXLONG         LEACMD__FFTCOMPLEXLONG
#define LEASCCMD__FFTLONG                LEACMD__FFTLONG
#define LEASCCMD__BITREVERSECOMPLEXLONGEVEN LEACMD__BITREVERSECOMPLEXLONGEVEN
#define LEASCCMD__BITREVERSECOMPLEXLONGODD LEACMD__BITREVERSECOMPLEXLONGODD
#define LEASCCMD__POLYNOMIALSCALAR       LEACMD__POLYNOMIALSCALAR
#define LEASCCMD__FFTCOMPLEXAUTOSCALING  LEACMD__FFTCOMPLEXAUTOSCALING
#define LEASCCMD__FIRLONG                LEACMD__FIRLONG
#define LEASCCMD__MACLONGMATRIX          LEACMD__MACLONGMATRIX
#define LEASCCMD__SUBLONGMATRIX          LEACMD__SUBLONGMATRIX
#define LEASCCMD__MAXLONGMATRIX          LEACMD__MAXLONGMATRIX
#define LEASCCMD__MINLONGMATRIX          LEACMD__MINLONGMATRIX
#define LEASCCMD__FIRCOMPLEX             LEACMD__FIRCOMPLEX
#define LEASCCMD__MAXUNSIGNEDMATRIX      LEACMD__MAXUNSIGNEDMATRIX
#define LEASCCMD__MINUNSIGNEDMATRIX      LEACMD__MINUNSIGNEDMATRIX
#define LEASCCMD__MACMATRIX              LEACMD__MACMATRIX
#define LEASCCMD__MAX                    LEACMD__MAX
#define LEASCCMD__MIN                    LEACMD__MIN
#define LEASCCMD__MAXUNSIGNED            LEACMD__MAXUNSIGNED
#define LEASCCMD__MINUNSIGNED            LEACMD__MINUNSIGNED
#define LEASCCMD__MAXUNSIGNEDLONGMATRIX  LEACMD__MAXUNSIGNEDLONGMATRIX
#define LEASCCMD__MINUNSIGNEDLONGMATRIX  LEACMD__MINUNSIGNEDLONGMATRIX
#define LEASCCMD__IIRBQ2                 LEACMD__IIRBQ2
#define LEASCCMD__FIRCOMPLEXLONG         LEACMD__FIRCOMPLEXLONG
#define LEASCCMD__DEINTERLEAVELONG       LEACMD__DEINTERLEAVELONG
#define LEASCCMD__WINDOW                 LEACMD__WINDOW
#define LEASCCMD__MAC3                   LEACMD__MAC3
#define LEASCCMD__SCALEDMAC              LEACMD__SCALEDMAC
#define LEASCCMD__SCALEDFIR              LEACMD__SCALEDFIR

#define LEASCS1VAL              LEAS1VAL
#define LEASCS1VAL0             LEAS1VAL0
#define LEASCS1VAL1             LEAS1VAL1
#define LEASCS1VAL2             LEAS1VAL2
#define LEASCS1VAL3             LEAS1VAL3
#define LEASCS1VAL4             LEAS1VAL4
#define LEASCS1VAL5             LEAS1VAL5
#define LEASCS1VAL6             LEAS1VAL6
#define LEASCS1VAL7             LEAS1VAL7
#define LEASCS1VAL8             LEAS1VAL8
#define LEASCS1VAL9             LEAS1VAL9
#define LEASCS1VAL10            LEAS1VAL10
#define LEASCS1VAL11            LEAS1VAL11
#define LEASCS1VAL12            LEAS1VAL12
#define LEASCS1VAL13            LEAS1VAL13
#define LEASCS1VAL14            LEAS1VAL14
#define LEASCS1VAL15            LEAS1VAL15
#define LEASCS1VAL16            LEAS1VAL16
#define LEASCS1VAL17            LEAS1VAL17
#define LEASCS1VAL18            LEAS1VAL18
#define LEASCS1VAL19            LEAS1VAL19
#define LEASCS1VAL20            LEAS1VAL20
#define LEASCS1VAL21            LEAS1VAL21
#define LEASCS1VAL22            LEAS1VAL22
#define LEASCS1VAL23            LEAS1VAL23
#define LEASCS1VAL24            LEAS1VAL24
#define LEASCS1VAL25            LEAS1VAL25
#define LEASCS1VAL26            LEAS1VAL26
#define LEASCS1VAL27            LEAS1VAL27
#define LEASCS1VAL28            LEAS1VAL28
#define LEASCS1VAL29            LEAS1VAL29
#define LEASCS1VAL30            LEAS1VAL30
#define LEASCS1VAL31            LEAS1VAL31

#define LEASCS0VAL              LEAS0VAL
#define LEASCS0VAL0             LEAS0VAL0
#define LEASCS0VAL1             LEAS0VAL1
#define LEASCS0VAL2             LEAS0VAL2
#define LEASCS0VAL3             LEAS0VAL3
#define LEASCS0VAL4             LEAS0VAL4
#define LEASCS0VAL5             LEAS0VAL5
#define LEASCS0VAL6             LEAS0VAL6
#define LEASCS0VAL7             LEAS0VAL7
#define LEASCS0VAL8             LEAS0VAL8
#define LEASCS0VAL9             LEAS0VAL9
#define LEASCS0VAL10            LEAS0VAL10
#define LEASCS0VAL11            LEAS0VAL11
#define LEASCS0VAL12            LEAS0VAL12
#define LEASCS0VAL13            LEAS0VAL13
#define LEASCS0VAL14            LEAS0VAL14
#define LEASCS0VAL15            LEAS0VAL15
#define LEASCS0VAL16            LEAS0VAL16
#define LEASCS0VAL17            LEAS0VAL17
#define LEASCS0VAL18            LEAS0VAL18
#define LEASCS0VAL19            LEAS0VAL19
#define LEASCS0VAL20            LEAS0VAL20
#define LEASCS0VAL21            LEAS0VAL21
#define LEASCS0VAL22            LEAS0VAL22
#define LEASCS0VAL23            LEAS0VAL23
#define LEASCS0VAL24            LEAS0VAL24
#define LEASCS0VAL25            LEAS0VAL25
#define LEASCS0VAL26            LEAS0VAL26
#define LEASCS0VAL27            LEAS0VAL27
#define LEASCS0VAL28            LEAS0VAL28
#define LEASCS0VAL29            LEAS0VAL29
#define LEASCS0VAL30            LEAS0VAL30
#define LEASCS0VAL31            LEAS0VAL31

#define LEASCCMDEN              LEACMDEN
#define LEASCCMDEN_0            LEACMDEN_0
#define LEASCCMDEN_1            LEACMDEN_1
#define LEASCTRG                LEATRG

#define LEASCCTX                LEACTX
#define LEASCCTX0               LEACTX0
#define LEASCCTX1               LEACTX1
#define LEASCCTX2               LEACTX2
#define LEASCCTX3               LEACTX3
#define LEASCCTX4               LEACTX4
#define LEASCCTX5               LEACTX5
#define LEASCCTX6               LEACTX6
#define LEASCCTX7               LEACTX7
#define LEASCCTX8               LEACTX8
#define LEASCCTX9               LEACTX9
#define LEASCCTX10              LEACTX10
#define LEASCCTX11              LEACTX11

#define LEASCCOVLIS             LEACOVLIS
#define LEASCCOVLIS_0           LEACOVLIS_0
#define LEASCCOVLIS_1           LEACOVLIS_1
#define LEASCTIS                LEATIS
#define LEASCTIS_0              LEATIS_0
#define LEASCTIS_1              LEATIS_1
#define LEASCOORIS              LEAOORIS
#define LEASCOORIS_0            LEAOORIS_0
#define LEASCOORIS_1            LEAOORIS_1
#define LEASCSDIIS              LEASDIIS
#define LEASCSDIIS_0            LEASDIIS_0
#define LEASCSDIIS_1            LEASDIIS_1
#define LEASCPMCMDIS            LEAPMCMDIS
#define LEASCPMCMDIS_0          LEAPMCMDIS_0
#define LEASCPMCMDIS_1          LEAPMCMDIS_1

#define LEASCCOVLIE             LEACOVLIE
#define LEASCCOVLIE_0           LEACOVLIE_0
#define LEASCCOVLIE_1           LEACOVLIE_1
#define LEASCTIE                LEATIE
#define LEASCTIE_0              LEATIE_0
#define LEASCTIE_1              LEATIE_1
#define LEASCOORIE              LEAOORIE
#define LEASCOORIE_0            LEAOORIE_0
#define LEASCOORIE_1            LEAOORIE_1
#define LEASCSDIIE              LEASDIIE
#define LEASCSDIIE_0            LEASDIIE_0
#define LEASCSDIIE_1            LEASDIIE_1
#define LEASCPMCMDIE            LEAPMCMDIE
#define LEASCPMCMDIE_0          LEAPMCMDIE_0
#define LEASCPMCMDIE_1          LEAPMCMDIE_1

#define LEASCCOVLIFG            LEACOVLIFG
#define LEASCCOVLIFG_0          LEACOVLIFG_0
#define LEASCCOVLIFG_1          LEACOVLIFG_1
#define LEASCTIFG               LEATIFG
#define LEASCTIFG_0             LEATIFG_0
#define LEASCTIFG_1             LEATIFG_1
#define LEASCOORIFG             LEAOORIFG
#define LEASCOORIFG_0           LEAOORIFG_0
#define LEASCOORIFG_1           LEAOORIFG_1
#define LEASCSDIIFG             LEASDIIFG
#define LEASCSDIIFG_0           LEASDIIFG_0
#define LEASCSDIIFG_1           LEASDIIFG_1
#define LEASCPMCMDIFG           LEAPMCMDIFG
#define LEASCPMCMDIFG_0         LEAPMCMDIFG_0
#define LEASCPMCMDIFG_1         LEAPMCMDIFG_1

#define LEASCIV0                LEAIV0
#define LEASCIV1                LEAIV1
#define LEASCIV2                LEAIV2
#define LEASCIV3                LEAIV3
#define LEASCIV4                LEAIV4
#define LEASCIV5                LEAIV5
#define LEASCIV6                LEAIV6
#define LEASCIV7                LEAIV7
#define LEASCIV_0               LEAIV_0
#define LEASCIV_2               LEAIV_2
#define LEASCIV_4               LEAIV_4
#define LEASCIV_6               LEAIV_6
#define LEASCIV_8               LEAIV_8
#define LEASCIV_10              LEAIV_10
#define LEASCIV__NONE           LEAIV__NONE
#define LEASCIV__COVLIFG        LEAIV__COVLIFG
#define LEASCIV__TIFG           LEAIV__TIFG
#define LEASCIV__OORIFG         LEAIV__OORIFG
#define LEASCIV__SDIIFG         LEAIV__SDIIFG
#define LEASCIV__PMCMDIFG       LEAIV__PMCMDIFG

#ifdef LEA_VECTOR
#define LEA_SC_VECTOR           LEA_VECTOR
#endif

#endif

/************************************************************
* MPY32
************************************************************/

#ifdef __MSP430_HAS_MPY32__

#define OP1_32                  MPYOP1_32
#define OP1_32_L                MPYOP1_32_L
#define OP2_32                  MPYOP2_32
#define OP2_32_L                MPYOP1_32_L

#define MPY_B                   MPY_L
#define MPYS_B                  MPYS_L
#define MAC_B                   MAC_L
#define MACS_B                  MACS_L
#define OP2_B                   OP2_L
#define MPY32L_B                MPY32L_L
#define MPY32H_B                MPY32H_L
#define MPYS32L_B               MPYS32L_L
#define MPYS32H_B               MPYS32H_L
#define MAC32L_B                MAC32L_L
#define MAC32H_B                MAC32H_L
#define MACS32L_B               MACS32L_L
#define MACS32H_B               MACS32H_L
#define OP2L_B                  OP2L_L
#define OP2H_B                  OP2H_L

#endif

/************************************************************
* PMM
************************************************************/

#ifdef __MSP430_HAS_PMM__
#define __MSP430_HAS_PMM_FRAM__
#endif

/************************************************************
* RAMCTL
************************************************************/

#ifdef __MSP430_HAS_RAMCTL__
#define RAM_BASE                RAMCTL_BASE
#define __MSP430_BASEADDRESS_RC_FRAM__ RAMCTL_BASE
#define __MSP430_HAS_RC_FRAM__
#endif

/************************************************************
* RTC
************************************************************/

#ifdef __MSP430_HAS_RTC__

#define RTCIV_NONE              RTCIV__NONE
#define RTCIV_RTCIF             RTCIV__RTCIFG

#define RTCIF                   RTCIFG
#define RTCIF_L                 RTCIFG_L

#endif

/************************************************************
* RTC_C
************************************************************/

#ifdef __MSP430_HAS_RTC_C__

#define RTCPWD                  RTCCTL0_H
#define RTCCTL1                 RTCCTL13_L
#define RTCCTL3                 RTCCTL13_H

#define RTC_VECTOR              RTC_C_VECTOR

#define RTC_NONE                RTCIV__NONE
#define RTC_RTCOFIFG            RTCIV__RTCOFIFG
#define RTC_RTCRDYIFG           RTCIV__RTCRDYIFG
#define RTC_RTCTEVIFG           RTCIV__RTCTEVIFG
#define RTC_RTCAIFG             RTCIV__RTCAIFG
#define RTC_RT0PSIFG            RTCIV__RT0PSIFG
#define RTC_RT1PSIFG            RTCIV__RT1PSIFG

#define RTCIV_NONE              RTCIV__NONE
#define RTCIV_RTCOFIFG          RTCIV__RTCOFIFG
#define RTCIV_RTCRDYIFG         RTCIV__RTCRDYIFG
#define RTCIV_RTCTEVIFG         RTCIV__RTCTEVIFG
#define RTCIV_RTCIF             RTCIV__RTCIFG
#define RTCIV_RTCAIFG           RTCIV__RTCAIFG
#define RTCIV_RT0PSIFG          RTCIV__RT0PSIFG
#define RTCIV_RT1PSIFG          RTCIV__RT1PSIFG

#define RTCCLKSEL               RTCCKSEL
#define RTCCLKSEL_H             RTCCKSEL_H

#define RTCAE                   (0x0080)

#define RTCSEC                  RTCTIM0_L
#define RTCMIN                  RTCTIM0_H
#define RTCHOUR                 RTCTIM1_L
#define RTCDOW                  RTCTIM1_H
#define RTCDAY                  RTCDATE_L
#define RTCMON                  RTCDATE_H
#define RTCYEARL                RTCYEAR_L
#define RT0PS                   RTCPS_L
#define RT1PS                   RTCPS_H
#define RTCAMIN                 RTCAMINHR_L
#define RTCAHOUR                RTCAMINHR_H
#define RTCADOW                 RTCADOWDAY_L
#define RTCADAY                 RTCADOWDAY_H

#endif

/************************************************************
* SAPH_A
************************************************************/

#ifdef __MSP430_HAS_SAPH_A__

#define SAPHIIDX                SAPH_AIIDX
#define SAPHIIDX_L              SAPH_AIIDX_L
#define SAPHIIDX_H              SAPH_AIIDX_H
#define SAPHMIS                 SAPH_AMIS
#define SAPHMIS_L               SAPH_AMIS_L
#define SAPHMIS_H               SAPH_AMIS_H
#define SAPHRIS                 SAPH_ARIS
#define SAPHRIS_L               SAPH_ARIS_L
#define SAPHRIS_H               SAPH_ARIS_H
#define SAPHIMSC                SAPH_AIMSC
#define SAPHIMSC_L              SAPH_AIMSC_L
#define SAPHIMSC_H              SAPH_AIMSC_H
#define SAPHICR                 SAPH_AICR
#define SAPHICR_L               SAPH_AICR_L
#define SAPHICR_H               SAPH_AICR_H
#define SAPHISR                 SAPH_AISR
#define SAPHISR_L               SAPH_AISR_L
#define SAPHISR_H               SAPH_AISR_H
#define SAPHDESCLO              SAPH_ADESCLO
#define SAPHDESCLO_L            SAPH_ADESCLO_L
#define SAPHDESCLO_H            SAPH_ADESCLO_H
#define SAPHDESCHI              SAPH_ADESCHI
#define SAPHDESCHI_L            SAPH_ADESCHI_L
#define SAPHDESCHI_H            SAPH_ADESCHI_H
#define SAPHKEY                 SAPH_AKEY
#define SAPHKEY_L               SAPH_AKEY_L
#define SAPHKEY_H               SAPH_AKEY_H
#define SAPHOCTL0               SAPH_AOCTL0
#define SAPHOCTL0_L             SAPH_AOCTL0_L
#define SAPHOCTL0_H             SAPH_AOCTL0_H
#define SAPHOCTL1               SAPH_AOCTL1
#define SAPHOCTL1_L             SAPH_AOCTL1_L
#define SAPHOCTL1_H             SAPH_AOCTL1_H
#define SAPHOSEL                SAPH_AOSEL
#define SAPHOSEL_L              SAPH_AOSEL_L
#define SAPHOSEL_H              SAPH_AOSEL_H
#define SAPHCH0PUT              SAPH_ACH0PUT
#define SAPHCH0PUT_L            SAPH_ACH0PUT_L
#define SAPHCH0PUT_H            SAPH_ACH0PUT_H
#define SAPHCH0PDT              SAPH_ACH0PDT
#define SAPHCH0PDT_L            SAPH_ACH0PDT_L
#define SAPHCH0PDT_H            SAPH_ACH0PDT_H
#define SAPHCH0TT               SAPH_ACH0TT
#define SAPHCH0TT_L             SAPH_ACH0TT_L
#define SAPHCH0TT_H             SAPH_ACH0TT_H
#define SAPHCH1PUT              SAPH_ACH1PUT
#define SAPHCH1PUT_L            SAPH_ACH1PUT_L
#define SAPHCH1PUT_H            SAPH_ACH1PUT_H
#define SAPHCH1PDT              SAPH_ACH1PDT
#define SAPHCH1PDT_L            SAPH_ACH1PDT_L
#define SAPHCH1PDT_H            SAPH_ACH1PDT_H
#define SAPHCH1TT               SAPH_ACH1TT
#define SAPHCH1TT_L             SAPH_ACH1TT_L
#define SAPHCH1TT_H             SAPH_ACH1TT_H
#define SAPHMCNF                SAPH_AMCNF
#define SAPHMCNF_L              SAPH_AMCNF_L
#define SAPHMCNF_H              SAPH_AMCNF_H
#define SAPHTACTL               SAPH_ATACTL
#define SAPHTACTL_L             SAPH_ATACTL_L
#define SAPHTACTL_H             SAPH_ATACTL_H
#define SAPHICTL0               SAPH_AICTL0
#define SAPHICTL0_L             SAPH_AICTL0_L
#define SAPHICTL0_H             SAPH_AICTL0_H
#define SAPHBCTL                SAPH_ABCTL
#define SAPHBCTL_L              SAPH_ABCTL_L
#define SAPHBCTL_H              SAPH_ABCTL_H
#define SAPHPGC                 SAPH_APGC
#define SAPHPGC_L               SAPH_APGC_L
#define SAPHPGC_H               SAPH_APGC_H
#define SAPHPGLPER              SAPH_APGLPER
#define SAPHPGLPER_L            SAPH_APGLPER_L
#define SAPHPGLPER_H            SAPH_APGLPER_H
#define SAPHPGHPER              SAPH_APGHPER
#define SAPHPGHPER_L            SAPH_APGHPER_L
#define SAPHPGHPER_H            SAPH_APGHPER_H
#define SAPHPGCTL               SAPH_APGCTL
#define SAPHPGCTL_L             SAPH_APGCTL_L
#define SAPHPGCTL_H             SAPH_APGCTL_H
#define SAPHPPGTRIG             SAPH_APPGTRIG
#define SAPHPPGTRIG_L           SAPH_APPGTRIG_L
#define SAPHPPGTRIG_H           SAPH_APPGTRIG_H
#define SAPHXPGCTL              SAPH_AXPGCTL
#define SAPHXPGCTL_L            SAPH_AXPGCTL_L
#define SAPHXPGCTL_H            SAPH_AXPGCTL_H
#define SAPHXPGLPER             SAPH_AXPGLPER
#define SAPHXPGLPER_L           SAPH_AXPGLPER_L
#define SAPHXPGLPER_H           SAPH_AXPGLPER_H
#define SAPHXPGHPER             SAPH_AXPGHPER
#define SAPHXPGHPER_L           SAPH_AXPGHPER_L
#define SAPHXPGHPER_H           SAPH_AXPGHPER_H
#define SAPHASCTL0              SAPH_AASCTL0
#define SAPHASCTL0_L            SAPH_AASCTL0_L
#define SAPHASCTL0_H            SAPH_AASCTL0_H
#define SAPHASCTL1              SAPH_AASCTL1
#define SAPHASCTL1_L            SAPH_AASCTL1_L
#define SAPHASCTL1_H            SAPH_AASCTL1_H
#define SAPHASQTRIG             SAPH_AASQTRIG
#define SAPHAPOL                SAPH_AAPOL
#define SAPHAPOL_L              SAPH_AAPOL_L
#define SAPHAPOL_H              SAPH_AAPOL_H
#define SAPHAPLEV               SAPH_AAPLEV
#define SAPHAPLEV_L             SAPH_AAPLEV_L
#define SAPHAPLEV_H             SAPH_AAPLEV_H
#define SAPHAPHIZ               SAPH_AAPHIZ
#define SAPHAPHIZ_L             SAPH_AAPHIZ_L
#define SAPHAPHIZ_H             SAPH_AAPHIZ_H
#define SAPHATM_A               SAPH_AATM_A
#define SAPHATM_A_L             SAPH_AATM_A_L
#define SAPHATM_A_H             SAPH_AATM_A_H
#define SAPHATM_B               SAPH_AATM_B
#define SAPHATM_B_L             SAPH_AATM_B_L
#define SAPHATM_B_H             SAPH_AATM_B_H
#define SAPHATM_C               SAPH_AATM_C
#define SAPHATM_C_L             SAPH_AATM_C_L
#define SAPHATM_C_H             SAPH_AATM_C_H
#define SAPHATM_D               SAPH_AATM_D
#define SAPHATM_D_L             SAPH_AATM_D_L
#define SAPHATM_D_H             SAPH_AATM_D_H
#define SAPHATM_E               SAPH_AATM_E
#define SAPHATM_E_L             SAPH_AATM_E_L
#define SAPHATM_E_H             SAPH_AATM_E_H
#define SAPHATM_F               SAPH_AATM_F
#define SAPHATM_F_L             SAPH_AATM_F_L
#define SAPHATM_F_H             SAPH_AATM_F_H
#define SAPHTBCTL               SAPH_ATBCTL
#define SAPHTBCTL_L             SAPH_ATBCTL_L
#define SAPHTBCTL_H             SAPH_ATBCTL_H
#define SAPHATIMLO              SAPH_AATIMLO
#define SAPHATIMLO_L            SAPH_AATIMLO_L
#define SAPHATIMLO_H            SAPH_AATIMLO_H
#define SAPHATIMHI              SAPH_AATIMHI
#define SAPHATIMHI_L            SAPH_AATIMHI_L
#define SAPHATIMHI_H            SAPH_AATIMHI_H

#define OFS_SAPHIIDX           OFS_SAPH_AIIDX
#define OFS_SAPHIIDX_L         OFS_SAPH_AIIDX
#define OFS_SAPHIIDX_H         OFS_SAPH_AIIDX+1
#define OFS_SAPHMIS            OFS_SAPH_AMIS
#define OFS_SAPHMIS_L          OFS_SAPH_AMIS
#define OFS_SAPHMIS_H          OFS_SAPH_AMIS+1
#define OFS_SAPHRIS            OFS_SAPH_ARIS
#define OFS_SAPHRIS_L          OFS_SAPH_ARIS
#define OFS_SAPHRIS_H          OFS_SAPH_ARIS+1
#define OFS_SAPHIMSC           OFS_SAPH_AIMSC
#define OFS_SAPHIMSC_L         OFS_SAPH_AIMSC
#define OFS_SAPHIMSC_H         OFS_SAPH_AIMSC+1
#define OFS_SAPHICR            OFS_SAPH_AICR
#define OFS_SAPHICR_L          OFS_SAPH_AICR
#define OFS_SAPHICR_H          OFS_SAPH_AICR+1
#define OFS_SAPHISR            OFS_SAPH_AISR
#define OFS_SAPHISR_L          OFS_SAPH_AISR
#define OFS_SAPHISR_H          OFS_SAPH_AISR+1
#define OFS_SAPHDESCLO         OFS_SAPH_ADESCLO
#define OFS_SAPHDESCLO_L       OFS_SAPH_ADESCLO
#define OFS_SAPHDESCLO_H       OFS_SAPH_ADESCLO+1
#define OFS_SAPHDESCHI         OFS_SAPH_ADESCHI
#define OFS_SAPHDESCHI_L       OFS_SAPH_ADESCHI
#define OFS_SAPHDESCHI_H       OFS_SAPH_ADESCHI+1
#define OFS_SAPHKEY            OFS_SAPH_AKEY
#define OFS_SAPHKEY_L          OFS_SAPH_AKEY
#define OFS_SAPHKEY_H          OFS_SAPH_AKEY+1
#define OFS_SAPHOCTL0          OFS_SAPH_AOCTL0
#define OFS_SAPHOCTL0_L        OFS_SAPH_AOCTL0
#define OFS_SAPHOCTL0_H        OFS_SAPH_AOCTL0+1
#define OFS_SAPHOCTL1          OFS_SAPH_AOCTL1
#define OFS_SAPHOCTL1_L        OFS_SAPH_AOCTL1
#define OFS_SAPHOCTL1_H        OFS_SAPH_AOCTL1+1
#define OFS_SAPHOSEL           OFS_SAPH_AOSEL
#define OFS_SAPHOSEL_L         OFS_SAPH_AOSEL
#define OFS_SAPHOSEL_H         OFS_SAPH_AOSEL+1
#define OFS_SAPHCH0PUT         OFS_SAPH_ACH0PUT
#define OFS_SAPHCH0PUT_L       OFS_SAPH_ACH0PUT
#define OFS_SAPHCH0PUT_H       OFS_SAPH_ACH0PUT+1
#define OFS_SAPHCH0PDT         OFS_SAPH_ACH0PDT
#define OFS_SAPHCH0PDT_L       OFS_SAPH_ACH0PDT
#define OFS_SAPHCH0PDT_H       OFS_SAPH_ACH0PDT+1
#define OFS_SAPHCH0TT          OFS_SAPH_ACH0TT
#define OFS_SAPHCH0TT_L        OFS_SAPH_ACH0TT
#define OFS_SAPHCH0TT_H        OFS_SAPH_ACH0TT+1
#define OFS_SAPHCH1PUT         OFS_SAPH_ACH1PUT
#define OFS_SAPHCH1PUT_L       OFS_SAPH_ACH1PUT
#define OFS_SAPHCH1PUT_H       OFS_SAPH_ACH1PUT+1
#define OFS_SAPHCH1PDT         OFS_SAPH_ACH1PDT
#define OFS_SAPHCH1PDT_L       OFS_SAPH_ACH1PDT
#define OFS_SAPHCH1PDT_H       OFS_SAPH_ACH1PDT+1
#define OFS_SAPHCH1TT          OFS_SAPH_ACH1TT
#define OFS_SAPHCH1TT_L        OFS_SAPH_ACH1TT
#define OFS_SAPHCH1TT_H        OFS_SAPH_ACH1TT+1
#define OFS_SAPHTACTL          OFS_SAPH_ATACTL
#define OFS_SAPHTACTL_L        OFS_SAPH_ATACTL
#define OFS_SAPHTACTL_H        OFS_SAPH_ATACTL+1
#define OFS_SAPHICTL0          OFS_SAPH_AICTL0
#define OFS_SAPHICTL0_L        OFS_SAPH_AICTL0
#define OFS_SAPHICTL0_H        OFS_SAPH_AICTL0+1
#define OFS_SAPHBCTL           OFS_SAPH_ABCTL
#define OFS_SAPHBCTL_L         OFS_SAPH_ABCTL
#define OFS_SAPHBCTL_H         OFS_SAPH_ABCTL+1
#define OFS_SAPHPGC            OFS_SAPH_APGC
#define OFS_SAPHPGC_L          OFS_SAPH_APGC
#define OFS_SAPHPGC_H          OFS_SAPH_APGC+1
#define OFS_SAPHPGLPER         OFS_SAPH_APGLPER
#define OFS_SAPHPGLPER_L       OFS_SAPH_APGLPER
#define OFS_SAPHPGLPER_H       OFS_SAPH_APGLPER+1
#define OFS_SAPHPGHPER         OFS_SAPH_APGHPER
#define OFS_SAPHPGHPER_L       OFS_SAPH_APGHPER
#define OFS_SAPHPGHPER_H       OFS_SAPH_APGHPER+1
#define OFS_SAPHPGCTL          OFS_SAPH_APGCTL
#define OFS_SAPHPGCTL_L        OFS_SAPH_APGCTL
#define OFS_SAPHPGCTL_H        OFS_SAPH_APGCTL+1
#define OFS_SAPHPPGTRIG        OFS_SAPH_APPGTRIG
#define OFS_SAPHPPGTRIG_L      OFS_SAPH_APPGTRIG
#define OFS_SAPHPPGTRIG_H      OFS_SAPH_APPGTRIG+1
#define OFS_SAPHXPGCTL         OFS_SAPH_AXPGCTL
#define OFS_SAPHXPGCTL_L       OFS_SAPH_AXPGCTL
#define OFS_SAPHXPGCTL_H       OFS_SAPH_AXPGCTL+1
#define OFS_SAPHXPGLPER        OFS_SAPH_AXPGLPER
#define OFS_SAPHXPGLPER_L      OFS_SAPH_AXPGLPER
#define OFS_SAPHXPGLPER_H      OFS_SAPH_AXPGLPER+1
#define OFS_SAPHXPGHPER        OFS_SAPH_AXPGHPER
#define OFS_SAPHXPGHPER_L      OFS_SAPH_AXPGHPER
#define OFS_SAPHXPGHPER_H      OFS_SAPH_AXPGHPER+1
#define OFS_SAPHASCTL0         OFS_SAPH_AASCTL0
#define OFS_SAPHASCTL0_L       OFS_SAPH_AASCTL0
#define OFS_SAPHASCTL0_H       OFS_SAPH_AASCTL0+1
#define OFS_SAPHASCTL1         OFS_SAPH_AASCTL1
#define OFS_SAPHASCTL1_L       OFS_SAPH_AASCTL1
#define OFS_SAPHASCTL1_H       OFS_SAPH_AASCTL1+1
#define OFS_SAPHASQTRIG        OFS_SAPH_AASQTRIG
#define OFS_SAPHAPOL           OFS_SAPH_AAPOL
#define OFS_SAPHAPOL_L         OFS_SAPH_AAPOL
#define OFS_SAPHAPOL_H         OFS_SAPH_AAPOL+1
#define OFS_SAPHAPLEV          OFS_SAPH_AAPLEV
#define OFS_SAPHAPLEV_L        OFS_SAPH_AAPLEV
#define OFS_SAPHAPLEV_H        OFS_SAPH_AAPLEV+1
#define OFS_SAPHAPHIZ          OFS_SAPH_AAPHIZ
#define OFS_SAPHAPHIZ_L        OFS_SAPH_AAPHIZ
#define OFS_SAPHAPHIZ_H        OFS_SAPH_AAPHIZ+1
#define OFS_SAPHATM_A          OFS_SAPH_AATM_A
#define OFS_SAPHATM_A_L        OFS_SAPH_AATM_A
#define OFS_SAPHATM_A_H        OFS_SAPH_AATM_A+1
#define OFS_SAPHATM_B          OFS_SAPH_AATM_B
#define OFS_SAPHATM_B_L        OFS_SAPH_AATM_B
#define OFS_SAPHATM_B_H        OFS_SAPH_AATM_B+1
#define OFS_SAPHATM_C          OFS_SAPH_AATM_C
#define OFS_SAPHATM_C_L        OFS_SAPH_AATM_C
#define OFS_SAPHATM_C_H        OFS_SAPH_AATM_C+1
#define OFS_SAPHATM_D          OFS_SAPH_AATM_D
#define OFS_SAPHATM_D_L        OFS_SAPH_AATM_D
#define OFS_SAPHATM_D_H        OFS_SAPH_AATM_D+1
#define OFS_SAPHATM_E          OFS_SAPH_AATM_E
#define OFS_SAPHATM_E_L        OFS_SAPH_AATM_E
#define OFS_SAPHATM_E_H        OFS_SAPH_AATM_E+1
#define OFS_SAPHATM_F          OFS_SAPH_AATM_F
#define OFS_SAPHATM_F_L        OFS_SAPH_AATM_F
#define OFS_SAPHATM_F_H        OFS_SAPH_AATM_F+1
#define OFS_SAPHTBCTL          OFS_SAPH_ATBCTL
#define OFS_SAPHTBCTL_L        OFS_SAPH_ATBCTL
#define OFS_SAPHTBCTL_H        OFS_SAPH_ATBCTL+1
#define OFS_SAPHATIMLO         OFS_SAPH_AATIMLO
#define OFS_SAPHATIMLO_L       OFS_SAPH_AATIMLO
#define OFS_SAPHATIMLO_H       OFS_SAPH_AATIMLO+1
#define OFS_SAPHATIMHI         OFS_SAPH_AATIMHI
#define OFS_SAPHATIMHI_L       OFS_SAPH_AATIMHI
#define OFS_SAPHATIMHI_H       OFS_SAPH_AATIMHI+1

#define SAPH_VECTOR            SAPH_A_VECTOR

#endif

/************************************************************
* SYS
************************************************************/

#ifdef __MSP430_HAS_SYS__

#define SYSRSTIV_NONE           SYSRSTIV__NONE
#define SYSRSTIV_BOR            SYSRSTIV__BOR
#define SYSRSTIV_CSPW           SYSRSTIV__CSPW
#define SYSRSTIV_DOBOR          SYSRSTIV__PMMSWBOR
#define SYSRSTIV_DOPOR          SYSRSTIV__PMMSWPOR
#define SYSRSTIV_FLLUL          SYSRSTIV__FLLUL
#define SYSRSTIV_FRCTLPW        SYSRSTIV__FRCTLPW
#define SYSRSTIV_LPM5WU         SYSRSTIV__LPM5WU
#define SYSRSTIV_MPUPW          SYSRSTIV__MPUPW
#define SYSRSTIV_MPUSEG1IFG     SYSRSTIV__MPUSEG1IFG
#define SYSRSTIV_MPUSEG2IFG     SYSRSTIV__MPUSEG2IFG
#define SYSRSTIV_MPUSEG3IFG     SYSRSTIV__MPUSEG3IFG
#define SYSRSTIV_MPUSEGIIFG     SYSRSTIV__MPUSEGIIFG
#define SYSRSTIV_MPUSEGPIFG     SYSRSTIV__MPUSEGPIFG
#define SYSRSTIV_PERF           SYSRSTIV__PERF
#define SYSRSTIV_PMMPW          SYSRSTIV__PMMPW
#define SYSRSTIV_RSTNMI         SYSRSTIV__RSTNMI
#define SYSRSTIV_SECYV          SYSRSTIV__SECYV
#define SYSRSTIV_SVSHIFG        SYSRSTIV__SVSHIFG
#define SYSRSTIV_UBDIFG         SYSRSTIV__UBDIFG
#define SYSRSTIV_WDTKEY         SYSRSTIV__WDTPW
#define SYSRSTIV_WDTPW          SYSRSTIV__WDTPW
#define SYSRSTIV_WDTTO          SYSRSTIV__WDTIFG

#define SYSSNIV_NONE            SYSSNIV__NONE
#define SYSSNIV_ACCTEIFG        SYSSNIV__ACCTEIFG
#define SYSSNIV_CBDIFG          SYSSNIV__CBDIFG
#define SYSSNIV_JMBINIFG        SYSSNIV__JMBINIFG
#define SYSSNIV_JMBOUTIFG       SYSSNIV__JMBOUTIFG
#define SYSSNIV_LEASCCMD        SYSSNIV__LEACMD
#define SYSSNIV_LEASCTO         SYSSNIV__LEATO
#define SYSSNIV_MPUSEG1IFG      SYSSNIV__MPUSEG1IFG
#define SYSSNIV_MPUSEG2IFG      SYSSNIV__MPUSEG2IFG
#define SYSSNIV_MPUSEG3IFG      SYSSNIV__MPUSEG3IFG
#define SYSSNIV_MPUSEGIIFG      SYSSNIV__MPUSEGIIFG
#define SYSSNIV_MPUSEGPIFG      SYSSNIV__MPUSEGPIFG
#define SYSSNIV_SVSLIFG         SYSSNIV__SVSLIFG
#define SYSSNIV_UBDIFG          SYSSNIV__UBDIFG
#define SYSSNIV_VMAIFG          SYSSNIV__VMAIFG
#define SYSSNIV_WPROT           SYSSNIV__WPROT

#define SYSUNIV_NONE            SYSUNIV__NONE
#define SYSUNIV_NMIIFG          SYSUNIV__NMIIFG
#define SYSUNIV_OFIFG           SYSUNIV__OFIFG

#ifdef __MSP430_HAS_LEA__

#define SYSSNIV__LEASCTO        SYSSNIV__LEATO
#define SYSSNIV__LEASCTO_L      SYSSNIV__LEATO_L
#define SYSSNIV__LEASCCMD       SYSSNIV__LEACMD
#define SYSSNIV__LEASCCMD_L     SYSSNIV__LEACMD_L

#endif

#endif

/************************************************************
* Timer A
************************************************************/

#ifdef  __MSP430_HAS_TAx__            /* Definition to show that Module is available */
#define __MSP430_HAS_TxA7__

#define OFS_TAxCTL             OFS_TA0CTL
#define OFS_TAxCCTL0           OFS_TA0CCTL0
#define OFS_TAxCCTL1           OFS_TA0CCTL1
#define OFS_TAxCCTL2           OFS_TA0CCTL2
#define OFS_TAxCCTL3           OFS_TA0CCTL3
#define OFS_TAxCCTL4           OFS_TA0CCTL4
#define OFS_TAxCCTL5           OFS_TA0CCTL5
#define OFS_TAxCCTL6           OFS_TA0CCTL6
#define OFS_TAxR               OFS_TA0R
#define OFS_TAxCCR0            OFS_TA0CCR0
#define OFS_TAxCCR1            OFS_TA0CCR1
#define OFS_TAxCCR2            OFS_TA0CCR2
#define OFS_TAxCCR3            OFS_TA0CCR3
#define OFS_TAxCCR4            OFS_TA0CCR4
#define OFS_TAxCCR5            OFS_TA0CCR5
#define OFS_TAxCCR6            OFS_TA0CCR6
#define OFS_TAxIV              OFS_TA0IV
#define OFS_TAxEX0             OFS_TA0EX0

/* TAxIV Definitions */
#define TAxIV_NONE             (0x0000)       /* No Interrupt pending */
#define TAxIV_TACCR1           (0x0002)       /* TAxCCR1_CCIFG */
#define TAxIV_TACCR2           (0x0004)       /* TAxCCR2_CCIFG */
#define TAxIV_TACCR3           (0x0006)       /* TAxCCR3_CCIFG */
#define TAxIV_TACCR4           (0x0008)       /* TAxCCR4_CCIFG */
#define TAxIV_TACCR5           (0x000A)       /* TAxCCR5_CCIFG */
#define TAxIV_TACCR6           (0x000C)       /* TAxCCR6_CCIFG */
#define TAxIV_TAIFG            (0x000E)       /* TAxIFG */

/* Legacy Defines */
#define TAxIV_TAxCCR1          (0x0002)       /* TAxCCR1_CCIFG */
#define TAxIV_TAxCCR2          (0x0004)       /* TAxCCR2_CCIFG */
#define TAxIV_TAxCCR3          (0x0006)       /* TAxCCR3_CCIFG */
#define TAxIV_TAxCCR4          (0x0008)       /* TAxCCR4_CCIFG */
#define TAxIV_TAxCCR5          (0x000A)       /* TAxCCR5_CCIFG */
#define TAxIV_TAxCCR6          (0x000C)       /* TAxCCR6_CCIFG */
#define TAxIV_TAxIFG           (0x000E)       /* TAxIFG */

#ifndef MC__CONTINOUS
#define MC__CONTINOUS      MC__CONTINUOUS
#endif

#endif

#ifdef __MSP430_HAS_TA0__
#if (__MSP430_HAS_TA0__ == 3)
	#define __MSP430_BASEADDRESS_T0A3__ TA0_BASE
	#define __MSP430_HAS_T0A3__
#endif
#define TIMER_A0_BASE       TA0_BASE

#define TA0IV_TA0CCR1       TAIV__TACCR1
#define TA0IV_TA0CCR2       TAIV__TACCR2
#define TA0IV_TA0IFG        TAIV__TAIFG
#define TA0IV_NONE          TAIV__NONE
#define TA0IV_TACCR1        TAIV__TACCR1
#define TA0IV_TACCR2        TAIV__TACCR2
#define TA0IV_3             TAIV__TACCR3
#define TA0IV_4             TAIV__TACCR4
#define TA0IV_5             TAIV__TACCR5
#define TA0IV_6             TAIV__TACCR6
#define TA0IV_TAIFG         TAIV__TAIFG
#endif

#ifdef __MSP430_HAS_TA1__
#if (__MSP430_HAS_TA1__ == 3)
	#define __MSP430_BASEADDRESS_T1A3__ TA1_BASE
	#define __MSP430_HAS_T1A3__
#endif
#define TIMER_A1_BASE       TA1_BASE

#define TA1IV_TA1CCR1       TAIV__TACCR1
#define TA1IV_TA1CCR2       TAIV__TACCR2
#define TA1IV_TA1IFG        TAIV__TAIFG
#define TA1IV_NONE          TAIV__NONE
#define TA1IV_TACCR1        TAIV__TACCR1
#define TA1IV_TACCR2        TAIV__TACCR2
#define TA1IV_3             TAIV__TACCR3
#define TA1IV_4             TAIV__TACCR4
#define TA1IV_5             TAIV__TACCR5
#define TA1IV_6             TAIV__TACCR6
#define TA1IV_TAIFG         TAIV__TAIFG
#endif

#ifdef __MSP430_HAS_TA2__
#if (__MSP430_HAS_TA2__ == 2)
	#define __MSP430_BASEADDRESS_T2A2__ TA2_BASE
	#define __MSP430_HAS_T2A2__
#endif
#define TIMER_A2_BASE       TA2_BASE

#define TA2IV_TA2CCR1       TAIV__TACCR1
#define TA2IV_TA2IFG        TAIV__TAIFG
#define TA2IV_NONE          TAIV__NONE
#define TA2IV_TACCR1        TAIV__TACCR1
#define TA2IV_TACCR2        TAIV__TACCR2
#define TA2IV_3             TAIV__TACCR3
#define TA2IV_4             TAIV__TACCR4
#define TA2IV_5             TAIV__TACCR5
#define TA2IV_6             TAIV__TACCR6
#define TA2IV_TAIFG         TAIV__TAIFG
#endif

#ifdef __MSP430_HAS_TA3__
#if (__MSP430_HAS_TA3__ == 2)
	#define __MSP430_BASEADDRESS_T3A2__ TA3_BASE
	#define __MSP430_HAS_T3A2__
#endif
#define TIMER_A3_BASE       TA3_BASE

#define TA3IV_TA3CCR1       TAIV__TACCR1
#define TA3IV_TA3IFG        TAIV__TAIFG
#define TA3IV_NONE          TAIV__NONE
#define TA3IV_TACCR1        TAIV__TACCR1
#define TA3IV_TACCR2        TAIV__TACCR2
#define TA3IV_3             TAIV__TACCR3
#define TA3IV_4             TAIV__TACCR4
#define TA3IV_5             TAIV__TACCR5
#define TA3IV_6             TAIV__TACCR6
#define TA3IV_TAIFG         TAIV__TAIFG
#endif

#ifdef __MSP430_HAS_TA4__
#if (__MSP430_HAS_TA4__ == 3)
	#define __MSP430_BASEADDRESS_T4A3__ TA4_BASE
	#define __MSP430_HAS_T4A3__
#endif
#define TIMER_A4_BASE       TA4_BASE

#define TA4IV_TA4CCR1       TAIV__TACCR1
#define TA4IV_TA4IFG        TAIV__TAIFG
#define TA4IV_NONE          TAIV__NONE
#define TA4IV_TACCR1        TAIV__TACCR1
#define TA4IV_TACCR2        TAIV__TACCR2
#define TA4IV_3             TAIV__TACCR3
#define TA4IV_4             TAIV__TACCR4
#define TA4IV_5             TAIV__TACCR5
#define TA4IV_6             TAIV__TACCR6
#define TA4IV_TAIFG         TAIV__TAIFG
#endif

/************************************************************
* Timer B
************************************************************/

#ifdef __MSP430_HAS_TBx__
#define __MSP430_HAS_TxB7__

#define OFS_TBxCTL             OFS_TB0CTL
#define OFS_TBxCCTL0           OFS_TB0CCTL0
#define OFS_TBxCCTL1           OFS_TB0CCTL1
#define OFS_TBxCCTL2           OFS_TB0CCTL2
#define OFS_TBxCCTL3           OFS_TB0CCTL3
#define OFS_TBxCCTL4           OFS_TB0CCTL4
#define OFS_TBxCCTL5           OFS_TB0CCTL5
#define OFS_TBxCCTL6           OFS_TB0CCTL6
#define OFS_TBxR               OFS_TB0R
#define OFS_TBxCCR0            OFS_TB0CCR0
#define OFS_TBxCCR1            OFS_TB0CCR1
#define OFS_TBxCCR2            OFS_TB0CCR2
#define OFS_TBxCCR3            OFS_TB0CCR3
#define OFS_TBxCCR4            OFS_TB0CCR4
#define OFS_TBxCCR5            OFS_TB0CCR5
#define OFS_TBxCCR6            OFS_TB0CCR6
#define OFS_TBxIV              OFS_TB0IV
#define OFS_TBxEX0             OFS_TB0EX0

/* TBxIV Definitions */
#define TBxIV_NONE             (0x0000)       /* No Interrupt pending */
#define TBxIV_TBCCR1           (0x0002)       /* TBxCCR1_CCIFG */
#define TBxIV_TBCCR2           (0x0004)       /* TBxCCR2_CCIFG */
#define TBxIV_TBCCR3           (0x0006)       /* TBxCCR3_CCIFG */
#define TBxIV_TBCCR4           (0x0008)       /* TBxCCR4_CCIFG */
#define TBxIV_TBCCR5           (0x000A)       /* TBxCCR5_CCIFG */
#define TBxIV_TBCCR6           (0x000C)       /* TBxCCR6_CCIFG */
#define TBxIV_TBIFG            (0x000E)       /* TBxIFG */

/* Legacy Defines */
#define TBxIV_TBxCCR1          (0x0002)       /* TBxCCR1_CCIFG */
#define TBxIV_TBxCCR2          (0x0004)       /* TBxCCR2_CCIFG */
#define TBxIV_TBxCCR3          (0x0006)       /* TBxCCR3_CCIFG */
#define TBxIV_TBxCCR4          (0x0008)       /* TBxCCR4_CCIFG */
#define TBxIV_TBxCCR5          (0x000A)       /* TBxCCR5_CCIFG */
#define TBxIV_TBxCCR6          (0x000C)       /* TBxCCR6_CCIFG */
#define TBxIV_TBxIFG           (0x000E)       /* TBxIFG */

#ifndef MC__CONTINOUS
#define MC__CONTINOUS          MC__CONTINUOUS
#endif

#define SHR0                    TBCLGRP0
#define SHR1                    TBCLGRP1
#define SHR_0                   TBCLGRP_0
#define SHR_1                   TBCLGRP_1
#define SHR_2                   TBCLGRP_2
#define SHR_3                   TBCLGRP_3

#define SLSHR0                  CLLD0
#define SLSHR1                  CLLD1
#define SLSHR_0                 CLLD_0
#define SLSHR_1                 CLLD_1
#define SLSHR_2                 CLLD_2
#define SLSHR_3                 CLLD_3

#endif

#ifdef __MSP430_HAS_TB0__
#if (__MSP430_HAS_TB0__ == 3)
    #define __MSP430_BASEADDRESS_T0B3__ TB0_BASE
    #define __MSP430_HAS_T0B3__
#endif
#if (__MSP430_HAS_TB0__ == 7)
    #define __MSP430_BASEADDRESS_T0B7__ TB0_BASE
    #define __MSP430_HAS_T0B7__
#endif
#define TIMER_B0_BASE           TB0_BASE
#define TIMERB0_VECTOR          TIMER0_B0_VECTOR

#define TBCTL                   TB0CTL    /* Control Register */
#define TBCCTL0                 TB0CCTL0  /* Capture/Compare Control 0 */
#define TBCCTL1                 TB0CCTL1  /* Capture/Compare Control 1 */
#define TBCCTL2                 TB0CCTL2  /* Capture/Compare Control 2 */
#define TBCCTL3                 TB0CCTL3  /* Capture/Compare Control 3 */
#define TBCCTL4                 TB0CCTL4  /* Capture/Compare Control 4 */
#define TBCCTL5                 TB0CCTL5  /* Capture/Compare Control 5 */
#define TBCCTL6                 TB0CCTL6  /* Capture/Compare Control 6 */
#define TBR                     TB0R
#define TBCCR0                  TB0CCR0   /* Capture/Compare 0 */
#define TBCCR1                  TB0CCR1   /* Capture/Compare 1 */
#define TBCCR2                  TB0CCR2   /* Capture/Compare 2 */
#define TBCCR3                  TB0CCR3   /* Capture/Compare 3 */
#define TBCCR4                  TB0CCR4   /* Capture/Compare 4 */
#define TBCCR5                  TB0CCR5   /* Capture/Compare 5 */
#define TBCCR6                  TB0CCR6   /* Capture/Compare 6 */
#define TBEX0                   TB0EX0    /* Expansion Register 0 */

#define TBSSEL__TACLK           TBSSEL__TBCLK
    
#define TB0IV_NONE              TBIV__NONE
#define TB0IV_TB0CCR1           TBIV__TBCCR1
#define TB0IV_TB0CCR2           TBIV__TBCCR2
#define TB0IV_TB0CCR3           TBIV__TBCCR3
#define TB0IV_TB0CCR4           TBIV__TBCCR4
#define TB0IV_TB0CCR5           TBIV__TBCCR5
#define TB0IV_TB0CCR6           TBIV__TBCCR6
#define TB0IV_TB0IFG            TBIV__TBIFG

#define TB0IV_TBCCR1            TBIV__TBCCR1
#define TB0IV_TBCCR2            TBIV__TBCCR2
#define TB0IV_TBCCR3            TBIV__TBCCR3
#define TB0IV_TBCCR4            TBIV__TBCCR4
#define TB0IV_TBCCR5            TBIV__TBCCR5
#define TB0IV_TBCCR6            TBIV__TBCCR6
#define TB0IV_TBIFG             TBIV__TBIFG

#endif

#ifdef __MSP430_HAS_TB1__
#if (__MSP430_HAS_TB1__ == 3)
    #define __MSP430_HAS_T1B3__
    #define __MSP430_BASEADDRESS_T1B3__ TB1_BASE
#endif
#define TIMER_B1_BASE           TB1_BASE
#define TIMERB1_VECTOR          TIMER0_B1_VECTOR

#define TB1IV_NONE              TBIV__NONE
#define TB1IV_TB1CCR1           TBIV__TBCCR1
#define TB1IV_TB1CCR2           TBIV__TBCCR2
#define TB1IV_TB1IFG            TBIV__TBIFG
#define TB1IV_TBCCR1            TBIV__TBCCR1
#define TB1IV_TBCCR2            TBIV__TBCCR2
#define TB1IV_TBIFG             TBIV__TBIFG

#endif

/************************************************************
* USCI
************************************************************/

#if defined(__MSP430_HAS_EUSCI_Ax__) || defined(__MSP430_HAS_EUSCI_Bx__)

#define UCADDMASK0              ADDMASK0
#define UCADDMASK0_L            ADDMASK0_L
#define UCADDMASK1              ADDMASK1
#define UCADDMASK1_L            ADDMASK1_L
#define UCADDMASK2              ADDMASK2
#define UCADDMASK2_L            ADDMASK2_L
#define UCADDMASK3              ADDMASK3
#define UCADDMASK3_L            ADDMASK3_L
#define UCADDMASK4              ADDMASK4
#define UCADDMASK4_L            ADDMASK4_L
#define UCADDMASK5              ADDMASK5
#define UCADDMASK5_L            ADDMASK5_L
#define UCADDMASK6              ADDMASK6
#define UCADDMASK6_L            ADDMASK6_L
#define UCADDMASK7              ADDMASK7
#define UCADDMASK7_L            ADDMASK7_L
#define UCADDMASK8              ADDMASK8
#define UCADDMASK8_H            ADDMASK8_H
#define UCADDMASK9              ADDMASK9
#define UCADDMASK9_H            ADDMASK9_H
 
#define UCADDR                  UCADDR_UCIDLE
#define UCIDLE                  UCADDR_UCIDLE
 
#define UCADDRX0                ADDRX0
#define UCADDRX0_L              ADDRX0_L
#define UCADDRX1                ADDRX1
#define UCADDRX1_L              ADDRX1_L
#define UCADDRX2                ADDRX2
#define UCADDRX2_L              ADDRX2_L
#define UCADDRX3                ADDRX3
#define UCADDRX3_L              ADDRX3_L
#define UCADDRX4                ADDRX4
#define UCADDRX4_L              ADDRX4_L
#define UCADDRX5                ADDRX5
#define UCADDRX5_L              ADDRX5_L
#define UCADDRX6                ADDRX6
#define UCADDRX6_L              ADDRX6_L
#define UCADDRX7                ADDRX7
#define UCADDRX7_L              ADDRX7_L
#define UCADDRX8                ADDRX8
#define UCADDRX8_H              ADDRX8_H
#define UCADDRX9                ADDRX9
#define UCADDRX9_H              ADDRX9_H

#define UCBRF_0                 (0x00)         /* USCI First Stage Modulation: 0 */
#define UCBRF_1                 (0x10)         /* USCI First Stage Modulation: 1 */
#define UCBRF_2                 (0x20)         /* USCI First Stage Modulation: 2 */
#define UCBRF_3                 (0x30)         /* USCI First Stage Modulation: 3 */
#define UCBRF_4                 (0x40)         /* USCI First Stage Modulation: 4 */
#define UCBRF_5                 (0x50)         /* USCI First Stage Modulation: 5 */
#define UCBRF_6                 (0x60)         /* USCI First Stage Modulation: 6 */
#define UCBRF_7                 (0x70)         /* USCI First Stage Modulation: 7 */
#define UCBRF_8                 (0x80)         /* USCI First Stage Modulation: 8 */
#define UCBRF_9                 (0x90)         /* USCI First Stage Modulation: 9 */
#define UCBRF_10                (0xA0)         /* USCI First Stage Modulation: A */
#define UCBRF_11                (0xB0)         /* USCI First Stage Modulation: B */
#define UCBRF_12                (0xC0)         /* USCI First Stage Modulation: C */
#define UCBRF_13                (0xD0)         /* USCI First Stage Modulation: D */
#define UCBRF_14                (0xE0)         /* USCI First Stage Modulation: E */
#define UCBRF_15                (0xF0)         /* USCI First Stage Modulation: F */

#define UCOA0                   I2COA00
#define UCOA0_L                 I2COA00_L
#define UCOA1                   I2COA01
#define UCOA1_L                 I2COA01_L
#define UCOA2                   I2COA02
#define UCOA2_L                 I2COA02_L
#define UCOA3                   I2COA03
#define UCOA3_L                 I2COA03_L
#define UCOA4                   I2COA04
#define UCOA4_L                 I2COA04_L
#define UCOA5                   I2COA05
#define UCOA5_L                 I2COA05_L
#define UCOA6                   I2COA06
#define UCOA6_L                 I2COA06_L
#define UCOA7                   I2COA07
#define UCOA7_L                 I2COA07_L
#define UCOA8                   I2COA08
#define UCOA8_H                 I2COA08_H
#define UCOA9                   I2COA09
#define UCOA9_H                 I2COA09_H

#define UCSA0                   I2CSA0
#define UCSA0_L                 I2CSA0_L
#define UCSA1                   I2CSA1
#define UCSA1_L                 I2CSA1_L
#define UCSA2                   I2CSA2
#define UCSA2_L                 I2CSA2_L
#define UCSA3                   I2CSA3
#define UCSA3_L                 I2CSA3_L
#define UCSA4                   I2CSA4
#define UCSA4_L                 I2CSA4_L
#define UCSA5                   I2CSA5
#define UCSA5_L                 I2CSA5_L
#define UCSA6                   I2CSA6
#define UCSA6_L                 I2CSA6_L
#define UCSA7                   I2CSA7
#define UCSA7_L                 I2CSA7_L
#define UCSA8                   I2CSA8
#define UCSA8_H                 I2CSA8_H
#define UCSA9                   I2CSA9
#define UCSA9_H                 I2CSA9_H

#define USCI_NONE               UCIV__NONE
#define USCI_I2C_UCALIFG        UCIV__UCALIFG
#define USCI_I2C_UCBCNTIFG      UCIV__UCBCNTIFG
#define USCI_I2C_UCBIT9IFG      UCIV__UCBIT9IFG
#define USCI_I2C_UCCLTOIFG      UCIV__UCCLTOIFG
#define USCI_I2C_UCNACKIFG      UCIV__UCNACKIFG
#define USCI_I2C_UCRXIFG0       UCIV__UCRXIFG0
#define USCI_I2C_UCRXIFG1       UCIV__UCRXIFG1
#define USCI_I2C_UCRXIFG2       UCIV__UCRXIFG2
#define USCI_I2C_UCRXIFG3       UCIV__UCRXIFG3
#define USCI_I2C_UCSTPIFG       UCIV__UCSTPIFG
#define USCI_I2C_UCSTTIFG       UCIV__UCSTTIFG
#define USCI_I2C_UCTXIFG0       UCIV__UCTXIFG0
#define USCI_I2C_UCTXIFG1       UCIV__UCTXIFG1
#define USCI_I2C_UCTXIFG2       UCIV__UCTXIFG2
#define USCI_I2C_UCTXIFG3       UCIV__UCTXIFG3
#define USCI_SPI_UCRXIFG        UCIV__UCRXIFG
#define USCI_SPI_UCTXIFG        UCIV__UCTXIFG
#define USCI_UART_UCRXIFG       UCIV__UCRXIFG
#define USCI_UART_UCSTTIFG      UCIV__UCSTTIFG
#define USCI_UART_UCTXCPTIFG    UCIV__UCTXCPTIFG
#define USCI_UART_UCTXIFG       UCIV__UCTXIFG

#endif

/************************************************************
* USCI Ax
************************************************************/

#ifdef  __MSP430_HAS_EUSCI_Ax__      /* Definition to show that Module is available */
#define __MSP430_HAS_USCI_Ax__

#define UCAxCTL1               UCA0CTLW0_L
#define UCAxCTL0               UCA0CTLW0_H
#define UCAxBR0                UCA0BRW_L
#define UCAxBR1                UCA0BRW_H
#define UCAxIRTCTL             UCA0IRCTL_L
#define UCAxIRRCTL             UCA0IRCTL_H

#define OFS_UCAxCTLW0          (0x0000)       /* USCI Ax Control Word Register 0 */
#define OFS_UCAxCTLW0_L        OFS_UCAxCTLW0
#define OFS_UCAxCTLW0_H        OFS_UCAxCTLW0+1
#define OFS_UCAxCTL0           (0x0001)
#define OFS_UCAxCTL1           (0x0000)
#define OFS_UCAxCTLW1          (0x0002)       /* USCI Ax Control Word Register 1 */
#define OFS_UCAxCTLW1_L        OFS_UCAxCTLW1
#define OFS_UCAxCTLW1_H        OFS_UCAxCTLW1+1
#define OFS_UCAxBRW            (0x0006)       /* USCI Ax Baud Word Rate 0 */
#define OFS_UCAxBRW_L          OFS_UCAxBRW
#define OFS_UCAxBRW_H          OFS_UCAxBRW+1
#define OFS_UCAxBR0            (0x0006)
#define OFS_UCAxBR1            (0x0007)
#define OFS_UCAxMCTLW          (0x0008)       /* USCI Ax Modulation Control */
#define OFS_UCAxMCTLW_L        OFS_UCAxMCTLW
#define OFS_UCAxMCTLW_H        OFS_UCAxMCTLW+1
#define OFS_UCAxSTATW          (0x000A)       /* USCI Ax Status Register */
#define OFS_UCAxRXBUF          (0x000C)       /* USCI Ax Receive Buffer */
#define OFS_UCAxRXBUF_L        OFS_UCAxRXBUF
#define OFS_UCAxRXBUF_H        OFS_UCAxRXBUF+1
#define OFS_UCAxTXBUF          (0x000E)       /* USCI Ax Transmit Buffer */
#define OFS_UCAxTXBUF_L        OFS_UCAxTXBUF
#define OFS_UCAxTXBUF_H        OFS_UCAxTXBUF+1
#define OFS_UCAxABCTL          (0x0010)       /* USCI Ax LIN Control */
#define OFS_UCAxIRCTL          (0x0012)       /* USCI Ax IrDA Transmit Control */
#define OFS_UCAxIRCTL_L        OFS_UCAxIRCTL
#define OFS_UCAxIRCTL_H        OFS_UCAxIRCTL+1
#define OFS_UCAxIRTCTL         (0x0012)
#define OFS_UCAxIRRCTL         (0x0013)
#define OFS_UCAxIE             (0x001A)       /* USCI Ax Interrupt Enable Register */
#define OFS_UCAxIE_L           OFS_UCAxIE
#define OFS_UCAxIE_H           OFS_UCAxIE+1
#define OFS_UCAxIFG            (0x001C)       /* USCI Ax Interrupt Flags Register */
#define OFS_UCAxIFG_L          OFS_UCAxIFG
#define OFS_UCAxIFG_H          OFS_UCAxIFG+1
#define OFS_UCAxIE__UART       (0x001A)
#define OFS_UCAxIE__UART_L     OFS_UCAxIE__UART
#define OFS_UCAxIE__UART_H     OFS_UCAxIE__UART+1
#define OFS_UCAxIFG__UART      (0x001C)
#define OFS_UCAxIFG__UART_L    OFS_UCAxIFG__UART
#define OFS_UCAxIFG__UART_H    OFS_UCAxIFG__UART+1
#define OFS_UCAxIV             (0x001E)       /* USCI Ax Interrupt Vector Register */

#define OFS_UCAxCTLW0__SPI     (0x0000)
#define OFS_UCAxCTLW0__SPI_L   OFS_UCAxCTLW0__SPI
#define OFS_UCAxCTLW0__SPI_H   OFS_UCAxCTLW0__SPI+1
#define OFS_UCAxCTL0__SPI      (0x0001)
#define OFS_UCAxCTL1__SPI      (0x0000)
#define OFS_UCAxBRW__SPI       (0x0006)
#define OFS_UCAxBRW__SPI_L     OFS_UCAxBRW__SPI
#define OFS_UCAxBRW__SPI_H     OFS_UCAxBRW__SPI+1
#define OFS_UCAxBR0__SPI       (0x0006)
#define OFS_UCAxBR1__SPI       (0x0007)
#define OFS_UCAxSTATW__SPI     (0x000A)
#define OFS_UCAxRXBUF__SPI     (0x000C)
#define OFS_UCAxRXBUF__SPI_L   OFS_UCAxRXBUF__SPI
#define OFS_UCAxRXBUF__SPI_H   OFS_UCAxRXBUF__SPI+1
#define OFS_UCAxTXBUF__SPI     (0x000E)
#define OFS_UCAxTXBUF__SPI_L   OFS_UCAxTXBUF__SPI
#define OFS_UCAxTXBUF__SPI_H   OFS_UCAxTXBUF__SPI+1
#define OFS_UCAxIE__SPI        (0x001A)
#define OFS_UCAxIFG__SPI       (0x001C)
#define OFS_UCAxIV__SPI        (0x001E)

#ifdef __MSP430_HAS_EUSCI_A0__
#define USCI_A0_VECTOR          EUSCI_A0_VECTOR

#define UCA0CTL1                UCA0CTLW0_L    /* USCI A0 Control Register 1 */
#define UCA0CTL0                UCA0CTLW0_H    /* USCI A0 Control Register 0 */
#define UCA0BR0                 UCA0BRW_L      /* USCI A0 Baud Rate 0 */
#define UCA0BR1                 UCA0BRW_H      /* USCI A0 Baud Rate 1 */
#define UCA0IRTCTL              UCA0IRCTL_L    /* USCI A0 IrDA Transmit Control */
#define UCA0IRRCTL              UCA0IRCTL_H    /* USCI A0 IrDA Receive Control */

#endif

#ifdef __MSP430_HAS_EUSCI_A1__
#define USCI_A1_VECTOR          EUSCI_A1_VECTOR

#define UCA1CTL1                UCA1CTLW0_L    /* USCI A1 Control Register 1 */
#define UCA1CTL0                UCA1CTLW0_H    /* USCI A1 Control Register 0 */
#define UCA1BR0                 UCA1BRW_L      /* USCI A1 Baud Rate 0 */
#define UCA1BR1                 UCA1BRW_H      /* USCI A1 Baud Rate 1 */
#define UCA1IRTCTL              UCA1IRCTL_L    /* USCI A1 IrDA Transmit Control */
#define UCA1IRRCTL              UCA1IRCTL_H    /* USCI A1 IrDA Receive Control */

#endif

#ifdef __MSP430_HAS_EUSCI_A2__
#define USCI_A2_VECTOR          EUSCI_A2_VECTOR

#define UCA2CTL1                UCA2CTLW0_L    /* USCI A2 Control Register 1 */
#define UCA2CTL0                UCA2CTLW0_H    /* USCI A2 Control Register 0 */
#define UCA2BR0                 UCA2BRW_L      /* USCI A2 Baud Rate 0 */
#define UCA2BR1                 UCA2BRW_H      /* USCI A2 Baud Rate 1 */
#define UCA2IRTCTL              UCA2IRCTL_L    /* USCI A2 IrDA Transmit Control */
#define UCA2IRRCTL              UCA2IRCTL_H    /* USCI A2 IrDA Receive Control */

#endif

#ifdef __MSP430_HAS_EUSCI_A3__
#define USCI_A3_VECTOR          EUSCI_A3_VECTOR

#define UCA3CTL1                UCA3CTLW0_L    /* USCI A3 Control Register 1 */
#define UCA3CTL0                UCA3CTLW0_H    /* USCI A3 Control Register 0 */
#define UCA3BR0                 UCA3BRW_L      /* USCI A3 Baud Rate 0 */
#define UCA3BR1                 UCA3BRW_H      /* USCI A3 Baud Rate 1 */
#define UCA3IRTCTL              UCA3IRCTL_L    /* USCI A3 IrDA Transmit Control */
#define UCA3IRRCTL              UCA3IRCTL_H    /* USCI A3 IrDA Receive Control */

#endif

#endif

/************************************************************
* USCI Bx
************************************************************/

#ifdef  __MSP430_HAS_EUSCI_Bx__       /* Definition to show that Module is available */
#define __MSP430_HAS_USCI_Bx__

#define UCBxCTL1               UCB0CTLW0_L
#define UCBxCTL0               UCB0CTLW0_H
#define UCBxBR0                UCB0BRW_L
#define UCBxBR1                UCB0BRW_H
#define UCBxSTAT               UCB0STATW_L
#define UCBxBCNT               UCB0STATW_H

#define OFS_UCBxCTLW0__SPI     (0x0000)
#define OFS_UCBxCTLW0__SPI_L   OFS_UCBxCTLW0__SPI
#define OFS_UCBxCTLW0__SPI_H   OFS_UCBxCTLW0__SPI+1
#define OFS_UCBxCTL0__SPI      (0x0001)
#define OFS_UCBxCTL1__SPI      (0x0000)
#define OFS_UCBxBRW__SPI       (0x0006)
#define OFS_UCBxBRW__SPI_L     OFS_UCBxBRW__SPI
#define OFS_UCBxBRW__SPI_H     OFS_UCBxBRW__SPI+1
#define OFS_UCBxBR0__SPI       (0x0006)
#define OFS_UCBxBR1__SPI       (0x0007)
#define OFS_UCBxSTATW__SPI     (0x0008)
#define OFS_UCBxSTATW__SPI_L   OFS_UCBxSTATW__SPI
#define OFS_UCBxSTATW__SPI_H   OFS_UCBxSTATW__SPI+1
#define OFS_UCBxRXBUF__SPI     (0x000C)
#define OFS_UCBxRXBUF__SPI_L   OFS_UCBxRXBUF__SPI
#define OFS_UCBxRXBUF__SPI_H   OFS_UCBxRXBUF__SPI+1
#define OFS_UCBxTXBUF__SPI     (0x000E)
#define OFS_UCBxTXBUF__SPI_L   OFS_UCBxTXBUF__SPI
#define OFS_UCBxTXBUF__SPI_H   OFS_UCBxTXBUF__SPI+1
#define OFS_UCBxIE__SPI        (0x002A)
#define OFS_UCBxIE__SPI_L      OFS_UCBxIE__SPI
#define OFS_UCBxIE__SPI_H      OFS_UCBxIE__SPI+1
#define OFS_UCBxIFG__SPI       (0x002C)
#define OFS_UCBxIFG__SPI_L     OFS_UCBxIFG__SPI
#define OFS_UCBxIFG__SPI_H     OFS_UCBxIFG__SPI+1
#define OFS_UCBxIV__SPI        (0x002E)

#define OFS_UCBxCTLW0          (0x0000)       /* USCI Bx Control Word Register 0 */
#define OFS_UCBxCTLW0_L        OFS_UCBxCTLW0
#define OFS_UCBxCTLW0_H        OFS_UCBxCTLW0+1
#define OFS_UCBxCTL0           (0x0001)
#define OFS_UCBxCTL1           (0x0000)
#define OFS_UCBxCTLW1          (0x0002)       /* USCI Bx Control Word Register 1 */
#define OFS_UCBxCTLW1_L        OFS_UCBxCTLW1
#define OFS_UCBxCTLW1_H        OFS_UCBxCTLW1+1
#define OFS_UCBxBRW            (0x0006)       /* USCI Bx Baud Word Rate 0 */
#define OFS_UCBxBRW_L          OFS_UCBxBRW
#define OFS_UCBxBRW_H          OFS_UCBxBRW+1
#define OFS_UCBxBR0            (0x0006)
#define OFS_UCBxBR1            (0x0007)
#define OFS_UCBxSTATW          (0x0008)       /* USCI Bx Status Word Register */
#define OFS_UCBxSTATW_L        OFS_UCBxSTATW
#define OFS_UCBxSTATW_H        OFS_UCBxSTATW+1
#define OFS_UCBxSTATW__I2C     (0x0008)
#define OFS_UCBxSTAT__I2C      (0x0008)
#define OFS_UCBxBCNT__I2C      (0x0009)
#define OFS_UCBxTBCNT          (0x000A)       /* USCI Bx Byte Counter Threshold Register */
#define OFS_UCBxTBCNT_L        OFS_UCBxTBCNT
#define OFS_UCBxTBCNT_H        OFS_UCBxTBCNT+1
#define OFS_UCBxRXBUF          (0x000C)       /* USCI Bx Receive Buffer */
#define OFS_UCBxRXBUF_L        OFS_UCBxRXBUF
#define OFS_UCBxRXBUF_H        OFS_UCBxRXBUF+1
#define OFS_UCBxTXBUF          (0x000E)       /* USCI Bx Transmit Buffer */
#define OFS_UCBxTXBUF_L        OFS_UCBxTXBUF
#define OFS_UCBxTXBUF_H        OFS_UCBxTXBUF+1
#define OFS_UCBxI2COA0         (0x0014)       /* USCI Bx I2C Own Address 0 */
#define OFS_UCBxI2COA0_L       OFS_UCBxI2COA0
#define OFS_UCBxI2COA0_H       OFS_UCBxI2COA0+1
#define OFS_UCBxI2COA1         (0x0016)       /* USCI Bx I2C Own Address 1 */
#define OFS_UCBxI2COA1_L       OFS_UCBxI2COA1
#define OFS_UCBxI2COA1_H       OFS_UCBxI2COA1+1
#define OFS_UCBxI2COA2         (0x0018)       /* USCI Bx I2C Own Address 2 */
#define OFS_UCBxI2COA2_L       OFS_UCBxI2COA2
#define OFS_UCBxI2COA2_H       OFS_UCBxI2COA2+1
#define OFS_UCBxI2COA3         (0x001A)       /* USCI Bx I2C Own Address 3 */
#define OFS_UCBxI2COA3_L       OFS_UCBxI2COA3
#define OFS_UCBxI2COA3_H       OFS_UCBxI2COA3+1
#define OFS_UCBxADDRX          (0x001C)       /* USCI Bx Received Address Register */
#define OFS_UCBxADDRX_L        OFS_UCBxADDRX
#define OFS_UCBxADDRX_H        OFS_UCBxADDRX+1
#define OFS_UCBxADDMASK        (0x001E)       /* USCI Bx Address Mask Register */
#define OFS_UCBxADDMASK_L      OFS_UCBxADDMASK
#define OFS_UCBxADDMASK_H      OFS_UCBxADDMASK+1
#define OFS_UCBxI2CSA          (0x0020)       /* USCI Bx I2C Slave Address */
#define OFS_UCBxI2CSA_L        OFS_UCBxI2CSA
#define OFS_UCBxI2CSA_H        OFS_UCBxI2CSA+1
#define OFS_UCBxIE             (0x002A)       /* USCI Bx Interrupt Enable Register */
#define OFS_UCBxIE_L           OFS_UCBxIE
#define OFS_UCBxIE_H           OFS_UCBxIE+1
#define OFS_UCBxIFG            (0x002C)       /* USCI Bx Interrupt Flags Register */
#define OFS_UCBxIFG_L          OFS_UCBxIFG
#define OFS_UCBxIFG_H          OFS_UCBxIFG+1
#define OFS_UCBxIE__I2C        (0x002A)
#define OFS_UCBxIE__I2C_L      OFS_UCBxIE__I2C
#define OFS_UCBxIE__I2C_H      OFS_UCBxIE__I2C+1
#define OFS_UCBxIFG__I2C       (0x002C)
#define OFS_UCBxIFG__I2C_L     OFS_UCBxIFG__I2C
#define OFS_UCBxIFG__I2C_H     OFS_UCBxIFG__I2C+1
#define OFS_UCBxIV             (0x002E)       /* USCI Bx Interrupt Vector Register */

#ifndef UCASTP_3
#define UCASTP_3                UCASTP
#endif

#ifdef __MSP430_HAS_EUSCI_B0__
#define USCI_B0_VECTOR          EUSCI_B0_VECTOR

#define UCB0CTL1                UCB0CTLW0_L    /* USCI B0 Control Register 1 */
#define UCB0CTL0                UCB0CTLW0_H    /* USCI B0 Control Register 0 */
#define UCB0BR0                 UCB0BRW_L      /* USCI B0 Baud Rate 0 */
#define UCB0BR1                 UCB0BRW_H      /* USCI B0 Baud Rate 1 */
#define UCB0STAT                UCB0STATW_L    /* USCI B0 Status Register */
#define UCB0BCNT                UCB0STATW_H    /* USCI B0 Byte Counter Register */

#endif

#ifdef __MSP430_HAS_EUSCI_B1__
#define USCI_B1_VECTOR          EUSCI_B1_VECTOR

#define UCB1CTL1                UCB1CTLW0_L    /* USCI B1 Control Register 1 */
#define UCB1CTL0                UCB1CTLW0_H    /* USCI B1 Control Register 0 */
#define UCB1BR0                 UCB1BRW_L      /* USCI B1 Baud Rate 0 */
#define UCB1BR1                 UCB1BRW_H      /* USCI B1 Baud Rate 1 */
#define UCB1STAT                UCB1STATW_L    /* USCI B1 Status Register */
#define UCB1BCNT                UCB1STATW_H    /* USCI B1 Byte Counter Register */

#endif

#ifdef __MSP430_HAS_EUSCI_B2__
#define USCI_B2_VECTOR          EUSCI_B2_VECTOR

#define UCB2CTL1                UCB2CTLW0_L    /* USCI B2 Control Register 1 */
#define UCB2CTL0                UCB2CTLW0_H    /* USCI B2 Control Register 0 */
#define UCB2BR0                 UCB2BRW_L      /* USCI B2 Baud Rate 0 */
#define UCB2BR1                 UCB2BRW_H      /* USCI B2 Baud Rate 1 */
#define UCB2STAT                UCB2STATW_L    /* USCI B2 Status Register */
#define UCB2BCNT                UCB2STATW_H    /* USCI B2 Byte Counter Register */

#endif

#ifdef __MSP430_HAS_EUSCI_B3__
#define USCI_B3_VECTOR          EUSCI_B3_VECTOR

#define UCB3CTL1                UCB3CTLW0_L    /* USCI B3 Control Register 1 */
#define UCB3CTL0                UCB3CTLW0_H    /* USCI B3 Control Register 0 */
#define UCB3BR0                 UCB3BRW_L      /* USCI B3 Baud Rate 0 */
#define UCB3BR1                 UCB3BRW_H      /* USCI B3 Baud Rate 1 */
#define UCB3STAT                UCB3STATW_L    /* USCI B3 Status Register */
#define UCB3BCNT                UCB3STATW_H    /* USCI B3 Byte Counter Register */

#endif

#endif

/************************************************************
* WDT
************************************************************/

#ifdef __MSP430_HAS_WDT_A__       /* Definition to show that Module is available */

/* WDT-interval times [1ms] coded with Bits 0-2 */
/* WDT is clocked by fSMCLK (assumed 1MHz) */
#define WDT_MDLY_32             (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2)                         /* 32ms interval (default) */
#define WDT_MDLY_8              (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTIS0)                  /* 8ms     " */
#define WDT_MDLY_0_5            (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTIS1)                  /* 0.5ms   " */
#define WDT_MDLY_0_064          (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTIS1+WDTIS0)           /* 0.064ms " */
/* WDT is clocked by fACLK (assumed 32KHz) */
#define WDT_ADLY_1000           (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0)                /* 1000ms  " */
#define WDT_ADLY_250            (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0+WDTIS0)         /* 250ms   " */
#define WDT_ADLY_16             (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0+WDTIS1)         /* 16ms    " */
#define WDT_ADLY_1_9            (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0+WDTIS1+WDTIS0)  /* 1.9ms   " */
/* Watchdog mode -> reset after expired time */
/* WDT is clocked by fSMCLK (assumed 1MHz) */
#define WDT_MRST_32             (WDTPW+WDTCNTCL+WDTIS2)                                  /* 32ms interval (default) */
#define WDT_MRST_8              (WDTPW+WDTCNTCL+WDTIS2+WDTIS0)                           /* 8ms     " */
#define WDT_MRST_0_5            (WDTPW+WDTCNTCL+WDTIS2+WDTIS1)                           /* 0.5ms   " */
#define WDT_MRST_0_064          (WDTPW+WDTCNTCL+WDTIS2+WDTIS1+WDTIS0)                    /* 0.064ms " */
/* WDT is clocked by fACLK (assumed 32KHz) */
#define WDT_ARST_1000           (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2)                         /* 1000ms  " */
#define WDT_ARST_250            (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS0)                  /* 250ms   " */
#define WDT_ARST_16             (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1)                  /* 16ms    " */
#define WDT_ARST_1_9            (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1+WDTIS0)           /* 1.9ms   " */

#define WDTSSEL__VLO            WDTSSEL__VLOCLK

#endif
