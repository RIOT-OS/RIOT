/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     cpu_kinetis
 * @{
 *
 * @file
 * @brief       Interrupt service routine declarations for Kinetis MCUs
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dummy handler
 */
void dummy_handler(void);

/* Kinetis specific interrupt vectors */
void isr_adc0(void);         /**< ADC0 interrupt handler */
void isr_adc1(void);         /**< ADC1 interrupt handler */
void isr_adc2(void);         /**< ADC2 interrupt handler */
void isr_can0_bus_off(void); /**< CAN0 bus off interrupt handler */
void isr_can0_error(void);   /**< CAN0 error interrupt handler */
void isr_can0_ored_message_buffer(void); /**< CAN0 OR'd message buffers interrupt handler */
void isr_can0_rx_warning(void); /**< CAN0 Rx warning interrupt handler */
void isr_can0_tx_warning(void); /**< CAN0 Tx warning interrupt handler */
void isr_can0_wake_up(void); /**< CAN0 wake up interrupt handler */
void isr_can1_bus_off(void); /**< CAN1 bus off interrupt handler */
void isr_can1_error(void);   /**< CAN1 error interrupt handler */
void isr_can1_ored_message_buffer(void); /**< CAN1 OR'd message buffers interrupt handler */
void isr_can1_rx_warning(void); /**< CAN1 Rx warning interrupt handler */
void isr_can1_tx_warning(void); /**< CAN1 Tx warning interrupt handler */
void isr_can1_wake_up(void); /**< CAN1 wake up interrupt handler */
void isr_mscan_rx(void);     /**< MSCAN RX interrupt handler */
void isr_mscan_tx(void);     /**< MSCAN TX/Err/Wake-up interrupt handler */
void isr_cmp0(void);         /**< CMP0 interrupt handler */
void isr_cmp1(void);         /**< CMP1 interrupt handler */
void isr_cmp2(void);         /**< CMP2 interrupt handler */
void isr_cmp3(void);         /**< CMP3 interrupt handler */
void isr_cmt(void);          /**< CMT interrupt handler */
void isr_dac0(void);         /**< DAC0 interrupt handler */
void isr_dac1(void);         /**< DAC1 interrupt handler */
void isr_dma0(void);         /**< DMA channel 0 transfer complete interrupt handler */
void isr_dma1(void);         /**< DMA channel 1 transfer complete interrupt handler */
void isr_dma2(void);         /**< DMA channel 2 transfer complete interrupt handler */
void isr_dma3(void);         /**< DMA channel 3 transfer complete interrupt handler */
void isr_dma4(void);         /**< DMA channel 4 transfer complete interrupt handler */
void isr_dma5(void);         /**< DMA channel 5 transfer complete interrupt handler */
void isr_dma6(void);         /**< DMA channel 6 transfer complete interrupt handler */
void isr_dma7(void);         /**< DMA channel 7 transfer complete interrupt handler */
void isr_dma8(void);         /**< DMA channel 8 transfer complete interrupt handler */
void isr_dma9(void);         /**< DMA channel 9 transfer complete interrupt handler */
void isr_dma10(void);        /**< DMA channel 10 transfer complete interrupt handler */
void isr_dma11(void);        /**< DMA channel 11 transfer complete interrupt handler */
void isr_dma12(void);        /**< DMA channel 12 transfer complete interrupt handler */
void isr_dma13(void);        /**< DMA channel 13 transfer complete interrupt handler */
void isr_dma14(void);        /**< DMA channel 14 transfer complete interrupt handler */
void isr_dma15(void);        /**< DMA channel 15 transfer complete interrupt handler */
void isr_dma0_dma16(void);   /**< DMA channel 0, 16 transfer complete interrupt handler */
void isr_dma1_dma17(void);   /**< DMA channel 1, 17 transfer complete interrupt handler */
void isr_dma2_dma18(void);   /**< DMA channel 2, 18 transfer complete interrupt handler */
void isr_dma3_dma19(void);   /**< DMA channel 3, 19 transfer complete interrupt handler */
void isr_dma4_dma20(void);   /**< DMA channel 4, 20 transfer complete interrupt handler */
void isr_dma5_dma21(void);   /**< DMA channel 5, 21 transfer complete interrupt handler */
void isr_dma6_dma22(void);   /**< DMA channel 6, 22 transfer complete interrupt handler */
void isr_dma7_dma23(void);   /**< DMA channel 7, 23 transfer complete interrupt handler */
void isr_dma8_dma24(void);   /**< DMA channel 8, 24 transfer complete interrupt handler */
void isr_dma9_dma25(void);   /**< DMA channel 9, 25 transfer complete interrupt handler */
void isr_dma10_dma26(void);  /**< DMA channel 10, 26 transfer complete interrupt handler */
void isr_dma11_dma27(void);  /**< DMA channel 11, 27 transfer complete interrupt handler */
void isr_dma12_dma28(void);  /**< DMA channel 12, 28 transfer complete interrupt handler */
void isr_dma13_dma29(void);  /**< DMA channel 13, 29 transfer complete interrupt handler */
void isr_dma14_dma30(void);  /**< DMA channel 14, 30 transfer complete interrupt handler */
void isr_dma15_dma31(void);  /**< DMA channel 15, 31 transfer complete interrupt handler */
void isr_dma_error(void);    /**< DMA Error interrupt handler */
void isr_enet_1588_timer(void); /**< Ethernet MAC IEEE 1588 timer interrupt handler */
void isr_enet_error(void);   /**< Ethernet MAC error and miscellaneous interrupt handler */
void isr_enet_receive(void); /**< Ethernet MAC receive interrupt handler */
void isr_enet_transmit(void); /**< Ethernet MAC transmit interrupt handler */
void isr_ftfa(void);         /**< FTFA Command complete interrupt handler */
void isr_ftfa_collision(void); /**< FTFA read collision interrupt handler */
void isr_ftfe(void);         /**< FTFE Command complete interrupt handler */
void isr_ftfe_collision(void); /**< FTFA read collision interrupt handler */
void isr_ftfl(void);         /**< FTFL command complete interrupt handler */
void isr_ftfl_collision(void); /**< FTFL read collision interrupt handler */
void isr_ftm0(void);         /**< FTM0 fault, overflow and channels interrupt handler */
void isr_ftm1(void);         /**< FTM1 fault, overflow and channels interrupt handler */
void isr_ftm2(void);         /**< FTM2 fault, overflow and channels interrupt handler */
void isr_ftm3(void);         /**< FTM3 fault, overflow and channels interrupt handler */
void isr_i2c0(void);         /**< I2C0 interrupt handler */
void isr_i2c1(void);         /**< I2C1 interrupt handler */
void isr_i2c2(void);         /**< I2C2 interrupt handler */
void isr_i2c3(void);         /**< I2C3 interrupt handler */
void isr_i2s0(void);         /**< I2S0 interrupt handler */
void isr_i2s0_rx(void);      /**< I2S0 receive interrupt handler */
void isr_i2s0_tx(void);      /**< I2S0 transmit interrupt handler */
void isr_llwu(void);         /**< Low leakage wakeup interrupt handler */
void isr_lptmr0(void);       /**< Low power timer interrupt handler */
void isr_lpuart0(void);      /**< LPUART0 status/error interrupt handler */
void isr_lpuart1(void);      /**< LPUART1 status/error interrupt handler */
void isr_lpuart2(void);      /**< LPUART2 status/error interrupt handler */
void isr_lpuart3(void);      /**< LPUART3 status/error interrupt handler */
void isr_lpuart4(void);      /**< LPUART4 status/error interrupt handler */
void isr_lpuart5(void);      /**< LPUART5 status/error interrupt handler */
void isr_lvd_lvw(void);      /**< PMC controller low-voltage detect, low-voltage warning interrupt handler */
void isr_mcg(void);          /**< MCG interrupt handler */
void isr_mcm(void);          /**< MCM normal interrupt handler */
void isr_pdb0(void);         /**< PDB0 interrupt handler */
void isr_pit(void);          /**< PIT any channel interrupt handler */
void isr_pit0(void);         /**< PIT timer channel 0 interrupt handler */
void isr_pit1(void);         /**< PIT timer channel 1 interrupt handler */
void isr_pit2(void);         /**< PIT timer channel 2 interrupt handler */
void isr_pit3(void);         /**< PIT timer channel 3 interrupt handler */
void isr_porta(void);        /**< Port A pin detect interrupt handler */
void isr_portb(void);        /**< Port B pin detect interrupt handler */
void isr_portc(void);        /**< Port C pin detect interrupt handler */
void isr_portd(void);        /**< Port D pin detect interrupt handler */
void isr_porte(void);        /**< Port E pin detect interrupt handler */
void isr_portb_portc(void);  /**< Port B, C combined pin detect interrupt handler */
void isr_portc_portd(void);  /**< Port C, D combined pin detect interrupt handler */
void isr_radio_0(void);      /**< Radio transceiver INT0 interrupt handler */
void isr_radio_1(void);      /**< Radio transceiver INT1 interrupt handler */
void isr_rng(void);          /**< RNG interrupt handler */
void isr_rtc(void);          /**< RTC interrupt handler */
void isr_rtc_seconds(void);  /**< RTC seconds interrupt handler */
void isr_sdhc(void);         /**< SDHC interrupt handler */
void isr_spi0(void);         /**< SPI0 interrupt handler */
void isr_spi1(void);         /**< SPI1 interrupt handler */
void isr_spi2(void);         /**< SPI2 interrupt handler */
void isr_swi(void);          /**< Software interrupt handler */
void isr_tpm0(void);         /**< TPM0 interrupt handler */
void isr_tpm1(void);         /**< TPM1 interrupt handler */
void isr_tpm2(void);         /**< TPM2 interrupt handler */
void isr_trng0(void);        /**< TRNG0 interrupt handler */
void isr_tsi0(void);         /**< TSI0 interrupt handler */
void isr_uart0(void);        /**< UART0 unified interrupt handler */
void isr_uart0_err(void);    /**< UART0 error interrupt handler */
void isr_uart0_lon(void);    /**< UART0 LON interrupt handler */
void isr_uart0_rx_tx(void);  /**< UART0 receive/transmit interrupt handler */
void isr_uart1(void);        /**< UART1 unified interrupt handler */
void isr_uart1_err(void);    /**< UART1 error interrupt handler */
void isr_uart1_rx_tx(void);  /**< UART1 receive/transmit interrupt handler */
void isr_uart2(void);        /**< UART2 unified interrupt handler */
void isr_uart2_err(void);    /**< UART2 error interrupt handler */
void isr_uart2_flexio(void); /**< UART2 or FlexIO interrupt handler */
void isr_uart2_rx_tx(void);  /**< UART2 receive/transmit interrupt handler */
void isr_uart3(void);        /**< UART3 unified interrupt handler */
void isr_uart3_err(void);    /**< UART3 error interrupt handler */
void isr_uart3_rx_tx(void);  /**< UART3 receive/transmit interrupt handler */
void isr_uart4(void);        /**< UART4 unified interrupt handler */
void isr_uart4_err(void);    /**< UART4 error interrupt handler */
void isr_uart4_rx_tx(void);  /**< UART4 receive/transmit interrupt handler */
void isr_uart5(void);        /**< UART5 unified interrupt handler */
void isr_uart5_err(void);    /**< UART5 error interrupt handler */
void isr_uart5_rx_tx(void);  /**< UART5 receive/transmit interrupt handler */
void isr_usb0(void);         /**< USB0 interrupt handler */
void isr_usbdcd(void);       /**< USB charger detect interrupt handler */
void isr_wdog_ewm(void);     /**< WDOG interrupt handler */

#ifdef __cplusplus
} /* extern "C" */
#endif
