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

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
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

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void *) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M3 handlers */
    (void *) reset_handler_default,  /* entry point of the program */
    (void *) nmi_default,            /* non maskable interrupt handler */
    (void *) hard_fault_default,     /* hard fault exception */
    (void *) mem_manage_default,     /* memory manage exception */
    (void *) bus_fault_default,      /* bus fault exception */
    (void *) usage_fault_default,    /* usage fault exception */
    NULL,                           /* reserved */
    NULL,                           /* reserved */
    NULL,                           /* reserved */
    NULL,                           /* reserved */
    (void *) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void *) debug_mon_default,      /* debug monitor exception */
    NULL,                           /* reserved */
    (void *) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void *) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* CC26x0 specific peripheral handlers */
    (void *) isr_edge,              /* 16 AON edge detect                        */
    (void *) isr_i2c,               /* 17 I2C                                    */
    (void *) isr_rfc_cpe1,          /* 18 RF Command and Packet Engine 1         */
    (void *) isr_aon_rx_tx_cs,      /* 19 AON SpiSplave Rx, Tx and CS            */
    (void *) isr_aon_rtc,           /* 20 AON RTC                                */
    (void *) isr_uart,              /* 21 UART0 Rx and Tx                        */
    (void *) isr_scse0_aon,         /* 22 Sensor Controller software event 0, through AON domain */
    (void *) isr_ssi0,              /* 23 SSI0 Rx and Tx                         */
    (void *) isr_ssi1,              /* 24 SSI1 Rx and Tx                         */
    (void *) isr_rfc_cpe0,          /* 25 RF Command and Packet Engine 0         */
    (void *) isr_rfc_hw,            /* 26 RF Core Hardware                       */
    (void *) isr_rfc_cmd_ack,       /* 27 RF Core Command Acknowledge            */
    (void *) isr_i2s,               /* 28 I2S                                    */
    (void *) isr_scse1_aon,         /* 29 Sensor Controller software event 1, through AON domain */
    (void *) isr_watchdog,          /* 30 Watchdog timer                         */
    (void *) isr_timer0_chan0,      /* 31 Timer 0 subtimer A                     */
    (void *) isr_timer0_chan1,      /* 32 Timer 0 subtimer B                     */
    (void *) isr_timer1_chan0,      /* 33 Timer 1 subtimer A                     */
    (void *) isr_timer1_chan1,      /* 34 Timer 1 subtimer B                     */
    (void *) isr_timer2_chan0,      /* 35 Timer 2 subtimer A                     */
    (void *) isr_timer2_chan1,      /* 36 Timer 2 subtimer B                     */
    (void *) isr_timer3_chan0,      /* 37 Timer 3 subtimer A                     */
    (void *) isr_timer3_chan1,      /* 38 Timer 3 subtimer B                     */
    (void *) isr_crypto_res,        /* 39 Crypto Core Result available           */
    (void *) isr_dma,               /* 40 uDMA Software                          */
    (void *) isr_dmaerr,            /* 41 uDMA Error                             */
    (void *) isr_flash,             /* 42 Flash controller                       */
    (void *) isr_se0,               /* 43 Software Event 0                       */
    (void *) isr_aux_ce,            /* 44 AUX combined event, directly to MCU domain */
    (void *) isr_aon_prog,          /* 45 AON programmable 0                     */
    (void *) isr_dyn_prog,          /* 46 Dynamic Programmable interrupt (default source: PRCM) */
    (void *) isr_comp,              /* 47 AUX Comparator A                       */
    (void *) isr_adc,               /* 48 AUX ADC IRQ                            */
    (void *) isr_trng,              /* 49 TRNG event                             */
};

/** @} */
