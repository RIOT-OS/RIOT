/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
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

/* STM32F4 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_dma1_stream0(void);
WEAK_DEFAULT void isr_dma1_stream1(void);
WEAK_DEFAULT void isr_dma1_stream2(void);
WEAK_DEFAULT void isr_dma1_stream3(void);
WEAK_DEFAULT void isr_dma1_stream4(void);
WEAK_DEFAULT void isr_dma1_stream5(void);
WEAK_DEFAULT void isr_dma1_stream6(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_can1_tx(void);
WEAK_DEFAULT void isr_can1_rx0(void);
WEAK_DEFAULT void isr_can1_rx1(void);
WEAK_DEFAULT void isr_can1_sce(void);
WEAK_DEFAULT void isr_tim1_brk_tim9(void);
WEAK_DEFAULT void isr_tim1_up_tim10(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim11(void);
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
WEAK_DEFAULT void isr_otg_fs_wkup(void);
WEAK_DEFAULT void isr_tim8_brk_tim12(void);
WEAK_DEFAULT void isr_tim8_up_tim13(void);
WEAK_DEFAULT void isr_tim8_trg_com_tim14(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_dma1_stream7(void);
WEAK_DEFAULT void isr_fsmc(void);
WEAK_DEFAULT void isr_sdio(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_dma2_stream0(void);
WEAK_DEFAULT void isr_dma2_stream1(void);
WEAK_DEFAULT void isr_dma2_stream2(void);
WEAK_DEFAULT void isr_dma2_stream3(void);
WEAK_DEFAULT void isr_dma2_stream4(void);
WEAK_DEFAULT void isr_eth(void);
WEAK_DEFAULT void isr_eth_wkup(void);
WEAK_DEFAULT void isr_can2_tx(void);
WEAK_DEFAULT void isr_can2_rx0(void);
WEAK_DEFAULT void isr_can2_rx1(void);
WEAK_DEFAULT void isr_can2_sce(void);
WEAK_DEFAULT void isr_otg_fs(void);
WEAK_DEFAULT void isr_dma2_stream5(void);
WEAK_DEFAULT void isr_dma2_stream6(void);
WEAK_DEFAULT void isr_dma2_stream7(void);
WEAK_DEFAULT void isr_usart6(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_can3_tx(void);
WEAK_DEFAULT void isr_can3_rx0(void);
WEAK_DEFAULT void isr_can3_rx1(void);
WEAK_DEFAULT void isr_can3_sce(void);
WEAK_DEFAULT void isr_usart7(void);
WEAK_DEFAULT void isr_usart8(void);
WEAK_DEFAULT void isr_uart9(void);
WEAK_DEFAULT void isr_uart10(void);
WEAK_DEFAULT void isr_dfsdm2_flt0(void);
WEAK_DEFAULT void isr_dfsdm2_flt1(void);
WEAK_DEFAULT void isr_dfsdm2_flt2(void);
WEAK_DEFAULT void isr_dfsdm2_flt3(void);
WEAK_DEFAULT void isr_otg_hs_ep1_out(void);
WEAK_DEFAULT void isr_otg_hs_ep1_in(void);
WEAK_DEFAULT void isr_otg_hs_wkup(void);
WEAK_DEFAULT void isr_otg_hs(void);
WEAK_DEFAULT void isr_dcmi(void);
WEAK_DEFAULT void isr_cryp(void);
WEAK_DEFAULT void isr_hash_rng(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_spi4(void);
WEAK_DEFAULT void isr_spi5(void);
WEAK_DEFAULT void isr_spi6(void);
WEAK_DEFAULT void isr_fmpi2c1_ev(void);
WEAK_DEFAULT void isr_fmpi2c1_er(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_sai2(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_cec(void);
WEAK_DEFAULT void isr_spdif_rx(void);
WEAK_DEFAULT void isr_ltdc(void);
WEAK_DEFAULT void isr_ltdc_er(void);
WEAK_DEFAULT void isr_dma2d(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_wwdg,               /* [0] Window WatchDog */
    isr_pvd,                /* [1] PVD through EXTI Line detection */
    isr_tamp_stamp,         /* [2] Tamper and TimeStamps through the EXTI line */
    isr_rtc_wkup,           /* [3] RTC Wakeup through the EXTI line */
    isr_flash,              /* [4] FLASH */
    isr_rcc,                /* [5] RCC */
    isr_exti,               /* [6] EXTI Line0 */
    isr_exti,               /* [7] EXTI Line1 */
    isr_exti,               /* [8] EXTI Line2 */
    isr_exti,               /* [9] EXTI Line3 */
    isr_exti,               /* [10] EXTI Line4 */
    isr_dma1_stream0,       /* [11] DMA1 Stream 0 */
    isr_dma1_stream1,       /* [12] DMA1 Stream 1 */
    isr_dma1_stream2,       /* [13] DMA1 Stream 2 */
    isr_dma1_stream3,       /* [14] DMA1 Stream 3 */
    isr_dma1_stream4,       /* [15] DMA1 Stream 4 */
    isr_dma1_stream5,       /* [16] DMA1 Stream 5 */
    isr_dma1_stream6,       /* [17] DMA1 Stream 6 */
    isr_adc,                /* [18] ADC1, ADC2 and ADC3s */
#if defined(CPU_MODEL_STM32F401RE)
    (0UL),                  /* [19] Reserved */
    (0UL),                  /* [20] Reserved */
    (0UL),                  /* [21] Reserved */
    (0UL),                  /* [22] Reserved */
    isr_exti,               /* [23] External Line[9:5]s */
    isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    isr_tim2,               /* [28] TIM2 */
    isr_tim3,               /* [29] TIM3 */
    isr_tim4,               /* [30] TIM4 */
    isr_i2c1_ev,            /* [31] I2C1 Event */
    isr_i2c1_er,            /* [32] I2C1 Error */
    isr_i2c2_ev,            /* [33] I2C2 Event */
    isr_i2c2_er,            /* [34] I2C2 Error */
    isr_spi1,               /* [35] SPI1 */
    isr_spi2,               /* [36] SPI2 */
    isr_usart1,             /* [37] USART1 */
    isr_usart2,             /* [38] USART2 */
    (0UL),                  /* [39] Reserved */
    isr_exti,               /* [40] External Line[15:10]s */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    (0UL),                  /* [42] Reserved */
    (0UL),                  /* [43] Reserved */
    (0UL),                  /* [44] Reserved */
    (0UL),                  /* [45] Reserved */
    (0UL),                  /* [46] Reserved */
    isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    (0UL),                  /* [48] Reserved */
    isr_sdio,               /* [49] SDIO */
    isr_tim5,               /* [50] TIM5 */
    (0UL),                  /* [51] Reserved */
    (0UL),                  /* [52] Reserved */
    (0UL),                  /* [53] Reserved */
    (0UL),                  /* [54] Reserved */
    (0UL),                  /* [55] Reserved */
    isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    (0UL),                  /* [64] Reserved */
    (0UL),                  /* [65] Reserved */
    (0UL),                  /* [66] Reserved */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    (0UL),                  /* [74] Reserved */
    (0UL),                  /* [75] Reserved */
    (0UL),                  /* [76] Reserved */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu,                /* [81] FPU */
    (0UL),                  /* [82] Reserved */
    (0UL),                  /* [83] Reserved */
    isr_spi4                /* [84] SPI4 */
#elif defined(CPU_MODEL_STM32F410RB)
    (0UL),                  /* [19] Reserved */
    (0UL),                  /* [20] Reserved */
    (0UL),                  /* [21] Reserved */
    (0UL),                  /* [22] Reserved */
    isr_exti,               /* [23] External Line[9:5]s */
    isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    (0UL),                  /* [28] Reserved */
    (0UL),                  /* [29] Reserved */
    (0UL),                  /* [30] Reserved */
    isr_i2c1_ev,            /* [31] I2C1 Event */
    isr_i2c1_er,            /* [32] I2C1 Error */
    isr_i2c2_ev,            /* [33] I2C2 Event */
    isr_i2c2_er,            /* [34] I2C2 Error */
    isr_spi1,               /* [35] SPI1 */
    isr_spi2,               /* [36] SPI2 */
    isr_usart1,             /* [37] USART1 */
    isr_usart2,             /* [38] USART2 */
    (0UL),                  /* [39] Reserved */
    isr_exti,               /* [40] External Line[15:10]s */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    (0UL),                  /* [42] Reserved */
    (0UL),                  /* [43] Reserved */
    (0UL),                  /* [44] Reserved */
    (0UL),                  /* [45] Reserved */
    (0UL),                  /* [46] Reserved */
    isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    (0UL),                  /* [48] Reserved */
    (0UL),                  /* [49] Reserved */
    isr_tim5,               /* [50] TIM5 */
    (0UL),                  /* [51] Reserved */
    (0UL),                  /* [52] Reserved */
    (0UL),                  /* [53] Reserved */
    isr_tim6_dac,           /* [54] TIM6 and DAC1&2 underrun errors */
    (0UL),                  /* [55] Reserved */
    isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    (0UL),                  /* [64] Reserved */
    (0UL),                  /* [65] Reserved */
    (0UL),                  /* [66] Reserved */
    (0UL),                  /* [67] Reserved */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    (0UL),                  /* [72] Reserved */
    (0UL),                  /* [73] Reserved */
    (0UL),                  /* [74] Reserved */
    (0UL),                  /* [75] Reserved */
    (0UL),                  /* [76] Reserved */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    isr_hash_rng,           /* [80] Hash and Rng */
    isr_fpu,                /* [81] FPU */
    (0UL),                  /* [82] Reserved */
    (0UL),                  /* [83] Reserved */
    (0UL),                  /* [84] Reserved */
    isr_spi5,               /* [85] SPI5 */
    (0UL),                  /* [86] Reserved */
    (0UL),                  /* [87] Reserved */
    (0UL),                  /* [88] Reserved */
    (0UL),                  /* [89] Reserved */
    (0UL),                  /* [90] Reserved */
    (0UL),                  /* [91] Reserved */
    (0UL),                  /* [92] Reserved */
    (0UL),                  /* [93] Reserved */
    (0UL),                  /* [94] Reserved */
    isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    isr_fmpi2c1_er,         /* [96] FMPI2C1 Error */
    isr_lptim1              /* [97] LP TIM1 */
#elif defined(CPU_MODEL_STM32F411RE)
    (0UL),                  /* [19] Reserved */
    (0UL),                  /* [20] Reserved */
    (0UL),                  /* [21] Reserved */
    (0UL),                  /* [22] Reserved */
    isr_exti,               /* [23] External Line[9:5]s */
    isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    isr_tim2,               /* [28] TIM2 */
    isr_tim3,               /* [29] TIM3 */
    isr_tim4,               /* [30] TIM4 */
    isr_i2c1_ev,            /* [31] I2C1 Event */
    isr_i2c1_er,            /* [32] I2C1 Error */
    isr_i2c2_ev,            /* [33] I2C2 Event */
    isr_i2c2_er,            /* [34] I2C2 Error */
    isr_spi1,               /* [35] SPI1 */
    isr_spi2,               /* [36] SPI2 */
    isr_usart1,             /* [37] USART1 */
    isr_usart2,             /* [38] USART2 */
    isr_usart3,             /* [39] USART3 */
    isr_exti,               /* [40] External Line[15:10]s */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    isr_otg_fs_wkup,        /* [42] USB OTG FS Wakeup through EXTI line */
    (0UL),                  /* [43] Reserved */
    (0UL),                  /* [44] Reserved */
    (0UL),                  /* [45] Reserved */
    isr_tim8_cc,            /* [46] TIM8 Capture Compare */
    isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    (0UL),                  /* [22] Reserved */
    isr_sdio,               /* [49] SDIO */
    isr_tim5,               /* [50] TIM5 */
    isr_spi3,               /* [51] SPI3 */
    (0UL),                  /* [52] Reserved */
    (0UL),                  /* [53] Reserved */
    (0UL),                  /* [54] Reserved */
    (0UL),                  /* [55] Reserved */
    isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    (0UL),                  /* [63] Reserved */
    (0UL),                  /* [64] Reserved */
    (0UL),                  /* [65] Reserved */
    (0UL),                  /* [66] Reserved */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    (0UL),                  /* [74] Reserved */
    (0UL),                  /* [75] Reserved */
    (0UL),                  /* [76] Reserved */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu,                /* [81] FPU */
    (0UL),                  /* [82] Reserved */
    (0UL),                  /* [83] Reserved */
    isr_spi4,               /* [84] SPI4 */
    isr_spi5                /* [85] SPI5 */
#else /* CPU_MODEL_STM32F413ZH, CPU_MODEL_STM32F413VG, CPU_MODEL_STM32F407VG
         CPU_MODEL_STM32F415RG, CPU_MODEL_STM32F412ZG, CPU_MODEL_STM32F446RE,
         CPU_MODEL_STM32F4446ZE */
    isr_can1_tx,            /* [19] CAN1 TX */
    isr_can1_rx0,           /* [20] CAN1 RX0 */
    isr_can1_rx1,           /* [21] CAN1 RX1 */
    isr_can1_sce,           /* [22] CAN1 SCE */
    isr_exti,               /* [23] External Line[9:5]s */
    isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    isr_tim2,               /* [28] TIM2 */
    isr_tim3,               /* [29] TIM3 */
    isr_tim4,               /* [30] TIM4 */
    isr_i2c1_ev,            /* [31] I2C1 Event */
    isr_i2c1_er,            /* [32] I2C1 Error */
    isr_i2c2_ev,            /* [33] I2C2 Event */
    isr_i2c2_er,            /* [34] I2C2 Error */
    isr_spi1,               /* [35] SPI1 */
    isr_spi2,               /* [36] SPI2 */
    isr_usart1,             /* [37] USART1 */
    isr_usart2,             /* [38] USART2 */
    isr_usart3,             /* [39] USART3 */
    isr_exti,               /* [40] External Line[15:10]s */
    isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    isr_otg_fs_wkup,        /* [42] USB OTG FS Wakeup through EXTI line */
    isr_tim8_brk_tim12,     /* [43] TIM8 Break and TIM12 */
    isr_tim8_up_tim13,      /* [44] TIM8 Update and TIM13 */
    isr_tim8_trg_com_tim14, /* [45] TIM8 Trigger and Commutation and TIM14 */
    isr_tim8_cc,            /* [46] TIM8 Capture Compare */
    isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    isr_fsmc,               /* [48] FSMC */
    isr_sdio,               /* [49] SDIO */
    isr_tim5,               /* [50] TIM5 */
    isr_spi3,               /* [51] SPI3 */
    isr_uart4,              /* [52] UART4 */
    isr_uart5,              /* [53] UART5 */
    isr_tim6_dac,           /* [54] TIM6 and DAC1&2 underrun errors */
    isr_tim7,               /* [55] TIM7 */
    isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
#if defined(CPU_MODEL_STM32F407VG)
    isr_eth,                /* [61] Ethernet */
    isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    isr_can2_tx,            /* [63] CAN2 TX */
    isr_can2_rx0,           /* [64] CAN2 RX0 */
    isr_can2_rx1,           /* [65] CAN2 RX1 */
    isr_can2_sce,           /* [66] CAN2 SCE */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    isr_otg_hs,             /* [77] USB OTG HS */
    isr_dcmi,               /* [78] DCMI */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu                 /* [81] FPU */
#elif defined(CPU_MODEL_STM32F415RG)
    isr_eth,                /* [61] Ethernet */
    isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    isr_can2_tx,            /* [63] CAN2 TX */
    isr_can2_rx0,           /* [64] CAN2 RX0 */
    isr_can2_rx1,           /* [65] CAN2 RX1 */
    isr_can2_sce,           /* [66] CAN2 SCE */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    isr_otg_hs,             /* [77] USB OTG HS */
    isr_dcmi,               /* [78] DCMI */
    isr_cryp,               /* [79] CRYP crypto */
    isr_hash_rng,           /* [80] Hash and Rng */
    isr_fpu                 /* [81] FPU */
#elif defined(CPU_MODEL_STM32F412ZG)
    isr_eth,                /* [61] Ethernet */
    isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    isr_can2_tx,            /* [63] CAN2 TX */
    isr_can2_rx0,           /* [64] CAN2 RX0 */
    isr_can2_rx1,           /* [65] CAN2 RX1 */
    isr_can2_sce,           /* [66] CAN2 SCE */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    (0UL),                  /* [74] Reserved */
    (0UL),                  /* [75] Reserved */
    (0UL),                  /* [76] Reserved */
    (0UL),                  /* [77] Reserved */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    isr_rng,                /* [80] Rng */
    isr_fpu,                /* [81] FPU */
    (0UL),                  /* [82] Reserved */
    (0UL),                  /* [83] Reserved */
    isr_spi4,               /* [84] SPI4 */
    isr_spi5,               /* [85] SPI5 */
    (0UL),                  /* [86] Reserved */
    (0UL),                  /* [87] Reserved */
    (0UL),                  /* [88] Reserved */
    (0UL),                  /* [89] Reserved */
    (0UL),                  /* [90] Reserved */
    (0UL),                  /* [91] Reserved */
    isr_quadspi,            /* [92] QuadSPI */
    (0UL),                  /* [93] Reserved */
    (0UL),                  /* [94] Reserved */
    isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    isr_fmpi2c1_er          /* [96] FMPI2C1 Error */
#elif defined(CPU_MODEL_STM32F446RE) || defined(CPU_MODEL_STM32F446ZE)
    (0UL),                  /* [61] Reserved */
    (0UL),                  /* [62] Reserved */
    isr_can2_tx,            /* [63] CAN2 TX */
    isr_can2_rx0,           /* [64] CAN2 RX0 */
    isr_can2_rx1,           /* [65] CAN2 RX1 */
    isr_can2_sce,           /* [66] CAN2 SCE */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    isr_otg_hs,             /* [77] USB OTG HS */
    isr_dcmi,               /* [78] DCMI */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [79] Reserved */
    isr_fpu,                /* [81] FPU */
    (0UL),                  /* [82] Reserved */
    (0UL),                  /* [83] Reserved */
    isr_spi4,               /* [84] SPI4 */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [86] Reserved */
    isr_sai1,               /* [87] SAI1 */
    (0UL),                  /* [88] Reserved */
    (0UL),                  /* [89] Reserved */
    (0UL),                  /* [90] Reserved */
    isr_sai2,               /* [91] SAI2 */
    isr_quadspi,            /* [92] QuadSPI */
    isr_cec,                /* [93] CEC */
    isr_spdif_rx,           /* [94] SPDIF-RX */
    isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    isr_fmpi2c1_er          /* [96] FMPI2C1 Error */
#elif defined(CPU_MODEL_STM32F413ZH) || defined(CPU_MODEL_STM32F413VG)
    isr_eth,                /* [61] Ethernet */
    isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    isr_can2_tx,            /* [63] CAN2 TX */
    isr_can2_rx0,           /* [64] CAN2 RX0 */
    isr_can2_rx1,           /* [65] CAN2 RX1 */
    isr_can2_sce,           /* [66] CAN2 SCE */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    isr_can3_tx,            /* [74] CAN3 TX */
    isr_can3_rx0,           /* [75] CAN3 RX0 */
    isr_can3_rx1,           /* [76] CAN3 RX1 */
    isr_can3_sce,           /* [77] CAN3 SCE */
    (0UL),                  /* [78] Reserved */
    (0UL),                  /* [79] Reserved */
    isr_hash_rng,           /* [80] Hash and Rng */
    isr_fpu,                /* [81] FPU */
    isr_usart7,             /* [82] USART7 */
    isr_usart8,             /* [83] USART8 */
    isr_spi4,               /* [84] SPI4 */
    isr_spi5,               /* [85] SPI5 */
    (0UL),                  /* [86] Reserved */
    isr_sai1,               /* [87] SAI1 */
    isr_uart9,              /* [88] UART9 */
    isr_uart10,             /* [89] UART10 */
    (0UL),                  /* [90] Reserved */
    (0UL),                  /* [91] Reserved */
    isr_quadspi,            /* [92] QuadSPI */
    (0UL),                  /* [93] Reserved */
    (0UL),                  /* [94] Reserved */
    isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    isr_fmpi2c1_er,         /* [96] FMPI2C1 Error */
    isr_lptim1,             /* [97] LP TIM1 */
    isr_dfsdm2_flt0,        /* [98] DFSDM2 Filter 0 */
    isr_dfsdm2_flt1,        /* [99] DFSDM2 Filter 1 */
    isr_dfsdm2_flt2,        /* [100] DFSDM2 Filter 2 */
    isr_dfsdm2_flt3         /* [101] DFSDM2 Filter 3 */
#else /* CPU_MODEL_STM32F429ZI */
    isr_eth,                /* [61] Ethernet */
    isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    isr_can2_tx,            /* [63] CAN2 TX */
    isr_can2_rx0,           /* [64] CAN2 RX0 */
    isr_can2_rx1,           /* [65] CAN2 RX1 */
    isr_can2_sce,           /* [66] CAN2 SCE */
    isr_otg_fs,             /* [67] USB OTG FS */
    isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    isr_usart6,             /* [71] USART6 */
    isr_i2c3_ev,            /* [72] I2C3 event */
    isr_i2c3_er,            /* [73] I2C3 error */
    isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    isr_otg_hs,             /* [77] USB OTG HS */
    isr_dcmi,               /* [78] DCMI */
    (0UL),                  /* [79] Reserved */
    (0UL),                  /* [80] Reserved */
    isr_fpu,                /* [81] FPU */
    isr_usart7,             /* [82] USART7 */
    isr_usart8,             /* [83] USART8 */
    isr_spi4,               /* [84] SPI4 */
    isr_spi5,               /* [85] SPI5 */
    isr_spi6,               /* [86] SPI6 */
    isr_sai1,               /* [87] SAI1 */
    isr_ltdc,               /* [88] LDTC */
    isr_ltdc_er,            /* [89] LTDC Error */
    isr_dma2d               /* [90] Reserved */
#endif
#endif
};
