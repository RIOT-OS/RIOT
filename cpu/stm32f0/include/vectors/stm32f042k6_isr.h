/*
 * Copyright (C) 2017 Inria
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
 * @brief       Interrupt vector definitions for stm32f042k6
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifndef STM32F042K6_ISR_H
#define STM32F042K6_ISR_H

#if defined(CPU_MODEL_STM32F042K6)

#define STM32F042K6_VECTORS                                                   \
    (void*) isr_wwdg,               /* windowed watchdog */                   \
    (void*) isr_pvd,                /* power control */                       \
    (void*) isr_rtc,                /* real time clock */                     \
    (void*) isr_flash,              /* flash memory controller */             \
    (void*) isr_rcc,                /* reset and clock control */             \
    (void*) isr_exti,               /* external interrupt lines 0 and 1 */    \
    (void*) isr_exti,               /* external interrupt lines 2 and 3 */    \
    (void*) isr_exti,               /* external interrupt lines 4 to 15 */    \
    (void*) isr_ts,                 /* touch sensing input*/                  \
    (void*) isr_dma1_ch1,           /* direct memory access controller 1, channel 1*/        \
    (void*) isr_dma1_ch2_3,         /* direct memory access controller 1, channel 2 and 3*/  \
    (void*) isr_dma1_ch4_5,         /* direct memory access controller 1, channel 4 and 5*/  \
    (void*) isr_adc1_comp,          /* analog digital converter */                           \
    (void*) isr_tim1_brk_up_trg_com, /* timer 1 break, update, trigger and communication */  \
    (void*) isr_tim1_cc,            /* timer 1 capture compare */            \
    (void*) isr_tim2,               /* timer 2 */                            \
    (void*) isr_tim3,               /* timer 3 */                            \
    (void*) (0UL),                  /* reserved */                           \
    (void*) (0UL),                  /* reserved */                           \
    (void*) isr_tim14,              /* timer 14 */                           \
    (void*) (0UL),                  /* reserved */                           \
    (void*) isr_tim16,              /* timer 16 */                           \
    (void*) isr_tim17,              /* timer 17 */                           \
    (void*) isr_i2c1,               /* I2C 1 */                              \
    (void*) (0UL),                  /* reserved */                           \
    (void*) isr_spi1,               /* SPI 1 */                              \
    (void*) isr_spi2,               /* SPI 2 */                              \
    (void*) isr_usart1,             /* USART 1 */                            \
    (void*) isr_usart2,             /* USART 2 */                            \
    (void*) (0UL),                  /* reserved */                           \
    (void*) isr_cec,                /* consumer electronics control */       \
    (void*) isr_usb,                /* USB */
#endif

#ifdef __cplusplus
extern "C" {}
#endif

#endif /* STM32F042K6_ISR_H */
