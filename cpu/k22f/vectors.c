/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k22f
 * @{
 *
 * @file
 * @brief       Interrupt vector definition for K22F MCUs
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "vectors_kinetis.h"

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_DMA0,            /* DMA Channel 0 Transfer Complete */
    [ 1] = isr_DMA1,            /* DMA Channel 1 Transfer Complete */
    [ 2] = isr_DMA2,            /* DMA Channel 2 Transfer Complete */
    [ 3] = isr_DMA3,            /* DMA Channel 3 Transfer Complete */
    [ 4] = isr_DMA4,            /* DMA Channel 4 Transfer Complete */
    [ 5] = isr_DMA5,            /* DMA Channel 5 Transfer Complete */
    [ 6] = isr_DMA6,            /* DMA Channel 6 Transfer Complete */
    [ 7] = isr_DMA7,            /* DMA Channel 7 Transfer Complete */
    [ 8] = isr_DMA8,            /* DMA Channel 8 Transfer Complete */
    [ 9] = isr_DMA9,            /* DMA Channel 9 Transfer Complete */
    [10] = isr_DMA10,           /* DMA Channel 10 Transfer Complete */
    [11] = isr_DMA11,           /* DMA Channel 11 Transfer Complete */
    [12] = isr_DMA12,           /* DMA Channel 12 Transfer Complete */
    [13] = isr_DMA13,           /* DMA Channel 13 Transfer Complete */
    [14] = isr_DMA14,           /* DMA Channel 14 Transfer Complete */
    [15] = isr_DMA15,           /* DMA Channel 15 Transfer Complete */
    [16] = isr_DMA_Error,       /* DMA Error Interrupt */
    [17] = isr_MCM,             /* Normal Interrupt */
    [18] = isr_FTFA,            /* FTFA Command complete interrupt */
    [19] = isr_FTFA_Collision,  /* Read Collision Interrupt */
    [20] = isr_LVD_LVW,         /* Low Voltage Detect, Low Voltage Warning */
    [21] = isr_LLWU,            /* Low Leakage Wakeup Unit */
    [22] = isr_WDOG_EWM,        /* WDOG Interrupt */
    [23] = isr_RNG,             /* RNG Interrupt */
    [24] = isr_I2C0,            /* I2C0 interrupt */
    [25] = isr_I2C1,            /* I2C1 interrupt */
    [26] = isr_SPI0,            /* SPI0 Interrupt */
    [27] = isr_SPI1,            /* SPI1 Interrupt */
    [28] = isr_I2S0_Tx,         /* I2S0 transmit interrupt */
    [29] = isr_I2S0_Rx,         /* I2S0 receive interrupt */
    [30] = isr_LPUART0,         /* LPUART0 status/error interrupt */
    [31] = isr_UART0_RX_TX,     /* UART0 Receive/Transmit interrupt */
    [32] = isr_UART0_ERR,       /* UART0 Error interrupt */
    [33] = isr_UART1_RX_TX,     /* UART1 Receive/Transmit interrupt */
    [34] = isr_UART1_ERR,       /* UART1 Error interrupt */
    [35] = isr_UART2_RX_TX,     /* UART2 Receive/Transmit interrupt */
    [36] = isr_UART2_ERR,       /* UART2 Error interrupt */
    [39] = isr_ADC0,            /* ADC0 interrupt */
    [40] = isr_CMP0,            /* CMP0 interrupt */
    [41] = isr_CMP1,            /* CMP1 interrupt */
    [42] = isr_FTM0,            /* FTM0 fault, overflow and channels interrupt */
    [43] = isr_FTM1,            /* FTM1 fault, overflow and channels interrupt */
    [44] = isr_FTM2,            /* FTM2 fault, overflow and channels interrupt */
    [46] = isr_RTC,             /* RTC interrupt */
    [47] = isr_RTC_Seconds,     /* RTC seconds interrupt */
    [48] = isr_PIT0,            /* PIT timer channel 0 interrupt */
    [49] = isr_PIT1,            /* PIT timer channel 1 interrupt */
    [50] = isr_PIT2,            /* PIT timer channel 2 interrupt */
    [51] = isr_PIT3,            /* PIT timer channel 3 interrupt */
    [52] = isr_PDB0,            /* PDB0 Interrupt */
    [53] = isr_USB0,            /* USB0 interrupt */
    [56] = isr_DAC0,            /* DAC0 interrupt */
    [57] = isr_MCG,             /* MCG Interrupt */
    [58] = isr_LPTMR0,          /* LPTimer interrupt */
    [59] = isr_PORTA,           /* Port A interrupt */
    [60] = isr_PORTB,           /* Port B interrupt */
    [61] = isr_PORTC,           /* Port C interrupt */
    [62] = isr_PORTD,           /* Port D interrupt */
    [63] = isr_PORTE,           /* Port E interrupt */
    [64] = isr_SWI,             /* Software interrupt */
    [71] = isr_FTM3,            /* FTM3 fault, overflow and channels interrupt */
    [72] = isr_DAC1,            /* DAC1 interrupt */
    [73] = isr_ADC1,            /* ADC1 interrupt */
};
