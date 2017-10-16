/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Default interrupt service routine definitions for Kinetis CPUs
 *
 * This file defines weak defaults for all available ISRs in Kinetis CPUs, these
 * weak defaults will act as fallback definitions if no driver defines a
 * specific handler for any interrupt.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "vectors_cortexm.h"
#include "vectors_kinetis.h"
#include "wdog.h"

void pre_startup(void)
{
    /* disable the WDOG */
    wdog_disable();
#ifdef SIM_SCGC7_FLEXBUS_SHIFT
    /*
     * Workaround for hardware errata e4218: "SIM/FLEXBUS: SIM_SCGC7[FLEXBUS]
     * bit should be cleared when the FlexBus is not being used."
     */
    BITBAND_REG32(SIM->SCGC7, SIM_SCGC7_FLEXBUS_SHIFT) = 0;
#endif
}

void dummy_handler(void)
{
    dummy_handler_default();
}

/* Kinetis specific interrupt service routines */
WEAK_DEFAULT void isr_ADC0(void);
WEAK_DEFAULT void isr_ADC1(void);
WEAK_DEFAULT void isr_CAN0_Bus_Off(void);
WEAK_DEFAULT void isr_CAN0_Error(void);
WEAK_DEFAULT void isr_CAN0_ORed_Message_buffer(void);
WEAK_DEFAULT void isr_CAN0_Rx_Warning(void);
WEAK_DEFAULT void isr_CAN0_Tx_Warning(void);
WEAK_DEFAULT void isr_CAN0_Wake_Up(void);
WEAK_DEFAULT void isr_CAN1_Bus_Off(void);
WEAK_DEFAULT void isr_CAN1_Error(void);
WEAK_DEFAULT void isr_CAN1_ORed_Message_buffer(void);
WEAK_DEFAULT void isr_CAN1_Rx_Warning(void);
WEAK_DEFAULT void isr_CAN1_Tx_Warning(void);
WEAK_DEFAULT void isr_CAN1_Wake_Up(void);
WEAK_DEFAULT void isr_CMP0(void);
WEAK_DEFAULT void isr_CMP1(void);
WEAK_DEFAULT void isr_CMP2(void);
WEAK_DEFAULT void isr_CMT(void);
WEAK_DEFAULT void isr_DAC0(void);
WEAK_DEFAULT void isr_DAC1(void);
WEAK_DEFAULT void isr_DMA0(void);
WEAK_DEFAULT void isr_DMA1(void);
WEAK_DEFAULT void isr_DMA10(void);
WEAK_DEFAULT void isr_DMA11(void);
WEAK_DEFAULT void isr_DMA12(void);
WEAK_DEFAULT void isr_DMA13(void);
WEAK_DEFAULT void isr_DMA14(void);
WEAK_DEFAULT void isr_DMA15(void);
WEAK_DEFAULT void isr_DMA2(void);
WEAK_DEFAULT void isr_DMA3(void);
WEAK_DEFAULT void isr_DMA4(void);
WEAK_DEFAULT void isr_DMA5(void);
WEAK_DEFAULT void isr_DMA6(void);
WEAK_DEFAULT void isr_DMA7(void);
WEAK_DEFAULT void isr_DMA8(void);
WEAK_DEFAULT void isr_DMA9(void);
WEAK_DEFAULT void isr_DMA_Error(void);
WEAK_DEFAULT void isr_ENET_1588_Timer(void);
WEAK_DEFAULT void isr_ENET_Error(void);
WEAK_DEFAULT void isr_ENET_Receive(void);
WEAK_DEFAULT void isr_ENET_Transmit(void);
WEAK_DEFAULT void isr_FTFA(void);
WEAK_DEFAULT void isr_FTFA_Collision(void);
WEAK_DEFAULT void isr_FTFE(void);
WEAK_DEFAULT void isr_FTFE_Collision(void);
WEAK_DEFAULT void isr_FTFL(void);
WEAK_DEFAULT void isr_FTFL_Collision(void);
WEAK_DEFAULT void isr_FTM0(void);
WEAK_DEFAULT void isr_FTM1(void);
WEAK_DEFAULT void isr_FTM2(void);
WEAK_DEFAULT void isr_FTM3(void);
WEAK_DEFAULT void isr_I2C0(void);
WEAK_DEFAULT void isr_I2C1(void);
WEAK_DEFAULT void isr_I2C2(void);
WEAK_DEFAULT void isr_I2S0_Rx(void);
WEAK_DEFAULT void isr_I2S0_Tx(void);
WEAK_DEFAULT void isr_LLWU(void);
WEAK_DEFAULT void isr_LPTMR0(void);
WEAK_DEFAULT void isr_LPUART0(void);
WEAK_DEFAULT void isr_LVD_LVW(void);
WEAK_DEFAULT void isr_MCG(void);
WEAK_DEFAULT void isr_MCM(void);
WEAK_DEFAULT void isr_PDB0(void);
WEAK_DEFAULT void isr_PIT0(void);
WEAK_DEFAULT void isr_PIT1(void);
WEAK_DEFAULT void isr_PIT2(void);
WEAK_DEFAULT void isr_PIT3(void);
WEAK_DEFAULT void isr_PORTA(void);
WEAK_DEFAULT void isr_PORTB(void);
WEAK_DEFAULT void isr_PORTC(void);
WEAK_DEFAULT void isr_PORTD(void);
WEAK_DEFAULT void isr_PORTE(void);
WEAK_DEFAULT void isr_RNG(void);
WEAK_DEFAULT void isr_RTC(void);
WEAK_DEFAULT void isr_RTC_Seconds(void);
WEAK_DEFAULT void isr_SDHC(void);
WEAK_DEFAULT void isr_SPI0(void);
WEAK_DEFAULT void isr_SPI1(void);
WEAK_DEFAULT void isr_SPI2(void);
WEAK_DEFAULT void isr_SWI(void);
WEAK_DEFAULT void isr_TSI0(void);
WEAK_DEFAULT void isr_UART0_ERR(void);
WEAK_DEFAULT void isr_UART0_LON(void);
WEAK_DEFAULT void isr_UART0_RX_TX(void);
WEAK_DEFAULT void isr_UART1_ERR(void);
WEAK_DEFAULT void isr_UART1_RX_TX(void);
WEAK_DEFAULT void isr_UART2_ERR(void);
WEAK_DEFAULT void isr_UART2_RX_TX(void);
WEAK_DEFAULT void isr_UART3_ERR(void);
WEAK_DEFAULT void isr_UART3_RX_TX(void);
WEAK_DEFAULT void isr_UART4_ERR(void);
WEAK_DEFAULT void isr_UART4_RX_TX(void);
WEAK_DEFAULT void isr_UART5_ERR(void);
WEAK_DEFAULT void isr_UART5_RX_TX(void);
WEAK_DEFAULT void isr_USB0(void);
WEAK_DEFAULT void isr_USBDCD(void);
WEAK_DEFAULT void isr_WDOG_EWM(void);
