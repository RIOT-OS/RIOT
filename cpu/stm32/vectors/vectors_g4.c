/*
 * Copyright (C) 2020
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions for STM32G4
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

/* STM32L4 specific interrupt vectors */
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_adc1_2(void);
WEAK_DEFAULT void isr_adc3(void);
WEAK_DEFAULT void isr_adc4(void);
WEAK_DEFAULT void isr_adc5(void);
WEAK_DEFAULT void isr_can1_rx0(void);
WEAK_DEFAULT void isr_can1_rx1(void);
WEAK_DEFAULT void isr_can1_sce(void);
WEAK_DEFAULT void isr_can1_tx(void);
WEAK_DEFAULT void isr_can2_rx0(void);
WEAK_DEFAULT void isr_can2_rx1(void);
WEAK_DEFAULT void isr_can2_sce(void);
WEAK_DEFAULT void isr_can2_tx(void);
WEAK_DEFAULT void isr_comp1_2_3(void);
WEAK_DEFAULT void isr_comp4_5_6(void);
WEAK_DEFAULT void isr_comp7(void);
WEAK_DEFAULT void isr_cordic(void);
WEAK_DEFAULT void isr_crs(void);
WEAK_DEFAULT void isr_dcmi(void);
WEAK_DEFAULT void isr_dfsdm1_flt0(void);
WEAK_DEFAULT void isr_dfsdm1_flt1(void);
WEAK_DEFAULT void isr_dfsdm1_flt2(void);
WEAK_DEFAULT void isr_dfsdm1_flt3(void);
WEAK_DEFAULT void isr_dmamux(void);
WEAK_DEFAULT void isr_dma1_channel1(void);
WEAK_DEFAULT void isr_dma1_channel2(void);
WEAK_DEFAULT void isr_dma1_channel3(void);
WEAK_DEFAULT void isr_dma1_channel4(void);
WEAK_DEFAULT void isr_dma1_channel5(void);
WEAK_DEFAULT void isr_dma1_channel6(void);
WEAK_DEFAULT void isr_dma1_channel7(void);
WEAK_DEFAULT void isr_dma1_channel8(void);
WEAK_DEFAULT void isr_dma2_channel1(void);
WEAK_DEFAULT void isr_dma2_channel2(void);
WEAK_DEFAULT void isr_dma2_channel3(void);
WEAK_DEFAULT void isr_dma2_channel4(void);
WEAK_DEFAULT void isr_dma2_channel5(void);
WEAK_DEFAULT void isr_dma2_channel6(void);
WEAK_DEFAULT void isr_dma2_channel7(void);
WEAK_DEFAULT void isr_dma2_channel8(void);
WEAK_DEFAULT void isr_dma2d(void);
WEAK_DEFAULT void isr_dmamux1_ovr(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_fdcan1_it0(void);
WEAK_DEFAULT void isr_fdcan1_it1(void);
WEAK_DEFAULT void isr_fdcan2_it0(void);
WEAK_DEFAULT void isr_fdcan2_it1(void);
WEAK_DEFAULT void isr_fdcan3_it0(void);
WEAK_DEFAULT void isr_fdcan3_it1(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_fmac(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_hrtim1_flt(void);
WEAK_DEFAULT void isr_hrtim1_master(void);
WEAK_DEFAULT void isr_hrtim1_tima(void);
WEAK_DEFAULT void isr_hrtim1_timb(void);
WEAK_DEFAULT void isr_hrtim1_timc(void);
WEAK_DEFAULT void isr_hrtim1_timd(void);
WEAK_DEFAULT void isr_hrtim1_time(void);
WEAK_DEFAULT void isr_hrtim1_timf(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_i2c4_ev(void);
WEAK_DEFAULT void isr_i2c4_er(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_lptim2(void);
WEAK_DEFAULT void isr_lpuart1(void);
WEAK_DEFAULT void isr_octospi1(void);
WEAK_DEFAULT void isr_octospi2(void);
WEAK_DEFAULT void isr_otg_fs(void);
WEAK_DEFAULT void isr_pvd_pvm(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_tamp_lsecss(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_sai2(void);
WEAK_DEFAULT void isr_sdmmc1(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_spi4(void);
WEAK_DEFAULT void isr_swpmi1(void);
WEAK_DEFAULT void isr_tim1_brk_tim15(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim1_trg_com(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim17(void);
WEAK_DEFAULT void isr_tim1_up_tim16(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7_dac(void);
WEAK_DEFAULT void isr_tim8_brk(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_tim8_trg_com(void);
WEAK_DEFAULT void isr_tim8_up(void);
WEAK_DEFAULT void isr_tim20_brk(void);
WEAK_DEFAULT void isr_tim20_up(void);
WEAK_DEFAULT void isr_tim20_trg_com(void);
WEAK_DEFAULT void isr_tim20_cc(void);
WEAK_DEFAULT void isr_tsc(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_ucpd1(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_usbwakeup(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [WWDG_IRQn               ] = isr_wwdg,                  /* [ 0] Window WatchDog Interrupt */
    [PVD_PVM_IRQn            ] = isr_pvd_pvm,               /* [ 1] PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection Interrupts */
    [RTC_TAMP_LSECSS_IRQn    ] = isr_rtc_tamp_lsecss,       /* [ 2] RTC Tamper and TimeStamp and RCC LSE CSS interrupts through the EXTI */
    [RTC_WKUP_IRQn           ] = isr_rtc_wkup,              /* [ 3] RTC Wakeup interrupt through the EXTI line */
    [FLASH_IRQn              ] = isr_flash,                 /* [ 4] FLASH global Interrupt */
    [RCC_IRQn                ] = isr_rcc,                   /* [ 5] RCC global Interrupt */
    [EXTI0_IRQn              ] = isr_exti,                  /* [ 6] EXTI Line0 Interrupt */
    [EXTI1_IRQn              ] = isr_exti,                  /* [ 7] EXTI Line1 Interrupt */
    [EXTI2_IRQn              ] = isr_exti,                  /* [ 8] EXTI Line2 Interrupt */
    [EXTI3_IRQn              ] = isr_exti,                  /* [ 9] EXTI Line3 Interrupt */
    [EXTI4_IRQn              ] = isr_exti,                  /* [10] EXTI Line4 Interrupt */
    [DMA1_Channel1_IRQn      ] = isr_dma1_channel1,         /* [11] DMA1 Channel 1 global Interrupt */
    [DMA1_Channel2_IRQn      ] = isr_dma1_channel2,         /* [12] DMA1 Channel 2 global Interrupt */
    [DMA1_Channel3_IRQn      ] = isr_dma1_channel3,         /* [13] DMA1 Channel 3 global Interrupt */
    [DMA1_Channel4_IRQn      ] = isr_dma1_channel4,         /* [14] DMA1 Channel 4 global Interrupt */
    [DMA1_Channel5_IRQn      ] = isr_dma1_channel5,         /* [15] DMA1 Channel 5 global Interrupt */
    [DMA1_Channel6_IRQn      ] = isr_dma1_channel6,         /* [16] DMA1 Channel 6 global Interrupt */
    [DMA1_Channel7_IRQn      ] = isr_dma1_channel7,         /* [17] DMA1 Channel 7 global Interrupt */
    [ADC1_2_IRQn             ] = isr_adc1_2,                /* [18] ADC1, ADC2 SAR global Interrupts */
    [USB_HP_IRQn             ] = isr_usb_hp,                /* [19] USB HP Interrupt */
    [USB_LP_IRQn             ] = isr_usb_lp,                /* [20] USB LP  Interrupt */
    [FDCAN1_IT0_IRQn         ] = isr_fdcan1_it0,            /* [21] FDCAN1 IT0 Interrupt */
    [FDCAN1_IT1_IRQn         ] = isr_fdcan1_it1,            /* [22] FDCAN1 IT1 Interrupt */
    [EXTI9_5_IRQn            ] = isr_exti,                  /* [23] External Line[9:5] Interrupts */
    [TIM1_BRK_TIM15_IRQn     ] = isr_tim1_brk_tim15,        /* [24] TIM1 Break interrupt and TIM15 global interrupt */
    [TIM1_UP_TIM16_IRQn      ] = isr_tim1_up_tim16,         /* [25] TIM1 Update Interrupt and TIM16 global interrupt */
    [TIM1_TRG_COM_TIM17_IRQn ] = isr_tim1_trg_com_tim17,    /* [26] TIM1 TIM1 Trigger, Commutation, Direction change, Index and TIM17 global interrupt */
    [TIM1_CC_IRQn            ] = isr_tim1_cc,               /* [27] TIM1 Capture Compare Interrupt */
    [TIM2_IRQn               ] = isr_tim2,                  /* [28] TIM2 global Interrupt */
    [TIM3_IRQn               ] = isr_tim3,                  /* [29] TIM3 global Interrupt */
    [TIM4_IRQn               ] = isr_tim4,                  /* [30] TIM4 global Interrupt */
    [I2C1_EV_IRQn            ] = isr_i2c1_ev,               /* [31] I2C1 Event Interrupt */
    [I2C1_ER_IRQn            ] = isr_i2c1_er,               /* [32] I2C1 Error Interrupt */
    [I2C2_EV_IRQn            ] = isr_i2c2_ev,               /* [33] I2C2 Event Interrupt */
    [I2C2_ER_IRQn            ] = isr_i2c2_er,               /* [34] I2C2 Error Interrupt */
    [SPI1_IRQn               ] = isr_spi1,                  /* [35] SPI1 global Interrupt */
    [SPI2_IRQn               ] = isr_spi2,                  /* [36] SPI2 global Interrupt */
    [USART1_IRQn             ] = isr_usart1,                /* [37] USART1 global Interrupt */
    [USART2_IRQn             ] = isr_usart2,                /* [38] USART2 global Interrupt */
    [USART3_IRQn             ] = isr_usart3,                /* [39] USART3 global Interrupt */
    [EXTI15_10_IRQn          ] = isr_exti,                  /* [40] External Line[15:10] Interrupts */
    [RTC_Alarm_IRQn          ] = isr_rtc_alarm,             /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    [USBWakeUp_IRQn          ] = isr_usbwakeup,             /* [42] USB Wakeup through EXTI line Interrupt */
    [TIM8_BRK_IRQn           ] = isr_tim8_brk,              /* [43] TIM8 Break Interrupt */
    [TIM8_UP_IRQn            ] = isr_tim8_up,               /* [44] TIM8 Update Interrupt */
    [TIM8_TRG_COM_IRQn       ] = isr_tim8_trg_com,          /* [45] TIM8 Trigger and Commutation Interrupt */
    [TIM8_CC_IRQn            ] = isr_tim8_cc,               /* [46] TIM8 Capture Compare Interrupt */
    [ADC3_IRQn               ] = isr_adc3,                  /* [47] ADC3 global  Interrupt */
    [FMC_IRQn                ] = isr_fmc,                   /* [48] FMC global Interrupt */
    [LPTIM1_IRQn             ] = isr_lptim1,                /* [49] LP TIM1 Interrupt */
    [TIM5_IRQn               ] = isr_tim5,                  /* [50] TIM5 global Interrupt */
    [SPI3_IRQn               ] = isr_spi3,                  /* [51] SPI3 global Interrupt */
    [UART4_IRQn              ] = isr_uart4,                 /* [52] UART4 global Interrupt */
    [UART5_IRQn              ] = isr_uart5,                 /* [53] UART5 global Interrupt */
    [TIM6_DAC_IRQn           ] = isr_tim6_dac,              /* [54] TIM6 global and DAC1&3 underrun error interrupts */
    [TIM7_DAC_IRQn           ] = isr_tim7_dac,              /* [55] TIM7 global and DAC2&4 underrun error interrupts */
    [DMA2_Channel1_IRQn      ] = isr_dma2_channel1,         /* [56] DMA2 Channel 1 global Interrupt */
    [DMA2_Channel2_IRQn      ] = isr_dma2_channel2,         /* [57] DMA2 Channel 2 global Interrupt */
    [DMA2_Channel3_IRQn      ] = isr_dma2_channel3,         /* [58] DMA2 Channel 3 global Interrupt */
    [DMA2_Channel4_IRQn      ] = isr_dma2_channel4,         /* [59] DMA2 Channel 4 global Interrupt */
    [DMA2_Channel5_IRQn      ] = isr_dma2_channel5,         /* [60] DMA2 Channel 5 global Interrupt */
    [ADC4_IRQn               ] = isr_adc4,                  /* [61] ADC4 global Interrupt */
    [ADC5_IRQn               ] = isr_adc5,                  /* [62] ADC5 global Interrupt */
    [UCPD1_IRQn              ] = isr_ucpd1,                 /* [63] UCPD global Interrupt */
    [COMP1_2_3_IRQn          ] = isr_comp1_2_3,             /* [64] COMP1, COMP2 and COMP3 Interrupt */
    [COMP4_5_6_IRQn          ] = isr_comp4_5_6,             /* [65] COMP4, COMP5 and Interrupt */
    [COMP7_IRQn              ] = isr_comp7,                 /* [66] COMP7 Interrupt */
    [HRTIM1_Master_IRQn      ] = isr_hrtim1_master,         /* [67] HRTIM Master Timer global Interrupt */
    [HRTIM1_TIMA_IRQn        ] = isr_hrtim1_tima,           /* [68] HRTIM Timer A global Interrupt */
    [HRTIM1_TIMB_IRQn        ] = isr_hrtim1_timb,           /* [69] HRTIM Timer B global Interrupt */
    [HRTIM1_TIMC_IRQn        ] = isr_hrtim1_timc,           /* [70] HRTIM Timer C global Interrupt */
    [HRTIM1_TIMD_IRQn        ] = isr_hrtim1_timd,           /* [71] HRTIM Timer D global Interrupt */
    [HRTIM1_TIME_IRQn        ] = isr_hrtim1_time,           /* [72] HRTIM Timer E global Interrupt */
    [HRTIM1_FLT_IRQn         ] = isr_hrtim1_flt,            /* [73] HRTIM Fault global Interrupt */
    [HRTIM1_TIMF_IRQn        ] = isr_hrtim1_timf,           /* [74] HRTIM Timer F global Interrupt */
    [CRS_IRQn                ] = isr_crs,                   /* [75] CRS global Interrupt */
    [SAI1_IRQn               ] = isr_sai1,                  /* [76] Serial Audio Interface global Interrupt */
    [TIM20_BRK_IRQn          ] = isr_tim20_brk,             /* [77] TIM20 Break, Transition error and Index error Interrupt */
    [TIM20_UP_IRQn           ] = isr_tim20_up,              /* [78] TIM20 Update Interrupt */
    [TIM20_TRG_COM_IRQn      ] = isr_tim20_trg_com,         /* [79] TIM20 Trigger, Commutation, Direction change and Index Interrupt */
    [TIM20_CC_IRQn           ] = isr_tim20_cc,              /* [80] TIM20 Capture Compare Interrupt */
    [FPU_IRQn                ] = isr_fpu,                   /* [81] FPU global Interrupt */
    [I2C4_EV_IRQn            ] = isr_i2c4_ev,               /* [82] I2C4 Event Interrupt */
    [I2C4_ER_IRQn            ] = isr_i2c4_er,               /* [83] I2C4 Error Interrupt */
    [SPI4_IRQn               ] = isr_spi4,                  /* [84] SPI4 Event Interrupt */
    [FDCAN2_IT0_IRQn         ] = isr_fdcan2_it0,            /* [86] FDCAN2 interrupt line 0 Interrupt */
    [FDCAN2_IT1_IRQn         ] = isr_fdcan2_it1,            /* [87] FDCAN2 interrupt line 1 Interrupt */
    [FDCAN3_IT0_IRQn         ] = isr_fdcan3_it0,            /* [88] FDCAN3 interrupt line 0 Interrupt */
    [FDCAN3_IT1_IRQn         ] = isr_fdcan3_it1,            /* [89] FDCAN3 interrupt line 1 Interrupt */
    [RNG_IRQn                ] = isr_rng,                   /* [90] RNG global Interrupt */
    [LPUART1_IRQn            ] = isr_lpuart1,               /* [91] LP UART 1 Interrupt */
    [I2C3_EV_IRQn            ] = isr_i2c3_ev,               /* [92] I2C3 Event Interrupt */
    [I2C3_ER_IRQn            ] = isr_i2c3_er,               /* [93] I2C3 Error Interrupt */
    [DMAMUX_OVR_IRQn         ] = isr_dmamux,                /* [94] DMAMUX overrun global Interrupt */
    [QUADSPI_IRQn            ] = isr_quadspi,               /* [95] QUADSPI Interrupt */
    [DMA1_Channel8_IRQn      ] = isr_dma1_channel8,         /* [96] DMA1 Channel 8 Interrupt */
    [DMA2_Channel6_IRQn      ] = isr_dma2_channel6,         /* [97] DMA2 Channel 6 Interrupt */
    [DMA2_Channel7_IRQn      ] = isr_dma2_channel7,         /* [98] DMA2 Channel 7 Interrupt */
    [DMA2_Channel8_IRQn      ] = isr_dma2_channel8,         /* [99] DMA2 Channel 8 Interrupt */
    [CORDIC_IRQn             ] = isr_cordic,                /* [100] CORDIC global Interrupt */
    [FMAC_IRQn               ] = isr_fmac,                  /* [101] FMAC global Interrupt */
};
