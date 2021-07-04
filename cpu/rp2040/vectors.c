/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
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

/*
 * CPU specific interrupt vector table.
 *
 * This IRQ vector table is placed right
 * next to the ARM system IRQ vector table.
 */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [0] = isr_timer_0,
    [1] = isr_timer_1,
    [2] = isr_timer_2,
    [3] = isr_timer_3,
    [4] = isr_pwm_wrap,
    [5] = isr_usbctrl,
    [6] = isr_xip,
    [7] = isr_pio0_0,
    [8] = isr_pio0_1,
    [9] = isr_pio1_0,
    [10] = isr_pio1_1,
    [11] = isr_dma_0,
    [12] = isr_dma_1,
    [13] = isr_io_bank0,
    [14] = isr_io_qspi,
    [15] = isr_sio_proc0,
    [16] = isr_sio_proc1,
    [17] = isr_clocks,
    [18] = isr_spi0,
    [19] = isr_spi1,
    [20] = isr_uart0,
    [21] = isr_uart1,
    [22] = isr_adc0_fifo,
    [23] = isr_i2c0,
    [24] = isr_i2c1,
    [25] = isr_rtc,
};
