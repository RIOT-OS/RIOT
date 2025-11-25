/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32pg1b
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
WEAK_DEFAULT void isr_emu(void);
WEAK_DEFAULT void isr_wdog0(void);
WEAK_DEFAULT void isr_ldma(void);
WEAK_DEFAULT void isr_gpio_even(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_usart0_rx(void);
WEAK_DEFAULT void isr_usart0_tx(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_idac0(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_gpio_odd(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_usart1_rx(void);
WEAK_DEFAULT void isr_usart1_tx(void);
WEAK_DEFAULT void isr_leuart0(void);
WEAK_DEFAULT void isr_pcnt0(void);
WEAK_DEFAULT void isr_cmu(void);
WEAK_DEFAULT void isr_msc(void);
WEAK_DEFAULT void isr_crypto(void);
WEAK_DEFAULT void isr_letimer0(void);
WEAK_DEFAULT void isr_rtcc(void);
WEAK_DEFAULT void isr_cryotimer(void);
WEAK_DEFAULT void isr_fpueh(void);

/* interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_emu,                /* EMU */
    [ 2] = isr_wdog0,              /* WDOG0 */
    [ 8] = isr_ldma,               /* LDMA */
    [ 9] = isr_gpio_even,          /* GPIO_EVEN */
    [10] = isr_timer0,             /* TIMER0 */
    [11] = isr_usart0_rx,          /* USART0_RX */
    [12] = isr_usart0_tx,          /* USART0_TX */
    [13] = isr_acmp0,              /* ACMP0 */
    [14] = isr_adc0,               /* ADC0 */
    [15] = isr_idac0,              /* IDAC0 */
    [16] = isr_i2c0,               /* I2C0 */
    [17] = isr_gpio_odd,           /* GPIO_ODD */
    [18] = isr_timer1,             /* TIMER1 */
    [19] = isr_usart1_rx,          /* USART1_RX */
    [20] = isr_usart1_tx,          /* USART1_TX */
    [21] = isr_leuart0,            /* LEUART0 */
    [22] = isr_pcnt0,              /* PCNT0 */
    [23] = isr_cmu,                /* CMU */
    [24] = isr_msc,                /* MSC */
    [25] = isr_crypto,             /* CRYPTO */
    [26] = isr_letimer0,           /* LETIMER0 */
    [29] = isr_rtcc,               /* RTCC */
    [31] = isr_cryotimer,          /* CRYOTIMER */
    [33] = isr_fpueh,              /* FPUEH */
};
