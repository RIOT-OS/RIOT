/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
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

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32F0 specific interrupt vectors */
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_adc1_comp(void);
WEAK_DEFAULT void isr_cec_can(void);
WEAK_DEFAULT void isr_dma1_ch1(void);
WEAK_DEFAULT void isr_dma1_ch2_3_dma2_ch1_2(void);
WEAK_DEFAULT void isr_dma1_ch4_7_dma2_ch3_5(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2_3(void);
WEAK_DEFAULT void isr_dma1_channel4_5(void);
WEAK_DEFAULT void isr_dma1_channel4_5_6_7(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_pvd_vddio2(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rcc_crs(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_tim14(void);
WEAK_DEFAULT void isr_tim15(void);
WEAK_DEFAULT void isr_tim16(void);
WEAK_DEFAULT void isr_tim17(void);
WEAK_DEFAULT void isr_tim1_brk_up_trg_com(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim6(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tsc(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3_4(void);
WEAK_DEFAULT void isr_usart3_8(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [ 0] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [ 2] = isr_rtc,                  /* [ 2] RTC Interrupt through EXTI Lines 17, 19 and 20 */
    [ 3] = isr_flash,                /* [ 3] FLASH global Interrupt */
    [14] = isr_tim1_cc,              /* [14] TIM1 Capture Compare Interrupt */
    [16] = isr_tim3,                 /* [16] TIM3 global Interrupt */
    [19] = isr_tim14,                /* [19] TIM14 global Interrupt */
    [21] = isr_tim16,                /* [21] TIM16 global Interrupt */
    [22] = isr_tim17,                /* [22] TIM17 global Interrupt */
    [25] = isr_spi1,                 /* [25] SPI1 global Interrupt */

#if defined(CPU_LINE_STM32F030x8)
    [ 4] = isr_rcc,                  /* [ 4] RCC global Interrupt */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupt */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupt */
    [11] = isr_dma1_channel4_5,      /* [11] DMA1 Channel 4 and Channel 5 Interrupt */
    [12] = isr_adc1,                 /* [12] ADC1 Interrupt */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupt */
    [17] = isr_tim6,                 /* [17] TIM6 global Interrupt */
    [20] = isr_tim15,                /* [20] TIM15 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt */
    [24] = isr_i2c2,                 /* [24] I2C2 Event Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 global Interrupt */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt */
    [28] = isr_usart2,               /* [28] USART2 global Interrupt */
#elif defined(CPU_LINE_STM32F031x6)
    [ 1] = isr_pvd,                  /* [ 1] PVD Interrupt through EXTI Lines 16 */
    [ 4] = isr_rcc,                  /* [ 4] RCC global Interrupt */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupt */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupt */
    [11] = isr_dma1_channel4_5,      /* [11] DMA1 Channel 4 and Channel 5 Interrupt */
    [12] = isr_adc1,                 /* [12] ADC1 Interrupt */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupt */
    [15] = isr_tim2,                 /* [15] TIM2 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
#elif defined(CPU_LINE_STM32F042x6)
    [ 1] = isr_pvd_vddio2,           /* [ 1] PVD & VDDIO2 Interrupts through EXTI Lines 16 and 31 */
    [ 4] = isr_rcc_crs,              /* [ 4] RCC & CRS Global Interrupts */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupts */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupts */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupts */
    [ 8] = isr_tsc,                  /* [ 8] Touch Sensing Controller Interrupts */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupts */
    [11] = isr_dma1_channel4_5,      /* [11] DMA1 Channel 4 and Channel 5 Interrupts */
    [12] = isr_adc1,                 /* [12] ADC1 Interrupt */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupts */
    [15] = isr_tim2,                 /* [15] TIM2 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    [26] = isr_spi2,                 /* [26] SPI2 global Interrupt */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    [28] = isr_usart2,               /* [28] USART2 global Interrupt */
    [30] = isr_cec_can,              /* [30] CEC and CAN global Interrupts & EXTI Line27 Interrupt */
    [31] = isr_usb,                  /* [31] USB global Interrupts & EXTI Line18 Interrupt */
#elif defined(CPU_LINE_STM32F051x8)
    [ 1] = isr_pvd,                  /* [ 1] PVD Interrupt through EXTI Lines 16 */
    [ 4] = isr_rcc,                  /* [ 4] RCC global Interrupt */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupts */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupts */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupts */
    [ 8] = isr_tsc,                  /* [ 8] Touch Sensing Controller Interrupts */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupts */
    [11] = isr_dma1_channel4_5,      /* [11] DMA1 Channel 4 and Channel 5 Interrupts */
    [12] = isr_adc1_comp,            /* [12] ADC1 and COMP interrupts (ADC interrupt combined with EXTI Lines 21 and 22 */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupts */
    [15] = isr_tim2,                 /* [15] TIM2 global Interrupt */
    [17] = isr_tim6_dac,             /* [17] TIM6 global and DAC channel underrun error Interrupts */
    [20] = isr_tim15,                /* [20] TIM15 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    [24] = isr_i2c2,                 /* [24] I2C2 Event Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 global Interrupt */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    [28] = isr_usart2,               /* [28] USART2 global Interrupt */
    [30] = isr_cec_can,              /* [30] CEC and CAN global Interrupts & EXTI Line27 Interrupt */
#elif defined(CPU_LINE_STM32F070xB)
    [ 4] = isr_rcc,                  /* [ 4] RCC global Interrupt */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupt */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupt */
    [11] = isr_dma1_channel4_5,      /* [11] DMA1 Channel 4 and Channel 5 Interrupt */
    [12] = isr_adc1,                 /* [12] ADC1 Interrupt */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupt */
    [17] = isr_tim6,                 /* [17] TIM6 global Interrupt */
    [18] = isr_tim7,                 /* [18] TIM7 global Interrupt */
    [20] = isr_tim15,                /* [20] TIM15 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    [24] = isr_i2c2,                 /* [24] I2C2 Event Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 global Interrupt */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt */
    [28] = isr_usart2,               /* [28] USART2 global Interrupt */
    [29] = isr_usart3_4,             /* [29] USART3 and USART4 global Interrupt */
    [31] = isr_usb,                  /* [31] USB global Interrupt  & EXTI Line18 Interrupt */
#elif defined(CPU_LINE_STM32F072xB)
    [ 1] = isr_pvd_vddio2,           /* [ 1] PVD & VDDIO2 Interrupt through EXTI Lines 16 and 31 */
    [ 4] = isr_rcc_crs,              /* [ 4] RCC & CRS global Interrupt */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupt */
    [ 8] = isr_tsc,                  /* [ 8] Touch Sensing Controller Interrupts */
    [ 9] = isr_dma1_channel1,        /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_channel2_3,      /* [10] DMA1 Channel 2 and Channel 3 Interrupt */
    [11] = isr_dma1_channel4_5_6_7,  /* [11] DMA1 Channel 4 to Channel 7 Interrupt */
    [12] = isr_adc1_comp,            /* [12] ADC1 and COMP interrupts (ADC interrupt combined with EXTI Lines 21 and 22 */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupt */
    [15] = isr_tim2,                 /* [15] TIM2 global Interrupt */
    [17] = isr_tim6_dac,             /* [17] TIM6 global and DAC channel underrun error Interrupt */
    [18] = isr_tim7,                 /* [18] TIM7 global Interrupt */
    [20] = isr_tim15,                /* [20] TIM15 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    [24] = isr_i2c2,                 /* [24] I2C2 Event Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 global Interrupt */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    [28] = isr_usart2,               /* [28] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    [29] = isr_usart3_4,             /* [29] USART3 and USART4 global Interrupt */
    [30] = isr_cec_can,              /* [30] CEC and CAN global Interrupts & EXTI Line27 Interrupt */
    [31] = isr_usb,                  /* [31] USB global Interrupt  & EXTI Line18 Interrupt */
#elif defined(CPU_LINE_STM32F091xC)
    [ 1] = isr_pvd_vddio2,           /* [ 1] PVD & VDDIO2 Interrupts through EXTI Lines 16 and 31 */
    [ 4] = isr_rcc_crs,              /* [ 4] RCC & CRS global Interrupts */
    [ 5] = isr_exti,                 /* [ 5] EXTI Line 0 and 1 Interrupts */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line 2 and 3 Interrupts */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line 4 to 15 Interrupts */
    [ 8] = isr_tsc,                  /* [ 8] Touch Sensing Controller Interrupts */
    [ 9] = isr_dma1_ch1,             /* [ 9] DMA1 Channel 1 Interrupt */
    [10] = isr_dma1_ch2_3_dma2_ch1_2,/* [10] DMA1 Channel 2 and 3 & DMA2 Channel 1 and 2 Interrupts */
    [11] = isr_dma1_ch4_7_dma2_ch3_5,/* [11] DMA1 Channel 4 to 7 & DMA2 Channel 3 to 5 Interrupts */
    [12] = isr_adc1_comp,            /* [12] ADC, COMP1 and COMP2 Interrupts (EXTI Lines 21 and 22) */
    [13] = isr_tim1_brk_up_trg_com,  /* [13] TIM1 Break, Update, Trigger and Commutation Interrupts */
    [15] = isr_tim2,                 /* [15] TIM2 global Interrupt */
    [17] = isr_tim6_dac,             /* [17] TIM6 global and DAC channel underrun error Interrupts */
    [18] = isr_tim7,                 /* [18] TIM7 global Interrupt */
    [20] = isr_tim15,                /* [20] TIM15 global Interrupt */
    [23] = isr_i2c1,                 /* [23] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    [24] = isr_i2c2,                 /* [24] I2C2 Event Interrupt */
    [26] = isr_spi2,                 /* [26] SPI2 global Interrupt */
    [27] = isr_usart1,               /* [27] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    [28] = isr_usart2,               /* [28] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    [29] = isr_usart3_8,             /* [29] USART3 to USART8 global Interrupts */
    [30] = isr_cec_can,              /* [30] CEC and CAN global Interrupts & EXTI Line27 Interrupt */
#endif
};
