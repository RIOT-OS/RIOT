/*
 * SPDX-FileCopyrightText: 2025 Tom Hert git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350_riscv
 * @{
 * @file
 * @brief           CPU configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_DEFAULT_IRQ_PRIO 1u
#define CPU_IRQ_NUMOF 52u

/**
 * @brief Interrupt numbers for RP2350 RISC-V
 */
typedef enum {
    TIMER0_IRQ_0_IRQn = 0,      /**< Timer 0 interrupt 0 */
    TIMER0_IRQ_1_IRQn = 1,      /**< Timer 0 interrupt 1 */
    TIMER0_IRQ_2_IRQn = 2,      /**< Timer 0 interrupt 2 */
    TIMER0_IRQ_3_IRQn = 3,      /**< Timer 0 interrupt 3 */
    TIMER1_IRQ_0_IRQn = 4,      /**< Timer 1 interrupt 0 */
    TIMER1_IRQ_1_IRQn = 5,      /**< Timer 1 interrupt 1 */
    TIMER1_IRQ_2_IRQn = 6,      /**< Timer 1 interrupt 2 */
    TIMER1_IRQ_3_IRQn = 7,      /**< Timer 1 interrupt 3 */
    PWM_IRQ_WRAP_0_IRQn = 8,    /**< PWM wrap interrupt 0 */
    PWM_IRQ_WRAP_1_IRQn = 9,    /**< PWM wrap interrupt 1 */
    DMA_IRQ_0_IRQn = 10,        /**< DMA interrupt 0 */
    DMA_IRQ_1_IRQn = 11,        /**< DMA interrupt 1 */
    DMA_IRQ_2_IRQn = 12,        /**< DMA interrupt 2 */
    DMA_IRQ_3_IRQn = 13,        /**< DMA interrupt 3 */
    USBCTRL_IRQ_IRQn = 14,      /**< USB controller interrupt */
    PIO0_IRQ_0_IRQn = 15,       /**< PIO 0 interrupt 0 */
    PIO0_IRQ_1_IRQn = 16,       /**< PIO 0 interrupt 1 */
    PIO1_IRQ_0_IRQn = 17,       /**< PIO 1 interrupt 0 */
    PIO1_IRQ_1_IRQn = 18,       /**< PIO 1 interrupt 1 */
    PIO2_IRQ_0_IRQn = 19,       /**< PIO 2 interrupt 0 */
    PIO2_IRQ_1_IRQn = 20,       /**< PIO 2 interrupt 1 */
    IO_IRQ_BANK0_IRQn = 21,     /**< IO bank 0 interrupt */
    IO_IRQ_BANK0_NS_IRQn = 22,  /**< IO bank 0 non-secure interrupt */
    IO_IRQ_QSPI_IRQn = 23,      /**< IO QSPI interrupt */
    IO_IRQ_QSPI_NS_IRQn = 24,   /**< IO QSPI non-secure interrupt */
    SIO_IRQ_FIFO_IRQn = 25,     /**< SIO FIFO interrupt */
    SIO_IRQ_BELL_IRQn = 26,     /**< SIO bell interrupt */
    SIO_IRQ_FIFO_NS_IRQn = 27,  /**< SIO FIFO non-secure interrupt */
    SIO_IRQ_BELL_NS_IRQn = 28,  /**< SIO bell non-secure interrupt */
    SIO_IRQ_MTIMECMP_IRQn = 29, /**< SIO MTIMECMP interrupt */
    CLOCKS_IRQ_IRQn = 30,       /**< Clocks interrupt */
    SPI0_IRQ_IRQn = 31,         /**< SPI 0 interrupt */
    SPI1_IRQ_IRQn = 32,         /**< SPI 1 interrupt */
    UART0_IRQ_IRQn = 33,        /**< UART 0 interrupt */
    UART1_IRQ_IRQn = 34,        /**< UART 1 interrupt */
    ADC_IRQ_FIFO_IRQn = 35,     /**< ADC FIFO interrupt */
    I2C0_IRQ_IRQn = 36,         /**< I2C 0 interrupt */
    I2C1_IRQ_IRQn = 37,         /**< I2C 1 interrupt */
    OTP_IRQ_IRQn = 38,          /**< OTP interrupt */
    TRNG_IRQ_IRQn = 39,         /**< TRNG interrupt */
    PROC0_IRQ_CTI_IRQn = 40,    /**< Processor 0 CTI interrupt */
    PROC1_IRQ_CTI_IRQn = 41,    /**< Processor 1 CTI interrupt */
    PLL_SYS_IRQ_IRQn = 42,      /**< PLL SYS interrupt */
    PLL_USB_IRQ_IRQn = 43,      /**< PLL USB interrupt */
    POWMAN_IRQ_POW_IRQn = 44,   /**< Power manager power interrupt */
    POWMAN_IRQ_TIMER_IRQn = 45, /**< Power manager timer interrupt */
    SPAREIRQ_IRQ_0_IRQn = 46,   /**< Spare interrupt 0 */
    SPAREIRQ_IRQ_1_IRQn = 47,   /**< Spare interrupt 1 */
    SPAREIRQ_IRQ_2_IRQn = 48,   /**< Spare interrupt 2 */
    SPAREIRQ_IRQ_3_IRQn = 49,   /**< Spare interrupt 3 */
    SPAREIRQ_IRQ_4_IRQn = 50,   /**< Spare interrupt 4 */
    SPAREIRQ_IRQ_5_IRQn = 51    /**< Spare interrupt 5 */
} IRQn_Type;

#ifdef __cplusplus
}
#endif

/** @} */
