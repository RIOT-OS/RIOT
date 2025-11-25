/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32gg
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
WEAK_DEFAULT void isr_ebi(void);
WEAK_DEFAULT void isr_emu(void);

/* interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_dma,                /* DMA */
    [ 1] = isr_gpio_even,          /* GPIO_EVEN */
    [ 2] = isr_timer0,             /* TIMER0 */
    [ 3] = isr_usart0_rx,          /* USART0_RX */
    [ 4] = isr_usart0_tx,          /* USART0_TX */
    [ 5] = isr_usb,                /* USB */
    [ 6] = isr_acmp0,              /* ACMP0 */
    [ 7] = isr_adc0,               /* ADC0 */
    [ 8] = isr_dac0,               /* DAC0 */
    [ 9] = isr_i2c0,               /* I2C0 */
    [10] = isr_i2c1,               /* I2C1 */
    [11] = isr_gpio_odd,           /* GPIO_ODD */
    [12] = isr_timer1,             /* TIMER1 */
    [13] = isr_timer2,             /* TIMER2 */
    [14] = isr_timer3,             /* TIMER3 */
    [15] = isr_usart1_rx,          /* USART1_RX */
    [16] = isr_usart1_tx,          /* USART1_TX */
    [17] = isr_lesense,            /* LESENSE */
    [18] = isr_usart2_rx,          /* USART2_RX */
    [19] = isr_usart2_tx,          /* USART2_TX */
    [20] = isr_uart0_rx,           /* UART0_RX */
    [21] = isr_uart0_tx,           /* UART0_TX */
    [22] = isr_uart1_rx,           /* UART1_RX */
    [23] = isr_uart1_tx,           /* UART1_TX */
    [24] = isr_leuart0,            /* LEUART0 */
    [25] = isr_leuart1,            /* LEUART1 */
    [26] = isr_letimer0,           /* LETIMER0 */
    [27] = isr_pcnt0,              /* PCNT0 */
    [28] = isr_pcnt1,              /* PCNT1 */
    [29] = isr_pcnt2,              /* PCNT2 */
    [30] = isr_rtc,                /* RTC */
    [31] = isr_burtc,              /* BURTC */
    [32] = isr_cmu,                /* CMU */
    [33] = isr_vcmp,               /* VCMP */
    [34] = isr_lcd,                /* LCD */
    [35] = isr_msc,                /* MSC */
    [36] = isr_aes,                /* AES */
    [37] = isr_ebi,                /* EBI */
    [38] = isr_emu,                /* EMU */
};
