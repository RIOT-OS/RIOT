/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_ezr32wg
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    __asm__("b     dummy_handler_default");
}

/* Silicon Labs specific interrupt vector */
WEAK_DEFAULT void isr_dma(void);
WEAK_DEFAULT void isr_gpio_even(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_usart0_rx(void);
WEAK_DEFAULT void isr_usart0_tx(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_dac0(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_gpio_odd(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_timer3(void);
WEAK_DEFAULT void isr_usart1_rx(void);
WEAK_DEFAULT void isr_usart1_tx(void);
WEAK_DEFAULT void isr_lesense(void);
WEAK_DEFAULT void isr_usart2_rx(void);
WEAK_DEFAULT void isr_usart2_tx(void);
WEAK_DEFAULT void isr_uart0_rx(void);
WEAK_DEFAULT void isr_uart0_tx(void);
WEAK_DEFAULT void isr_uart1_rx(void);
WEAK_DEFAULT void isr_uart1_tx(void);
WEAK_DEFAULT void isr_leuart0(void);
WEAK_DEFAULT void isr_leuart1(void);
WEAK_DEFAULT void isr_letimer0(void);
WEAK_DEFAULT void isr_pcnt0(void);
WEAK_DEFAULT void isr_pcnt1(void);
WEAK_DEFAULT void isr_pcnt2(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_burtc(void);
WEAK_DEFAULT void isr_cmu(void);
WEAK_DEFAULT void isr_vcmp(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_msc(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_emu(void);
WEAK_DEFAULT void isr_fpueh(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_dma,                /* 0 - DMA       */
    isr_gpio_even,          /* 1 - GPIO_EVEN       */
    isr_timer0,             /* 2 - TIMER0       */
    isr_usart0_rx,          /* 3 - USART0_RX       */
    isr_usart0_tx,          /* 4 - USART0_TX       */
    isr_usb,                /* 5 - USB       */
    isr_acmp0,              /* 6 - ACMP0       */
    isr_adc0,               /* 7 - ADC0       */
    isr_dac0,               /* 8 - DAC0       */
    isr_i2c0,               /* 9 - I2C0       */
    isr_i2c1,               /* 10 - I2C1       */
    isr_gpio_odd,           /* 11 - GPIO_ODD       */
    isr_timer1,             /* 12 - TIMER1       */
    isr_timer2,             /* 13 - TIMER2       */
    isr_timer3,             /* 14 - TIMER3       */
    isr_usart1_rx,          /* 15 - USART1_RX       */
    isr_usart1_tx,          /* 16 - USART1_TX       */
    isr_lesense,            /* 17 - LESENSE       */
    isr_usart2_rx,          /* 18 - USART2_RX       */
    isr_usart2_tx,          /* 19 - USART2_TX       */
    isr_uart0_rx,           /* 20 - UART0_RX       */
    isr_uart0_tx,           /* 21 - UART0_TX       */
    isr_uart1_rx,           /* 22 - UART1_RX       */
    isr_uart1_tx,           /* 23 - UART1_TX       */
    isr_leuart0,            /* 24 - LEUART0       */
    isr_leuart1,            /* 25 - LEUART1       */
    isr_letimer0,           /* 26 - LETIMER0       */
    isr_pcnt0,              /* 27 - PCNT0       */
    isr_pcnt1,              /* 28 - PCNT1       */
    isr_pcnt2,              /* 29 - PCNT2       */
    isr_rtc,                /* 30 - RTC       */
    isr_burtc,              /* 31 - BURTC       */
    isr_cmu,                /* 32 - CMU       */
    isr_vcmp,               /* 33 - VCMP       */
    isr_lcd,                /* 34 - LCD       */
    isr_msc,                /* 35 - MSC       */
    isr_aes,                /* 36 - AES       */
    (0UL),                  /* 37 - Reserved */
    isr_emu,                /* 38 - EMU       */
    isr_fpueh,              /* 39 - FPUEH       */
};
