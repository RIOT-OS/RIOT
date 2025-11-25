/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32gg11b
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
WEAK_DEFAULT void isr_usart0_rx(void);
WEAK_DEFAULT void isr_usart0_tx(void);
WEAK_DEFAULT void isr_acmp0(void);
WEAK_DEFAULT void isr_adc0(void);
WEAK_DEFAULT void isr_idac0(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_gpio_odd(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_timer3(void);
WEAK_DEFAULT void isr_usart1_rx(void);
WEAK_DEFAULT void isr_usart1_tx(void);
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
WEAK_DEFAULT void isr_rtcc(void);
WEAK_DEFAULT void isr_cmu(void);
WEAK_DEFAULT void isr_msc(void);
WEAK_DEFAULT void isr_crypto0(void);
WEAK_DEFAULT void isr_cryotimer(void);
WEAK_DEFAULT void isr_fpueh(void);
WEAK_DEFAULT void isr_usart3_rx(void);
WEAK_DEFAULT void isr_usart3_tx(void);
WEAK_DEFAULT void isr_usart4_rx(void);
WEAK_DEFAULT void isr_usart4_tx(void);
WEAK_DEFAULT void isr_wtimer0(void);
WEAK_DEFAULT void isr_wtimer1(void);
WEAK_DEFAULT void isr_wtimer2(void);
WEAK_DEFAULT void isr_wtimer3(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_vdac0(void);
WEAK_DEFAULT void isr_timer4(void);
WEAK_DEFAULT void isr_timer5(void);
WEAK_DEFAULT void isr_timer6(void);
WEAK_DEFAULT void isr_usart5_rx(void);
WEAK_DEFAULT void isr_usart5_tx(void);
WEAK_DEFAULT void isr_csen(void);
WEAK_DEFAULT void isr_lesense(void);
WEAK_DEFAULT void isr_ebi(void);
WEAK_DEFAULT void isr_acmp2(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_sdio(void);
WEAK_DEFAULT void isr_eth(void);
WEAK_DEFAULT void isr_can0(void);
WEAK_DEFAULT void isr_can1(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_wdog1(void);
WEAK_DEFAULT void isr_letimer1(void);
WEAK_DEFAULT void isr_trng0(void);
WEAK_DEFAULT void isr_qspi0(void);

/* interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_emu,                /* EMU */
    [ 1] = isr_wdog0,              /* WDOG0 */
    [ 2] = isr_ldma,               /* LDMA */
    [ 3] = isr_gpio_even,          /* GPIO_EVEN */
    [ 4] = isr_smu,                /* SMU */
    [ 5] = isr_timer0,             /* TIMER0 */
    [ 6] = isr_usart0_rx,          /* USART0_RX */
    [ 7] = isr_usart0_tx,          /* USART0_TX */
    [ 8] = isr_acmp0,              /* ACMP0 */
    [ 9] = isr_adc0,               /* ADC0 */
    [10] = isr_idac0,              /* IDAC0 */
    [11] = isr_i2c0,               /* I2C0 */
    [12] = isr_i2c1,               /* I2C1 */
    [13] = isr_gpio_odd,           /* GPIO_ODD */
    [14] = isr_timer1,             /* TIMER1 */
    [15] = isr_timer2,             /* TIMER2 */
    [16] = isr_timer3,             /* TIMER3 */
    [17] = isr_usart1_rx,          /* USART1_RX */
    [18] = isr_usart1_tx,          /* USART1_TX */
    [19] = isr_usart2_rx,          /* USART2_RX */
    [20] = isr_usart2_tx,          /* USART2_TX */
    [21] = isr_uart0_rx,           /* UART0_RX */
    [22] = isr_uart0_tx,           /* UART0_TX */
    [23] = isr_uart1_rx,           /* UART1_RX */
    [24] = isr_uart1_tx,           /* UART1_TX */
    [25] = isr_leuart0,            /* LEUART0 */
    [26] = isr_leuart1,            /* LEUART1 */
    [27] = isr_letimer0,           /* LETIMER0 */
    [28] = isr_pcnt0,              /* PCNT0 */
    [29] = isr_pcnt1,              /* PCNT1 */
    [30] = isr_pcnt2,              /* PCNT2 */
    [31] = isr_rtcc,               /* RTCC */
    [32] = isr_cmu,                /* CMU */
    [33] = isr_msc,                /* MSC */
    [34] = isr_crypto0,            /* CRYPTO0 */
    [35] = isr_cryotimer,          /* CRYOTIMER */
    [36] = isr_fpueh,              /* FPUEH */
    [37] = isr_usart3_rx,          /* USART3_RX */
    [38] = isr_usart3_tx,          /* USART3_TX */
    [39] = isr_usart4_rx,          /* USART4_RX */
    [40] = isr_usart4_tx,          /* USART4_TX */
    [41] = isr_wtimer0,            /* WTIMER0 */
    [42] = isr_wtimer1,            /* WTIMER1 */
    [43] = isr_wtimer2,            /* WTIMER2 */
    [44] = isr_wtimer3,            /* WTIMER3 */
    [45] = isr_i2c2,               /* I2C2 */
    [46] = isr_vdac0,              /* VDAC0 */
    [47] = isr_timer4,             /* TIMER4 */
    [48] = isr_timer5,             /* TIMER5 */
    [49] = isr_timer6,             /* TIMER6 */
    [50] = isr_usart5_rx,          /* USART5_RX */
    [51] = isr_usart5_tx,          /* USART5_TX */
    [52] = isr_csen,               /* CSEN */
    [53] = isr_lesense,            /* LESENSE */
    [54] = isr_ebi,                /* EBI */
    [55] = isr_acmp2,              /* ACMP2 */
    [56] = isr_adc1,               /* ADC1 */
    [57] = isr_lcd,                /* LCD */
    [58] = isr_sdio,               /* SDIO */
    [59] = isr_eth,                /* ETH */
    [60] = isr_can0,               /* CAN0 */
    [61] = isr_can1,               /* CAN1 */
    [62] = isr_usb,                /* USB */
    [63] = isr_rtc,                /* RTC */
    [64] = isr_wdog1,              /* WDOG1 */
    [65] = isr_letimer1,           /* LETIMER1 */
    [66] = isr_trng0,              /* TRNG0 */
    [67] = isr_qspi0,              /* QSPI0 */
};
