/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kw0x
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include "cpu-conf.h"
#include "fault_handlers.h"

/**
 * memory markers as defined in the linker script
 */
extern uint32_t _estack;

extern void reset_handler(void);

void dummy_handler(void)
{
    isr_unhandled();
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* MKW01Z128 specific interrupt vector */
void isr_dma0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dma3(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_ftfa(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_lvd_lvw(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_llwu(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_uart0(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_uart1(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_uart2(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_adc0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_cmp0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tpm0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tpm1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tpm2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_seconds(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_pit(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_i2s0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dac0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tsi0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_mcg(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_lptmr0(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_porta(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_portc_portd(void)          __attribute__ ((weak, alias("dummy_handler")));

/* interrupt vector table */
__attribute__((section(".vector_table")))
const void *interrupt_vector[64] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* MKW01Z128 specific peripheral handlers */
    (void*) isr_dma0,		    /* DMA channel 0 transfer complete */
    (void*) isr_dma1,		    /* DMA channel 1 transfer complete */
    (void*) isr_dma2,		    /* DMA channel 2 transfer complete */
    (void*) isr_dma3,		    /* DMA channel 3 transfer complete */
    (void*) dummy_handler,          /* Reserved interrupt 4 */
    (void*) isr_ftfa,		    /* FTFA command complete */
    (void*) isr_lvd_lvw,            /* Low-Voltage detect and warning */
    (void*) isr_llwu,		    /* Low leakage wakeup */
    (void*) isr_i2c0,		    /* Inter-integrated circuit 0 */
    (void*) isr_i2c1,		    /* Inter-integrated circuit 1 */
    (void*) isr_spi0,		    /* Serial peripheral Interface 0 */
    (void*) isr_spi1,		    /* Serial peripheral Interface 1 */
    (void*) isr_uart0,	            /* UART0 receive/transmit/error interrupt */
    (void*) isr_uart1,	            /* UART1 receive/transmit/error interrupt */
    (void*) isr_uart2,	            /* UART2 receive/transmit/error interrupt */
    (void*) isr_adc0,		    /* Analog-to-digital converter 0 */
    (void*) isr_cmp0,		    /* Comparator 0 */
    (void*) isr_tpm0, 		    /* Timer module 0 status interrupt */
    (void*) isr_tpm1, 		    /* Timer module 1 status interrupt */
    (void*) isr_tpm2, 		    /* Timer module 2 status interrupt */
    (void*) isr_rtc,		    /* Real time clock */
    (void*) isr_rtc_seconds,	    /* Real time clock seconds */
    (void*) isr_pit,		    /* Periodic interrupt timer channel 0 */
    (void*) isr_i2s0,	            /* Integrated interchip sound 0 transmit interrupt */
    (void*) dummy_handler,          /* Reserved interrupt */
    (void*) isr_dac0,		    /* Digital-to-analog converter 0 */
    (void*) isr_tsi0,		    /* TSI0 interrupt */
    (void*) isr_mcg,		    /* Multipurpose clock generator */
    (void*) isr_lptmr0,		    /* Low power timer interrupt */
    (void*) dummy_handler,          /* Reserved interrupt 29 */
    (void*) isr_porta,		    /* Port A pin detect interrupt */
    (void*) isr_portc_portd,	    /* Port C+D pin detect interrupt */
};
