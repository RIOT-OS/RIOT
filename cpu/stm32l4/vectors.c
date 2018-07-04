/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32l4
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

/* STM32L4 specific interrupt vectors */
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_adc1_2(void);
WEAK_DEFAULT void isr_adc3(void);
WEAK_DEFAULT void isr_can1_rx0(void);
WEAK_DEFAULT void isr_can1_rx1(void);
WEAK_DEFAULT void isr_can1_sce(void);
WEAK_DEFAULT void isr_can1_tx(void);
WEAK_DEFAULT void isr_can2_rx0(void);
WEAK_DEFAULT void isr_can2_rx1(void);
WEAK_DEFAULT void isr_can2_sce(void);
WEAK_DEFAULT void isr_can2_tx(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_crs(void);
WEAK_DEFAULT void isr_dcmi(void);
WEAK_DEFAULT void isr_dfsdm1_flt0(void);
WEAK_DEFAULT void isr_dfsdm1_flt1(void);
WEAK_DEFAULT void isr_dfsdm1_flt2(void);
WEAK_DEFAULT void isr_dfsdm1_flt3(void);
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
WEAK_DEFAULT void isr_dma2_channel6(void);
WEAK_DEFAULT void isr_dma2_channel7(void);
WEAK_DEFAULT void isr_dma2d(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_fpu(void);
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
WEAK_DEFAULT void isr_otg_fs(void);
WEAK_DEFAULT void isr_pvd_pvm(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_sai2(void);
WEAK_DEFAULT void isr_sdmmc1(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_swpmi1(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
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
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim8_brk(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_tim8_trg_com(void);
WEAK_DEFAULT void isr_tim8_up(void);
WEAK_DEFAULT void isr_tsc(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [WWDG_IRQn               ] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [PVD_PVM_IRQn            ] = isr_pvd_pvm,              /* [ 1] PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection Interrupts */
    [TAMP_STAMP_IRQn         ] = isr_tamp_stamp,           /* [ 2] Tamper and TimeStamp interrupts through the EXTI line */
    [RTC_WKUP_IRQn           ] = isr_rtc_wkup,             /* [ 3] RTC Wakeup interrupt through the EXTI line */
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
    [CAN1_TX_IRQn            ] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [CAN1_RX0_IRQn           ] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [CAN1_RX1_IRQn           ] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [CAN1_SCE_IRQn           ] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [EXTI9_5_IRQn            ] = isr_exti,                 /* [23] External Line[9:5] Interrupts */
    [TIM1_BRK_TIM15_IRQn     ] = isr_tim1_brk_tim15,       /* [24] TIM1 Break interrupt and TIM15 global interrupt */
    [TIM1_UP_TIM16_IRQn      ] = isr_tim1_up_tim16,        /* [25] TIM1 Update Interrupt and TIM16 global interrupt */
    [TIM1_CC_IRQn            ] = isr_tim1_cc,              /* [27] TIM1 Capture Compare Interrupt */
    [TIM2_IRQn               ] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [I2C1_EV_IRQn            ] = isr_i2c1_ev,              /* [31] I2C1 Event Interrupt */
    [I2C1_ER_IRQn            ] = isr_i2c1_er,              /* [32] I2C1 Error Interrupt */
    [SPI1_IRQn               ] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [USART1_IRQn             ] = isr_usart1,               /* [37] USART1 global Interrupt */
    [USART2_IRQn             ] = isr_usart2,               /* [38] USART2 global Interrupt */
    [EXTI15_10_IRQn          ] = isr_exti,                 /* [40] External Line[15:10] Interrupts */
    [RTC_Alarm_IRQn          ] = isr_rtc_alarm,            /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    [SPI3_IRQn               ] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [TIM6_DAC_IRQn           ] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [DMA2_Channel1_IRQn      ] = isr_dma2_channel1,        /* [56] DMA2 Channel 1 global Interrupt */
    [DMA2_Channel2_IRQn      ] = isr_dma2_channel2,        /* [57] DMA2 Channel 2 global Interrupt */
    [DMA2_Channel3_IRQn      ] = isr_dma2_channel3,        /* [58] DMA2 Channel 3 global Interrupt */
    [DMA2_Channel4_IRQn      ] = isr_dma2_channel4,        /* [59] DMA2 Channel 4 global Interrupt */
    [DMA2_Channel5_IRQn      ] = isr_dma2_channel5,        /* [60] DMA2 Channel 5 global Interrupt */
    [COMP_IRQn               ] = isr_comp,                 /* [64] COMP1 and COMP2 Interrupts */
    [LPTIM1_IRQn             ] = isr_lptim1,               /* [65] LP TIM1 interrupt */
    [LPTIM2_IRQn             ] = isr_lptim2,               /* [66] LP TIM2 interrupt */
    [DMA2_Channel6_IRQn      ] = isr_dma2_channel6,        /* [68] DMA2 Channel 6 global interrupt */
    [DMA2_Channel7_IRQn      ] = isr_dma2_channel7,        /* [69] DMA2 Channel 7 global interrupt */
    [LPUART1_IRQn            ] = isr_lpuart1,              /* [70] LP UART1 interrupt */
    [QUADSPI_IRQn            ] = isr_quadspi,              /* [71] Quad SPI global interrupt */
    [I2C3_EV_IRQn            ] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [I2C3_ER_IRQn            ] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [SAI1_IRQn               ] = isr_sai1,                 /* [74] Serial Audio Interface 1 global interrupt */
    [TSC_IRQn                ] = isr_tsc,                  /* [77] Touch Sense Controller global interrupt */
    [RNG_IRQn                ] = isr_rng,                  /* [80] RNG global interrupt */
    [FPU_IRQn                ] = isr_fpu,                  /* [81] FPU global interrupt */

#if defined(CPU_MODEL_STM32L432KC) || defined(CPU_MODEL_STM32L433RC)
    [ADC1_IRQn               ] = isr_adc1,                 /* [18] ADC1 global Interrupt */
    [TIM1_TRG_COM_IRQn       ] = isr_tim1_trg_com,         /* [26] TIM1 Trigger and Commutation Interrupt */
    [USB_IRQn                ] = isr_usb,                  /* [67] USB event Interrupt */
    [CRS_IRQn                ] = isr_crs,                  /* [82] CRS global interrupt */
#endif
#if defined(CPU_MODEL_STM32L476RG) || defined(CPU_MODEL_STM32L476VG) || \
    defined(CPU_MODEL_STM32L475VG) || defined(CPU_MODEL_STM32L452RE) || \
    defined(CPU_MODEL_STM32L496ZG)
    [ADC1_2_IRQn             ] = isr_adc1_2,               /* [18] ADC1, ADC2 SAR global Interrupts */
    [TIM1_TRG_COM_TIM17_IRQn ] = isr_tim1_trg_com_tim17,   /* [26] TIM1 Trigger and Commutation Interrupt and TIM17 global interrupt */
    [TIM3_IRQn               ] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [I2C2_EV_IRQn            ] = isr_i2c2_ev,              /* [33] I2C2 Event Interrupt */
    [I2C2_ER_IRQn            ] = isr_i2c2_er,              /* [34] I2C2 Error Interrupt */
    [SPI2_IRQn               ] = isr_spi2,                 /* [36] SPI2 global Interrupt */
    [USART3_IRQn             ] = isr_usart3,               /* [39] USART3 global Interrupt */
    [SDMMC1_IRQn             ] = isr_sdmmc1,               /* [49] SDMMC1 global Interrupt */
    [UART4_IRQn              ] = isr_uart4,                /* [52] UART4 global Interrupt */
    [DFSDM1_FLT0_IRQn        ] = isr_dfsdm1_flt0,          /* [61] DFSDM1 Filter 0 global Interrupt */
    [DFSDM1_FLT1_IRQn        ] = isr_dfsdm1_flt1,          /* [62] DFSDM1 Filter 1 global Interrupt */
#endif
#if defined(CPU_MODEL_STM32L476RG) || defined(CPU_MODEL_STM32L476VG) || \
    defined(CPU_MODEL_STM32L475VG)
    [TIM4_IRQn               ] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [DFSDM1_FLT3_IRQn        ] = isr_dfsdm1_flt3,          /* [42] DFSDM1 Filter 3 global Interrupt */
    [TIM8_BRK_IRQn           ] = isr_tim8_brk,             /* [43] TIM8 Break Interrupt */
    [TIM8_UP_IRQn            ] = isr_tim8_up,              /* [44] TIM8 Update Interrupt */
    [TIM8_TRG_COM_IRQn       ] = isr_tim8_trg_com,         /* [45] TIM8 Trigger and Commutation Interrupt */
    [TIM8_CC_IRQn            ] = isr_tim8_cc,              /* [46] TIM8 Capture Compare Interrupt */
    [ADC3_IRQn               ] = isr_adc3,                 /* [47] ADC3 global  Interrupt */
    [FMC_IRQn                ] = isr_fmc,                  /* [48] FMC global Interrupt */
    [TIM5_IRQn               ] = isr_tim5,                 /* [50] TIM5 global Interrupt */
    [UART5_IRQn              ] = isr_uart5,                /* [53] UART5 global Interrupt */
    [DFSDM1_FLT2_IRQn        ] = isr_dfsdm1_flt2,          /* [63] DFSDM1 Filter 2 global Interrupt */
    [OTG_FS_IRQn             ] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [SAI2_IRQn               ] = isr_sai2,                 /* [75] Serial Audio Interface 2 global interrupt */
#endif
#if defined(CPU_MODEL_STM32L432KC) || defined(CPU_MODEL_STM32L476RG) || \
    defined(CPU_MODEL_STM32L476VG) || defined(CPU_MODEL_STM32L475VG) || \
    defined(CPU_MODEL_STM32L496ZG)
    [TIM7_IRQn               ] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [SWPMI1_IRQn             ] = isr_swpmi1,               /* [76] Serial Wire Interface 1 global interrupt */
#endif
#if defined(CPU_MODEL_STM32L476RG) || defined(CPU_MODEL_STM32L476VG)
    [LCD_IRQn                ] = isr_lcd,                  /* [78] LCD global interrupt */
#endif
#if defined(CPU_MODEL_STM32L496ZG)
    [TIM4_IRQn               ] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [TIM5_IRQn               ] = isr_tim5,                 /* [50] TIM5 global Interrupt */
    [UART5_IRQn              ] = isr_uart5,                /* [53] UART5 global Interrupt */
    [DFSDM1_FLT2_IRQn        ] = isr_dfsdm1_flt2,          /* [63] DFSDM1 Filter 2 global Interrupt */
    [OTG_FS_IRQn             ] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [SAI2_IRQn               ] = isr_sai2,                 /* [75] Serial Audio Interface 2 global interrupt */
    [LCD_IRQn                ] = isr_lcd,                  /* [78] LCD global interrupt */
    [CRS_IRQn                ] = isr_crs,                  /* [82] CRS global interrupt */
    [I2C4_EV_IRQn            ] = isr_i2c4_ev,              /* [83] I2C4 Event interrupt */
    [I2C4_ER_IRQn            ] = isr_i2c4_er,              /* [84] I2C4 Error interrupt */
    [DCMI_IRQn               ] = isr_dcmi,                 /* [85] DCMI global interrupt */
    [CAN2_TX_IRQn            ] = isr_can2_tx,              /* [86] CAN2 TX interrupt */
    [CAN2_RX0_IRQn           ] = isr_can2_rx0,             /* [87] CAN2 RX0 interrupt */
    [CAN2_RX1_IRQn           ] = isr_can2_rx1,             /* [88] CAN2 RX1 interrupt */
    [CAN2_SCE_IRQn           ] = isr_can2_sce,             /* [89] CAN2 SCE interrupt */
    [DMA2D_IRQn              ] = isr_dma2d,                /* [90] DMA2D global interrupt */
#endif
};
