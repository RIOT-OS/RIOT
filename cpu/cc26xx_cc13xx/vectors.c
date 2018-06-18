/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Leon M. George <leon@georgemail.eu>
 * @author      Anton Gerasimov <tossel@gmail.com>
 */

#include <stdint.h>
#include "cpu.h"
#include "board.h"
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* CC26xx_CC13xx specific interrupt vectors */
WEAK_DEFAULT void isr_edge(void);
WEAK_DEFAULT void isr_i2c(void);
WEAK_DEFAULT void isr_rfc_cpe1(void);
WEAK_DEFAULT void isr_pka(void);
WEAK_DEFAULT void isr_aon_rtc(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_aux0_aon(void);
WEAK_DEFAULT void isr_ssi0(void);
WEAK_DEFAULT void isr_ssi1(void);
WEAK_DEFAULT void isr_rfc_cpe0(void);
WEAK_DEFAULT void isr_rfc_hw(void);
WEAK_DEFAULT void isr_rfc_cmd_ack(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_aux1_aon(void);
WEAK_DEFAULT void isr_watchdog(void);
WEAK_DEFAULT void isr_timer0_chan0(void);
WEAK_DEFAULT void isr_timer0_chan1(void);
WEAK_DEFAULT void isr_timer1_chan0(void);
WEAK_DEFAULT void isr_timer1_chan1(void);
WEAK_DEFAULT void isr_timer2_chan0(void);
WEAK_DEFAULT void isr_timer2_chan1(void);
WEAK_DEFAULT void isr_timer3_chan0(void);
WEAK_DEFAULT void isr_timer3_chan1(void);
WEAK_DEFAULT void isr_crypto_res(void);
WEAK_DEFAULT void isr_dma(void);
WEAK_DEFAULT void isr_dmaerr(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_se0(void);
WEAK_DEFAULT void isr_aux_ce(void);
WEAK_DEFAULT void isr_aon_prog(void);
WEAK_DEFAULT void isr_dyn_prog(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_trng(void);
#ifdef CPU_VARIANT_X2
WEAK_DEFAULT void isr_osc(void);
WEAK_DEFAULT void isr_aux_timer2(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_batmon(void);
#endif // CPU_VARIANT_X2

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[] = {
    isr_edge,              /* 16 AON edge detect                        */
    isr_i2c,               /* 17 I2C                                    */
    isr_rfc_cpe1,          /* 18 RF Command and Packet Engine 1         */
    isr_pka,               /* 19 PKA interrupt                          */
    isr_aon_rtc,           /* 20 AON RTC                                */
    isr_uart0,             /* 21 UART0 Rx and Tx                        */
    isr_aux0_aon,          /* 22 AUX event 0, through AON domain */
    isr_ssi0,              /* 23 SSI0 Rx and Tx                         */
    isr_ssi1,              /* 24 SSI1 Rx and Tx                         */
    isr_rfc_cpe0,          /* 25 RF Command and Packet Engine 0         */
    isr_rfc_hw,            /* 26 RF Core Hardware                       */
    isr_rfc_cmd_ack,       /* 27 RF Core Command Acknowledge            */
    isr_i2s,               /* 28 I2S                                    */
    isr_aux1_aon,          /* 29 AUX event 1, through AON domain */
    isr_watchdog,          /* 30 Watchdog timer                         */
    isr_timer0_chan0,      /* 31 Timer 0 subtimer A                     */
    isr_timer0_chan1,      /* 32 Timer 0 subtimer B                     */
    isr_timer1_chan0,      /* 33 Timer 1 subtimer A                     */
    isr_timer1_chan1,      /* 34 Timer 1 subtimer B                     */
    isr_timer2_chan0,      /* 35 Timer 2 subtimer A                     */
    isr_timer2_chan1,      /* 36 Timer 2 subtimer B                     */
    isr_timer3_chan0,      /* 37 Timer 3 subtimer A                     */
    isr_timer3_chan1,      /* 38 Timer 3 subtimer B                     */
    isr_crypto_res,        /* 39 Crypto Core Result available           */
    isr_dma,               /* 40 uDMA Software                          */
    isr_dmaerr,            /* 41 uDMA Error                             */
    isr_flash,             /* 42 Flash controller                       */
    isr_se0,               /* 43 Software Event 0                       */
    isr_aux_ce,            /* 44 AUX combined event, directly to MCU domain */
    isr_aon_prog,          /* 45 AON programmable 0                     */
    isr_dyn_prog,          /* 46 Dynamic Programmable interrupt (default source: PRCM) */
    isr_comp,              /* 47 AUX Comparator A                       */
    isr_adc,               /* 48 AUX ADC IRQ                            */
    isr_trng,              /* 49 TRNG event                             */
#ifdef CPU_VARIANT_X2
    isr_osc,               /* 50 Combined event from oscillator control */
    isr_aux_timer2,        /* 51 AUX Timer 2 event 0                    */
    isr_uart1,             /* 52 UART 1 RX and TX                       */
    isr_batmon,            /* 53 BATMON interrupt                       */
#endif // CPU_VARIANT_X2
};

/** @} */
