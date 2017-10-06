/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
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

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_adc1_2(void);
WEAK_DEFAULT void isr_adc3(void);
WEAK_DEFAULT void isr_adc4(void);
WEAK_DEFAULT void isr_can_rx0(void);
WEAK_DEFAULT void isr_can_rx1(void);
WEAK_DEFAULT void isr_can_sce(void);
WEAK_DEFAULT void isr_can_tx(void);
WEAK_DEFAULT void isr_comp1_2_3(void);
WEAK_DEFAULT void isr_comp2(void);
WEAK_DEFAULT void isr_comp4_5_6(void);
WEAK_DEFAULT void isr_comp4_6(void);
WEAK_DEFAULT void isr_comp7(void);
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
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_hrtim1_flt(void);
WEAK_DEFAULT void isr_hrtim1_master(void);
WEAK_DEFAULT void isr_hrtim1_tima(void);
WEAK_DEFAULT void isr_hrtim1_timb(void);
WEAK_DEFAULT void isr_hrtim1_timc(void);
WEAK_DEFAULT void isr_hrtim1_timd(void);
WEAK_DEFAULT void isr_hrtim1_time(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_spi4(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
WEAK_DEFAULT void isr_tim1_brk_tim15(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim17(void);
WEAK_DEFAULT void isr_tim1_up_tim16(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim20_brk(void);
WEAK_DEFAULT void isr_tim20_cc(void);
WEAK_DEFAULT void isr_tim20_trg_com(void);
WEAK_DEFAULT void isr_tim20_up(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim6_dac1(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim7_dac2(void);
WEAK_DEFAULT void isr_tim8_brk(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_tim8_trg_com(void);
WEAK_DEFAULT void isr_tim8_up(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_hp_can_tx(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_usb_lp_can_rx0(void);
WEAK_DEFAULT void isr_usbwakeup(void);
WEAK_DEFAULT void isr_usbwakeup_rmp(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [WWDG_IRQn               ] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [PVD_IRQn                ] = isr_pvd,                  /* [ 1] PVD through EXTI Line detection Interrupt */
    [TAMP_STAMP_IRQn         ] = isr_tamp_stamp,           /* [ 2] Tamper and TimeStamp interrupts through the EXTI line 19 */
    [RTC_WKUP_IRQn           ] = isr_rtc_wkup,             /* [ 3] RTC Wakeup interrupt through the EXTI line 20 */
    [FLASH_IRQn              ] = isr_flash,                /* [ 4] FLASH global Interrupt */
    [RCC_IRQn                ] = isr_rcc,                  /* [ 5] RCC global Interrupt */
    [EXTI0_IRQn              ] = isr_exti,                 /* [ 6] EXTI Line0 Interrupt */
    [EXTI1_IRQn              ] = isr_exti,                 /* [ 7] EXTI Line1 Interrupt */
    [EXTI2_TSC_IRQn          ] = isr_exti,                 /* [ 8] EXTI Line2 Interrupt and Touch Sense Controller Interrupt */
    [EXTI3_IRQn              ] = isr_exti,                 /* [ 9] EXTI Line3 Interrupt */
    [EXTI4_IRQn              ] = isr_exti,                 /* [10] EXTI Line4 Interrupt */
    [DMA1_Channel1_IRQn      ] = isr_dma1_channel1,        /* [11] DMA1 Channel 1 Interrupt */
    [DMA1_Channel2_IRQn      ] = isr_dma1_channel2,        /* [12] DMA1 Channel 2 Interrupt */
    [DMA1_Channel3_IRQn      ] = isr_dma1_channel3,        /* [13] DMA1 Channel 3 Interrupt */
    [DMA1_Channel4_IRQn      ] = isr_dma1_channel4,        /* [14] DMA1 Channel 4 Interrupt */
    [DMA1_Channel5_IRQn      ] = isr_dma1_channel5,        /* [15] DMA1 Channel 5 Interrupt */
    [DMA1_Channel6_IRQn      ] = isr_dma1_channel6,        /* [16] DMA1 Channel 6 Interrupt */
    [DMA1_Channel7_IRQn      ] = isr_dma1_channel7,        /* [17] DMA1 Channel 7 Interrupt */
    [CAN_RX1_IRQn            ] = isr_can_rx1,              /* [21] CAN RX1 Interrupt */
    [CAN_SCE_IRQn            ] = isr_can_sce,              /* [22] CAN SCE Interrupt */
    [EXTI9_5_IRQn            ] = isr_exti,                 /* [23] External Line[9:5] Interrupts */
    [TIM1_BRK_TIM15_IRQn     ] = isr_tim1_brk_tim15,       /* [24] TIM1 Break and TIM15 Interrupts */
    [TIM1_UP_TIM16_IRQn      ] = isr_tim1_up_tim16,        /* [25] TIM1 Update and TIM16 Interrupts */
    [TIM1_TRG_COM_TIM17_IRQn ] = isr_tim1_trg_com_tim17,   /* [26] TIM1 Trigger and Commutation and TIM17 Interrupt */
    [TIM1_CC_IRQn            ] = isr_tim1_cc,              /* [27] TIM1 Capture Compare Interrupt */
    [TIM2_IRQn               ] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [I2C1_EV_IRQn            ] = isr_i2c1_ev,              /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    [I2C1_ER_IRQn            ] = isr_i2c1_er,              /* [32] I2C1 Error Interrupt */
    [USART1_IRQn             ] = isr_usart1,               /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    [USART2_IRQn             ] = isr_usart2,               /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    [USART3_IRQn             ] = isr_usart3,               /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    [EXTI15_10_IRQn          ] = isr_exti,                 /* [40] External Line[15:10] Interrupts */
    [RTC_Alarm_IRQn          ] = isr_rtc_alarm,            /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    [FPU_IRQn                ] = isr_fpu,                  /* [81] Floating point Interrupt */

#if defined(CPU_MODEL_STM32F302R8)
    [ADC1_IRQn               ] = isr_adc1,                 /* [18] ADC1 Interrupts */
    [USB_HP_CAN_TX_IRQn      ] = isr_usb_hp_can_tx,        /* [19] USB Device High Priority or CAN TX Interrupts */
    [USB_LP_CAN_RX0_IRQn     ] = isr_usb_lp_can_rx0,       /* [20] USB Device Low Priority or CAN RX0 Interrupts */
    [I2C2_EV_IRQn            ] = isr_i2c2_ev,              /* [33] I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup) */
    [I2C2_ER_IRQn            ] = isr_i2c2_er,              /* [34] I2C2 Error Interrupt */
    [SPI2_IRQn               ] = isr_spi2,                 /* [36] SPI2 global Interrupt */
    [USBWakeUp_IRQn          ] = isr_usbwakeup,            /* [42] USB Wakeup Interrupt */
    [SPI3_IRQn               ] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [TIM6_DAC_IRQn           ] = isr_tim6_dac,             /* [54] TIM6 global and DAC underrun error Interrupt */
    [COMP2_IRQn              ] = isr_comp2,                /* [64] COMP2 global Interrupt via EXTI Line22 */
    [COMP4_6_IRQn            ] = isr_comp4_6,              /* [65] COMP4 and COMP6 global Interrupt via EXTI Line30 and 32 */
    [I2C3_EV_IRQn            ] = isr_i2c3_ev,              /* [72] I2C3 Event Interrupt & EXTI Line27 Interrupt (I2C3 wakeup) */
    [I2C3_ER_IRQn            ] = isr_i2c3_er,              /* [73] I2C3 Error Interrupt */
    [USB_HP_IRQn             ] = isr_usb_hp,               /* [74] USB High Priority global Interrupt */
    [USB_LP_IRQn             ] = isr_usb_lp,               /* [75] USB Low Priority global Interrupt */
    [USBWakeUp_RMP_IRQn      ] = isr_usbwakeup_rmp,        /* [76] USB Wakeup Interrupt remap */
#elif defined(CPU_MODEL_STM32F303K8)
    [ADC1_2_IRQn             ] = isr_adc1_2,               /* [18] ADC1 & ADC2 Interrupts */
    [CAN_TX_IRQn             ] = isr_can_tx,               /* [19] CAN TX Interrupt */
    [CAN_RX0_IRQn            ] = isr_can_rx0,              /* [20] CAN RX0 Interrupt */
    [TIM3_IRQn               ] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [SPI1_IRQn               ] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [COMP2_IRQn              ] = isr_comp2,                /* [64] COMP2 global Interrupt via EXTI Line22 */
    [TIM6_DAC1_IRQn          ] = isr_tim6_dac1,            /* [54] TIM6 global and DAC1 underrun error Interrupts */
    [TIM7_DAC2_IRQn          ] = isr_tim7_dac2,            /* [55] TIM7 global and DAC2 channel1 underrun error Interrupt */
    [COMP4_6_IRQn            ] = isr_comp4_6,              /* [65] COMP4 and COMP6 global Interrupt via EXTI Line30 and 32 */
#elif defined(CPU_MODEL_STM32F303RE) || defined(CPU_MODEL_STM32F303ZE)
    [ADC1_2_IRQn             ] = isr_adc1_2,               /* [18] ADC1 & ADC2 Interrupts */
    [USB_HP_CAN_TX_IRQn      ] = isr_usb_hp_can_tx,        /* [19] USB Device High Priority or CAN TX Interrupts */
    [USB_LP_CAN_RX0_IRQn     ] = isr_usb_lp_can_rx0,       /* [20] USB Device Low Priority or CAN RX0 Interrupts */
    [TIM3_IRQn               ] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [TIM4_IRQn               ] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [I2C2_EV_IRQn            ] = isr_i2c2_ev,              /* [33] I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup) */
    [I2C2_ER_IRQn            ] = isr_i2c2_er,              /* [34] I2C2 Error Interrupt */
    [SPI1_IRQn               ] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [SPI2_IRQn               ] = isr_spi2,                 /* [36] SPI2 global Interrupt */
    [USBWakeUp_IRQn          ] = isr_usbwakeup,            /* [42] USB Wakeup Interrupt */
    [TIM8_BRK_IRQn           ] = isr_tim8_brk,             /* [43] TIM8 Break Interrupt */
    [TIM8_UP_IRQn            ] = isr_tim8_up,              /* [44] TIM8 Update Interrupt */
    [TIM8_TRG_COM_IRQn       ] = isr_tim8_trg_com,         /* [45] TIM8 Trigger and Commutation Interrupt */
    [TIM8_CC_IRQn            ] = isr_tim8_cc,              /* [46] TIM8 Capture Compare Interrupt */
    [ADC3_IRQn               ] = isr_adc3,                 /* [47] ADC3 global Interrupt */
    [FMC_IRQn                ] = isr_fmc,                  /* [48] FMC global Interrupt */
    [SPI3_IRQn               ] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [UART4_IRQn              ] = isr_uart4,                /* [52] UART4 global Interrupt & EXTI Line34 Interrupt (UART4 wakeup) */
    [UART5_IRQn              ] = isr_uart5,                /* [53] UART5 global Interrupt & EXTI Line35 Interrupt (UART5 wakeup) */
    [TIM6_DAC_IRQn           ] = isr_tim6_dac,             /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    [TIM7_IRQn               ] = isr_tim7,                 /* [55] TIM7 global Interrupt */
    [DMA2_Channel1_IRQn      ] = isr_dma2_channel1,        /* [56] DMA2 Channel 1 global Interrupt */
    [DMA2_Channel2_IRQn      ] = isr_dma2_channel2,        /* [57] DMA2 Channel 2 global Interrupt */
    [DMA2_Channel3_IRQn      ] = isr_dma2_channel3,        /* [58] DMA2 Channel 3 global Interrupt */
    [DMA2_Channel4_IRQn      ] = isr_dma2_channel4,        /* [59] DMA2 Channel 4 global Interrupt */
    [DMA2_Channel5_IRQn      ] = isr_dma2_channel5,        /* [60] DMA2 Channel 5 global Interrupt */
    [ADC4_IRQn               ] = isr_adc4,                 /* [61] ADC4  global Interrupt */
    [COMP1_2_3_IRQn          ] = isr_comp1_2_3,            /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    [COMP4_5_6_IRQn          ] = isr_comp4_5_6,            /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    [COMP7_IRQn              ] = isr_comp7,                /* [66] COMP7 global Interrupt via EXTI Line33 */
    [I2C3_EV_IRQn            ] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [I2C3_ER_IRQn            ] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [USB_HP_IRQn             ] = isr_usb_hp,               /* [74] USB High Priority global Interrupt remap */
    [USB_LP_IRQn             ] = isr_usb_lp,               /* [75] USB Low Priority global Interrupt  remap */
    [USBWakeUp_RMP_IRQn      ] = isr_usbwakeup_rmp,        /* [76] USB Wakeup Interrupt remap */
    [TIM20_BRK_IRQn          ] = isr_tim20_brk,            /* [77] TIM20 Break Interrupt */
    [TIM20_UP_IRQn           ] = isr_tim20_up,             /* [78] TIM20 Update Interrupt */
    [TIM20_TRG_COM_IRQn      ] = isr_tim20_trg_com,        /* [79] TIM20 Trigger and Commutation Interrupt */
    [TIM20_CC_IRQn           ] = isr_tim20_cc,             /* [80] TIM20 Capture Compare Interrupt */
    [SPI4_IRQn               ] = isr_spi4,                 /* [84] SPI4 global Interrupt */
#elif defined(CPU_MODEL_STM32F303VC)
    [ADC1_2_IRQn             ] = isr_adc1_2,               /* [18] ADC1 & ADC2 Interrupts */
    [USB_HP_CAN_TX_IRQn      ] = isr_usb_hp_can_tx,        /* [19] USB Device High Priority or CAN TX Interrupts */
    [USB_LP_CAN_RX0_IRQn     ] = isr_usb_lp_can_rx0,       /* [20] USB Device Low Priority or CAN RX0 Interrupts */
    [TIM3_IRQn               ] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [TIM4_IRQn               ] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [I2C2_EV_IRQn            ] = isr_i2c2_ev,              /* [33] I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup) */
    [I2C2_ER_IRQn            ] = isr_i2c2_er,              /* [34] I2C2 Error Interrupt */
    [SPI1_IRQn               ] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [SPI2_IRQn               ] = isr_spi2,                 /* [36] SPI2 global Interrupt */
    [USBWakeUp_IRQn          ] = isr_usbwakeup,            /* [42] USB Wakeup Interrupt */
    [TIM8_BRK_IRQn           ] = isr_tim8_brk,             /* [43] TIM8 Break Interrupt */
    [TIM8_UP_IRQn            ] = isr_tim8_up,              /* [44] TIM8 Update Interrupt */
    [TIM8_TRG_COM_IRQn       ] = isr_tim8_trg_com,         /* [45] TIM8 Trigger and Commutation Interrupt */
    [TIM8_CC_IRQn            ] = isr_tim8_cc,              /* [46] TIM8 Capture Compare Interrupt */
    [ADC3_IRQn               ] = isr_adc3,                 /* [47] ADC3 global Interrupt */
    [SPI3_IRQn               ] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [UART4_IRQn              ] = isr_uart4,                /* [52] UART4 global Interrupt & EXTI Line34 Interrupt (UART4 wakeup) */
    [UART5_IRQn              ] = isr_uart5,                /* [53] UART5 global Interrupt & EXTI Line35 Interrupt (UART5 wakeup) */
    [TIM6_DAC_IRQn           ] = isr_tim6_dac,             /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    [TIM7_IRQn               ] = isr_tim7,                 /* [55] TIM7 global Interrupt */
    [DMA2_Channel1_IRQn      ] = isr_dma2_channel1,        /* [56] DMA2 Channel 1 global Interrupt */
    [DMA2_Channel2_IRQn      ] = isr_dma2_channel2,        /* [57] DMA2 Channel 2 global Interrupt */
    [DMA2_Channel3_IRQn      ] = isr_dma2_channel3,        /* [58] DMA2 Channel 3 global Interrupt */
    [DMA2_Channel4_IRQn      ] = isr_dma2_channel4,        /* [59] DMA2 Channel 4 global Interrupt */
    [DMA2_Channel5_IRQn      ] = isr_dma2_channel5,        /* [60] DMA2 Channel 5 global Interrupt */
    [ADC4_IRQn               ] = isr_adc4,                 /* [61] ADC4  global Interrupt */
    [COMP1_2_3_IRQn          ] = isr_comp1_2_3,            /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    [COMP4_5_6_IRQn          ] = isr_comp4_5_6,            /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    [COMP7_IRQn              ] = isr_comp7,                /* [66] COMP7 global Interrupt via EXTI Line33 */
    [USB_HP_IRQn             ] = isr_usb_hp,               /* [74] USB High Priority global Interrupt remap */
    [USB_LP_IRQn             ] = isr_usb_lp,               /* [75] USB Low Priority global Interrupt  remap */
    [USBWakeUp_RMP_IRQn      ] = isr_usbwakeup_rmp,        /* [76] USB Wakeup Interrupt remap */
#elif defined(CPU_MODEL_STM32F334R8)
    [ADC1_2_IRQn             ] = isr_adc1_2,               /* [18] ADC1 & ADC2 Interrupts */
    [CAN_TX_IRQn             ] = isr_can_tx,               /* [19] CAN TX Interrupts */
    [CAN_RX0_IRQn            ] = isr_can_rx0,              /* [20] CAN RX0 Interrupts */
    [TIM3_IRQn               ] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [SPI1_IRQn               ] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [TIM6_DAC1_IRQn          ] = isr_tim6_dac1,            /* [54] TIM6 global and DAC1 channel1 & 2 underrun error interrupts */
    [TIM7_DAC2_IRQn          ] = isr_tim7_dac2,            /* [55] TIM7 global and DAC2 channel1 underrun error Interrupt */
    [COMP2_IRQn              ] = isr_comp2,                /* [64] COMP2 global Interrupt via EXT Line22 */
    [COMP4_6_IRQn            ] = isr_comp4_6,              /* [65] COMP4 and COMP6 global Interrupt via EXT Line30 and 32 */
    [HRTIM1_Master_IRQn      ] = isr_hrtim1_master,        /* [67] HRTIM Master Timer global Interrupts */
    [HRTIM1_TIMA_IRQn        ] = isr_hrtim1_tima,          /* [68] HRTIM Timer A global Interrupt */
    [HRTIM1_TIMB_IRQn        ] = isr_hrtim1_timb,          /* [69] HRTIM Timer B global Interrupt */
    [HRTIM1_TIMC_IRQn        ] = isr_hrtim1_timc,          /* [70] HRTIM Timer C global Interrupt */
    [HRTIM1_TIMD_IRQn        ] = isr_hrtim1_timd,          /* [71] HRTIM Timer D global Interrupt */
    [HRTIM1_TIME_IRQn        ] = isr_hrtim1_time,          /* [72] HRTIM Timer E global Interrupt */
    [HRTIM1_FLT_IRQn         ] = isr_hrtim1_flt,           /* [73] HRTIM Fault global Interrupt */
#endif
};
