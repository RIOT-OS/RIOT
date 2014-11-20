/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
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
    while (1) {
        for (int i = 0; i < 250000; i++) {
            asm ("nop");
        }
        LED_RED_TOGGLE;
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

/* STM32F051R8 specific interrupt vector */
void isr_wwdg(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_pvd(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_flash(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_rcc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_exti0_1(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_exti2_3(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_exti4_15(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_ts(void)                   __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch1(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch2_3(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_ch4_5(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_adc1_comp(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_brk_up_trg_com(void)  __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_cc(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tim2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tim3(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tim6_dac(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_tim14(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_tim15(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_tim16(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_tim17(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_usart1(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_usart2(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_cec(void)                  __attribute__ ((weak, alias("dummy_handler")));

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
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,               /* windowed watchdog */
    (void*) isr_pvd,                /* power control */
    (void*) isr_rtc,                /* real time clock */
    (void*) isr_flash,              /* flash memory controller */
    (void*) isr_rcc,                /* reset and clock control */
    (void*) isr_exti0_1,            /* external interrupt lines 0 and 1 */
    (void*) isr_exti2_3,            /* external interrupt lines 2 and 3 */
    (void*) isr_exti4_15,           /* external interrupt lines 4 to 15 */
    (void*) isr_ts,                 /* touch sensing input*/
    (void*) isr_dma1_ch1,           /* direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,         /* direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,         /* direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,          /* analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /* timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,            /* timer 1 capture compare */
    (void*) isr_tim2,               /* timer 2 */
    (void*) isr_tim3,               /* timer 3 */
    (void*) isr_tim6_dac,           /* timer 6 and digital to analog converter */
    (void*) (0UL),                  /* reserved */
    (void*) isr_tim14,              /* timer 14 */
    (void*) isr_tim15,              /* timer 15 */
    (void*) isr_tim16,              /* timer 16 */
    (void*) isr_tim17,              /* timer 17 */
    (void*) isr_i2c1,               /* I2C 1 */
    (void*) isr_i2c2,               /* I2C 2 */
    (void*) isr_spi1,               /* SPI 1 */
    (void*) isr_spi2,               /* SPI 2 */
    (void*) isr_usart1,             /* USART 1 */
    (void*) isr_usart2,             /* USART 2 */
    (void*) (0UL),                  /* reserved */
    (void*) isr_cec,                /* consumer electronics control */
    (void*) (0UL)                   /* reserved */
};
