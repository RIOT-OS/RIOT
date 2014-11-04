/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file        startup.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "crash.h"
 #include "ps.h"
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

/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
void dummy_handler(void)
{
    core_panic(DUMMY_HANDLER, "DUMMY HANDLER");
    while (1) {asm ("nop");}
}

void isr_nmi(void)
{
    while (1) {asm ("nop");}
}

void isr_mem_manage(void)
{
    while (1) {asm ("nop");}
}

void isr_debug_mon(void)
{
    while (1) {asm ("nop");}
}

void isr_hard_fault(void)
{
    //thread_print_all();
    core_panic(HARD_FAULT, "HARD FAULT");
    while (1) {asm ("nop");}
}

void isr_bus_fault(void)
{
    core_panic(BUS_FAULT, "BUS FAULT");
    while (1) {asm ("nop");}
}

void isr_usage_fault(void)
{
    core_panic(USAGE_FAULT, "USAGE FAULT");
    while (1) {asm ("nop");}
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* SAMR21 specific interrupt vector */
void isr_pm(void)                   __attribute__ ((weak, alias("dummy_handler")));
void isr_sysctrl(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_wdt(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_eic(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_nvmctrl(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_dmac(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_usb(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_evsys(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom0(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom1(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom2(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom3(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom4(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_sercom5(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tcc0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tcc1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tcc2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tc3(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc4(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc5(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc6(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc7(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_adc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_ac(void)                   __attribute__ ((weak, alias("dummy_handler")));
void isr_dac(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_ptc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c(void)                  __attribute__ ((weak, alias("dummy_handler")));

/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) isr_mem_manage,         /* memory controller interrupt */
    (void*) isr_bus_fault,          /* also not good to end up here */
    (void*) isr_usage_fault,        /* autsch */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) isr_debug_mon,          /* debug interrupt */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* Atmel specific peripheral handlers */
    (void*) isr_pm,                 /*  0 Power Manager */
    (void*) isr_sysctrl,            /*  1 System Control */
    (void*) isr_wdt,                /*  2 Watchdog Timer */
    (void*) isr_rtc,                /*  3 Real-Time Counter */
    (void*) isr_eic,                /*  4 External Interrupt Controller */
    (void*) isr_nvmctrl,            /*  5 Non-Volatile Memory Controller */
    (void*) isr_dmac,               /*  6 Direct Memory Access Controller */
    (void*) isr_usb,                /*  7 Universal Serial Bus */
    (void*) isr_evsys,              /*  8 Event System Interface */
    (void*) isr_sercom0,            /*  9 Serial Communication Interface 0 */
    (void*) isr_sercom1,            /* 10 Serial Communication Interface 1 */
    (void*) isr_sercom2,            /* 11 Serial Communication Interface 2 */
    (void*) isr_sercom3,            /* 12 Serial Communication Interface 3 */
    (void*) isr_sercom4,            /* 13 Serial Communication Interface 4 */
    (void*) isr_sercom5,            /* 14 Serial Communication Interface 5 */
    (void*) isr_tcc0,               /* 15 Timer Counter Control 0 */
    (void*) isr_tcc1,               /* 16 Timer Counter Control 1 */
    (void*) isr_tcc2,               /* 17 Timer Counter Control 2 */
    (void*) isr_tc3,                /* 18 Basic Timer Counter 0 */
    (void*) isr_tc4,                /* 19 Basic Timer Counter 1 */
    (void*) isr_tc5,                /* 20 Basic Timer Counter 2 */
    (void*) isr_tc6,                /* 21 Basic Timer Counter 3 */
    (void*) isr_tc7,                /* 22 Basic Timer Counter 4 */
    (void*) isr_adc,                /* 23 Analog Digital Converter */
    (void*) isr_ac,                 /* 24 Analog Comparators */
    (void*) isr_dac,                /* 25 Digital Analog Converter */
    (void*) isr_ptc,                /* 26 Peripheral Touch Controller */
    (void*) isr_i2c                 /* 27 Inter-IC Sound Interface */
};
