/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kw2xd
 * @{
 *
 * @file
 * @brief       Interrupt vector definition for MKW2XDXXX MCUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "vectors_kinetis.h"

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_DMA0,            /* DMA channel 0 transfer complete */
    [ 1] = isr_DMA1,            /* DMA channel 1 transfer complete */
    [ 2] = isr_DMA2,            /* DMA channel 2 transfer complete */
    [ 3] = isr_DMA3,            /* DMA channel 3 transfer complete */
    [ 4] = isr_DMA4,            /* DMA channel 4 transfer complete */
    [ 5] = isr_DMA5,            /* DMA channel 5 transfer complete */
    [ 6] = isr_DMA6,            /* DMA channel 6 transfer complete */
    [ 7] = isr_DMA7,            /* DMA channel 7 transfer complete */
    [ 8] = isr_DMA8,            /* DMA channel 8 transfer complete */
    [ 9] = isr_DMA9,            /* DMA channel 9 transfer complete */
    [10] = isr_DMA10,           /* DMA channel 10 transfer complete */
    [11] = isr_DMA11,           /* DMA channel 11 transfer complete */
    [12] = isr_DMA12,           /* DMA channel 12 transfer complete */
    [13] = isr_DMA13,           /* DMA channel 13 transfer complete */
    [14] = isr_DMA14,           /* DMA channel 14 transfer complete */
    [15] = isr_DMA15,           /* DMA channel 15 transfer complete */
    [16] = isr_DMA_Error,       /* DMA channel 0 - 15 error */
    [17] = isr_MCM,             /* MCM normal interrupt */
    [18] = isr_FTFL,            /* FTFL command complete */
    [19] = isr_FTFL_Collision,  /* FTFL read collision */
    [20] = isr_LVD_LVW,         /* PMC controller low-voltage detect, low-voltage warning */
    [21] = isr_LLWU,            /* Low leakage wakeup */
    [22] = isr_WDOG_EWM,        /* Single interrupt vector for WDOG and EWM */
    [23] = isr_RNG,             /* Randon number generator */
    [24] = isr_I2C0,            /* Inter-integrated circuit 0 */
    [25] = isr_I2C1,            /* Inter-integrated circuit 1 */
    [26] = isr_SPI0,            /* Serial peripheral Interface 0 */
    [27] = isr_SPI1,            /* Serial peripheral Interface 1 */
    [28] = isr_I2S0_Tx,         /* Integrated interchip sound 0 transmit interrupt */
    [29] = isr_I2S0_Rx,         /* Integrated interchip sound 0 receive interrupt */
    [31] = isr_UART0_RX_TX,     /* UART0 receive/transmit interrupt */
    [32] = isr_UART0_ERR,       /* UART0 error interrupt */
    [33] = isr_UART1_RX_TX,     /* UART1 receive/transmit interrupt */
    [34] = isr_UART1_ERR,       /* UART1 error interrupt */
    [35] = isr_UART2_RX_TX,     /* UART2 receive/transmit interrupt */
    [36] = isr_UART2_ERR,       /* UART2 error interrupt */
    [39] = isr_ADC0,            /* Analog-to-digital converter 0 */
    [40] = isr_CMP0,            /* Comparator 0 */
    [41] = isr_CMP1,            /* Comparator 1 */
    [42] = isr_FTM0,            /* FlexTimer module 0 fault, overflow and channels interrupt */
    [43] = isr_FTM1,            /* FlexTimer module 1 fault, overflow and channels interrupt */
    [44] = isr_FTM2,            /* FlexTimer module 2 fault, overflow and channels interrupt */
    [45] = isr_CMT,             /* Carrier modulator transmitter */
    [46] = isr_RTC,             /* Real time clock */
    [47] = isr_RTC_Seconds,     /* Real time clock seconds */
    [48] = isr_PIT0,            /* Periodic interrupt timer channel 0 */
    [49] = isr_PIT1,            /* Periodic interrupt timer channel 1 */
    [50] = isr_PIT2,            /* Periodic interrupt timer channel 2 */
    [51] = isr_PIT3,            /* Periodic interrupt timer channel 3 */
    [52] = isr_PDB0,            /* Programmable delay block */
    [53] = isr_USB0,            /* USB OTG interrupt */
    [54] = isr_USBDCD,          /* USB charger detect */
    [57] = isr_MCG,             /* Multipurpose clock generator */
    [58] = isr_LPTMR0,          /* Low power timer interrupt */
    [59] = isr_PORTA,           /* Port A pin detect interrupt */
    [60] = isr_PORTB,           /* Port B pin detect interrupt */
    [61] = isr_PORTC,           /* Port C pin detect interrupt */
    [62] = isr_PORTD,           /* Port D pin detect interrupt */
    [63] = isr_PORTE,           /* Port E pin detect interrupt */
    [64] = isr_SWI,             /* Software interrupt */
};
