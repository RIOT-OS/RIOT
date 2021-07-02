/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_rp2040
 * @{
 *
 * @file vectors.c
 * @brief Startup code and interrupt vector definition
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include "cpu_conf.h"
#include "vectors_cortexm.h"

void dummy_handler(void) {
    dummy_handler_default();
}

// RP2040 specific interrupt vectors.
WEAK_DEFAULT void isr_timer_0(void);
WEAK_DEFAULT void isr_timer_1(void);
WEAK_DEFAULT void isr_timer_2(void);
WEAK_DEFAULT void isr_timer_3(void);
WEAK_DEFAULT void isr_pwm_wrap(void);
WEAK_DEFAULT void isr_usbctrl(void);
WEAK_DEFAULT void isr_xip(void);
WEAK_DEFAULT void isr_pio0_0(void);
WEAK_DEFAULT void isr_pio0_1(void);
WEAK_DEFAULT void isr_pio1_0(void);
WEAK_DEFAULT void isr_pio1_1(void);
WEAK_DEFAULT void isr_dma_0(void);
WEAK_DEFAULT void isr_dma_1(void);
WEAK_DEFAULT void isr_io_bank0(void);
WEAK_DEFAULT void isr_io_qspi(void);
WEAK_DEFAULT void isr_sio_proc0(void);
WEAK_DEFAULT void isr_sio_proc1(void);
WEAK_DEFAULT void isr_clocks(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_adc0_fifo(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_rtc(void);

// CPU specific interrupt vector table.
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_timer_0, /* 0 TIMER_0_IRQn */
    isr_timer_1, /* 1 TIMER_1_IRQn */
    isr_timer_2, /* 2 TIMER_2_IRQn */
    isr_timer_3, /* 3 TIMER_3_IRQn */
    isr_pwm_wrap, /* 4 PWM_WRAP_IRQn */
    isr_usbctrl, /* 5 USBCTRL_IRQn */
    isr_xip, /* 6 XIP_IRQn */
    isr_pio0_0, /* 7 PIO0_0_IRQn */
    isr_pio0_1, /* 8 PIO0_1_IRQn Meter */
    isr_pio1_0, /* 9 PIO1_0_IRQn */
    isr_pio1_1, /* 10 PIO1_1_IRQn */
    isr_dma_0, /* 11 DMA_0_IRQn*/
    isr_dma_1, /*  12 DMA_1_IRQn */
    isr_io_bank0, /* 13 IO_BANK0_IRQn */
    isr_io_qspi, /* 14 IO_QSPI_IRQn*/
    isr_sio_proc0, /* 15 SIO_PROC0_IRQn */
    isr_sio_proc1, /* 16 SIO_PROC1_IRQn */
    isr_clocks, /* 17 CLOCKS_IRQn */
    isr_spi0, /* 18 SPI0_IRQn */
    isr_spi1, /* 19 SPI1_IRQn */
    isr_uart0, /* 20 UART0_IRQn */
    isr_uart1, /* 21 UART1_IRQn */
    isr_adc0_fifo, /* 22 ADC0_FIFO_IRQn*/
    isr_i2c0, /* 23 I2C0_IRQn */
    isr_i2c1, /* 24 I2C1_IRQn */
    isr_rtc, /* 25 RTC_IRQn */
};
