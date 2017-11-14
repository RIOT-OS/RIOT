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
    [WWDG_IRQn               ] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [PVD_IRQn                ] = isr_pvd,                  /* [ 1] PVD through EXTI Line detection Interrupt */
    [TAMPER_STAMP_IRQn       ] = isr_tamper_stamp,         /* [ 2] Tamper and TimeStamp interrupts through the EXTI line */
    [RTC_WKUP_IRQn           ] = isr_rtc_wkup,             /* [ 3] RTC Wakeup Timer through EXTI Line Interrupt */
    [FLASH_IRQn              ] = isr_flash,                /* [ 4] FLASH global Interrupt */
    [RCC_IRQn                ] = isr_rcc,                  /* [ 5] RCC global Interrupt */
    [EXTI0_IRQn              ] = isr_exti,                 /* [ 6] EXTI Line0 Interrupt */
    [EXTI1_IRQn              ] = isr_exti,                 /* [ 7] EXTI Line1 Interrupt */
    [EXTI2_IRQn              ] = isr_exti,                 /* [ 8] EXTI Line2 Interrupt */
    [EXTI3_IRQn              ] = isr_exti,                 /* [ 9] EXTI Line3 Interrupt */
    [EXTI4_IRQn              ] = isr_exti,                 /* [10] EXTI Line4 Interrupt */
    [DMA1_Channel1_IRQn      ] = isr_dma1_channel1,        /* [11] DMA1 Channel 1 global Interrupt */
    [DMA1_Channel2_IRQn      ] = isr_dma1_channel2,        /* [12] DMA1 Channel 2 global Interrupt */
    [DMA1_Channel3_IRQn      ] = isr_dma1_channel3,        /* [13] DMA1 Channel 3 global Interrupt */
    [DMA1_Channel4_IRQn      ] = isr_dma1_channel4,        /* [14] DMA1 Channel 4 global Interrupt */
    [DMA1_Channel5_IRQn      ] = isr_dma1_channel5,        /* [15] DMA1 Channel 5 global Interrupt */
    [DMA1_Channel6_IRQn      ] = isr_dma1_channel6,        /* [16] DMA1 Channel 6 global Interrupt */
    [DMA1_Channel7_IRQn      ] = isr_dma1_channel7,        /* [17] DMA1 Channel 7 global Interrupt */
    [ADC1_IRQn               ] = isr_adc1,                 /* [18] ADC1 global Interrupt */
    [USB_HP_IRQn             ] = isr_usb_hp,               /* [19] USB High Priority Interrupt */
    [USB_LP_IRQn             ] = isr_usb_lp,               /* [20] USB Low Priority Interrupt */
    [DAC_IRQn                ] = isr_dac,                  /* [21] DAC Interrupt */
    [COMP_IRQn               ] = isr_comp,                 /* [22] Comparator through EXTI Line Interrupt */
    [EXTI9_5_IRQn            ] = isr_exti,                 /* [23] External Line[9:5] Interrupts */
    [TIM9_IRQn               ] = isr_tim9,                 /* [25] TIM9 global Interrupt */
    [TIM10_IRQn              ] = isr_tim10,                /* [26] TIM10 global Interrupt */
    [TIM11_IRQn              ] = isr_tim11,                /* [27] TIM11 global Interrupt */
    [TIM2_IRQn               ] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [TIM3_IRQn               ] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [TIM4_IRQn               ] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [I2C1_EV_IRQn            ] = isr_i2c1_ev,              /* [31] I2C1 Event Interrupt */
    [I2C1_ER_IRQn            ] = isr_i2c1_er,              /* [32] I2C1 Error Interrupt */
    [I2C2_EV_IRQn            ] = isr_i2c2_ev,              /* [33] I2C2 Event Interrupt */
    [I2C2_ER_IRQn            ] = isr_i2c2_er,              /* [34] I2C2 Error Interrupt */
    [SPI1_IRQn               ] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [SPI2_IRQn               ] = isr_spi2,                 /* [36] SPI2 global Interrupt */
    [USART1_IRQn             ] = isr_usart1,               /* [37] USART1 global Interrupt */
    [USART2_IRQn             ] = isr_usart2,               /* [38] USART2 global Interrupt */
    [USART3_IRQn             ] = isr_usart3,               /* [39] USART3 global Interrupt */
    [EXTI15_10_IRQn          ] = isr_exti,                 /* [40] External Line[15:10] Interrupts */
    [RTC_Alarm_IRQn          ] = isr_rtc_alarm,            /* [41] RTC Alarm through EXTI Line Interrupt */
    [USB_FS_WKUP_IRQn        ] = isr_usb_fs_wkup,          /* [42] USB FS WakeUp from suspend through EXTI Line Interrupt */
    [TIM6_IRQn               ] = isr_tim6,                 /* [43] TIM6 global Interrupt */
    [TIM7_IRQn               ] = isr_tim7,                 /* [44] TIM7 global Interrupt */

#if defined(CPU_MODEL_STM32L151RC)
    [TIM5_IRQn               ] = isr_tim5,                 /* [46] TIM5 global Interrupt */
    [SPI3_IRQn               ] = isr_spi3,                 /* [47] SPI3 global Interrupt */
    [DMA2_Channel1_IRQn      ] = isr_dma2_channel1,        /* [50] DMA2 Channel 1 global Interrupt */
    [DMA2_Channel2_IRQn      ] = isr_dma2_channel2,        /* [51] DMA2 Channel 2 global Interrupt */
    [DMA2_Channel3_IRQn      ] = isr_dma2_channel3,        /* [52] DMA2 Channel 3 global Interrupt */
    [DMA2_Channel4_IRQn      ] = isr_dma2_channel4,        /* [53] DMA2 Channel 4 global Interrupt */
    [DMA2_Channel5_IRQn      ] = isr_dma2_channel5,        /* [54] DMA2 Channel 5 global Interrupt */
    [COMP_ACQ_IRQn           ] = isr_comp_acq,             /* [56] Comparator Channel Acquisition global Interrupt */
#elif defined(CPU_MODEL_STM32L152RE)
    [LCD_IRQn                ] = isr_lcd,                  /* [24] LCD Interrupt */
    [TIM5_IRQn               ] = isr_tim5,                 /* [46] TIM5 global Interrupt */
    [SPI3_IRQn               ] = isr_spi3,                 /* [47] SPI3 global Interrupt */
    [UART4_IRQn              ] = isr_uart4,                /* [48] UART4 global Interrupt */
    [UART5_IRQn              ] = isr_uart5,                /* [49] UART5 global Interrupt */
    [DMA2_Channel1_IRQn      ] = isr_dma2_channel1,        /* [50] DMA2 Channel 1 global Interrupt */
    [DMA2_Channel2_IRQn      ] = isr_dma2_channel2,        /* [51] DMA2 Channel 2 global Interrupt */
    [DMA2_Channel3_IRQn      ] = isr_dma2_channel3,        /* [52] DMA2 Channel 3 global Interrupt */
    [DMA2_Channel4_IRQn      ] = isr_dma2_channel4,        /* [53] DMA2 Channel 4 global Interrupt */
    [DMA2_Channel5_IRQn      ] = isr_dma2_channel5,        /* [54] DMA2 Channel 5 global Interrupt */
    [COMP_ACQ_IRQn           ] = isr_comp_acq,             /* [56] Comparator Channel Acquisition global Interrupt */
#endif
};
