/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
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
/* STM32F0 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti0_1(void);
WEAK_DEFAULT void isr_exti2_3(void);
WEAK_DEFAULT void isr_exti4_15(void);
WEAK_DEFAULT void isr_ts(void);
WEAK_DEFAULT void isr_dma1_ch1(void);
WEAK_DEFAULT void isr_dma1_ch2_3(void);
WEAK_DEFAULT void isr_dma1_ch4_5(void);
WEAK_DEFAULT void isr_adc1_comp(void);
WEAK_DEFAULT void isr_tim1_brk_up_trg_com(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim14(void);
WEAK_DEFAULT void isr_tim15(void);
WEAK_DEFAULT void isr_tim16(void);
WEAK_DEFAULT void isr_tim17(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3_8(void);
WEAK_DEFAULT void isr_cec(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M0 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
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
    (void*) isr_tim7,               /* timer 7 */
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
    (void*) isr_usart3_8,           /* USART 3 to 8 */
    (void*) isr_cec,                /* consumer electronics control */
    (void*) (0UL)                   /* reserved */
};
