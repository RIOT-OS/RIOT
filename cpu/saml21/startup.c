/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *               2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml21
 * @{
 *
 * @file        startup.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

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

void isr_nmi(void)
{
    core_panic(NMI_HANDLER, "NMI HANDLER");
}

void isr_hard_fault(void)
{
    core_panic(HARD_FAULT, "HARD FAULT");
}

/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
void dummy_handler(void)
{
    core_panic(DUMMY_HANDLER, "DUMMY HANDLER");
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)          __attribute__ ((weak, alias("dummy_handler")));
/* SAML21 specific interrupt vector */
void isr_pm(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_wdt(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_eic(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_nvmctrl(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_dmac(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_usb(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_evsys(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom0(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom1(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom2(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom3(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom4(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom5(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_tcc0(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_tcc1(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_tcc2(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_tc0(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tc1(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tc2(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tc3(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tc4(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_adc(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_ac(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_dac(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_ptc(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_aes(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_trng(void)             __attribute__ ((weak, alias("dummy_handler")));

/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M0+ handlers */
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
    /* Atmel SAML21 specific peripheral handlers */
    (void*) isr_pm,                 /*  0 Power Manager */
    (void*) isr_wdt,                /*  1 Watchdog Timer */
    (void*) isr_rtc,                /*  2 Real-Time Counter */
    (void*) isr_eic,                /*  3 External Interrupt Controller */
    (void*) isr_nvmctrl,            /*  4 Non-Volatile Memory Controller */
    (void*) isr_dmac,               /*  5 Direct Memory Access Controller */
    (void*) isr_usb,                /*  6 Universal Serial Bus */
    (void*) isr_evsys,              /*  7 Event System Interface */
    (void*) isr_sercom0,            /*  8 Serial Communication Interface 0 */
    (void*) isr_sercom1,            /*  9 Serial Communication Interface 1 */
    (void*) isr_sercom2,            /* 10 Serial Communication Interface 2 */
    (void*) isr_sercom3,            /* 11 Serial Communication Interface 3 */
    (void*) isr_sercom4,            /* 12 Serial Communication Interface 4 */
    (void*) isr_sercom5,            /* 13 Serial Communication Interface 5 */
    (void*) isr_tcc0,               /* 14 Timer Counter Control 0 */
    (void*) isr_tcc1,               /* 15 Timer Counter Control 1 */
    (void*) isr_tcc2,               /* 16 Timer Counter Control 2 */
    (void*) isr_tc0,                /* 17 Basic Timer Counter 0 */
    (void*) isr_tc1,                /* 18 Basic Timer Counter 1 */
    (void*) isr_tc2,                /* 19 Basic Timer Counter 2 */
    (void*) isr_tc3,                /* 20 Basic Timer Counter 3 */
    (void*) isr_tc4,                /* 21 Basic Timer Counter 4 */
    (void*) isr_adc,                /* 22 Analog Digital Converter */
    (void*) isr_ac,                 /* 23 Analog Comparators */
    (void*) isr_dac,                /* 24 Digital Analog Converter */
    (void*) isr_ptc,                /* 25 Peripheral Touch Controller */
    (void*) isr_aes,                /* 26 AES */
    (void*) isr_trng,               /* 27 TRNG True Random Number Generator */
};
