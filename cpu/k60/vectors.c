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

/**
 * @brief Interrupt vector definition
 */
ISR_VECTORS const isr_func_t interrupt_vector[] = {
    /* Stack pointer */
    (isr_func_t)(&_estack),     /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    reset_handler_default,  /* entry point of the program */
    nmi_default,            /* non maskable interrupt handler */
    hard_fault_default,     /* hard fault exception */
    mem_manage_default,     /* memory manage exception */
    bus_fault_default,      /* bus fault exception */
    usage_fault_default,    /* usage fault exception */
    dummy_handler,          /* Reserved */
    dummy_handler,          /* Reserved */
    dummy_handler,          /* Reserved */
    dummy_handler,          /* Reserved */
    isr_svc,                /* system call interrupt, in RIOT used for
                             * switching into thread context on boot */
    debug_mon_default,      /* debug monitor exception */
    dummy_handler,          /* Reserved */
    isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                             * context switching is happening here */
    isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* K60 specific peripheral handlers */
    isr_dma0,
    isr_dma1,
    isr_dma2,
    isr_dma3,
    isr_dma4,
    isr_dma5,
    isr_dma6,
    isr_dma7,
    isr_dma8,
    isr_dma9,
    isr_dma10,
    isr_dma11,
    isr_dma12,
    isr_dma13,
    isr_dma14,
    isr_dma15,
    isr_dma_error,
    isr_mcm,
    isr_ftfl,
    isr_ftfl_collision,
    isr_pmc,
    isr_llwu,
    isr_wdog_ewm,
    isr_rng,
    isr_i2c0,
    isr_i2c1,
    isr_spi0,
    isr_spi1,
    isr_spi2,
    isr_can0_mb,
    isr_can0_bus_off,
    isr_can0_error,
    isr_can0_tx_warn,
    isr_can0_rx_warn,
    isr_can0_wake_up,
    isr_i2s0_tx,
    isr_i2s0_rx,
    isr_can1_mb,
    isr_can1_bus_off,
    isr_can1_error,
    isr_can1_tx_warn,
    isr_can1_rx_warn,
    isr_can1_wake_up,
    dummy_handler,
    isr_uart0_lon,
    isr_uart0_rx_tx,
    isr_uart0_error,
    isr_uart1_rx_tx,
    isr_uart1_error,
    isr_uart2_rx_tx,
    isr_uart2_error,
    isr_uart3_rx_tx,
    isr_uart3_error,
    isr_uart4_rx_tx,
    isr_uart4_error,
    dummy_handler,
    dummy_handler,
    isr_adc0,
    isr_adc1,
    isr_cmp0,
    isr_cmp1,
    isr_cmp2,
    isr_ftm0,
    isr_ftm1,
    isr_ftm2,
    isr_cmt,
    isr_rtc,
    isr_rtc_seconds,
    isr_pit0,
    isr_pit1,
    isr_pit2,
    isr_pit3,
    isr_pdb0,
    isr_usb0,
    isr_usbdcd,
    isr_enet_1588_timer,
    isr_enet_tx,
    isr_enet_rx,
    isr_enet_error,
    dummy_handler,
    isr_sdhc,
    isr_dac0,
    dummy_handler,
    isr_tsi,
    isr_mcg,
    isr_lptmr0,
    dummy_handler,
    isr_porta,
    isr_portb,
    isr_portc,
    isr_portd,
    isr_porte,
    dummy_handler,
    dummy_handler,
    isr_swi, /* Vector 110 */
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler,
    dummy_handler /* vector 255 */
};

/** @} */
