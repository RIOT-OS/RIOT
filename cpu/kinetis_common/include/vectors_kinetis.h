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
 * @brief       Interrupt service routine declarations for Kinetis MCUs
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#ifndef VECTORS_KINETIS_H
#define VECTORS_KINETIS_H

#include <stdint.h>
#include "vectors_cortexm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief memory markers as defined in the linker script
 */
extern uint32_t _estack;

/**
 * @brief Dummy handler
 */
void dummy_handler(void);

/* Kinetis specific interrupt vectors */
void isr_ADC0(void);         /**< ADC0 interrupt handler */
void isr_ADC1(void);         /**< ADC1 interrupt handler */
void isr_CAN0_Bus_Off(void); /**< CAN0 bus off interrupt handler */
void isr_CAN0_Error(void);   /**< CAN0 error interrupt handler */
void isr_CAN0_ORed_Message_buffer(void); /**< CAN0 OR'd message buffers interrupt handler */
void isr_CAN0_Rx_Warning(void); /**< CAN0 Rx warning interrupt handler */
void isr_CAN0_Tx_Warning(void); /**< CAN0 Tx warning interrupt handler */
void isr_CAN0_Wake_Up(void); /**< CAN0 wake up interrupt handler */
void isr_CAN1_Bus_Off(void); /**< CAN1 bus off interrupt handler */
void isr_CAN1_Error(void);   /**< CAN1 error interrupt handler */
void isr_CAN1_ORed_Message_buffer(void); /**< CAN1 OR'd message buffers interrupt handler */
void isr_CAN1_Rx_Warning(void); /**< CAN1 Rx warning interrupt handler */
void isr_CAN1_Tx_Warning(void); /**< CAN1 Tx warning interrupt handler */
void isr_CAN1_Wake_Up(void); /**< CAN1 wake up interrupt handler */
void isr_CMP0(void);         /**< CMP0 interrupt handler */
void isr_CMP1(void);         /**< CMP1 interrupt handler */
void isr_CMP2(void);         /**< CMP2 interrupt handler */
void isr_CMT(void);          /**< CMT interrupt handler */
void isr_DAC0(void);         /**< DAC0 interrupt handler */
void isr_DAC1(void);         /**< DAC1 interrupt handler */
void isr_DMA0(void);         /**< DMA channel 0 transfer complete interrupt handler */
void isr_DMA1(void);         /**< DMA channel 1 transfer complete interrupt handler */
void isr_DMA2(void);         /**< DMA channel 2 transfer complete interrupt handler */
void isr_DMA3(void);         /**< DMA channel 3 transfer complete interrupt handler */
void isr_DMA4(void);         /**< DMA channel 4 transfer complete interrupt handler */
void isr_DMA5(void);         /**< DMA channel 5 transfer complete interrupt handler */
void isr_DMA6(void);         /**< DMA channel 6 transfer complete interrupt handler */
void isr_DMA7(void);         /**< DMA channel 7 transfer complete interrupt handler */
void isr_DMA8(void);         /**< DMA channel 8 transfer complete interrupt handler */
void isr_DMA9(void);         /**< DMA channel 9 transfer complete interrupt handler */
void isr_DMA10(void);        /**< DMA channel 10 transfer complete interrupt handler */
void isr_DMA11(void);        /**< DMA channel 11 transfer complete interrupt handler */
void isr_DMA12(void);        /**< DMA channel 12 transfer complete interrupt handler */
void isr_DMA13(void);        /**< DMA channel 13 transfer complete interrupt handler */
void isr_DMA14(void);        /**< DMA channel 14 transfer complete interrupt handler */
void isr_DMA15(void);        /**< DMA channel 15 transfer complete interrupt handler */
void isr_DMA_Error(void);    /**< DMA Error interrupt handler */
void isr_ENET_1588_Timer(void); /**< Ethernet MAC IEEE 1588 timer interrupt handler */
void isr_ENET_Error(void);   /**< Ethernet MAC error and miscelaneous interrupt handler */
void isr_ENET_Receive(void); /**< Ethernet MAC receive interrupt handler */
void isr_ENET_Transmit(void); /**< Ethernet MAC transmit interrupt handler */
void isr_FTFA(void);         /**< FTFA Command complete interrupt handler */
void isr_FTFA_Collision(void); /**< FTFA read collision interrupt handler */
void isr_FTFE(void);         /**< FTFE Command complete interrupt handler */
void isr_FTFE_Collision(void); /**< FTFA read collision interrupt handler */
void isr_FTFL(void);         /**< FTFL command complete interrupt handler */
void isr_FTFL_Collision(void); /**< FTFL read collision interrupt handler */
void isr_FTM0(void);         /**< FTM0 fault, overflow and channels interrupt handler */
void isr_FTM1(void);         /**< FTM1 fault, overflow and channels interrupt handler */
void isr_FTM2(void);         /**< FTM2 fault, overflow and channels interrupt handler */
void isr_FTM3(void);         /**< FTM3 fault, overflow and channels interrupt handler */
void isr_I2C0(void);         /**< I2C0 interrupt handler */
void isr_I2C1(void);         /**< I2C1 interrupt handler */
void isr_I2C2(void);         /**< I2C2 interrupt handler */
void isr_I2S0_Rx(void);      /**< I2S0 receive interrupt handler */
void isr_I2S0_Tx(void);      /**< I2S0 transmit interrupt handler */
void isr_LLWU(void);         /**< Low leakage wakeup interrupt handler */
void isr_LPTMR0(void);       /**< Low power timer interrupt handler */
void isr_LPUART0(void);      /**< LPUART0 status/error interrupt handler */
void isr_LVD_LVW(void);      /**< PMC controller low-voltage detect, low-voltage warning interrupt handler */
void isr_MCG(void);          /**< MCG interrupt handler */
void isr_MCM(void);          /**< MCM normal interrupt handler */
void isr_PDB0(void);         /**< PDB0 interrupt handler */
void isr_PIT0(void);         /**< PIT timer channel 0 interrupt handler */
void isr_PIT1(void);         /**< PIT timer channel 1 interrupt handler */
void isr_PIT2(void);         /**< PIT timer channel 2 interrupt handler */
void isr_PIT3(void);         /**< PIT timer channel 3 interrupt handler */
void isr_PORTA(void);        /**< Port A pin detect interrupt handler */
void isr_PORTB(void);        /**< Port B pin detect interrupt handler */
void isr_PORTC(void);        /**< Port C pin detect interrupt handler */
void isr_PORTD(void);        /**< Port D pin detect interrupt handler */
void isr_PORTE(void);        /**< Port E pin detect interrupt handler */
void isr_RNG(void);          /**< RNG interrupt handler */
void isr_RTC(void);          /**< RTC interrupt handler */
void isr_RTC_Seconds(void);  /**< RTC seconds interrupt handler */
void isr_SDHC(void);         /**< SDHC interrupt handler */
void isr_SPI0(void);         /**< SPI0 interrupt handler */
void isr_SPI1(void);         /**< SPI1 interrupt handler */
void isr_SPI2(void);         /**< SPI2 interrupt handler */
void isr_SWI(void);          /**< Software interrupt handler */
void isr_TSI0(void);         /**< TSI0 interrupt handler */
void isr_UART0_ERR(void);    /**< UART0 error interrupt handler */
void isr_UART0_LON(void);    /**< UART0 LON interrupt handler */
void isr_UART0_RX_TX(void);  /**< UART0 receive/transmit interrupt handler */
void isr_UART1_ERR(void);    /**< UART1 error interrupt handler */
void isr_UART1_RX_TX(void);  /**< UART1 receive/transmit interrupt handler */
void isr_UART2_ERR(void);    /**< UART2 error interrupt handler */
void isr_UART2_RX_TX(void);  /**< UART2 receive/transmit interrupt handler */
void isr_UART3_ERR(void);    /**< UART3 error interrupt handler */
void isr_UART3_RX_TX(void);  /**< UART3 receive/transmit interrupt handler */
void isr_UART4_ERR(void);    /**< UART4 error interrupt handler */
void isr_UART4_RX_TX(void);  /**< UART4 receive/transmit interrupt handler */
void isr_UART5_ERR(void);    /**< UART5 error interrupt handler */
void isr_UART5_RX_TX(void);  /**< UART5 receive/transmit interrupt handler */
void isr_USB0(void);         /**< USB0 interrupt handler */
void isr_USBDCD(void);       /**< USB charger detect interrupt handler */
void isr_WDOG_EWM(void);     /**< WDOG interrupt handler */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VECTORS_KINETIS_H */
