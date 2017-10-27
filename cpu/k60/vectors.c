/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 *
 * @brief Interrupt vector for K60 MCU.
 *
 * @author Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @note It is not necessary to modify this file to define custom interrupt
 * service routines. All symbols are defined weak, it is only necessary to
 * define a function with the same name in another file to override the default
 * interrupt handlers.
 */

/**
 * @name Interrupt vector definition
 * @{
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
    [28] = isr_SPI2,            /* Serial peripheral Interface 1 */
    [29] = isr_CAN0_ORed_Message_buffer, /* CAN0 ORed message buffers */
    [30] = isr_CAN0_Bus_Off,    /* CAN0 bus off */
    [31] = isr_CAN0_Error,      /* CAN0 error */
    [32] = isr_CAN0_Tx_Warning, /* CAN0 Tx warning */
    [33] = isr_CAN0_Rx_Warning, /* CAN0 Rx warning */
    [34] = isr_CAN0_Wake_Up,    /* CAN0 wake up */
    [35] = isr_I2S0_Tx,         /* Integrated interchip sound 0 transmit interrupt */
    [36] = isr_I2S0_Rx,         /* Integrated interchip sound 0 receive interrupt */
    [37] = isr_CAN1_ORed_Message_buffer, /* CAN1 OR'd message buffers interrupt */
    [38] = isr_CAN1_Bus_Off,    /* CAN1 bus off interrupt */
    [39] = isr_CAN1_Error,      /* CAN1 error interrupt */
    [40] = isr_CAN1_Tx_Warning, /* CAN1 Tx warning interrupt */
    [41] = isr_CAN1_Rx_Warning, /* CAN1 Rx warning interrupt */
    [42] = isr_CAN1_Wake_Up,    /* CAN1 wake up interrupt */
    [44] = isr_UART0_LON,       /* UART0 LON */
    [45] = isr_UART0_RX_TX,     /* UART0 receive/transmit interrupt */
    [46] = isr_UART0_ERR,       /* UART0 error interrupt */
    [47] = isr_UART1_RX_TX,     /* UART1 receive/transmit interrupt */
    [48] = isr_UART1_ERR,       /* UART1 error interrupt */
    [49] = isr_UART2_RX_TX,     /* UART2 receive/transmit interrupt */
    [50] = isr_UART2_ERR,       /* UART2 error interrupt */
    [51] = isr_UART3_RX_TX,     /* UART3 receive/transmit interrupt */
    [52] = isr_UART3_ERR,       /* UART3 error interrupt */
    [53] = isr_UART4_RX_TX,     /* UART4 receive/transmit interrupt */
    [54] = isr_UART4_ERR,       /* UART4 error interrupt */
    [55] = isr_UART5_RX_TX,     /* UART5 receive/transmit interrupt */
    [56] = isr_UART5_ERR,       /* UART5 error interrupt */
    [57] = isr_ADC0,            /* Analog-to-digital converter 0 */
    [58] = isr_ADC1,            /* Analog-to-digital converter 1 */
    [59] = isr_CMP0,            /* Comparator 0 */
    [60] = isr_CMP1,            /* Comparator 1 */
    [61] = isr_CMP2,            /* Comparator 2 */
    [62] = isr_FTM0,            /* FlexTimer module 0 fault, overflow and channels interrupt */
    [63] = isr_FTM1,            /* FlexTimer module 1 fault, overflow and channels interrupt */
    [64] = isr_FTM2,            /* FlexTimer module 2 fault, overflow and channels interrupt */
    [65] = isr_CMT,             /* Carrier modulator transmitter */
    [66] = isr_RTC,             /* Real time clock */
    [67] = isr_RTC_Seconds,     /* Real time clock seconds */
    [68] = isr_PIT0,            /* Periodic interrupt timer channel 0 */
    [69] = isr_PIT1,            /* Periodic interrupt timer channel 1 */
    [70] = isr_PIT2,            /* Periodic interrupt timer channel 2 */
    [71] = isr_PIT3,            /* Periodic interrupt timer channel 3 */
    [72] = isr_PDB0,            /* Programmable delay block */
    [73] = isr_USB0,            /* USB OTG interrupt */
    [74] = isr_USBDCD,          /* USB charger detect */
    [75] = isr_ENET_1588_Timer, /* Ethernet MAC IEEE 1588 timer */
    [76] = isr_ENET_Transmit,   /* Ethernet MAC transmit */
    [77] = isr_ENET_Receive,    /* Ethernet MAC receive */
    [78] = isr_ENET_Error,      /* Ethernet MAC error and miscelaneous */
    [80] = isr_SDHC,            /* Secure digital host controller */
    [81] = isr_DAC0,            /* Digital-to-analog converter 0 */
    [82] = isr_DAC1,            /* Digital-to-analog converter 1 */
    [83] = isr_TSI0,            /* TSI0 Interrupt */
    [84] = isr_MCG,             /* Multipurpose clock generator */
    [85] = isr_LPTMR0,          /* Low power timer interrupt */
    [87] = isr_PORTA,           /* Port A interrupt */
    [88] = isr_PORTB,           /* Port B interrupt */
    [89] = isr_PORTC,           /* Port C interrupt */
    [90] = isr_PORTD,           /* Port D interrupt */
    [91] = isr_PORTE,           /* Port E interrupt */
    [94] = isr_SWI,             /* Software interrupt */
};

/** @} */
