/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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

#include <stdint.h>
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32L1 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_tamper_stamp(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_dma1_ch1(void);
WEAK_DEFAULT void isr_dma1_ch2(void);
WEAK_DEFAULT void isr_dma1_ch3(void);
WEAK_DEFAULT void isr_dma1_ch4(void);
WEAK_DEFAULT void isr_dma1_ch5(void);
WEAK_DEFAULT void isr_dma1_ch6(void);
WEAK_DEFAULT void isr_dma1_ch7(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_dac(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_tim9(void);
WEAK_DEFAULT void isr_tim10(void);
WEAK_DEFAULT void isr_tim11(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_usb_fs_wkup(void);
WEAK_DEFAULT void isr_tim6(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_sdio(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_dma2_ch1(void);
WEAK_DEFAULT void isr_dma2_ch2(void);
WEAK_DEFAULT void isr_dma2_ch3(void);
WEAK_DEFAULT void isr_dma2_ch4(void);
WEAK_DEFAULT void isr_dma2_ch5(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_comp_acq(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_wwdg,               /* [0] Window WatchDog Interrupt */
    isr_pvd,                /* [1] PVD through EXTI Line detection Interrupt */
    isr_tamper_stamp,       /* [2] Tamper and Time Stamp through EXTI Line Interrupts */
    isr_rtc_wkup,           /* [3] RTC Wakeup Timer through EXTI Line Interrupt */
    isr_flash,              /* [4] FLASH global Interrupt */
    isr_rcc,                /* [5] RCC global Interrupt */
    isr_exti,               /* [6] EXTI Line0 Interrupt */
    isr_exti,               /* [7] EXTI Line1 Interrupt */
    isr_exti,               /* [8] EXTI Line2 Interrupt */
    isr_exti,               /* [9] EXTI Line3 Interrupt */
    isr_exti,               /* [10] EXTI Line4 Interrupt */
    isr_dma1_ch1,           /* [11] DMA1 Channel 1 global Interrupt */
    isr_dma1_ch2,           /* [12] DMA1 Channel 2 global Interrupt */
    isr_dma1_ch3,           /* [13] DMA1 Channel 3 global Interrupt */
    isr_dma1_ch4,           /* [14] DMA1 Channel 4 global Interrupt */
    isr_dma1_ch5,           /* [15] DMA1 Channel 5 global Interrupt */
    isr_dma1_ch6,           /* [16] DMA1 Channel 6 global Interrupt */
    isr_dma1_ch7,           /* [17] DMA1 Channel 7 global Interrupt */
    isr_adc1,               /* [18] ADC1 global Interrupt */
    isr_usb_hp,             /* [19] USB High Priority Interrupt */
    isr_usb_lp,             /* [20] USB Low Priority Interrupt */
    isr_dac,                /* [21] DAC Interrupt */
    isr_comp,               /* [22] Comparator through EXTI Line Interrupt */
    isr_exti,               /* [23] External Line[9:5] Interrupts */
    isr_lcd,                /* [24] LCD Interrupt */
    isr_tim9,               /* [25] TIM9 global Interrupt */
    isr_tim10,              /* [26] TIM10 global Interrupt */
    isr_tim11,              /* [27] TIM11 global Interrupt */
    isr_tim2,               /* [28] TIM2 global Interrupt */
    isr_tim3,               /* [29] TIM3 global Interrupt */
    isr_tim4,               /* [30] TIM4 global Interrupt */
    isr_i2c1_ev,            /* [31] I2C1 Event Interrupt */
    isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    isr_i2c2_ev,            /* [33] I2C2 Event Interrupt */
    isr_i2c2_er,            /* [34] I2C2 Error Interrupt */
    isr_spi1,               /* [35] SPI1 global Interrupt */
    isr_spi2,               /* [36] SPI2 global Interrupt */
    isr_usart1,             /* [37] USART1 global Interrupt */
    isr_usart2,             /* [38] USART2 global Interrupt */
    isr_usart3,             /* [39] USART3 global Interrupt */
    isr_exti,               /* [40] External Line[15:10] Interrupts */
    isr_rtc_alarm,          /* [41] RTC Alarm through EXTI Line Interrupt */
    isr_usb_fs_wkup,        /* [42] USB FS WakeUp from suspend through EXTI Line Interrupt */
    isr_tim6,               /* [43] TIM6 global Interrupt */
    isr_tim7,               /* [44] TIM7 global Interrupt */
#if defined(CPU_MODEL_STM32L151RC)
    (0UL),                  /* [45] Reserved */
    isr_tim5,               /* [46] TIM5 global Interrupt */
    isr_spi3,               /* [47] SPI3 global Interrupt */
    (0UL),                  /* [48] Reserved */
    (0UL),                  /* [49] Reserved */
    isr_dma2_ch1,           /* [50] DMA2 Channel 1 global Interrupt */
    isr_dma2_ch2,           /* [51] DMA2 Channel 2 global Interrupt */
    isr_dma2_ch3,           /* [52] DMA2 Channel 3 global Interrupt */
    isr_dma2_ch4,           /* [53] DMA2 Channel 4 global Interrupt */
    isr_dma2_ch5,           /* [54] DMA2 Channel 5 global Interrupt */
    isr_aes,                /* [55] AES global Interrupt */
    isr_comp_acq            /* [56] Comparator Channel Acquisition global Interrupt */
#elif defined(CPU_MODEL_STM32L152RE)
    (0UL),                  /* [45] Reserved */
    isr_tim5,               /* [46] TIM5 global Interrupt */
    isr_spi3,               /* [47] SPI3 global Interrupt */
    isr_uart4,              /* [48] UART4 global Interrupt */
    isr_uart5,              /* [49] UART5 global Interrupt */
    isr_dma2_ch1,           /* [50] DMA2 Channel 1 global Interrupt */
    isr_dma2_ch2,           /* [51] DMA2 Channel 2 global Interrupt */
    isr_dma2_ch3,           /* [52] DMA2 Channel 3 global Interrupt */
    isr_dma2_ch4,           /* [53] DMA2 Channel 4 global Interrupt */
    isr_dma2_ch5,           /* [54] DMA2 Channel 5 global Interrupt */
    isr_aes,                /* [55] AES global Interrupt */
    isr_comp_acq            /* [56] Comparator Channel Acquisition global Interrupt */
#endif
};
