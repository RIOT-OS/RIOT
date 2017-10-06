/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32L1 specific interrupt vectors */
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_comp_acq(void);
WEAK_DEFAULT void isr_dac(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2(void);
WEAK_DEFAULT void isr_dma1_channel3(void);
WEAK_DEFAULT void isr_dma1_channel4(void);
WEAK_DEFAULT void isr_dma1_channel5(void);
WEAK_DEFAULT void isr_dma1_channel6(void);
WEAK_DEFAULT void isr_dma1_channel7(void);
WEAK_DEFAULT void isr_dma2_channel1(void);
WEAK_DEFAULT void isr_dma2_channel2(void);
WEAK_DEFAULT void isr_dma2_channel3(void);
WEAK_DEFAULT void isr_dma2_channel4(void);
WEAK_DEFAULT void isr_dma2_channel5(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_tamper_stamp(void);
WEAK_DEFAULT void isr_tim10(void);
WEAK_DEFAULT void isr_tim11(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_tim6(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim9(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_usb_fs_wkup(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [ 0] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [ 1] = isr_pvd,                  /* [ 1] PVD through EXTI Line detection Interrupt */
    [ 2] = isr_tamper_stamp,         /* [ 2] Tamper and TimeStamp interrupts through the EXTI line */
    [ 3] = isr_rtc_wkup,             /* [ 3] RTC Wakeup Timer through EXTI Line Interrupt */
    [ 4] = isr_flash,                /* [ 4] FLASH global Interrupt */
    [ 5] = isr_rcc,                  /* [ 5] RCC global Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line0 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line1 Interrupt */
    [ 8] = isr_exti,                 /* [ 8] EXTI Line2 Interrupt */
    [ 9] = isr_exti,                 /* [ 9] EXTI Line3 Interrupt */
    [10] = isr_exti,                 /* [10] EXTI Line4 Interrupt */
    [11] = isr_dma1_channel1,        /* [11] DMA1 Channel 1 global Interrupt */
    [12] = isr_dma1_channel2,        /* [12] DMA1 Channel 2 global Interrupt */
    [13] = isr_dma1_channel3,        /* [13] DMA1 Channel 3 global Interrupt */
    [14] = isr_dma1_channel4,        /* [14] DMA1 Channel 4 global Interrupt */
    [15] = isr_dma1_channel5,        /* [15] DMA1 Channel 5 global Interrupt */
    [16] = isr_dma1_channel6,        /* [16] DMA1 Channel 6 global Interrupt */
    [17] = isr_dma1_channel7,        /* [17] DMA1 Channel 7 global Interrupt */
    [18] = isr_adc1,                 /* [18] ADC1 global Interrupt */
    [19] = isr_usb_hp,               /* [19] USB High Priority Interrupt */
    [20] = isr_usb_lp,               /* [20] USB Low Priority Interrupt */
    [21] = isr_dac,                  /* [21] DAC Interrupt */
    [22] = isr_comp,                 /* [22] Comparator through EXTI Line Interrupt */
    [23] = isr_exti,                 /* [23] External Line[9:5] Interrupts */
    [25] = isr_tim9,                 /* [25] TIM9 global Interrupt */
    [26] = isr_tim10,                /* [26] TIM10 global Interrupt */
    [27] = isr_tim11,                /* [27] TIM11 global Interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [31] = isr_i2c1_ev,              /* [31] I2C1 Event Interrupt */
    [32] = isr_i2c1_er,              /* [32] I2C1 Error Interrupt */
    [33] = isr_i2c2_ev,              /* [33] I2C2 Event Interrupt */
    [34] = isr_i2c2_er,              /* [34] I2C2 Error Interrupt */
    [35] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [36] = isr_spi2,                 /* [36] SPI2 global Interrupt */
    [37] = isr_usart1,               /* [37] USART1 global Interrupt */
    [38] = isr_usart2,               /* [38] USART2 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [40] = isr_exti,                 /* [40] External Line[15:10] Interrupts */
    [41] = isr_rtc_alarm,            /* [41] RTC Alarm through EXTI Line Interrupt */
    [42] = isr_usb_fs_wkup,          /* [42] USB FS WakeUp from suspend through EXTI Line Interrupt */
    [43] = isr_tim6,                 /* [43] TIM6 global Interrupt */
    [44] = isr_tim7,                 /* [44] TIM7 global Interrupt */

#if defined(CPU_MODEL_STM32L151RC)
    [46] = isr_tim5,                 /* [46] TIM5 global Interrupt */
    [47] = isr_spi3,                 /* [47] SPI3 global Interrupt */
    [50] = isr_dma2_channel1,        /* [50] DMA2 Channel 1 global Interrupt */
    [51] = isr_dma2_channel2,        /* [51] DMA2 Channel 2 global Interrupt */
    [52] = isr_dma2_channel3,        /* [52] DMA2 Channel 3 global Interrupt */
    [53] = isr_dma2_channel4,        /* [53] DMA2 Channel 4 global Interrupt */
    [54] = isr_dma2_channel5,        /* [54] DMA2 Channel 5 global Interrupt */
    [56] = isr_comp_acq,             /* [56] Comparator Channel Acquisition global Interrupt */
#elif defined(CPU_MODEL_STM32L152RE)
    [24] = isr_lcd,                  /* [24] LCD Interrupt */
    [46] = isr_tim5,                 /* [46] TIM5 global Interrupt */
    [47] = isr_spi3,                 /* [47] SPI3 global Interrupt */
    [48] = isr_uart4,                /* [48] UART4 global Interrupt */
    [49] = isr_uart5,                /* [49] UART5 global Interrupt */
    [50] = isr_dma2_channel1,        /* [50] DMA2 Channel 1 global Interrupt */
    [51] = isr_dma2_channel2,        /* [51] DMA2 Channel 2 global Interrupt */
    [52] = isr_dma2_channel3,        /* [52] DMA2 Channel 3 global Interrupt */
    [53] = isr_dma2_channel4,        /* [53] DMA2 Channel 4 global Interrupt */
    [54] = isr_dma2_channel5,        /* [54] DMA2 Channel 5 global Interrupt */
    [56] = isr_comp_acq,             /* [56] Comparator Channel Acquisition global Interrupt */
#endif
};
