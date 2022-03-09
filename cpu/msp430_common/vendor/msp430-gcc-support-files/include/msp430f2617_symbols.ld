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

/* This file supports MSP430F2617 devices. */
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
PROVIDE(IE2                = 0x0001);
PROVIDE(IFG2               = 0x0003);
PROVIDE(UC1IE              = 0x0006);
PROVIDE(UC1IFG             = 0x0007);
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
* Basic Clock Module
************************************************************/
PROVIDE(DCOCTL             = 0x0056);
PROVIDE(BCSCTL1            = 0x0057);
PROVIDE(BCSCTL2            = 0x0058);
PROVIDE(BCSCTL3            = 0x0053);
/************************************************************
* Comparator A
************************************************************/
PROVIDE(CACTL1             = 0x0059);
PROVIDE(CACTL2             = 0x005A);
PROVIDE(CAPD               = 0x005B);
/************************************************************
* DAC12
************************************************************/
PROVIDE(DAC12_0CTL         = 0x01C0);
PROVIDE(DAC12_1CTL         = 0x01C2);
PROVIDE(DAC12_0DAT         = 0x01C8);
PROVIDE(DAC12_1DAT         = 0x01CA);
/************************************************************
* DMA_X
************************************************************/
PROVIDE(DMACTL0            = 0x0122);
PROVIDE(DMACTL1            = 0x0124);
PROVIDE(DMAIV              = 0x0126);
PROVIDE(DMA0CTL            = 0x01D0);
PROVIDE(DMA1CTL            = 0x01DC);
PROVIDE(DMA2CTL            = 0x01E8);
PROVIDE(DMA0SA             = 0x01D2);
PROVIDE(DMA0SAL            = 0x01D2);
PROVIDE(DMA0DA             = 0x01D6);
PROVIDE(DMA0DAL            = 0x01D6);
PROVIDE(DMA0SZ             = 0x01DA);
PROVIDE(DMA1SA             = 0x01DE);
PROVIDE(DMA1SAL            = 0x01DE);
PROVIDE(DMA1DA             = 0x01E2);
PROVIDE(DMA1DAL            = 0x01E2);
PROVIDE(DMA1SZ             = 0x01E6);
PROVIDE(DMA2SA             = 0x01EA);
PROVIDE(DMA2SAL            = 0x01EA);
PROVIDE(DMA2DA             = 0x01EE);
PROVIDE(DMA2DAL            = 0x01EE);
PROVIDE(DMA2SZ             = 0x01F2);
/*************************************************************
* Flash Memory
*************************************************************/
PROVIDE(FCTL1              = 0x0128);
PROVIDE(FCTL2              = 0x012A);
PROVIDE(FCTL3              = 0x012C);
PROVIDE(FCTL4              = 0x01BE);
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
* DIGITAL I/O Port1/2 Pull up / Pull down Resistors
************************************************************/
PROVIDE(P1IN               = 0x0020);
PROVIDE(P1OUT              = 0x0021);
PROVIDE(P1DIR              = 0x0022);
PROVIDE(P1IFG              = 0x0023);
PROVIDE(P1IES              = 0x0024);
PROVIDE(P1IE               = 0x0025);
PROVIDE(P1SEL              = 0x0026);
PROVIDE(P1REN              = 0x0027);
PROVIDE(P2IN               = 0x0028);
PROVIDE(P2OUT              = 0x0029);
PROVIDE(P2DIR              = 0x002A);
PROVIDE(P2IFG              = 0x002B);
PROVIDE(P2IES              = 0x002C);
PROVIDE(P2IE               = 0x002D);
PROVIDE(P2SEL              = 0x002E);
PROVIDE(P2REN              = 0x002F);
/************************************************************
* DIGITAL I/O Port3/4 Pull up / Pull down Resistors
************************************************************/
PROVIDE(P3IN               = 0x0018);
PROVIDE(P3OUT              = 0x0019);
PROVIDE(P3DIR              = 0x001A);
PROVIDE(P3SEL              = 0x001B);
PROVIDE(P3REN              = 0x0010);
PROVIDE(P4IN               = 0x001C);
PROVIDE(P4OUT              = 0x001D);
PROVIDE(P4DIR              = 0x001E);
PROVIDE(P4SEL              = 0x001F);
PROVIDE(P4REN              = 0x0011);
/************************************************************
* DIGITAL I/O Port5/6 Pull up / Pull down Resistors
************************************************************/
PROVIDE(P5IN               = 0x0030);
PROVIDE(P5OUT              = 0x0031);
PROVIDE(P5DIR              = 0x0032);
PROVIDE(P5SEL              = 0x0033);
PROVIDE(P5REN              = 0x0012);
PROVIDE(P6IN               = 0x0034);
PROVIDE(P6OUT              = 0x0035);
PROVIDE(P6DIR              = 0x0036);
PROVIDE(P6SEL              = 0x0037);
PROVIDE(P6REN              = 0x0013);
/************************************************************
* DIGITAL I/O Port7/8 Pull up / Pull down Resistors
************************************************************/
PROVIDE(P7IN               = 0x0038);
PROVIDE(P7OUT              = 0x003A);
PROVIDE(P7DIR              = 0x003C);
PROVIDE(P7SEL              = 0x003E);
PROVIDE(P7REN              = 0x0014);
PROVIDE(P8IN               = 0x0039);
PROVIDE(P8OUT              = 0x003B);
PROVIDE(P8DIR              = 0x003D);
PROVIDE(P8SEL              = 0x003F);
PROVIDE(P8REN              = 0x0015);
PROVIDE(PAIN               = 0x0038);
PROVIDE(PAOUT              = 0x003A);
PROVIDE(PADIR              = 0x003C);
PROVIDE(PASEL              = 0x003E);
PROVIDE(PAREN              = 0x0014);
/************************************************************
* Brown-Out, Supply Voltage Supervision (SVS)
************************************************************/
PROVIDE(SVSCTL             = 0x0055);
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
* USCI
************************************************************/
PROVIDE(UCA0CTL0           = 0x0060);
PROVIDE(UCA0CTL1           = 0x0061);
PROVIDE(UCA0BR0            = 0x0062);
PROVIDE(UCA0BR1            = 0x0063);
PROVIDE(UCA0MCTL           = 0x0064);
PROVIDE(UCA0STAT           = 0x0065);
PROVIDE(UCA0RXBUF          = 0x0066);
PROVIDE(UCA0TXBUF          = 0x0067);
PROVIDE(UCA0ABCTL          = 0x005D);
PROVIDE(UCA0IRTCTL         = 0x005E);
PROVIDE(UCA0IRRCTL         = 0x005F);
PROVIDE(UCB0CTL0           = 0x0068);
PROVIDE(UCB0CTL1           = 0x0069);
PROVIDE(UCB0BR0            = 0x006A);
PROVIDE(UCB0BR1            = 0x006B);
PROVIDE(UCB0I2CIE          = 0x006C);
PROVIDE(UCB0STAT           = 0x006D);
PROVIDE(UCB0RXBUF          = 0x006E);
PROVIDE(UCB0TXBUF          = 0x006F);
PROVIDE(UCB0I2COA          = 0x0118);
PROVIDE(UCB0I2CSA          = 0x011A);
PROVIDE(UCA1CTL0           = 0x00D0);
PROVIDE(UCA1CTL1           = 0x00D1);
PROVIDE(UCA1BR0            = 0x00D2);
PROVIDE(UCA1BR1            = 0x00D3);
PROVIDE(UCA1MCTL           = 0x00D4);
PROVIDE(UCA1STAT           = 0x00D5);
PROVIDE(UCA1RXBUF          = 0x00D6);
PROVIDE(UCA1TXBUF          = 0x00D7);
PROVIDE(UCA1ABCTL          = 0x00CD);
PROVIDE(UCA1IRTCTL         = 0x00CE);
PROVIDE(UCA1IRRCTL         = 0x00CF);
PROVIDE(UCB1CTL0           = 0x00D8);
PROVIDE(UCB1CTL1           = 0x00D9);
PROVIDE(UCB1BR0            = 0x00DA);
PROVIDE(UCB1BR1            = 0x00DB);
PROVIDE(UCB1I2CIE          = 0x00DC);
PROVIDE(UCB1STAT           = 0x00DD);
PROVIDE(UCB1RXBUF          = 0x00DE);
PROVIDE(UCB1TXBUF          = 0x00DF);
PROVIDE(UCB1I2COA          = 0x017C);
PROVIDE(UCB1I2CSA          = 0x017E);
/************************************************************
* WATCHDOG TIMER
************************************************************/
PROVIDE(WDTCTL             = 0x0120);
/************************************************************
* Calibration Data in Info Mem
************************************************************/
PROVIDE(TLV_CHECKSUM       = 0x10C0);
PROVIDE(TLV_DCO_30_TAG     = 0x10F6);
PROVIDE(TLV_DCO_30_LEN     = 0x10F7);
PROVIDE(TLV_ADC12_1_TAG    = 0x10DA);
PROVIDE(TLV_ADC12_1_LEN    = 0x10DB);
/************************************************************
* Calibration Data in Info Mem
************************************************************/
PROVIDE(CALDCO_16MHZ       = 0x10F8);
PROVIDE(CALBC1_16MHZ       = 0x10F9);
PROVIDE(CALDCO_12MHZ       = 0x10FA);
PROVIDE(CALBC1_12MHZ       = 0x10FB);
PROVIDE(CALDCO_8MHZ        = 0x10FC);
PROVIDE(CALBC1_8MHZ        = 0x10FD);
PROVIDE(CALDCO_1MHZ        = 0x10FE);
PROVIDE(CALBC1_1MHZ        = 0x10FF);
/************************************************************
* Interrupt Vectors (offset from 0xFFC0)
************************************************************/
/************************************************************
* End of Modules
************************************************************/
