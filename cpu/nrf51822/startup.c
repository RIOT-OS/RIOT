/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "board.h"
#include "cpu.h"
#include "panic.h"

/**
 * memory markers as defined in the linker script
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
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. load data section from flash to ram
 * 2. overwrite uninitialized data section (BSS) with zeros
 * 3. initialize the newlib
 * 4. initialize the board (sync clock, setup std-IO)
 * 5. initialize and start RIOTs kernel
 */
void reset_handler(void)
{
    uint32_t *dst;
    uint32_t *src = &_etext;

    /* make sure all RAM blocks are turned on
     * -> see NRF51822 Product Anomaly Notice (PAN) #16 for more details
     */
    NRF_POWER->RAMON = 0xf;

    /* load data section from flash to ram */
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
void dummy_handler(void)
{
    core_panic(PANIC_DUMMY_HANDLER, "DUMMY ISR HANDLER");
}

void isr_nmi(void)
{
    core_panic(PANIC_NMI_HANDLER, "NMI HANDLER");
}

void isr_hard_fault(void)
{
    core_panic(PANIC_HARD_FAULT, "HARD FAULT");
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* nRF51822qfaa specific interrupt vector */
void isr_power_clock(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_radio(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_uart0(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_spi0_twi0(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_spi1_twi1(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_gpiote(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_adc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_timer0(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_timer1(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_timer2(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_temp(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_rng(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_ecb(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_ccm_aar(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_wdt(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_qdec(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_lpcomp(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_swi0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_swi1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_swi2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_swi3(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_swi4(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_swi5(void)                 __attribute__ ((weak, alias("dummy_handler")));


/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* nRF51 specific peripheral handlers */
    (void*) isr_power_clock,        /* power_clock */
    (void*) isr_radio,              /* radio */
    (void*) isr_uart0,              /* uart0 */
    (void*) isr_spi0_twi0,          /* spi0_twi0 */
    (void*) isr_spi1_twi1,          /* spi1_twi1 */
    (void*) (0UL),                  /* reserved */
    (void*) isr_gpiote,             /* gpiote */
    (void*) isr_adc,                /* adc */
    (void*) isr_timer0,             /* timer0 */
    (void*) isr_timer1,             /* timer1 */
    (void*) isr_timer2,             /* timer2 */
    (void*) isr_rtc0,               /* rtc0 */
    (void*) isr_temp,               /* temp */
    (void*) isr_rng,                /* rng */
    (void*) isr_ecb,                /* ecb */
    (void*) isr_ccm_aar,            /* ccm_aar */
    (void*) isr_wdt,                /* wdt */
    (void*) isr_rtc1,               /* rtc1 */
    (void*) isr_qdec,               /* qdec */
    (void*) isr_lpcomp,             /* lpcomp */
    (void*) isr_swi0,               /* swi0 */
    (void*) isr_swi1,               /* swi1 */
    (void*) isr_swi2,               /* swi2 */
    (void*) isr_swi3,               /* swi3 */
    (void*) isr_swi4,               /* swi4 */
    (void*) isr_swi5,               /* swi5 */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
};
