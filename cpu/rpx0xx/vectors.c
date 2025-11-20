/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       rpx0xx interrupt vector definitions
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <stdint.h>

#include "vectors_cortexm.h"

void dummy_handler(void) {
    dummy_handler_default();
}

WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_timer3(void);
WEAK_DEFAULT void isr_pwm_wrap(void);
WEAK_DEFAULT void isr_usbctrl(void);
WEAK_DEFAULT void isr_xip(void);
WEAK_DEFAULT void isr_pio00(void);
WEAK_DEFAULT void isr_pio01(void);
WEAK_DEFAULT void isr_pio10(void);
WEAK_DEFAULT void isr_pio11(void);
WEAK_DEFAULT void isr_dma0(void);
WEAK_DEFAULT void isr_dma1(void);
WEAK_DEFAULT void isr_io_bank0(void);
WEAK_DEFAULT void isr_io_qspi(void);
WEAK_DEFAULT void isr_sio_proc0(void);
WEAK_DEFAULT void isr_sio_proc1(void);
WEAK_DEFAULT void isr_clocks(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_adc_fifo(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_rtc0(void);
WEAK_DEFAULT void isr_swi0(void);
WEAK_DEFAULT void isr_swi1(void);
WEAK_DEFAULT void isr_swi2(void);
WEAK_DEFAULT void isr_swi3(void);
WEAK_DEFAULT void isr_swi4(void);
WEAK_DEFAULT void isr_swi5(void);

ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_timer0,
    isr_timer1,
    isr_timer2,
    isr_timer3,
    isr_pwm_wrap,
    isr_usbctrl,
    isr_xip,
    isr_pio00,
    isr_pio01,
    isr_pio10,
    isr_pio11,
    isr_dma0,
    isr_dma1,
    isr_io_bank0,
    isr_io_qspi,
    isr_sio_proc0,
    isr_sio_proc1,
    isr_clocks,
    isr_spi0,
    isr_spi1,
    isr_uart0,
    isr_uart1,
    isr_adc_fifo,
    isr_i2c0,
    isr_i2c1,
    isr_rtc0,
    isr_swi0,
    isr_swi1,
    isr_swi2,
    isr_swi3,
    isr_swi4,
    isr_swi5,
};
