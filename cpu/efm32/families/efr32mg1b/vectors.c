/*
 * SPDX-FileCopyrightText: 2015-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efr32mg1b
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
WEAK_DEFAULT void isr_frc_pri(void);
WEAK_DEFAULT void isr_wdog0(void);
WEAK_DEFAULT void isr_frc(void);
WEAK_DEFAULT void isr_modem(void);
WEAK_DEFAULT void isr_rac_seq(void);
WEAK_DEFAULT void isr_rac_rsm(void);
WEAK_DEFAULT void isr_bufc(void);
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
WEAK_DEFAULT void isr_agc(void);
WEAK_DEFAULT void isr_protimer(void);
WEAK_DEFAULT void isr_rtcc(void);
WEAK_DEFAULT void isr_synth(void);
WEAK_DEFAULT void isr_cryotimer(void);
WEAK_DEFAULT void isr_rfsense(void);
WEAK_DEFAULT void isr_fpueh(void);

/* interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_emu,                /* EMU */
    [ 1] = isr_frc_pri,            /* FRC_PRI */
    [ 2] = isr_wdog0,              /* WDOG0 */
    [ 3] = isr_frc,                /* FRC */
    [ 4] = isr_modem,              /* MODEM */
    [ 5] = isr_rac_seq,            /* RAC_SEQ */
    [ 6] = isr_rac_rsm,            /* RAC_RSM */
    [ 7] = isr_bufc,               /* BUFC */
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
    [27] = isr_agc,                /* AGC */
    [28] = isr_protimer,           /* PROTIMER */
    [29] = isr_rtcc,               /* RTCC */
    [30] = isr_synth,              /* SYNTH */
    [31] = isr_cryotimer,          /* CRYOTIMER */
    [32] = isr_rfsense,            /* RFSENSE */
    [33] = isr_fpueh,              /* FPUEH */
};
