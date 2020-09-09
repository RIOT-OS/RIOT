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

/* This file supports MSP430F1611 devices. */
/* Version: 1.210 */

/************************************************************
* STANDARD BITS
************************************************************/
/************************************************************
* STATUS REGISTER BITS
************************************************************/
/************************************************************
* PERIPHERAL FILE MAP
************************************************************/
/************************************************************
* SPECIAL FUNCTION REGISTER ADDRESSES + CONTROL BITS
************************************************************/
PROVIDE(IE1                = 0x0000);
PROVIDE(IFG1               = 0x0002);
PROVIDE(ME1                = 0x0004);
PROVIDE(IE2                = 0x0001);
PROVIDE(IFG2               = 0x0003);
PROVIDE(ME2                = 0x0005);
/************************************************************
* WATCHDOG TIMER
************************************************************/
PROVIDE(WDTCTL             = 0x0120);
/************************************************************
* HARDWARE MULTIPLIER
************************************************************/
PROVIDE(MPY                = 0x0130);
PROVIDE(MPYS               = 0x0132);
PROVIDE(MAC                = 0x0134);
PROVIDE(MACS               = 0x0136);
PROVIDE(OP2                = 0x0138);
PROVIDE(RESLO              = 0x013A);
PROVIDE(RESHI              = 0x013C);
PROVIDE(SUMEXT             = 0x013E);
/************************************************************
* DIGITAL I/O Port1/2
************************************************************/
PROVIDE(P1IN               = 0x0020);
PROVIDE(P1OUT              = 0x0021);
PROVIDE(P1DIR              = 0x0022);
PROVIDE(P1IFG              = 0x0023);
PROVIDE(P1IES              = 0x0024);
PROVIDE(P1IE               = 0x0025);
PROVIDE(P1SEL              = 0x0026);
PROVIDE(P2IN               = 0x0028);
PROVIDE(P2OUT              = 0x0029);
PROVIDE(P2DIR              = 0x002A);
PROVIDE(P2IFG              = 0x002B);
PROVIDE(P2IES              = 0x002C);
PROVIDE(P2IE               = 0x002D);
PROVIDE(P2SEL              = 0x002E);
/************************************************************
* DIGITAL I/O Port3/4
************************************************************/
PROVIDE(P3IN               = 0x0018);
PROVIDE(P3OUT              = 0x0019);
PROVIDE(P3DIR              = 0x001A);
PROVIDE(P3SEL              = 0x001B);
PROVIDE(P4IN               = 0x001C);
PROVIDE(P4OUT              = 0x001D);
PROVIDE(P4DIR              = 0x001E);
PROVIDE(P4SEL              = 0x001F);
/************************************************************
* DIGITAL I/O Port5/6
************************************************************/
PROVIDE(P5IN               = 0x0030);
PROVIDE(P5OUT              = 0x0031);
PROVIDE(P5DIR              = 0x0032);
PROVIDE(P5SEL              = 0x0033);
PROVIDE(P6IN               = 0x0034);
PROVIDE(P6OUT              = 0x0035);
PROVIDE(P6DIR              = 0x0036);
PROVIDE(P6SEL              = 0x0037);
/************************************************************
* USART
************************************************************/
/************************************************************
* USART 0
************************************************************/
PROVIDE(U0CTL              = 0x0070);
PROVIDE(U0TCTL             = 0x0071);
PROVIDE(U0RCTL             = 0x0072);
PROVIDE(U0MCTL             = 0x0073);
PROVIDE(U0BR0              = 0x0074);
PROVIDE(U0BR1              = 0x0075);
PROVIDE(U0RXBUF            = 0x0076);
PROVIDE(U0TXBUF            = 0x0077);
/************************************************************
* USART 1
************************************************************/
PROVIDE(U1CTL              = 0x0078);
PROVIDE(U1TCTL             = 0x0079);
PROVIDE(U1RCTL             = 0x007A);
PROVIDE(U1MCTL             = 0x007B);
PROVIDE(U1BR0              = 0x007C);
PROVIDE(U1BR1              = 0x007D);
PROVIDE(U1RXBUF            = 0x007E);
PROVIDE(U1TXBUF            = 0x007F);
/************************************************************
* USART0  I2C
************************************************************/
PROVIDE(I2CIE              = 0x0050);
PROVIDE(I2CIFG             = 0x0051);
PROVIDE(I2CNDAT            = 0x0052);
PROVIDE(I2CTCTL            = 0x0071);
PROVIDE(I2CDCTL            = 0x0072);
PROVIDE(I2CPSC             = 0x0073);
PROVIDE(I2CSCLH            = 0x0074);
PROVIDE(I2CSCLL            = 0x0075);
PROVIDE(I2CDRB             = 0x0076);
PROVIDE(I2CDRW             = 0x0076);
PROVIDE(I2COA              = 0x0118);
PROVIDE(I2CSA              = 0x011A);
PROVIDE(I2CIV              = 0x011C);
/************************************************************
* Timer A3
************************************************************/
PROVIDE(TAIV               = 0x012E);
PROVIDE(TACTL              = 0x0160);
PROVIDE(TACCTL0            = 0x0162);
PROVIDE(TACCTL1            = 0x0164);
PROVIDE(TACCTL2            = 0x0166);
PROVIDE(TAR                = 0x0170);
PROVIDE(TACCR0             = 0x0172);
PROVIDE(TACCR1             = 0x0174);
PROVIDE(TACCR2             = 0x0176);
/************************************************************
* Timer B7
************************************************************/
PROVIDE(TBIV               = 0x011E);
PROVIDE(TBCTL              = 0x0180);
PROVIDE(TBCCTL0            = 0x0182);
PROVIDE(TBCCTL1            = 0x0184);
PROVIDE(TBCCTL2            = 0x0186);
PROVIDE(TBCCTL3            = 0x0188);
PROVIDE(TBCCTL4            = 0x018A);
PROVIDE(TBCCTL5            = 0x018C);
PROVIDE(TBCCTL6            = 0x018E);
PROVIDE(TBR                = 0x0190);
PROVIDE(TBCCR0             = 0x0192);
PROVIDE(TBCCR1             = 0x0194);
PROVIDE(TBCCR2             = 0x0196);
PROVIDE(TBCCR3             = 0x0198);
PROVIDE(TBCCR4             = 0x019A);
PROVIDE(TBCCR5             = 0x019C);
PROVIDE(TBCCR6             = 0x019E);
/************************************************************
* Basic Clock Module
************************************************************/
PROVIDE(DCOCTL             = 0x0056);
PROVIDE(BCSCTL1            = 0x0057);
PROVIDE(BCSCTL2            = 0x0058);
/************************************************************
* Brown-Out, Supply Voltage Supervision (SVS)
************************************************************/
PROVIDE(SVSCTL             = 0x0055);
/*************************************************************
* Flash Memory
*************************************************************/
PROVIDE(FCTL1              = 0x0128);
PROVIDE(FCTL2              = 0x012A);
PROVIDE(FCTL3              = 0x012C);
/************************************************************
* Comparator A
************************************************************/
PROVIDE(CACTL1             = 0x0059);
PROVIDE(CACTL2             = 0x005A);
PROVIDE(CAPD               = 0x005B);
/************************************************************
* ADC12
************************************************************/
PROVIDE(ADC12CTL0          = 0x01A0);
PROVIDE(ADC12CTL1          = 0x01A2);
PROVIDE(ADC12IFG           = 0x01A4);
PROVIDE(ADC12IE            = 0x01A6);
PROVIDE(ADC12IV            = 0x01A8);
PROVIDE(ADC12MEM0          = 0x0140);
PROVIDE(ADC12MEM1          = 0x0142);
PROVIDE(ADC12MEM2          = 0x0144);
PROVIDE(ADC12MEM3          = 0x0146);
PROVIDE(ADC12MEM4          = 0x0148);
PROVIDE(ADC12MEM5          = 0x014A);
PROVIDE(ADC12MEM6          = 0x014C);
PROVIDE(ADC12MEM7          = 0x014E);
PROVIDE(ADC12MEM8          = 0x0150);
PROVIDE(ADC12MEM9          = 0x0152);
PROVIDE(ADC12MEM10         = 0x0154);
PROVIDE(ADC12MEM11         = 0x0156);
PROVIDE(ADC12MEM12         = 0x0158);
PROVIDE(ADC12MEM13         = 0x015A);
PROVIDE(ADC12MEM14         = 0x015C);
PROVIDE(ADC12MEM15         = 0x015E);
PROVIDE(ADC12MCTL0         = 0x0080);
PROVIDE(ADC12MCTL1         = 0x0081);
PROVIDE(ADC12MCTL2         = 0x0082);
PROVIDE(ADC12MCTL3         = 0x0083);
PROVIDE(ADC12MCTL4         = 0x0084);
PROVIDE(ADC12MCTL5         = 0x0085);
PROVIDE(ADC12MCTL6         = 0x0086);
PROVIDE(ADC12MCTL7         = 0x0087);
PROVIDE(ADC12MCTL8         = 0x0088);
PROVIDE(ADC12MCTL9         = 0x0089);
PROVIDE(ADC12MCTL10        = 0x008A);
PROVIDE(ADC12MCTL11        = 0x008B);
PROVIDE(ADC12MCTL12        = 0x008C);
PROVIDE(ADC12MCTL13        = 0x008D);
PROVIDE(ADC12MCTL14        = 0x008E);
PROVIDE(ADC12MCTL15        = 0x008F);
/************************************************************
* DAC12
************************************************************/
PROVIDE(DAC12_0CTL         = 0x01C0);
PROVIDE(DAC12_1CTL         = 0x01C2);
PROVIDE(DAC12_0DAT         = 0x01C8);
PROVIDE(DAC12_1DAT         = 0x01CA);
/************************************************************
* DMA
************************************************************/
PROVIDE(DMACTL0            = 0x0122);
PROVIDE(DMACTL1            = 0x0124);
PROVIDE(DMA0CTL            = 0x01E0);
PROVIDE(DMA1CTL            = 0x01E8);
PROVIDE(DMA2CTL            = 0x01F0);
PROVIDE(DMA0SA             = 0x01E2);
PROVIDE(DMA0DA             = 0x01E4);
PROVIDE(DMA0SZ             = 0x01E6);
PROVIDE(DMA1SA             = 0x01EA);
PROVIDE(DMA1DA             = 0x01EC);
PROVIDE(DMA1SZ             = 0x01EE);
PROVIDE(DMA2SA             = 0x01F2);
PROVIDE(DMA2DA             = 0x01F4);
PROVIDE(DMA2SZ             = 0x01F6);
/************************************************************
* Interrupt Vectors (offset from 0xFFE0)
************************************************************/
/************************************************************
* End of Modules
************************************************************/
