/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l0
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32L0 specific interrupt vectors */
WEAK_DEFAULT void isr_adc1_comp(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2_3(void);
WEAK_DEFAULT void isr_dma1_channel4_5_6_7(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_i2c3(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_lpuart1(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rcc_crs(void);
WEAK_DEFAULT void isr_rng_lpuart1(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim21(void);
WEAK_DEFAULT void isr_tim22(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tsc(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart4_5(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [ 0] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [ 1] = isr_pvd,                  /* [ 1] PVD through EXTI Line detect Interrupt */
    [ 2] = isr_rtc,                  /* [ 2] RTC through EXTI Line Interrupt */
    [ 3] = isr_flash,                /* [ 3] FLASH Interrupt */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupts */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupts */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupts */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupts */
    [11] = isr_dma1_channel4_5_6_7,  /* [11] DMA1 Channel 4, Channel 5, Channel 6 and Channel 7 Interrupts */
    [12] = isr_adc1_comp,            /* [12] ADC1, COMP1 and COMP2 Interrupts */
    [13] = isr_lptim1,               /* [13] LPTIM1 Interrupt */
    [15] = isr_tim2,                 /* [15] TIM2 Interrupt */
    [20] = isr_tim21,                /* [20] TIM21 Interrupt */
    [22] = isr_tim22,                /* [22] TIM22 Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Interrupt */
    [25] = isr_spi1,                 /* [25] SPI1 Interrupt */
    [28] = isr_usart2,               /* [28] USART2 Interrupt */

#if defined(CPU_LINE_STM32L031xx)
    [ 4] = isr_rcc,                  /* [ 4] RCC Interrupt */
    [29] = isr_lpuart1,              /* [29] LPUART1 Interrupt */
#elif defined(CPU_LINE_STM32L053xx)
    [ 4] = isr_rcc_crs,              /* [ 4] RCC and CRS Interrupts */
    [ 8] = isr_tsc,                  /* [ 8] TSC Interrupt */
    [17] = isr_tim6_dac,             /* [17] TIM6 and DAC Interrupts */
    [24] = isr_i2c2,                 /* [24] I2C2 Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 Interrupt */
    [27] = isr_usart1,               /* [27] USART1 Interrupt */
    [29] = isr_rng_lpuart1,          /* [29] RNG and LPUART1 Interrupts */
    [30] = isr_lcd,                  /* [30] LCD Interrupt */
    [31] = isr_usb,                  /* [31] USB global Interrupt */
#elif defined(CPU_LINE_STM32L072xx)
    [ 4] = isr_rcc_crs,              /* [ 4] RCC and CRS Interrupts */
    [ 8] = isr_tsc,                  /* [ 8] TSC Interrupt */
    [14] = isr_usart4_5,             /* [14] USART4 and USART5 Interrupt */
    [16] = isr_tim3,                 /* [16] TIM3 Interrupt */
    [17] = isr_tim6_dac,             /* [17] TIM6 and DAC Interrupts */
    [18] = isr_tim7,                 /* [18] TIM7 Interrupt */
    [21] = isr_i2c3,                 /* [21] I2C3 Interrupt */
    [24] = isr_i2c2,                 /* [24] I2C2 Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 Interrupt */
    [27] = isr_usart1,               /* [27] USART1 Interrupt */
    [29] = isr_rng_lpuart1,          /* [29] RNG and LPUART1 Interrupts */
    [31] = isr_usb,                  /* [31] USB global Interrupt */
#elif defined(CPU_LINE_STM32L073xx)
    [ 4] = isr_rcc_crs,              /* [ 4] RCC and CRS Interrupts */
    [ 8] = isr_tsc,                  /* [ 8] TSC Interrupt */
    [14] = isr_usart4_5,             /* [14] USART4 and USART5 Interrupt */
    [16] = isr_tim3,                 /* [16] TIM3 Interrupt */
    [17] = isr_tim6_dac,             /* [17] TIM6 and DAC Interrupts */
    [18] = isr_tim7,                 /* [18] TIM7 Interrupt */
    [21] = isr_i2c3,                 /* [21] I2C3 Interrupt */
    [24] = isr_i2c2,                 /* [24] I2C2 Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 Interrupt */
    [27] = isr_usart1,               /* [27] USART1 Interrupt */
    [29] = isr_rng_lpuart1,          /* [29] RNG and LPUART1 Interrupts */
    [30] = isr_lcd,                  /* [30] LCD Interrupt */
    [31] = isr_usb,                  /* [31] USB global Interrupt */
#endif
};
