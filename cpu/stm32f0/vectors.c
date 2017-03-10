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

#if defined(CPU_MODEL_STM32F030R8)
#include "vectors/stm32f030r8_isr.h"
#elif defined(CPU_MODEL_STM32F031K6)
#include "vectors/stm32f031k6_isr.h"
#elif defined(CPU_MODEL_STM32F042K6)
#include "vectors/stm32f042k6_isr.h"
#elif defined(CPU_MODEL_STM32F051R8)
#include "vectors/stm32f051r8_isr.h"
#elif defined(CPU_MODEL_STM32F070RB)
#include "vectors/stm32f070rb_isr.h"
#elif defined(CPU_MODEL_STM32F072RB)
#include "vectors/stm32f072rb_isr.h"
#elif defined(CPU_MODEL_STM32F091RC)
#include "vectors/stm32f091rc_isr.h"
#endif

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
WEAK_DEFAULT void isr_exti(void);
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
WEAK_DEFAULT void isr_usart3_4(void);
WEAK_DEFAULT void isr_cec(void);
WEAK_DEFAULT void isr_usb(void);

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
#if defined(CPU_MODEL_STM32F030R8)
STM32F030R8_VECTORS
#elif defined(CPU_MODEL_STM32F031K6)
STM32F031K6_VECTORS
#elif defined(CPU_MODEL_STM32F042K6)
STM32F042K6_VECTORS
#elif defined(CPU_MODEL_STM32F051R8)
STM32F051R8_VECTORS
#elif defined(CPU_MODEL_STM32F070RB)
STM32F070RB_VECTORS
#elif defined(CPU_MODEL_STM32F072RB)
STM32F072RB_VECTORS
#elif defined(CPU_MODEL_STM32F091RC)
STM32F091RC_VECTORS
#endif
};
