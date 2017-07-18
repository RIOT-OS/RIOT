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

/* Cortex-M specific interrupt vectors */
void isr_svc(void);          /**< Supervisor call */
void isr_pendsv(void);       /**< Pending SVC */
void isr_systick(void);      /**< System tick interrupt */
/* Kinetis specific interrupt vectors */
void isr_adc0(void);         /**< ADC0 interrupt handler */
void isr_adc1(void);         /**< ADC1 interrupt handler */
void isr_can0_bus_off(void); /**< CAN0 bus off interrupt handler */
void isr_can0_error(void);   /**< CAN0 error interrupt handler */
void isr_can0_mb(void);      /**< CAN0 message buffer interrupt handler */
void isr_can0_rx_warn(void); /**< CAN0 receive warning interrupt handler */
void isr_can0_tx_warn(void); /**< CAN0 transmit warning interrupt handler */
void isr_can0_wake_up(void); /**< CAN0 wake up interrupt handler */
void isr_can1_bus_off(void); /**< CAN1 bus off interrupt handler */
void isr_can1_error(void);   /**< CAN1 error interrupt handler */
void isr_can1_mb(void);      /**< CAN1 message buffer interrupt handler */
void isr_can1_rx_warn(void); /**< CAN1 receive warning interrupt handler */
void isr_can1_tx_warn(void); /**< CAN1 transmit warning interrupt handler */
void isr_can1_wake_up(void); /**< CAN1 wake up interrupt handler */
void isr_cmp0(void);         /**< CMP0 interrupt handler */
void isr_cmp1(void);         /**< CMP1 interrupt handler */
void isr_cmp2(void);         /**< CMP2 interrupt handler */
void isr_cmt(void);          /**< CMT interrupt handler */
void isr_dac0(void);         /**< DAC0 interrupt handler */
void isr_dac1(void);         /**< DAC1 interrupt handler */
void isr_dma0(void);         /**< DMA channel 0 interrupt handler */
void isr_dma1(void);         /**< DMA channel 1 interrupt handler */
void isr_dma2(void);         /**< DMA channel 2 interrupt handler */
void isr_dma3(void);         /**< DMA channel 3 interrupt handler */
void isr_dma4(void);         /**< DMA channel 4 interrupt handler */
void isr_dma5(void);         /**< DMA channel 5 interrupt handler */
void isr_dma6(void);         /**< DMA channel 6 interrupt handler */
void isr_dma7(void);         /**< DMA channel 7 interrupt handler */
void isr_dma8(void);         /**< DMA channel 8 interrupt handler */
void isr_dma9(void);         /**< DMA channel 9 interrupt handler */
void isr_dma10(void);        /**< DMA channel 10 interrupt handler */
void isr_dma11(void);        /**< DMA channel 11 interrupt handler */
void isr_dma12(void);        /**< DMA channel 12 interrupt handler */
void isr_dma13(void);        /**< DMA channel 13 interrupt handler */
void isr_dma14(void);        /**< DMA channel 14 interrupt handler */
void isr_dma15(void);        /**< DMA channel 15 interrupt handler */
void isr_dma_error(void);    /**< DMA error interrupt handler */
void isr_enet_1588_timer(void); /**< ENET 1588 timer interrupt handler */
void isr_enet_error(void);   /**< ENET error interrupt handler */
void isr_enet_rx(void);      /**< ENET receive interrupt handler */
void isr_enet_tx(void);      /**< ENET transmit interrupt handler */
void isr_ftfl(void);         /**< FTFL command complete interrupt handler */
void isr_ftfl_collision(void); /**< FTFL collision interrupt handler */
void isr_ftm0(void);         /**< FTM0 interrupt handler */
void isr_ftm1(void);         /**< FTM1 interrupt handler */
void isr_ftm2(void);         /**< FTM2 interrupt handler */
void isr_ftm3(void);         /**< FTM3 interrupt handler */
void isr_i2c0(void);         /**< I2C0 interrupt handler */
void isr_i2c1(void);         /**< I2C1 interrupt handler */
void isr_i2c2(void);         /**< I2C2 interrupt handler */
void isr_i2s0_rx(void);      /**< I2S0 receive interrupt handler */
void isr_i2s0_tx(void);      /**< I2S0 transmit interrupt handler */
void isr_llwu(void);         /**< LLWU interrupt handler */
void isr_lpuart0(void);      /**< LPUART0 interrupt handler */
void isr_lptmr0(void);       /**< LPTMR0 interrupt handler */
void isr_mcg(void);          /**< MCG interrupt handler */
void isr_mcm(void);          /**< MCM interrupt handler */
void isr_pdb0(void);         /**< PDB0 interrupt handler */
void isr_pit0(void);         /**< PIT channel 0 interrupt handler */
void isr_pit1(void);         /**< PIT channel 1 interrupt handler */
void isr_pit2(void);         /**< PIT channel 2 interrupt handler */
void isr_pit3(void);         /**< PIT channel 3 interrupt handler */
void isr_pmc(void);          /**< PMC interrupt handler */
void isr_porta(void);        /**< PORTA interrupt handler */
void isr_portb(void);        /**< PORTB interrupt handler */
void isr_portc(void);        /**< PORTC interrupt handler */
void isr_portd(void);        /**< PORTD interrupt handler */
void isr_porte(void);        /**< PORTE interrupt handler */
void isr_rng(void);          /**< Random number generator interrupt handler */
void isr_rtc(void);          /**< RTC alarm interrupt handler */
void isr_rtc_seconds(void);  /**< RTC seconds interrupt handler */
void isr_sdhc(void);         /**< SDHC interrupt handler */
void isr_spi0(void);         /**< SPI0 interrupt handler */
void isr_spi1(void);         /**< SPI1 interrupt handler */
void isr_spi2(void);         /**< SPI2 interrupt handler */
void isr_swi(void);          /**< Software interrupt handler */
void isr_tsi(void);          /**< TSI interrupt handler */
void isr_uart0_lon(void);    /**< UART0 LON sources interrupt handler */
void isr_uart0_error(void);  /**< UART0 error interrupt handler */
void isr_uart0_rx_tx(void);  /**< UART0 receive/transmit interrupt handler */
void isr_uart1_error(void);  /**< UART1 error interrupt handler */
void isr_uart1_rx_tx(void);  /**< UART1 receive/transmit interrupt handler */
void isr_uart2_error(void);  /**< UART2 error interrupt handler */
void isr_uart2_rx_tx(void);  /**< UART2 receive/transmit interrupt handler */
void isr_uart3_error(void);  /**< UART3 error interrupt handler */
void isr_uart3_rx_tx(void);  /**< UART3 receive/transmit interrupt handler */
void isr_uart4_error(void);  /**< UART4 error interrupt handler */
void isr_uart4_rx_tx(void);  /**< UART4 receive/transmit interrupt handler */
void isr_uart5_error(void);  /**< UART5 error interrupt handler */
void isr_uart5_rx_tx(void);  /**< UART5 receive/transmit interrupt handler */
void isr_usb0(void);         /**< USB OTG interrupt handler */
void isr_usbdcd(void);       /**< USB charger detection interrupt handler */
void isr_wdog_ewm(void);     /**< WDOG and EWM interrupt handler */

/**
 * @brief Interrupt vector pointer type
 */
typedef void (*isr_func_t)(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VECTORS_KINETIS_H */
