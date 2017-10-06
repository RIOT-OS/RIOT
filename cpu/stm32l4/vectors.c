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

#include <stdint.h>
#include "vectors_cortexm.h"

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32L4 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd_pvm(void);
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
WEAK_DEFAULT void isr_can1_tx(void);
WEAK_DEFAULT void isr_can1_rx0(void);
WEAK_DEFAULT void isr_can1_rx1(void);
WEAK_DEFAULT void isr_can1_sce(void);
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
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_dfsdm1_flt3(void);
WEAK_DEFAULT void isr_tim8_brk(void);
WEAK_DEFAULT void isr_tim8_up(void);
WEAK_DEFAULT void isr_tim8_trg_com(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_adc3(void);
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_sdmmc1(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_dma2_channel1(void);
WEAK_DEFAULT void isr_dma2_channel2(void);
WEAK_DEFAULT void isr_dma2_channel3(void);
WEAK_DEFAULT void isr_dma2_channel4(void);
WEAK_DEFAULT void isr_dma2_channel5(void);
WEAK_DEFAULT void isr_dfsdm1_flt0(void);
WEAK_DEFAULT void isr_dfsdm1_flt1(void);
WEAK_DEFAULT void isr_dfsdm1_flt2(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_lptim2(void);
WEAK_DEFAULT void isr_otg_fs(void);
WEAK_DEFAULT void isr_dma2_channel6(void);
WEAK_DEFAULT void isr_dma2_channel7(void);
WEAK_DEFAULT void isr_lpuart1(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_sai2(void);
WEAK_DEFAULT void isr_swpmi1(void);
WEAK_DEFAULT void isr_tsc(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_0(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_crs(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[] = {
    isr_wwdg,               /* [0] Window WatchDog Interrupt */
    isr_pvd_pvm,            /* [1] PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection Interrupts */
    isr_tamp_stamp,         /* [2] Tamper and TimeStamp interrupts through the EXTI line */
    isr_rtc_wkup,           /* [3] RTC Wakeup interrupt through the EXTI line */
    isr_flash,              /* [4] FLASH global Interrupt */
    isr_rcc,                /* [5] RCC global Interrupt */
    isr_exti,               /* [6] EXTI Line0 Interrupt */
    isr_exti,               /* [7] EXTI Line1 Interrupt */
    isr_exti,               /* [8] EXTI Line2 Interrupt */
    isr_exti,               /* [9] EXTI Line3 Interrupt */
    isr_exti,               /* [10] EXTI Line4 Interrupt */
    isr_dma1_channel1,      /* [11] DMA1 Channel 1 global Interrupt */
    isr_dma1_channel2,      /* [12] DMA1 Channel 2 global Interrupt */
    isr_dma1_channel3,      /* [13] DMA1 Channel 3 global Interrupt */
    isr_dma1_channel4,      /* [14] DMA1 Channel 4 global Interrupt */
    isr_dma1_channel5,      /* [15] DMA1 Channel 5 global Interrupt */
    isr_dma1_channel6,      /* [16] DMA1 Channel 6 global Interrupt */
    isr_dma1_channel7,      /* [17] DMA1 Channel 7 global Interrupt */
    isr_adc1_2,             /* [18] ADC1 global Interrupt */
    isr_can1_tx,            /* [19] CAN1 TX Interrupt */
    isr_can1_rx0,           /* [20] CAN1 RX0 Interrupt */
    isr_can1_rx1,           /* [21] CAN1 RX1 Interrupt */
    isr_can1_sce,           /* [22] CAN1 SCE Interrupt */
    isr_exti,               /* [23] External Line[9:5] Interrupts */
    isr_tim1_brk_tim15,     /* [24] TIM1 Break interrupt and TIM15 global interrupt */
    isr_tim1_up_tim16,      /* [25] TIM1 Update Interrupt and TIM16 global interrupt */
    isr_tim1_trg_com_tim17, /* [26] TIM1 Trigger and Commutation Interrupt */
    isr_tim1_cc,            /* [27] TIM1 Capture Compare Interrupt */
    isr_tim2,               /* [28] TIM2 global Interrupt */
#if defined(CPU_MODEL_STM32L432KC)
    (0UL),                  /* [29] Reserved */
    (0UL),                  /* [30] Reserved */
    isr_i2c1_ev,            /* [31] I2C1 Event Interrupt */
    isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (0UL),                  /* [33] Reserved */
    (0UL),                  /* [34] Reserved */
    isr_spi1,               /* [35] SPI1 global Interrupt */
    (0UL),                  /* [36] Reserved */
    isr_usart1,             /* [37] USART1 global Interrupt */
    isr_usart2,             /* [38] USART2 global Interrupt */
    (0UL),                  /* [39] Reserved */
    isr_exti,               /* [40] External Line[15:10] Interrupts */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    (0UL),                  /* [42] Reserved */
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
    isr_tim6_dac,           /* [54] TIM6 global and DAC1&2 underrun error interrupts */
    isr_tim7,               /* [55] TIM7 global interrupt */
    isr_dma2_channel1,      /* [56] DMA2 Channel 1 global Interrupt */
    isr_dma2_channel2,      /* [57] DMA2 Channel 2 global Interrupt */
    isr_dma2_channel3,      /* [58] DMA2 Channel 3 global Interrupt */
    isr_dma2_channel4,      /* [59] DMA2 Channel 4 global Interrupt */
    isr_dma2_channel5,      /* [60] DMA2 Channel 5 global Interrupt */
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    isr_comp,               /* [64] COMP1 and COMP2 Interrupts */
    isr_lptim1,             /* [65] LP TIM1 interrupt */
    isr_lptim2,             /* [66] LP TIM2 interrupt */
    isr_otg_fs,             /* [67] USB OTG FS global Interrupt */
    isr_dma2_channel6,      /* [68] DMA2 Channel 6 global interrupt */
    isr_dma2_channel7,      /* [69] DMA2 Channel 7 global interrupt */
    isr_lpuart1,            /* [70] LP UART1 interrupt */
    isr_quadspi,            /* [71] Quad SPI global interrupt */
    isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    isr_i2c3_er,            /* [73] I2C3 error interrupt */
    isr_sai1,               /* [74] Serial Audio Interface 1 global interrupt */
    (0UL),                  /* [75] Reserved */
    isr_swpmi1,             /* [76] Serial Wire Interface 1 global interrupt */
    isr_tsc,                /* [77] Touch Sense Controller global interrupt */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved*/
    isr_rng,                /* [80] RNG global interrupt */
    isr_fpu,                /* [81] FPU global interrupt */
    isr_crs                 /* [82] CRS global interrupt */
#else /* CPU_MODEL_STM32L476RG */
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
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    isr_dfsdm1_flt3,        /* [42] DFSDM1 Filter 3 global Interrupt */
    isr_tim8_brk,           /* [43] TIM8 Break Interrupt */
    isr_tim8_up,            /* [44] TIM8 Update Interrupt */
    isr_tim8_trg_com,       /* [45] TIM8 Trigger and Commutation Interrupt */
    isr_tim8_cc,            /* [46] TIM8 Capture Compare Interrupt */
    isr_adc3,               /* [47] ADC3 global  Interrupt */
    isr_fmc,                /* [48] FMC global Interrupt */
    isr_sdmmc1,             /* [49] SDMMC1 global Interrupt */
    isr_tim5,               /* [50] TIM5 global Interrupt */
    isr_spi3,               /* [51] SPI3 global Interrupt */
    isr_uart4,              /* [52] UART4 global Interrupt */
    isr_uart5,              /* [53] UART5 global Interrupt */
    isr_tim6_dac,           /* [54] TIM6 global and DAC1&2 underrun error interrupts */
    isr_tim7,               /* [55] TIM7 global interrupt */
    isr_dma2_channel1,      /* [56] DMA2 Channel 1 global Interrupt */
    isr_dma2_channel2,      /* [57] DMA2 Channel 2 global Interrupt */
    isr_dma2_channel3,      /* [58] DMA2 Channel 3 global Interrupt */
    isr_dma2_channel4,      /* [59] DMA2 Channel 4 global Interrupt */
    isr_dma2_channel5,      /* [60] DMA2 Channel 5 global Interrupt */
    isr_dfsdm1_flt0,        /* [61] DFSDM1 Filter 0 global Interrupt */
    isr_dfsdm1_flt1,        /* [62] DFSDM1 Filter 1 global Interrupt */
    isr_dfsdm1_flt2,        /* [63] DFSDM1 Filter 2 global Interrupt */
    isr_comp,               /* [64] COMP1 and COMP2 Interrupts */
    isr_lptim1,             /* [65] LP TIM1 interrupt */
    isr_lptim2,             /* [66] LP TIM2 interrupt */
    isr_otg_fs,             /* [67] USB OTG FS global Interrupt */
    isr_dma2_channel6,      /* [68] DMA2 Channel 6 global interrupt */
    isr_dma2_channel7,      /* [69] DMA2 Channel 7 global interrupt */
    isr_lpuart1,            /* [70] LP UART1 interrupt */
    isr_quadspi,            /* [71] Quad SPI global interrupt */
    isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    isr_i2c3_er,            /* [73] I2C3 error interrupt */
    isr_sai1,               /* [74] Serial Audio Interface 1 global interrupt */
    isr_sai2,               /* [75] Serial Audio Interface 2 global interrupt */
    isr_swpmi1,             /* [76] Serial Wire Interface 1 global interrupt */
    isr_tsc,                /* [77] Touch Sense Controller global interrupt */
    isr_lcd,                /* [78] LCD global interrupt */
    (0UL),                  /* [79] Reserved*/
    isr_rng,                /* [80] RNG global interrupt */
    isr_fpu                 /* [81] FPU global interrupt */
#endif
};
