/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Leon M. George <leon@georgemail.eu>
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

/* CC26x0 specific interrupt vectors */
WEAK_DEFAULT void isr_edge(void);
WEAK_DEFAULT void isr_i2c(void);
WEAK_DEFAULT void isr_rfc_cpe1(void);
WEAK_DEFAULT void isr_aon_rx_tx_cs(void);
WEAK_DEFAULT void isr_aon_rtc(void);
WEAK_DEFAULT void isr_uart(void);
WEAK_DEFAULT void isr_scse0_aon(void);
WEAK_DEFAULT void isr_ssi0(void);
WEAK_DEFAULT void isr_ssi1(void);
WEAK_DEFAULT void isr_rfc_cpe0(void);
WEAK_DEFAULT void isr_rfc_hw(void);
WEAK_DEFAULT void isr_rfc_cmd_ack(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_scse1_aon(void);
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

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_edge,              /* 16 AON edge detect                        */
    isr_i2c,               /* 17 I2C                                    */
    isr_rfc_cpe1,          /* 18 RF Command and Packet Engine 1         */
    isr_aon_rx_tx_cs,      /* 19 AON SpiSplave Rx, Tx and CS            */
    isr_aon_rtc,           /* 20 AON RTC                                */
    isr_uart,              /* 21 UART0 Rx and Tx                        */
    isr_scse0_aon,         /* 22 Sensor Controller software event 0, through AON domain */
    isr_ssi0,              /* 23 SSI0 Rx and Tx                         */
    isr_ssi1,              /* 24 SSI1 Rx and Tx                         */
    isr_rfc_cpe0,          /* 25 RF Command and Packet Engine 0         */
    isr_rfc_hw,            /* 26 RF Core Hardware                       */
    isr_rfc_cmd_ack,       /* 27 RF Core Command Acknowledge            */
    isr_i2s,               /* 28 I2S                                    */
    isr_scse1_aon,         /* 29 Sensor Controller software event 1, through AON domain */
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
};

/** @} */
