/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32zg
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void)
{
    dummy_handler_default();
}

/* Silicon Labs specific interrupt vector */
WEAK_DEFAULT void isr_dma(void);
WEAK_DEFAULT void isr_gpio_even(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_gpio_odd(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_usart1_rx(void);
WEAK_DEFAULT void isr_usart1_tx(void);
WEAK_DEFAULT void isr_leuart0(void);
WEAK_DEFAULT void isr_pcnt0(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_cmu(void);
WEAK_DEFAULT void isr_vcmp(void);
WEAK_DEFAULT void isr_msc(void);
WEAK_DEFAULT void isr_aes(void);

/* interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_dma,                /* DMA */
    [ 1] = isr_gpio_even,          /* GPIO_EVEN */
    [ 2] = isr_timer0,             /* TIMER0 */
    [ 3] = isr_acmp0,              /* ACMP0 */
    [ 4] = isr_adc0,               /* ADC0 */
    [ 5] = isr_i2c0,               /* I2C0 */
    [ 6] = isr_gpio_odd,           /* GPIO_ODD */
    [ 7] = isr_timer1,             /* TIMER1 */
    [ 8] = isr_usart1_rx,          /* USART1_RX */
    [ 9] = isr_usart1_tx,          /* USART1_TX */
    [10] = isr_leuart0,            /* LEUART0 */
    [11] = isr_pcnt0,              /* PCNT0 */
    [12] = isr_rtc,                /* RTC */
    [13] = isr_cmu,                /* CMU */
    [14] = isr_vcmp,               /* VCMP */
    [15] = isr_msc,                /* MSC */
    [16] = isr_aes,                /* AES */
};
