/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3s
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stdint.h>

#include "panic.h"

/**
 * Memory positions corresponding to linker script
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/**
 * @brief Functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This function implements the reset handler
 */
void reset_handler(void)
{
    uint32_t *pSrc, *pDest;

    /* Relocate data section into RAM */
    pSrc = &_etext;
    pDest = &_srelocate;

    if (pSrc != pDest) {
        for (; pDest < &_erelocate;) {
            *pDest++ = *pSrc++;
        }
    }

    /* Clear the bss section */
    for (pDest = &_szero; pDest < &_ezero;) {
        *pDest++ = 0;
    }

    /* Initialize the C library */
    __libc_init_array();

    /* initialize board (calls cpu_init)*/
    board_init();

    /* jump to RIOT kernel */
    kernel_init();
}

void dummy_handler(void)
{
    core_panic(PANIC_DUMMY_HANDLER, "DUMMY HANDLER");
}

void isr_nmi(void)
{
    core_panic(PANIC_NMI_HANDLER, "NMI");
}

void isr_mem_manage(void)
{
    core_panic(PANIC_MEM_MANAGE, "MEM MANAGE");
}

void isr_debug_mon(void)
{
    core_panic(PANIC_DEBUG_MON, "DEBUG MON");
}

void isr_hard_fault(void)
{
    core_panic(PANIC_HARD_FAULT, "HARD FAULT");
}

void isr_bus_fault(void)
{
    core_panic(PANIC_BUS_FAULT, "BUS FAULT");
}

void isr_usage_fault(void)
{
    core_panic(PANIC_USAGE_FAULT, "USAGE FAULT");
}

/* Cortex-M specific interrupt handlers */
void isr_svc(void)              __attribute__((weak, alias("dummy_handler")));
void isr_pendsv(void)           __attribute__((weak, alias("dummy_handler")));
void isr_systick(void)          __attribute__((weak, alias("dummy_handler")));

/* SAM3S specific interrupt handlers */
void isr_supc(void)             __attribute__((weak, alias("dummy_handler")));
void isr_rstc(void)             __attribute__((weak, alias("dummy_handler")));
void isr_rtc(void)              __attribute__((weak, alias("dummy_handler")));
void isr_rtt(void)              __attribute__((weak, alias("dummy_handler")));
void isr_wdt(void)              __attribute__((weak, alias("dummy_handler")));
void isr_pmc(void)              __attribute__((weak, alias("dummy_handler")));
void isr_efc(void)              __attribute__((weak, alias("dummy_handler")));
void isr_uart0(void)            __attribute__((weak, alias("dummy_handler")));
void isr_uart1(void)            __attribute__((weak, alias("dummy_handler")));

#ifdef ID_SMC
void isr_smc(void)              __attribute__((weak, alias("dummy_handler")));
#endif

void isr_pioa(void)             __attribute__((weak, alias("dummy_handler")));
void isr_piob(void)             __attribute__((weak, alias("dummy_handler")));

#ifdef ID_PIOC
void isr_pioc(void)             __attribute__((weak, alias("dummy_handler")));
#endif

void isr_usart0(void)           __attribute__((weak, alias("dummy_handler")));

#ifdef ID_USART1
void isr_usart1(void)           __attribute__((weak, alias("dummy_handler")));
#endif

#ifdef ID_HSMCI
void isr_hsmci(void)            __attribute__((weak, alias("dummy_handler")));
#endif


void isr_twi0(void)             __attribute__((weak, alias("dummy_handler")));
void isr_twi1(void)             __attribute__((weak, alias("dummy_handler")));
void isr_spi(void)              __attribute__((weak, alias("dummy_handler")));
void isr_ssc(void)              __attribute__((weak, alias("dummy_handler")));
void isr_tc0(void)              __attribute__((weak, alias("dummy_handler")));
void isr_tc1(void)              __attribute__((weak, alias("dummy_handler")));
void isr_tc2(void)              __attribute__((weak, alias("dummy_handler")));

#ifdef ID_TC3
void isr_tc3(void)              __attribute__((weak, alias("dummy_handler")));
#endif

#ifdef ID_TC4
void isr_tc4(void)              __attribute__((weak, alias("dummy_handler")));
#endif

void isr_adc(void)              __attribute__((weak, alias("dummy_handler")));

#ifdef ID_DACC
void isr_dacc(void)             __attribute__((weak, alias("dummy_handler")));
#endif

void isr_pwm(void)              __attribute__((weak, alias("dummy_handler")));
void isr_crccu(void)            __attribute__((weak, alias("dummy_handler")));
void isr_acc(void)              __attribute__((weak, alias("dummy_handler")));
void isr_udp(void)              __attribute__((weak, alias("dummy_handler")));

/* Interrupt vector table */
__attribute__((section(".vectors")))
const void *interrupt_vector[] = {
    /* Initial stack pointer */
    (void *)(&_estack),

    /* Cortex-M3 core exceptions */
    (void *) reset_handler,     /*  1 */
    (void *) isr_nmi,           /*  2 */
    (void *) isr_hard_fault,    /*  3 */
    (void *) isr_mem_manage,    /*  4 */
    (void *) isr_bus_fault,     /*  5 */
    (void *) isr_usage_fault,   /*  6 */
    (void *)(0UL),              /*  7 Reserved */
    (void *)(0UL),              /*  8 Reserved */
    (void *)(0UL),              /*  9 Reserved */
    (void *)(0UL),              /* 10 Reserved */
    (void *) isr_svc,           /* 11 Supervisor Call */
    (void *) isr_debug_mon,     /* 12 Reserved for Debug */
    (void *)(0UL),              /* 13 Reserved */
    (void *) isr_pendsv,        /* 14 PendSV */
    (void *) isr_systick,       /* 15 Systick */

    /* SAM3S specific exceptions */
    (void *) isr_supc,          /*  0 Supply Controller */
    (void *) isr_rstc,          /*  1 Reset Controller */
    (void *) isr_rtc,           /*  2 Real Time Clock */
    (void *) isr_rtt,           /*  3 Real Time Timer */
    (void *) isr_wdt,           /*  4 Watchdog Timer */
    (void *) isr_pmc,           /*  5 Power Management Controller */
    (void *) isr_efc,           /*  6 Enhanced Embedded Flash Controller */
    (void *)(0UL),              /*  7 Reserved */
    (void *) isr_uart0,         /*  8 UART 0 */
    (void *) isr_uart1,         /*  9 UART 1 */
#ifdef ID_SMC
    (void *) isr_smc,           /* 10 SMC */
#else
    (void *)(0UL),              /* 10 Reserved */
#endif
    (void *) isr_pioa,          /* 11 PIO Controller A */
    (void *) isr_piob,          /* 12 PIO Controller B */
#ifdef ID_PIOC
    (void *) isr_pioc,          /* 13 PIO Controller C */
#else
    (void *)(0UL),              /* 13 Reserved */
#endif
    (void *) isr_usart0,        /* 14 USART 0 */
#ifdef ID_USART1
    (void *) isr_usart1,        /* 15 USART 1 */
#else
    (void *)(0UL),              /* 15 Reserved */
#endif
    (void *)(0UL),              /* 16 Reserved */
    (void *)(0UL),              /* 17 Reserved */
#ifdef ID_HSMCI
    (void *) isr_hsmci,         /* 18 HSMCI */
#else
    (void *)(0UL),              /* 18 Reserved */
#endif
    (void *) isr_twi0,          /* 19 Two-wire Interface 0 */
    (void *) isr_twi1,          /* 20 Two-wire Interface 1 */
    (void *) isr_spi,           /* 21 Serial Peripheral Interface */
    (void *) isr_ssc,           /* 22 Synchronous Serial Controller */
    (void *) isr_tc0,           /* 23 Timer/Counter 0 */
    (void *) isr_tc1,           /* 24 Timer/Counter 1 */
    (void *) isr_tc2,           /* 25 Timer/Counter 2 */
#ifdef ID_TC3
    (void *) isr_tc3,           /* 26 Timer/Counter 3 */
#else
    (void *)(0UL),              /* 26 Reserved */
#endif
#ifdef ID_TC4
    (void *) isr_tc4,           /* 27 Timer/Counter 4 */
#else
    (void *)(0UL),              /* 27 Reserved */
#endif
#ifdef ID_TC5
    (void *) isr_tc5,           /* 28 Timer/Counter 5 */
#else
    (void *)(0UL),              /* 28 Reserved */
#endif
    (void *) isr_adc,           /* 29 Analog to Digital Converter */
#ifdef ID_DACC
    (void *) isr_dacc,          /* 30 Digital to Analog Converter */
#else
    (void *)(0UL),              /* 30 Reserved */
#endif
    (void *) isr_pwm,           /* 31 Pulse Width Modulation */
    (void *) isr_crccu,         /* 32 CRC Calculation Unit */
    (void *) isr_acc,           /* 33 Analog Comparator */
    (void *) isr_udp,           /* 34 USB Device Port */
    (void *)(0UL),              /* 35 Reserved */
};

