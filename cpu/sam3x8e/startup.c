/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3x8e
 * @{
 *
 * @file        startup.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "board.h"

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
    puts("\n### HARD FAULT ###\n");
    while (1) {
        LED_RED_TOGGLE;
        for (int i = 0; i < 2000000; i++) {
            asm ("nop");
        }
    }
}

void isr_bus_fault(void)
{
    while (1) {asm ("nop");}
}

void isr_usage_fault(void)
{
    while (1) {asm ("nop");}
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* SAM3X8E specific interrupt vector */
void isr_supc(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_rstc(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_rtt(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_wdt(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pmc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_efc0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_efc1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_uart(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_smc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pioa(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_piob(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_pioc(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_piod(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_usart0(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_usart1(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_usart2(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_usart3(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_hsmci(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_twi0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_twi1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_ssc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc0(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc1(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc2(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc3(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc4(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc5(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc6(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc7(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tc8(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pwm(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_adc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_dacc(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dmac(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_uotghs(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_trng(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_emac(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_can0(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_can1(void)                 __attribute__ ((weak, alias("dummy_handler")));


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
    /* STM specific peripheral handlers */
    (void*) isr_supc,               /*  0 supply controller */
    (void*) isr_rstc,               /*  1 reset controller */
    (void*) isr_rtc,                /*  2 real time clock */
    (void*) isr_rtt,                /*  3 real timer timer */
    (void*) isr_wdt,                /*  4 watchdog timer */
    (void*) isr_pmc,                /*  5 power management controller */
    (void*) isr_efc0,               /*  6 enhanced flash controller 0 */
    (void*) isr_efc1,               /*  7 enhanced flash controller 1 */
    (void*) isr_uart,               /*  8 universal asynchronous receiver transceiver */
    (void*) isr_smc,                /*  9 static memory controller */
    (void*) (0UL),
    (void*) isr_pioa,               /* 11 GPIO port A */
    (void*) isr_piob,               /* 12 GPIO port B */
    (void*) isr_pioc,               /* 13 GPIO port C */
    (void*) isr_piod,               /* 14 GPIO port D */
    (void*) (0UL),
    (void*) (0UL),
    (void*) isr_usart0,             /* 17 USART0 */
    (void*) isr_usart1,             /* 18 USART1 */
    (void*) isr_usart2,             /* 19 USART2 */
    (void*) isr_usart3,             /* 20 USART3 */
    (void*) isr_hsmci,              /* 21 multimedia card interface */
    (void*) isr_twi0,               /* 22 two-wire interface 0 */
    (void*) isr_twi1,               /* 23 two-wire interface 1 */
    (void*) isr_spi0,               /* 24 serial peripheral interface */
    (void*) (0UL),
    (void*) isr_ssc,                /* 26 synchronous serial controller */
    (void*) isr_tc0,                /* 27 timer counter 0 */
    (void*) isr_tc1,                /* 28 timer counter 1 */
    (void*) isr_tc2,                /* 29 timer counter 2 */
    (void*) isr_tc3,                /* 30 timer counter 3 */
    (void*) isr_tc4,                /* 31 timer counter 4 */
    (void*) isr_tc5,                /* 32 timer counter 5 */
    (void*) isr_tc6,                /* 33 timer counter 6 */
    (void*) isr_tc7,                /* 34 timer counter 7 */
    (void*) isr_tc8,                /* 35 timer counter 8 */
    (void*) isr_pwm,                /* 36 pulse width modulation controller */
    (void*) isr_adc,                /* 37 ADC controller */
    (void*) isr_dacc,               /* 38 DAC controller */
    (void*) isr_dmac,               /* 39 DMA controller */
    (void*) isr_uotghs,             /* 40 USB OTG high speed */
    (void*) isr_trng,               /* 41 true random number generator */
    (void*) isr_emac,               /* 42 Ethernet MAC*/
    (void*) isr_can0,               /* 43 CAN controller 0*/
    (void*) isr_can1,               /* 44 CAN controller 1*/
};
