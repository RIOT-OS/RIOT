/*
 * SPDX-FileCopyrightText: 2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_kinetis
 * @{
 *
 * @file
 *
 * @brief Interrupt vector for Kinetis MCUs
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
#ifdef DMA0
/* Devices with >16 DMA channels combine two channels per IRQ number */
#if defined(DMA_INT_INT16_MASK)
    [DMA0_DMA16_IRQn ] = isr_dma0_dma16,      /* DMA Channel 0, 16 Transfer Complete */
#elif defined(DMA_INT_INT0_MASK)
    [DMA0_IRQn       ] = isr_dma0,            /* DMA Channel 0 Transfer Complete */
#endif
#if defined(DMA_INT_INT17_MASK)
    [DMA1_DMA17_IRQn ] = isr_dma1_dma17,      /* DMA Channel 1, 17 Transfer Complete */
#elif defined(DMA_INT_INT1_MASK)
    [DMA1_IRQn       ] = isr_dma1,            /* DMA Channel 1 Transfer Complete */
#endif
#if defined(DMA_INT_INT18_MASK)
    [DMA2_DMA18_IRQn ] = isr_dma2_dma18,      /* DMA Channel 2, 18 Transfer Complete */
#elif defined(DMA_INT_INT2_MASK)
    [DMA2_IRQn       ] = isr_dma2,            /* DMA Channel 2 Transfer Complete */
#endif
#if defined(DMA_INT_INT19_MASK)
    [DMA3_DMA19_IRQn ] = isr_dma3_dma19,      /* DMA Channel 3, 19 Transfer Complete */
#elif defined(DMA_INT_INT3_MASK)
    [DMA3_IRQn       ] = isr_dma3,            /* DMA Channel 3 Transfer Complete */
#endif
#if defined(DMA_INT_INT20_MASK)
    [DMA4_DMA20_IRQn ] = isr_dma4_dma20,      /* DMA Channel 4, 20 Transfer Complete */
#elif defined(DMA_INT_INT4_MASK)
    [DMA4_IRQn       ] = isr_dma4,            /* DMA Channel 4 Transfer Complete */
#endif
#if defined(DMA_INT_INT21_MASK)
    [DMA5_DMA21_IRQn ] = isr_dma5_dma21,      /* DMA Channel 5, 21 Transfer Complete */
#elif defined(DMA_INT_INT5_MASK)
    [DMA5_IRQn       ] = isr_dma5,            /* DMA Channel 5 Transfer Complete */
#endif
#if defined(DMA_INT_INT22_MASK)
    [DMA6_DMA22_IRQn ] = isr_dma6_dma22,      /* DMA Channel 6, 22 Transfer Complete */
#elif defined(DMA_INT_INT6_MASK)
    [DMA6_IRQn       ] = isr_dma6,            /* DMA Channel 6 Transfer Complete */
#endif
#if defined(DMA_INT_INT23_MASK)
    [DMA7_DMA23_IRQn ] = isr_dma7_dma23,      /* DMA Channel 7, 23 Transfer Complete */
#elif defined(DMA_INT_INT7_MASK)
    [DMA7_IRQn       ] = isr_dma7,            /* DMA Channel 7 Transfer Complete */
#endif
#if defined(DMA_INT_INT24_MASK)
    [DMA8_DMA24_IRQn ] = isr_dma8_dma24,      /* DMA Channel 8, 24 Transfer Complete */
#elif defined(DMA_INT_INT8_MASK)
    [DMA8_IRQn       ] = isr_dma8,            /* DMA Channel 8 Transfer Complete */
#endif
#if defined(DMA_INT_INT25_MASK)
    [DMA9_DMA25_IRQn ] = isr_dma9_dma25,      /* DMA Channel 9, 25 Transfer Complete */
#elif defined(DMA_INT_INT9_MASK)
    [DMA9_IRQn       ] = isr_dma9,            /* DMA Channel 9 Transfer Complete */
#endif
#if defined(DMA_INT_INT26_MASK)
    [DMA10_DMA26_IRQn] = isr_dma10_dma26,     /* DMA Channel 10, 26 Transfer Complete */
#elif defined(DMA_INT_INT10_MASK)
    [DMA10_IRQn      ] = isr_dma10,           /* DMA Channel 10 Transfer Complete */
#endif
#if defined(DMA_INT_INT27_MASK)
    [DMA11_DMA27_IRQn] = isr_dma11_dma27,     /* DMA Channel 11, 27 Transfer Complete */
#elif defined(DMA_INT_INT11_MASK)
    [DMA11_IRQn      ] = isr_dma11,           /* DMA Channel 11 Transfer Complete */
#endif
#if defined(DMA_INT_INT28_MASK)
    [DMA12_DMA28_IRQn] = isr_dma12_dma28,     /* DMA Channel 12, 28 Transfer Complete */
#elif defined(DMA_INT_INT12_MASK)
    [DMA12_IRQn      ] = isr_dma12,           /* DMA Channel 12 Transfer Complete */
#endif
#if defined(DMA_INT_INT29_MASK)
    [DMA13_DMA29_IRQn] = isr_dma13_dma29,     /* DMA Channel 13, 29 Transfer Complete */
#elif defined(DMA_INT_INT13_MASK)
    [DMA13_IRQn      ] = isr_dma13,           /* DMA Channel 13 Transfer Complete */
#endif
#if defined(DMA_INT_INT30_MASK)
    [DMA14_DMA30_IRQn] = isr_dma14_dma30,     /* DMA Channel 14, 30 Transfer Complete */
#elif defined(DMA_INT_INT14_MASK)
    [DMA14_IRQn      ] = isr_dma14,           /* DMA Channel 14 Transfer Complete */
#endif
#if defined(DMA_INT_INT31_MASK)
    [DMA15_DMA31_IRQn] = isr_dma15_dma31,     /* DMA Channel 15, 31 Transfer Complete */
#elif defined(DMA_INT_INT15_MASK)
    [DMA15_IRQn      ] = isr_dma15,           /* DMA Channel 15 Transfer Complete */
#endif
#ifndef KINETIS_CORE_Z
    [DMA_Error_IRQn  ] = isr_dma_error,       /* DMA Error Interrupt */
#endif
#endif /* defined(DMA0) */
#if defined(MCM) && !defined(KINETIS_CORE_Z)
    [MCM_IRQn        ] = isr_mcm,             /* Normal Interrupt */
#endif
#if defined(FTFA)
    [FTFA_IRQn       ] = isr_ftfa,            /* FTFA command complete */
#ifndef KINETIS_CORE_Z
    [FTFA_Collision_IRQn] = isr_ftfa_collision, /* FTFA read collision */
#endif
#elif defined(FTFE)
    [FTFE_IRQn       ] = isr_ftfe,            /* FTFE command complete */
#ifndef KINETIS_CORE_Z
    [FTFE_Collision_IRQn] = isr_ftfe_collision, /* FTFE read collision */
#endif
#elif defined(FTFL)
    [FTFL_IRQn       ] = isr_ftfl,            /* FTFL command complete */
#ifndef KINETIS_CORE_Z
    [FTFL_Collision_IRQn] = isr_ftfl_collision, /* FTFL read collision */
#endif
#endif
#ifdef PMC
    [LVD_LVW_IRQn    ] = isr_lvd_lvw,         /* Low Voltage Detect, Low Voltage Warning */
#endif
#ifdef LLWU
    [LLWU_IRQn       ] = isr_llwu,            /* Low Leakage Wakeup Unit */
#endif
#ifdef WDOG
    [WDOG_EWM_IRQn   ] = isr_wdog_ewm,        /* WDOG/EWM Interrupt */
#endif
#ifdef RNG
    [RNG_IRQn        ] = isr_rng,             /* RNG Interrupt */
#endif
#ifdef I2C0
    [I2C0_IRQn       ] = isr_i2c0,            /* I2C0 interrupt */
#endif
#ifdef I2C1
    [I2C1_IRQn       ] = isr_i2c1,            /* I2C1 interrupt */
#endif
#ifdef I2C2
    [I2C2_IRQn       ] = isr_i2c2,            /* I2C2 interrupt */
#endif
#ifdef I2C3
    [I2C3_IRQn       ] = isr_i2c3,            /* I2C3 interrupt */
#endif
#ifdef SPI0
    [SPI0_IRQn       ] = isr_spi0,            /* SPI0 Interrupt */
#endif
#ifdef SPI1
    [SPI1_IRQn       ] = isr_spi1,            /* SPI1 Interrupt */
#endif
#ifdef SPI2
    [SPI2_IRQn       ] = isr_spi2,            /* SPI2 Interrupt */
#endif
#ifdef I2S0
#ifdef I2S_TCR1_TFW_MASK
    /* K parts */
    [I2S0_Tx_IRQn    ] = isr_i2s0_tx,         /* I2S0 transmit interrupt */
    [I2S0_Rx_IRQn    ] = isr_i2s0_rx,         /* I2S0 receive interrupt */
#else
    /* KL parts */
    [I2S0_IRQn       ] = isr_i2s0,            /* I2S0 interrupt */
#endif
#endif
#ifdef UART0
#ifdef KINETIS_SINGLE_UART_IRQ
    [UART0_IRQn] = isr_uart0,     /* UART0 interrupt */
#else
#ifdef UART_RPL_RPL_MASK
    [UART0_LON_IRQn  ] = isr_uart0_lon,       /* UART0 LON interrupt */
#endif
    [UART0_RX_TX_IRQn] = isr_uart0_rx_tx,     /* UART0 Receive/Transmit interrupt */
    [UART0_ERR_IRQn  ] = isr_uart0_err,       /* UART0 Error interrupt */
#endif
#endif
#ifdef UART1
#ifdef KINETIS_SINGLE_UART_IRQ
    [UART1_IRQn] = isr_uart1,     /* UART1 interrupt */
#else
    [UART1_RX_TX_IRQn] = isr_uart1_rx_tx,     /* UART1 Receive/Transmit interrupt */
    [UART1_ERR_IRQn  ] = isr_uart1_err,       /* UART1 Error interrupt */
#endif
#endif
#ifdef UART2
#if defined(KINETIS_SINGLE_UART_IRQ)
    [UART2_IRQn] = isr_uart2,                 /* UART2 interrupt */
#elif defined(FLEXIO_VERID_MAJOR_MASK)
    /* KL parts with FlexIO uses combined IRQ */
    [UART2_FLEXIO_IRQn] = isr_uart2_flexio,   /* UART2 or FLEXIO */
#else
    [UART2_RX_TX_IRQn] = isr_uart2_rx_tx,     /* UART2 Receive/Transmit interrupt */
    [UART2_ERR_IRQn  ] = isr_uart2_err,       /* UART2 Error interrupt */
#endif
#endif
#ifdef UART3
#ifdef KINETIS_SINGLE_UART_IRQ
    [UART3_IRQn] = isr_uart3,     /* UART3 interrupt */
#else
    [UART3_RX_TX_IRQn] = isr_uart3_rx_tx,     /* UART3 Receive/Transmit interrupt */
    [UART3_ERR_IRQn  ] = isr_uart3_err,       /* UART3 Error interrupt */
#endif
#endif
#ifdef UART4
#ifdef KINETIS_SINGLE_UART_IRQ
    [UART4_IRQn] = isr_uart4,     /* UART4 interrupt */
#else
    [UART4_RX_TX_IRQn] = isr_uart4_rx_tx,     /* UART4 Receive/Transmit interrupt */
    [UART4_ERR_IRQn  ] = isr_uart4_err,       /* UART4 Error interrupt */
#endif
#endif
#ifdef UART5
#ifdef KINETIS_SINGLE_UART_IRQ
    [UART5_IRQn] = isr_uart5,     /* UART5 interrupt */
#else
    [UART5_RX_TX_IRQn] = isr_uart5_rx_tx,     /* UART5 Receive/Transmit interrupt */
    [UART5_ERR_IRQn  ] = isr_uart5_err,       /* UART5 Error interrupt */
#endif
#endif
#ifdef ADC0
    [ADC0_IRQn       ] = isr_adc0,            /* ADC0 interrupt */
#endif
#ifdef ADC1
    [ADC1_IRQn       ] = isr_adc1,            /* ADC1 interrupt */
#endif
#ifdef ADC2
    [ADC2_IRQn       ] = isr_adc2,            /* ADC2 interrupt */
#endif
#ifdef CMP0
    [CMP0_IRQn       ] = isr_cmp0,            /* CMP0 interrupt */
#endif
#ifdef CMP1
    [CMP1_IRQn       ] = isr_cmp1,            /* CMP1 interrupt */
#endif
#ifdef CMP2
    [CMP2_IRQn       ] = isr_cmp2,            /* CMP2 interrupt */
#endif
#ifdef CMP3
    [CMP3_IRQn       ] = isr_cmp3,            /* CMP3 interrupt */
#endif
#ifdef FTM0
    [FTM0_IRQn       ] = isr_ftm0,            /* FTM0 fault, overflow and channels interrupt */
#endif
#ifdef FTM1
    [FTM1_IRQn       ] = isr_ftm1,            /* FTM1 fault, overflow and channels interrupt */
#endif
#ifdef FTM2
    [FTM2_IRQn       ] = isr_ftm2,            /* FTM2 fault, overflow and channels interrupt */
#endif
#ifdef FTM3
    [FTM3_IRQn       ] = isr_ftm3,            /* FTM3 fault, overflow and channels interrupt */
#endif
#ifdef CMT
    [CMT_IRQn        ] = isr_cmt,             /* CMT interrupt */
#endif
#ifdef RTC
    [RTC_IRQn        ] = isr_rtc,             /* RTC interrupt */
#  ifndef KINETIS_SERIES_EA
    [RTC_Seconds_IRQn] = isr_rtc_seconds,     /* RTC seconds interrupt */
#  endif
#endif
#ifdef PIT
#ifdef KINETIS_CORE_Z
#  ifdef KINETIS_SERIES_EA
    [PIT0_IRQn        ] = isr_pit0,             /* PIT timer channel 0 interrupt */
    [PIT1_IRQn        ] = isr_pit1,             /* PIT timer channel 1 interrupt */
#  else
    [PIT_IRQn        ] = isr_pit,             /* PIT any channel interrupt */
#endif
#else
    [PIT0_IRQn       ] = isr_pit0,            /* PIT timer channel 0 interrupt */
    [PIT1_IRQn       ] = isr_pit1,            /* PIT timer channel 1 interrupt */
    [PIT2_IRQn       ] = isr_pit2,            /* PIT timer channel 2 interrupt */
    [PIT3_IRQn       ] = isr_pit3,            /* PIT timer channel 3 interrupt */
#endif
#endif /* defined(PIT) */
#ifdef PDB0
    [PDB0_IRQn       ] = isr_pdb0,            /* PDB0 Interrupt */
#endif
#ifdef USB0
    [USB0_IRQn       ] = isr_usb0,            /* USB0 interrupt */
#endif
#ifdef USBDCD
    [USBDCD_IRQn     ] = isr_usbdcd,          /* USBDCD Interrupt */
#endif
#if DAC0_BASE /* Not #ifdef because of error in MKW2xD.h files */
    [DAC0_IRQn       ] = isr_dac0,            /* DAC0 interrupt */
#endif
#ifdef DAC1
    [DAC1_IRQn       ] = isr_dac1,            /* DAC1 interrupt */
#endif
#ifdef MCG
#ifndef MCG_MC_LIRC_DIV2_MASK
    /* Only on full MCG, not MCG_Lite */
    [MCG_IRQn        ] = isr_mcg,             /* MCG Interrupt */
#endif /* MCG_MC_LIRC_DIV2_MASK */
#endif /* MCG */
#ifdef LPTMR0
    [LPTMR0_IRQn     ] = isr_lptmr0,          /* LPTimer interrupt */
#endif
#ifdef PORTA
    [PORTA_IRQn      ] = isr_porta,           /* Port A interrupt */
#endif
#ifdef KINETIS_CORE_Z
#if defined(PORTB) && defined(PORTC) && !defined(PORTD)
    [PORTB_PORTC_IRQn] = isr_portb_portc,     /* Port B, C combined interrupt */
#elif defined(PORTC) && defined(PORTD)
    [PORTC_PORTD_IRQn] = isr_portc_portd,     /* Port C, D combined interrupt */
#endif
#else
#ifdef PORTB
    [PORTB_IRQn      ] = isr_portb,           /* Port B interrupt */
#endif
#ifdef PORTC
    [PORTC_IRQn      ] = isr_portc,           /* Port C interrupt */
#endif
#ifdef PORTD
    [PORTD_IRQn      ] = isr_portd,           /* Port D interrupt */
#endif
#ifdef PORTE
    [PORTE_IRQn      ] = isr_porte,           /* Port E interrupt */
#endif
#endif
#if __CORTEX_M >= 3
    [SWI_IRQn        ] = isr_swi,             /* Software interrupt */
#endif
#ifdef CAN0
    [CAN0_ORed_Message_buffer_IRQn] = isr_can0_ored_message_buffer, /* CAN0 OR'd message buffers interrupt */
    [CAN0_Bus_Off_IRQn] = isr_can0_bus_off,    /* CAN0 bus off interrupt */
    [CAN0_Error_IRQn ] = isr_can0_error,      /* CAN0 error interrupt */
    [CAN0_Tx_Warning_IRQn] = isr_can0_tx_warning, /* CAN0 Tx warning interrupt */
    [CAN0_Rx_Warning_IRQn] = isr_can0_rx_warning, /* CAN0 Rx warning interrupt */
    [CAN0_Wake_Up_IRQn] = isr_can0_wake_up,    /* CAN0 wake up interrupt */
#endif
#ifdef CAN1
    [CAN1_ORed_Message_buffer_IRQn] = isr_can1_ored_message_buffer, /* CAN1 OR'd message buffers interrupt */
    [CAN1_Bus_Off_IRQn] = isr_can1_bus_off,    /* CAN1 bus off interrupt */
    [CAN1_Error_IRQn ] = isr_can1_error,      /* CAN1 error interrupt */
    [CAN1_Tx_Warning_IRQn] = isr_can1_tx_warning, /* CAN1 Tx warning interrupt */
    [CAN1_Rx_Warning_IRQn] = isr_can1_rx_warning, /* CAN1 Rx warning interrupt */
    [CAN1_Wake_Up_IRQn] = isr_can1_wake_up,    /* CAN1 wake up interrupt */
#endif
#ifdef MSCAN
    [MSCAN_RX_IRQn] = isr_mscan_rx, /* MSCAN RX interrupt */
    [MSCAN_TX_IRQn] = isr_mscan_tx, /* MSCAN TX/Err/Wake-up interrupt */
#endif
#ifdef SDHC
    [SDHC_IRQn       ] = isr_sdhc,            /* SDHC interrupt */
#endif
#ifdef ENET
    [ENET_1588_Timer_IRQn] = isr_enet_1588_timer, /* Ethernet MAC IEEE 1588 Timer Interrupt */
    [ENET_Transmit_IRQn] = isr_enet_transmit,   /* Ethernet MAC Transmit Interrupt */
    [ENET_Receive_IRQn] = isr_enet_receive,    /* Ethernet MAC Receive Interrupt */
    [ENET_Error_IRQn ] = isr_enet_error,      /* Ethernet MAC Error and miscellaneous Interrupt */
#endif
#ifdef LPUART0
    [LPUART0_IRQn    ] = isr_lpuart0,         /* LPUART0 status/error interrupt */
#endif
#ifdef LPUART1
    [LPUART1_IRQn    ] = isr_lpuart1,         /* LPUART1 status/error interrupt */
#endif
#ifdef LPUART2
    [LPUART2_IRQn    ] = isr_lpuart2,         /* LPUART2 status/error interrupt */
#endif
#ifdef LPUART3
    [LPUART3_IRQn    ] = isr_lpuart3,         /* LPUART3 status/error interrupt */
#endif
#ifdef LPUART4
    [LPUART4_IRQn    ] = isr_lpuart4,         /* LPUART4 status/error interrupt */
#endif
#ifdef LPUART5
    [LPUART5_IRQn    ] = isr_lpuart5,         /* LPUART5 status/error interrupt */
#endif
#ifdef TSI0
    [TSI0_IRQn       ] = isr_tsi0,            /* TSI0 interrupt */
#endif
#ifdef TPM0
    [TPM0_IRQn       ] = isr_tpm0,            /* TPM1 fault, overflow and channels interrupt */
#endif
#ifdef TPM1
    [TPM1_IRQn       ] = isr_tpm1,            /* TPM1 fault, overflow and channels interrupt */
#endif
#ifdef TPM2
    [TPM2_IRQn       ] = isr_tpm2,            /* TPM2 fault, overflow and channels interrupt */
#endif
#ifdef USBHSDCD
    [USBHSDCD_IRQn   ] = isr_usbhsdcd,        /* USBHSDCD, USBHS Phy Interrupt */
#endif
#ifdef USBHS
    [USBHS_IRQn      ] = isr_usbhs,           /* USB high speed OTG interrupt */
#endif
#ifdef BTLE_RF
    [Radio_0_IRQn    ] = isr_radio_0,         /* Radio INT0 interrupt */
#endif
#ifdef ZLL
    [Radio_1_IRQn    ] = isr_radio_1,         /* Radio INT1 interrupt */
#endif
};

/** @} */
