/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f3
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

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32F3 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2(void);
WEAK_DEFAULT void isr_dma1_channel3(void);
WEAK_DEFAULT void isr_dma1_channel4(void);
WEAK_DEFAULT void isr_dma1_channel5(void);
WEAK_DEFAULT void isr_dma1_channel6(void);
WEAK_DEFAULT void isr_dma1_channel7(void);
WEAK_DEFAULT void isr_adc1_2(void);
WEAK_DEFAULT void isr_usb_hp_can_tx(void);
WEAK_DEFAULT void isr_usb_lp_can_rx0(void);
WEAK_DEFAULT void isr_can_rx1(void);
WEAK_DEFAULT void isr_can_sce(void);
WEAK_DEFAULT void isr_tim1_brk_tim15(void);
WEAK_DEFAULT void isr_tim1_up_tim16(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim17(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_usbwakeup(void);
WEAK_DEFAULT void isr_tim8_brk(void);
WEAK_DEFAULT void isr_tim8_up(void);
WEAK_DEFAULT void isr_tim8_trg_com(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_adc3(void);
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim7_dac(void);
WEAK_DEFAULT void isr_dma2_channel1(void);
WEAK_DEFAULT void isr_dma2_channel2(void);
WEAK_DEFAULT void isr_dma2_channel3(void);
WEAK_DEFAULT void isr_dma2_channel4(void);
WEAK_DEFAULT void isr_dma2_channel5(void);
WEAK_DEFAULT void isr_adc4(void);
WEAK_DEFAULT void isr_comp1_2_3(void);
WEAK_DEFAULT void isr_comp4_5_6(void);
WEAK_DEFAULT void isr_comp7(void);
WEAK_DEFAULT void isr_hrtim(void);
WEAK_DEFAULT void isr_hrtim_tim_a(void);
WEAK_DEFAULT void isr_hrtim_tim_b(void);
WEAK_DEFAULT void isr_hrtim_tim_c(void);
WEAK_DEFAULT void isr_hrtim_tim_d(void);
WEAK_DEFAULT void isr_hrtim_tim_e(void);
WEAK_DEFAULT void isr_hrtim_flt(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_usbwakeup_rmp(void);
WEAK_DEFAULT void isr_tim20_brk(void);
WEAK_DEFAULT void isr_tim20_up(void);
WEAK_DEFAULT void isr_tim20_trg_com(void);
WEAK_DEFAULT void isr_tim20_cc(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_spi4(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_wwdg,               /* [0] Window WatchDog Interrupt */
    isr_pvd,                /* [1] PVD through EXTI Line detection Interrupt */
    isr_tamp_stamp,         /* [2] Tamper and TimeStamp interrupts through the EXTI line 19 */
    isr_rtc_wkup,           /* [3] RTC Wakeup interrupt through the EXTI line 20 */
    isr_flash,              /* [4] FLASH global Interrupt */
    isr_rcc,                /* [5] RCC global Interrupt */
    isr_exti,               /* [6] EXTI Line0 Interrupt */
    isr_exti,               /* [7] EXTI Line1 Interrupt */
    isr_exti,               /* [8] EXTI Line2 Interrupt and Touch Sense Controller Interrupt */
    isr_exti,               /* [9] EXTI Line3 Interrupt */
    isr_exti,               /* [10] EXTI Line4 Interrupt */
    isr_dma1_channel1,      /* [11] DMA1 Channel 1 Interrupt */
    isr_dma1_channel2,      /* [12] DMA1 Channel 2 Interrupt */
    isr_dma1_channel3,      /* [13] DMA1 Channel 3 Interrupt */
    isr_dma1_channel4,      /* [14] DMA1 Channel 4 Interrupt */
    isr_dma1_channel5,      /* [15] DMA1 Channel 5 Interrupt */
    isr_dma1_channel6,      /* [16] DMA1 Channel 6 Interrupt */
    isr_dma1_channel7,      /* [17] DMA1 Channel 7 Interrupt */
    isr_adc1_2,             /* [18] ADC1 & ADC2 Interrupts */
    isr_usb_hp_can_tx,      /* [19] USB Device High Priority or CAN TX Interrupts */
    isr_usb_lp_can_rx0,     /* [20] USB Device Low Priority or CAN RX0 Interrupts */
    isr_can_rx1,            /* [21] CAN RX1 Interrupt */
    isr_can_sce,            /* [22] CAN SCE Interrupt */
    isr_exti,               /* [23] External Line[9:5] Interrupts */
    isr_tim1_brk_tim15,     /* [24] TIM1 Break and TIM15 Interrupts */
    isr_tim1_up_tim16,      /* [25] TIM1 Update and TIM16 Interrupts */
    isr_tim1_trg_com_tim17, /* [26] TIM1 Trigger and Commutation and TIM17 Interrupt */
    isr_tim1_cc,            /* [27] TIM1 Capture Compare Interrupt */
    isr_tim2,               /* [28] TIM2 global Interrupt */
#if defined(CPU_LINE_STM32F302x8)
    (0UL),                  /* [29] Reserved */
    (0UL),                  /* [30] Reserved */
    isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    isr_i2c2_ev,            /* [33] I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup) */
    isr_i2c2_er,            /* [34] I2C2 Error Interrupt */
    (0UL),                  /* [35] Reserved */
    isr_spi2,               /* [36] SPI2 global Interrupt */
    isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    isr_exti,               /* [40] External Line[15:10] Interrupts */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    isr_usbwakeup,          /* [42] USB Wakeup Interrupt */
    (0UL),                  /* [43] Reserved */
    (0UL),                  /* [44] Reserved */
    (0UL),                  /* [45] Reserved */
    (0UL),                  /* [46] Reserved */
    (0UL),                  /* [47] Reserved */
    (0UL),                  /* [48] Reserved */
    (0UL),                  /* [49] Reserved */
    (0UL),                  /* [50] Reserved */
    isr_spi3,               /* [51] SPI3 global Interrupt */
    (0UL),                  /* [52] Reserved */
    (0UL),                  /* [53] Reserved */
    isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    (0UL),                  /* [55] Reserved */
    (0UL),                  /* [56] Reserved */
    (0UL),                  /* [57] Reserved */
    (0UL),                  /* [58] Reserved */
    (0UL),                  /* [59] Reserved */
    (0UL),                  /* [60] Reserved */
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    (0UL),                  /* [66] Reserved */
    (0UL),                  /* [67] Reserved */
    (0UL),                  /* [68] Reserved */
    (0UL),                  /* [69] Reserved */
    (0UL),                  /* [70] Reserved */
    (0UL),                  /* [71] Reserved */
    isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    isr_i2c3_er,            /* [73] I2C3 error interrupt */
    isr_usb_hp,             /* [74] USB High Priority global Interrupt remap */
    isr_usb_lp,             /* [75] USB Low Priority global Interrupt  remap */
    isr_usbwakeup_rmp,      /* [76] USB Wakeup Interrupt remap */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu                 /* [81] Floating point Interrupt */
#elif defined(CPU_LINE_STM32F303x8)
    isr_tim3,               /* [29] TIM3 global Interrupt */
    (0UL),                  /* [30] Reserved */
    isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (0UL),                  /* [33] Reserved */
    (0UL),                  /* [34] Reserved */
    isr_spi1,               /* [35] SPI1 global Interrupt */
    (0UL),                  /* [36] Reserved */
    isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    isr_exti,               /* [40] External Line[15:10] Interrupts */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    (0UL),                  /* [42] Reserved */
    (0UL),                  /* [43] Reserved */
    (0UL),                  /* [44] Reserved */
    (0UL),                  /* [45] Reserved */
    (0UL),                  /* [46] Reserved */
    (0UL),                  /* [47] Reserved */
    (0UL),                  /* [48] Reserved */
    (0UL),                  /* [49] Reserved */
    (0UL),                  /* [50] Reserved */
    (0UL),                  /* [51] Reserved */
    (0UL),                  /* [52] Reserved */
    (0UL),                  /* [53] Reserved */
    isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error interrupts */
    isr_tim7_dac,           /* [55] TIM7 global and DAC2 channel1 underrun error Interrupt */
    (0UL),                  /* [56] Reserved */
    (0UL),                  /* [57] Reserved */
    (0UL),                  /* [58] Reserved */
    (0UL),                  /* [59] Reserved */
    (0UL),                  /* [60] Reserved */
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    (0UL),                  /* [66] Reserved */
    (0UL),                  /* [67] Reserved */
    (0UL),                  /* [68] Reserved */
    (0UL),                  /* [69] Reserved */
    (0UL),                  /* [70] Reserved */
    (0UL),                  /* [71] Reserved */
    (0UL),                  /* [72] Reserved */
    (0UL),                  /* [73] Reserved */
    (0UL),                  /* [74] Reserved */
    (0UL),                  /* [75] Reserved */
    (0UL),                  /* [76] Reserved */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu                 /* [81] Floating point Interrupt */
#elif (CPU_LINE_STM32F334x8)
    isr_tim3,               /* [29] TIM3 global Interrupt */
    isr_tim4,               /* [30] TIM4 global Interrupt */
    isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (0UL),                  /* [33] Reserved */
    (0UL),                  /* [34] Reserved */
    isr_spi1,               /* [35] SPI1 global Interrupt */
    (0UL),                  /* [36] Reserved */
    isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    isr_exti,               /* [40] External Line[15:10] Interrupts */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    (0UL),                  /* [42] Reserved */
    (0UL),                  /* [43] Reserved */
    (0UL),                  /* [44] Reserved */
    (0UL),                  /* [45] Reserved */
    (0UL),                  /* [46] Reserved */
    (0UL),                  /* [47] Reserved */
    (0UL),                  /* [48] Reserved */
    (0UL),                  /* [49] Reserved */
    (0UL),                  /* [50] Reserved */
    (0UL),                  /* [51] Reserved */
    (0UL),                  /* [52] Reserved */
    (0UL),                  /* [53] Reserved */
    isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    isr_tim7_dac,           /* [55] TIM7 global and DAC2 channel1 underrun error Interrupt */
    (0UL),                  /* [56] Reserved */
    (0UL),                  /* [57] Reserved */
    (0UL),                  /* [58] Reserved */
    (0UL),                  /* [59] Reserved */
    (0UL),                  /* [60] Reserved */
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    (0UL),                  /* [66] Reserved */
    isr_hrtim,              /* [67] HRTIM Master Timer global Interrupts */
    isr_hrtim_tim_a,        /* [68] HRTIM Timer A global Interrupt */
    isr_hrtim_tim_b,        /* [69] HRTIM Timer B global Interrupt */
    isr_hrtim_tim_c,        /* [70] HRTIM Timer C global Interrupt */
    isr_hrtim_tim_d,        /* [71] HRTIM Timer D global Interrupt */
    isr_hrtim_tim_e,        /* [72] HRTIM Timer E global Interrupt */
    isr_hrtim_flt,          /* [73] HRTIM Fault global Interrupt */
    (0UL),                  /* [74] Reserved */
    (0UL),                  /* [75] Reserved */
    (0UL),                  /* [76] Reserved */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu                 /* [81] Floating point Interrupt */
#else /* CPU_LINE_STM32F303xC, CPU_LINE_STM32F303xE */
    isr_tim3,               /* [29] TIM3 global Interrupt */
    isr_tim4,               /* [30] TIM4 global Interrupt */
    isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    isr_i2c2_ev,            /* [33] I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup) */
    isr_i2c2_er,            /* [34] I2C2 Error Interrupt */
    isr_spi1,               /* [35] SPI1 global Interrupt */
    isr_spi2,               /* [36] SPI2 global Interrupt */
    isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    isr_exti,               /* [40] External Line[15:10] Interrupts */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    isr_usbwakeup,          /* [42] USB Wakeup Interrupt */
    isr_tim8_brk,           /* [43] TIM8 Break Interrupt */
    isr_tim8_up,            /* [44] TIM8 Update Interrupt */
    isr_tim8_trg_com,       /* [45] TIM8 Trigger and Commutation Interrupt */
    isr_tim8_cc,            /* [46] TIM8 Capture Compare Interrupt */
    isr_adc3,               /* [47] ADC3 global Interrupt */
#if defined(CPU_LINE_STM32F303xC)
    (0UL),                  /* [48] Reserved */
#else
    isr_fmc,                /* [48] FMC global Interrupt */
#endif
    (0UL),                  /* [49] Reserved */
    (0UL),                  /* [50] Reserved */
    isr_spi3,               /* [51] SPI3 global Interrupt */
    isr_uart4,              /* [52] UART4 global Interrupt & EXTI Line34 Interrupt (UART4 wakeup) */
    isr_uart5,              /* [53] UART5 global Interrupt & EXTI Line35 Interrupt (UART5 wakeup) */
    isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    isr_tim7,               /* [55] TIM7 global Interrupt */
    isr_dma2_channel1,      /* [56] DMA2 Channel 1 global Interrupt */
    isr_dma2_channel2,      /* [57] DMA2 Channel 2 global Interrupt */
    isr_dma2_channel3,      /* [58] DMA2 Channel 3 global Interrupt */
    isr_dma2_channel4,      /* [59] DMA2 Channel 4 global Interrupt */
    isr_dma2_channel5,      /* [60] DMA2 Channel 5 global Interrupt */
    isr_adc4,               /* [61] ADC4  global Interrupt */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    isr_comp7,              /* [66] COMP7 global Interrupt via EXTI Line33 */
    (0UL),                  /* [67] Reserved */
    (0UL),                  /* [68] Reserved */
    (0UL),                  /* [69] Reserved */
    (0UL),                  /* [70] Reserved */
    (0UL),                  /* [71] Reserved */
#if defined(CPU_LINE_STM32F303xC)
    (0UL),                  /* [72] Reserved */
    (0UL),                  /* [73] Reserved */
    isr_usb_hp,             /* [74] USB High Priority global Interrupt remap */
    isr_usb_lp,             /* [75] USB Low Priority global Interrupt  remap */
    isr_usbwakeup_rmp,      /* [76] USB Wakeup Interrupt remap */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu                 /* [81] Floating point Interrupt */
#else /* CPU_LINE_STM32F303xE */
    isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    isr_i2c3_er,            /* [73] I2C3 error interrupt */
    isr_usb_hp,             /* [74] USB High Priority global Interrupt remap */
    isr_usb_lp,             /* [75] USB Low Priority global Interrupt  remap */
    isr_usbwakeup_rmp,      /* [76] USB Wakeup Interrupt remap */
    isr_tim20_brk,          /* [77] TIM20 Break Interrupt */
    isr_tim20_up,           /* [78] TIM20 Update Interrupt */
    isr_tim20_trg_com,      /* [79] TIM20 Trigger and Commutation Interrupt */
    isr_tim8_cc,            /* [80] TIM20 Capture Compare Interrupt */
    isr_fpu,                /* [81] Floating point Interrupt */
    (0UL),                  /* [82] Reserved */
    (0UL),                  /* [83] Reserved */
    isr_spi4,               /* [84] SPI4 global Interrupt */
#endif
#endif
};
