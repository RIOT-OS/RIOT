/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

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
    puts("##### HARD FAULT #####");
    while (1) {asm ("nop");}
}

void isr_bus_fault(void)
{
    puts("##### BUS FAULT #####");
    while (1) {asm ("nop");}
}

void isr_usage_fault(void)
{
    puts("##### USAGE FAULT #####");
    while (1) {asm ("nop");}
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)        __attribute__ ((weak, alias("dummy_handler")));

/* LPC1768 specific interrupt vector */
void isr_wdt(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_timer0(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_timer1(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_timer2(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_timer3(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_uart0(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_uart1(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_uart2(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_uart3(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_pwm1(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c0(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_spi(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_ssp0(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_ssp1(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_pll0(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_eint0(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_eint1(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_eint2(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_eint3(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_adc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_bod(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_usb(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_can(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_dma(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_i2s(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_enet(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_rit(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_mcpwm(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_qei(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_pll1(void)           __attribute__ ((weak, alias("dummy_handler")));


/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) isr_mem_manage,         /* called on memory fault */
    (void*) isr_bus_fault,          /* called on bus fault */
    (void*) isr_usage_fault,        /* callee on usage fault */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) isr_debug_mon,          /* debug monitor interrupt */
    (void*) (0UL),                  /* reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* LPC specific peripheral handlers */
    (void*) isr_wdt,                /* watchdog timer */
    (void*) isr_timer0,             /* timer0 */
    (void*) isr_timer1,             /* timer1 */
    (void*) isr_timer2,             /* timer2 */
    (void*) isr_timer3,             /* timer3 */
    (void*) isr_uart0,              /* uart0 */
    (void*) isr_uart1,              /* uart1 */
    (void*) isr_uart2,              /* uart2 */
    (void*) isr_uart3,              /* uart3 */
    (void*) isr_pwm1,               /* pwm1 */
    (void*) isr_i2c0,               /* i2c0 */
    (void*) isr_i2c1,               /* i2c1 */
    (void*) isr_i2c2,               /* i2c2 */
    (void*) isr_spi,                /* spi */
    (void*) isr_ssp0,               /* ssp0 */
    (void*) isr_ssp1,               /* ssp1 */
    (void*) isr_pll0,               /* pll0 (main pll) */
    (void*) isr_rtc,                /* real time clock */
    (void*) isr_eint0,              /* external interrupt 0 */
    (void*) isr_eint1,              /* external interrupt 1 */
    (void*) isr_eint2,              /* external interrupt 2 */
    (void*) isr_eint3,              /* external interrupt 3 */
    (void*) isr_adc,                /* a/d converter */
    (void*) isr_bod,                /* brown out detect */
    (void*) isr_usb,                /* usb */
    (void*) isr_can,                /* can */
    (void*) isr_dma,                /* gp dma */
    (void*) isr_i2s,                /* i2s */
    (void*) isr_enet,               /* ethernet */
    (void*) isr_rit,                /* repetitive interrupt timer */
    (void*) isr_mcpwm,              /* motor control pwm */
    (void*) isr_qei,                /* quadrature encoder interface */
    (void*) isr_pll1,               /* pll1 (usb pll) */
};
