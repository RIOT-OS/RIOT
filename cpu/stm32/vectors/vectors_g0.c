/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions for STM32G0
 *
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

/* STM32G0 specific interrupt vectors */
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2_3(void);
WEAK_DEFAULT void isr_dma1_ch4_7_dmamux1_ovr(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rtc_tamp(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_tim1_brk_up_trg_com(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim6(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim14(void);
WEAK_DEFAULT void isr_tim15(void);
WEAK_DEFAULT void isr_tim16(void);
WEAK_DEFAULT void isr_tim17(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3_4(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [WWDG_IRQn                          ] = isr_wwdg,                   /* [ 0] Window WatchDog Interrupt */
    [RTC_TAMP_IRQn                      ] = isr_rtc_tamp,               /* [ 2] RTC interrupt through the EXTI line 19 & 21 */
    [FLASH_IRQn                         ] = isr_flash,                  /* [ 3] FLASH global Interrupt */
    [RCC_IRQn                           ] = isr_rcc,                    /* [ 4] RCC global Interrupt */
    [EXTI0_1_IRQn                       ] = isr_exti,                   /* [ 5] EXTI 0 and 1 Interrupts */
    [EXTI2_3_IRQn                       ] = isr_exti,                   /* [ 6] EXTI 2 and 3 Interrupts */
    [EXTI4_15_IRQn                      ] = isr_exti,                   /* [ 7] EXTI 4 to 15 Interrupts */
    [DMA1_Channel1_IRQn                 ] = isr_dma1_channel1,          /* [ 9] DMA1 Channel 1 Interrupt */
    [DMA1_Channel2_3_IRQn               ] = isr_dma1_channel2_3,        /* [10] DMA1 Channel 2 and Channel 3 Interrupts */
    [DMA1_Ch4_7_DMAMUX1_OVR_IRQn        ] = isr_dma1_ch4_7_dmamux1_ovr, /* [11] DMA1 Channel 4 to Channel 7 and DMAMUX1 Overrun Interrupts */
    [ADC1_IRQn                          ] = isr_adc1,                   /* [12] ADC1 Interrupts */
    [TIM1_BRK_UP_TRG_COM_IRQn           ] = isr_tim1_brk_up_trg_com,    /* [13] TIM1 Break, Update, Trigger and Commutation Interrupts */
    [TIM1_CC_IRQn                       ] = isr_tim1_cc,                /* [14] TIM1 Capture Compare Interrupt */
    [TIM3_IRQn                          ] = isr_tim3,                   /* [16] TIM3 global Interrupt */
    [TIM6_IRQn                          ] = isr_tim6,                   /* [17] TIM6 global Interrupts */
    [TIM7_IRQn                          ] = isr_tim7,                   /* [18] TIM7 global Interrupt */
    [TIM14_IRQn                         ] = isr_tim14,                  /* [19] TIM14 global Interrupt */
    [TIM15_IRQn                         ] = isr_tim15,                  /* [20] TIM15 global Interrupt */
    [TIM16_IRQn                         ] = isr_tim16,                  /* [21] TIM16 global Interrupt */
    [TIM17_IRQn                         ] = isr_tim17,                  /* [22] TIM17 global Interrupt */
    [I2C1_IRQn                          ] = isr_i2c1,                   /* [23] I2C1 Interrupt  (combined with EXTI 23) */
    [I2C2_IRQn                          ] = isr_i2c2,                   /* [24] I2C2 Interrupt */
    [SPI1_IRQn                          ] = isr_spi1,                   /* [25] SPI1/I2S1 Interrupt */
    [SPI2_IRQn                          ] = isr_spi2,                   /* [26] SPI2 Interrupt */
    [USART1_IRQn                        ] = isr_usart1,                 /* [27] USART1 Interrupt */
    [USART2_IRQn                        ] = isr_usart2,                 /* [28] USART2 Interrupt */
    [USART3_4_IRQn                      ] = isr_usart3_4,               /* [29] USART3, USART4 globlal Interrupts */
};
