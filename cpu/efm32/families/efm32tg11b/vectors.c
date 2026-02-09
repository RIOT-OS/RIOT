/*
 * SPDX-FileCopyrightText: 2015-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32pg12b
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
WEAK_DEFAULT void isr_smu(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_usart0(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_gpio_odd(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_leuart0(void);
WEAK_DEFAULT void isr_letimer0(void);
WEAK_DEFAULT void isr_pcnt0(void);
WEAK_DEFAULT void isr_rtcc(void);
WEAK_DEFAULT void isr_cmu(void);
WEAK_DEFAULT void isr_msc(void);
WEAK_DEFAULT void isr_crypto0(void);
WEAK_DEFAULT void isr_cryotimer(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_wtimer0(void);
WEAK_DEFAULT void isr_wtimer1(void);
WEAK_DEFAULT void isr_vdac0(void);
WEAK_DEFAULT void isr_csen(void);
WEAK_DEFAULT void isr_lesense(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_can0(void);

/* interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_emu,                /* EMU */
    [ 1] = isr_wdog0,              /* WDOG0 */
    [ 2] = isr_ldma,               /* LDMA */
    [ 3] = isr_gpio_even,          /* GPIO_EVEN */
    [ 4] = isr_smu,                /* SMU */
    [ 5] = isr_timer0,             /* TIMER0 */
    [ 6] = isr_usart0,             /* USART0 */
    [ 7] = isr_acmp0,              /* ACMP0 */
    [ 8] = isr_adc0,               /* ADC0 */
    [ 9] = isr_i2c0,               /* I2C0 */
    [10] = isr_i2c1,               /* I2C1 */
    [11] = isr_gpio_odd,           /* GPIO_ODD */
    [12] = isr_timer1,             /* TIMER1 */
    [13] = isr_usart1,             /* USART1 */
    [14] = isr_usart2,             /* USART2 */
    [15] = isr_uart0,              /* UART0 */
    [16] = isr_leuart0,            /* LEUART0 */
    [17] = isr_letimer0,           /* LETIMER0 */
    [18] = isr_pcnt0,              /* PCNT0 */
    [19] = isr_rtcc,               /* RTCC */
    [20] = isr_cmu,                /* CMU */
    [21] = isr_msc,                /* MSC */
    [22] = isr_crypto0,            /* CRYPTO0 */
    [23] = isr_cryotimer,          /* CRYOTIMER */
    [24] = isr_usart3,             /* USART3 */
    [25] = isr_wtimer0,            /* WTIMER0 */
    [26] = isr_wtimer1,            /* WTIMER1 */
    [27] = isr_vdac0,              /* VDAC0 */
    [28] = isr_csen,               /* CSEN */
    [29] = isr_lesense,            /* LESENSE */
    [30] = isr_lcd,                /* LCD */
    [31] = isr_can0,               /* CAN0 */
};
