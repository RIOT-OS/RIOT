/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2017-2018 OTA keys S.A.
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

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_can1_rx0(void);
WEAK_DEFAULT void isr_can1_rx1(void);
WEAK_DEFAULT void isr_can1_sce(void);
WEAK_DEFAULT void isr_can1_tx(void);
WEAK_DEFAULT void isr_can2_rx0(void);
WEAK_DEFAULT void isr_can2_rx1(void);
WEAK_DEFAULT void isr_can2_sce(void);
WEAK_DEFAULT void isr_can2_tx(void);
WEAK_DEFAULT void isr_can3_rx0(void);
WEAK_DEFAULT void isr_can3_rx1(void);
WEAK_DEFAULT void isr_can3_sce(void);
WEAK_DEFAULT void isr_can3_tx(void);
WEAK_DEFAULT void isr_cec(void);
WEAK_DEFAULT void isr_cryp(void);
WEAK_DEFAULT void isr_dcmi(void);
WEAK_DEFAULT void isr_dfsdm1_flt0(void);
WEAK_DEFAULT void isr_dfsdm1_flt1(void);
WEAK_DEFAULT void isr_dfsdm2_flt0(void);
WEAK_DEFAULT void isr_dfsdm2_flt1(void);
WEAK_DEFAULT void isr_dfsdm2_flt2(void);
WEAK_DEFAULT void isr_dfsdm2_flt3(void);
WEAK_DEFAULT void isr_dma1_stream0(void);
WEAK_DEFAULT void isr_dma1_stream1(void);
WEAK_DEFAULT void isr_dma1_stream2(void);
WEAK_DEFAULT void isr_dma1_stream3(void);
WEAK_DEFAULT void isr_dma1_stream4(void);
WEAK_DEFAULT void isr_dma1_stream5(void);
WEAK_DEFAULT void isr_dma1_stream6(void);
WEAK_DEFAULT void isr_dma1_stream7(void);
WEAK_DEFAULT void isr_dma2_stream0(void);
WEAK_DEFAULT void isr_dma2_stream1(void);
WEAK_DEFAULT void isr_dma2_stream2(void);
WEAK_DEFAULT void isr_dma2_stream3(void);
WEAK_DEFAULT void isr_dma2_stream4(void);
WEAK_DEFAULT void isr_dma2_stream5(void);
WEAK_DEFAULT void isr_dma2_stream6(void);
WEAK_DEFAULT void isr_dma2_stream7(void);
WEAK_DEFAULT void isr_dma2d(void);
WEAK_DEFAULT void isr_eth(void);
WEAK_DEFAULT void isr_eth_wkup(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_fmpi2c1_er(void);
WEAK_DEFAULT void isr_fmpi2c1_ev(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_fsmc(void);
WEAK_DEFAULT void isr_hash_rng(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_ltdc(void);
WEAK_DEFAULT void isr_ltdc_er(void);
WEAK_DEFAULT void isr_otg_fs(void);
WEAK_DEFAULT void isr_otg_fs_wkup(void);
WEAK_DEFAULT void isr_otg_hs(void);
WEAK_DEFAULT void isr_otg_hs_ep1_in(void);
WEAK_DEFAULT void isr_otg_hs_ep1_out(void);
WEAK_DEFAULT void isr_otg_hs_wkup(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_sai2(void);
WEAK_DEFAULT void isr_sdio(void);
WEAK_DEFAULT void isr_spdif_rx(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_spi4(void);
WEAK_DEFAULT void isr_spi5(void);
WEAK_DEFAULT void isr_spi6(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
WEAK_DEFAULT void isr_tim1_brk_tim9(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim11(void);
WEAK_DEFAULT void isr_tim1_up(void);
WEAK_DEFAULT void isr_tim1_up_tim10(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_tim6(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim8_brk_tim12(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_tim8_trg_com_tim14(void);
WEAK_DEFAULT void isr_tim8_up_tim13(void);
WEAK_DEFAULT void isr_uart10(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_uart7(void);
WEAK_DEFAULT void isr_uart8(void);
WEAK_DEFAULT void isr_uart9(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_usart6(void);
WEAK_DEFAULT void isr_wwdg(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    /* shared vectors for all family members */
    [ 0] = isr_wwdg,                 /* [ 0] Window WatchDog Interrupt */
    [ 1] = isr_pvd,                  /* [ 1] PVD through EXTI Line detection Interrupt */
    [ 2] = isr_tamp_stamp,           /* [ 2] Tamper and TimeStamp interrupts through the EXTI line */
    [ 3] = isr_rtc_wkup,             /* [ 3] RTC Wakeup interrupt through the EXTI line */
    [ 4] = isr_flash,                /* [ 4] FLASH global Interrupt */
    [ 5] = isr_rcc,                  /* [ 5] RCC global Interrupt */
    [ 6] = isr_exti,                 /* [ 6] EXTI Line0 Interrupt */
    [ 7] = isr_exti,                 /* [ 7] EXTI Line1 Interrupt */
    [ 8] = isr_exti,                 /* [ 8] EXTI Line2 Interrupt */
    [ 9] = isr_exti,                 /* [ 9] EXTI Line3 Interrupt */
    [10] = isr_exti,                 /* [10] EXTI Line4 Interrupt */
    [11] = isr_dma1_stream0,         /* [11] DMA1 Stream 0 global Interrupt */
    [12] = isr_dma1_stream1,         /* [12] DMA1 Stream 1 global Interrupt */
    [13] = isr_dma1_stream2,         /* [13] DMA1 Stream 2 global Interrupt */
    [14] = isr_dma1_stream3,         /* [14] DMA1 Stream 3 global Interrupt */
    [15] = isr_dma1_stream4,         /* [15] DMA1 Stream 4 global Interrupt */
    [16] = isr_dma1_stream5,         /* [16] DMA1 Stream 5 global Interrupt */
    [17] = isr_dma1_stream6,         /* [17] DMA1 Stream 6 global Interrupt */
    [23] = isr_exti,                 /* [23] External Line[9:5] Interrupts */
    [24] = isr_tim1_brk_tim9,        /* [24] TIM1 Break interrupt and TIM9 global interrupt */
    [26] = isr_tim1_trg_com_tim11,   /* [26] TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
    [27] = isr_tim1_cc,              /* [27] TIM1 Capture Compare Interrupt */
    [31] = isr_i2c1_ev,              /* [31] I2C1 Event Interrupt */
    [32] = isr_i2c1_er,              /* [32] I2C1 Error Interrupt */
    [33] = isr_i2c2_ev,              /* [33] I2C2 Event Interrupt */
    [34] = isr_i2c2_er,              /* [34] I2C2 Error Interrupt */
    [35] = isr_spi1,                 /* [35] SPI1 global Interrupt */
    [36] = isr_spi2,                 /* [36] SPI2 global Interrupt */
    [37] = isr_usart1,               /* [37] USART1 global Interrupt */
    [38] = isr_usart2,               /* [38] USART2 global Interrupt */
    [40] = isr_exti,                 /* [40] External Line[15:10] Interrupts */
    [41] = isr_rtc_alarm,            /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    [47] = isr_dma1_stream7,         /* [47] DMA1 Stream7 Interrupt */
    [50] = isr_tim5,                 /* [50] TIM5 global Interrupt */
    [56] = isr_dma2_stream0,         /* [56] DMA2 Stream 0 global Interrupt */
    [57] = isr_dma2_stream1,         /* [57] DMA2 Stream 1 global Interrupt */
    [58] = isr_dma2_stream2,         /* [58] DMA2 Stream 2 global Interrupt */
    [59] = isr_dma2_stream3,         /* [59] DMA2 Stream 3 global Interrupt */
    [60] = isr_dma2_stream4,         /* [60] DMA2 Stream 4 global Interrupt */
    [68] = isr_dma2_stream5,         /* [68] DMA2 Stream 5 global interrupt */
    [69] = isr_dma2_stream6,         /* [69] DMA2 Stream 6 global interrupt */
    [70] = isr_dma2_stream7,         /* [70] DMA2 Stream 7 global interrupt */
    [71] = isr_usart6,               /* [71] USART6 global interrupt */
    [81] = isr_fpu,                  /* [81] FPU global interrupt */

#if defined(CPU_LINE_STM32F401xE)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
#elif defined(CPU_LINE_STM32F405xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [48] = isr_fsmc,                 /* [48] FSMC global Interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_otg_hs_ep1_out,       /* [74] USB OTG HS End Point 1 Out global interrupt */
    [75] = isr_otg_hs_ep1_in,        /* [75] USB OTG HS End Point 1 In global interrupt */
    [76] = isr_otg_hs_wkup,          /* [76] USB OTG HS Wakeup through EXTI interrupt */
    [77] = isr_otg_hs,               /* [77] USB OTG HS global interrupt */
    [78] = isr_dcmi,                 /* [78] DCMI global interrupt */
    [80] = isr_rng,                  /* [80] RNG global Interrupt */
#elif defined(CPU_LINE_STM32F407xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [48] = isr_fsmc,                 /* [48] FSMC global Interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [61] = isr_eth,                  /* [61] Ethernet global Interrupt */
    [62] = isr_eth_wkup,             /* [62] Ethernet Wakeup through EXTI line Interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_otg_hs_ep1_out,       /* [74] USB OTG HS End Point 1 Out global interrupt */
    [75] = isr_otg_hs_ep1_in,        /* [75] USB OTG HS End Point 1 In global interrupt */
    [76] = isr_otg_hs_wkup,          /* [76] USB OTG HS Wakeup through EXTI interrupt */
    [77] = isr_otg_hs,               /* [77] USB OTG HS global interrupt */
    [78] = isr_dcmi,                 /* [78] DCMI global interrupt */
#elif defined(CPU_LINE_STM32F410Rx)
    [18] = isr_adc,                  /* [18] ADC1 global Interrupts */
    [25] = isr_tim1_up,              /* [25] TIM1 Update Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global Interrupt and DAC Global Interrupt */
    [80] = isr_rng,                  /* [80] RNG global Interrupt */
    [85] = isr_spi5,                 /* [85] SPI5 global Interrupt */
    [95] = isr_fmpi2c1_ev,           /* [95] FMPI2C1 Event Interrupt */
    [96] = isr_fmpi2c1_er,           /* [96] FMPI2C1 Error Interrupt */
    [97] = isr_lptim1,               /* [97] LPTIM1 interrupt */
#elif defined(CPU_LINE_STM32F411xE)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
    [85] = isr_spi5,                 /* [85] SPI5 global Interrupt */
#elif defined(CPU_LINE_STM32F412Zx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [54] = isr_tim6,                 /* [54] TIM6 global interrupt */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [61] = isr_dfsdm1_flt0,          /* [61] DFSDM1 Filter 0 global Interrupt */
    [62] = isr_dfsdm1_flt1,          /* [62] DFSDM1 Filter 1 global Interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [80] = isr_rng,                  /* [80] RNG global Interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
    [85] = isr_spi5,                 /* [85] SPI5 global Interrupt */
    [92] = isr_quadspi,              /* [92] QuadSPI global Interrupt */
    [95] = isr_fmpi2c1_ev,           /* [95] FMPI2C1 Event Interrupt */
    [96] = isr_fmpi2c1_er,           /* [96] FMPI2C1 Error Interrupt */
#elif defined(CPU_LINE_STM32F413xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [61] = isr_dfsdm1_flt0,          /* [61] DFSDM1 Filter 0 global Interrupt */
    [62] = isr_dfsdm1_flt1,          /* [62] DFSDM1 Filter 1 global Interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_can3_tx,              /* [74] CAN3 TX Interrupt */
    [75] = isr_can3_rx0,             /* [75] CAN3 RX0 Interrupt */
    [76] = isr_can3_rx1,             /* [76] CAN3 RX1 Interrupt */
    [77] = isr_can3_sce,             /* [77] CAN3 SCE Interrupt */
    [80] = isr_rng,                  /* [80] RNG global Interrupt */
    [82] = isr_uart7,                /* [82] UART7 global interrupt */
    [83] = isr_uart8,                /* [83] UART8 global interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
    [85] = isr_spi5,                 /* [85] SPI5 global Interrupt */
    [87] = isr_sai1,                 /* [87] SAI1 global Interrupt */
    [88] = isr_uart9,                /* [88] UART9 global Interrupt */
    [89] = isr_uart10,               /* [89] UART10 global Interrupt */
    [92] = isr_quadspi,              /* [92] QuadSPI global Interrupt */
    [95] = isr_fmpi2c1_ev,           /* [95] FMPI2C1 Event Interrupt */
    [96] = isr_fmpi2c1_er,           /* [96] FMPI2C1 Error Interrupt */
    [97] = isr_lptim1,               /* [97] LP TIM1 interrupt */
    [98] = isr_dfsdm2_flt0,          /* [98] DFSDM2 Filter 0 global Interrupt */
    [99] = isr_dfsdm2_flt1,          /* [99] DFSDM2 Filter 1 global Interrupt */
    [100] = isr_dfsdm2_flt2,          /* [100] DFSDM2 Filter 2 global Interrupt */
    [101] = isr_dfsdm2_flt3,          /* [101] DFSDM2 Filter 3 global Interrupt */
#elif defined(CPU_LINE_STM32F415xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [48] = isr_fsmc,                 /* [48] FSMC global Interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_otg_hs_ep1_out,       /* [74] USB OTG HS End Point 1 Out global interrupt */
    [75] = isr_otg_hs_ep1_in,        /* [75] USB OTG HS End Point 1 In global interrupt */
    [76] = isr_otg_hs_wkup,          /* [76] USB OTG HS Wakeup through EXTI interrupt */
    [77] = isr_otg_hs,               /* [77] USB OTG HS global interrupt */
    [79] = isr_cryp,                 /* [79] CRYP crypto global interrupt */
    [80] = isr_hash_rng,             /* [80] Hash and Rng global interrupt */
#elif defined(CPU_LINE_STM32F423xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [61] = isr_dfsdm1_flt0,          /* [61] DFSDM1 Filter 0 global Interrupt */
    [62] = isr_dfsdm1_flt1,          /* [62] DFSDM1 Filter 1 global Interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_can3_tx,              /* [74] CAN3 TX Interrupt */
    [75] = isr_can3_rx0,             /* [75] CAN3 RX0 Interrupt */
    [76] = isr_can3_rx1,             /* [76] CAN3 RX1 Interrupt */
    [77] = isr_can3_sce,             /* [77] CAN3 SCE Interrupt */
    [79] = isr_aes,                  /* [79] AES global Interrupt */
    [80] = isr_rng,                  /* [80] RNG global Interrupt */
    [82] = isr_uart7,                /* [82] UART7 global interrupt */
    [83] = isr_uart8,                /* [83] UART8 global interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
    [85] = isr_spi5,                 /* [85] SPI5 global Interrupt */
    [87] = isr_sai1,                 /* [87] SAI1 global Interrupt */
    [88] = isr_uart9,                /* [88] UART9 global Interrupt */
    [89] = isr_uart10,               /* [89] UART10 global Interrupt */
    [92] = isr_quadspi,              /* [92] QuadSPI global Interrupt */
    [95] = isr_fmpi2c1_ev,           /* [95] FMPI2C1 Event Interrupt */
    [96] = isr_fmpi2c1_er,           /* [96] FMPI2C1 Error Interrupt */
    [97] = isr_lptim1,               /* [97] LP TIM1 interrupt */
    [98] = isr_dfsdm2_flt0,          /* [98] DFSDM2 Filter 0 global Interrupt */
    [99] = isr_dfsdm2_flt1,          /* [99] DFSDM2 Filter 1 global Interrupt */
    [100] = isr_dfsdm2_flt2,          /* [100] DFSDM2 Filter 2 global Interrupt */
    [101] = isr_dfsdm2_flt3,          /* [101] DFSDM2 Filter 3 global Interrupt */
#elif defined(CPU_LINE_STM32F429xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [48] = isr_fmc,                  /* [48] FMC global Interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [61] = isr_eth,                  /* [61] Ethernet global Interrupt */
    [62] = isr_eth_wkup,             /* [62] Ethernet Wakeup through EXTI line Interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_otg_hs_ep1_out,       /* [74] USB OTG HS End Point 1 Out global interrupt */
    [75] = isr_otg_hs_ep1_in,        /* [75] USB OTG HS End Point 1 In global interrupt */
    [76] = isr_otg_hs_wkup,          /* [76] USB OTG HS Wakeup through EXTI interrupt */
    [77] = isr_otg_hs,               /* [77] USB OTG HS global interrupt */
    [78] = isr_dcmi,                 /* [78] DCMI global interrupt */
    [82] = isr_uart7,                /* [82] UART7 global interrupt */
    [83] = isr_uart8,                /* [83] UART8 global interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
    [85] = isr_spi5,                 /* [85] SPI5 global Interrupt */
    [86] = isr_spi6,                 /* [86] SPI6 global Interrupt */
    [87] = isr_sai1,                 /* [87] SAI1 global Interrupt */
    [88] = isr_ltdc,                 /* [88] LTDC global Interrupt */
    [89] = isr_ltdc_er,              /* [89] LTDC Error global Interrupt */
    [90] = isr_dma2d,                /* [90] DMA2D global Interrupt */
#elif defined(CPU_LINE_STM32F437xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [48] = isr_fmc,                  /* [48] FMC global Interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [61] = isr_eth,                  /* [61] Ethernet global Interrupt */
    [62] = isr_eth_wkup,             /* [62] Ethernet Wakeup through EXTI line Interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_otg_hs_ep1_out,       /* [74] USB OTG HS End Point 1 Out global interrupt */
    [75] = isr_otg_hs_ep1_in,        /* [75] USB OTG HS End Point 1 In global interrupt */
    [76] = isr_otg_hs_wkup,          /* [76] USB OTG HS Wakeup through EXTI interrupt */
    [77] = isr_otg_hs,               /* [77] USB OTG HS global interrupt */
    [78] = isr_dcmi,                 /* [78] DCMI global interrupt */
    [79] = isr_cryp,                 /* [79] CRYP crypto global interrupt */
    [80] = isr_hash_rng,             /* [80] Hash and Rng global interrupt */
    [82] = isr_uart7,                /* [82] UART7 global interrupt */
    [83] = isr_uart8,                /* [83] UART8 global interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
    [85] = isr_spi5,                 /* [85] SPI5 global Interrupt */
    [86] = isr_spi6,                 /* [86] SPI6 global Interrupt */
    [87] = isr_sai1,                 /* [87] SAI1 global Interrupt */
    [90] = isr_dma2d,                /* [90] DMA2D global Interrupt */
#elif defined(CPU_LINE_STM32F446xx)
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [28] = isr_tim2,                 /* [28] TIM2 global Interrupt */
    [29] = isr_tim3,                 /* [29] TIM3 global Interrupt */
    [30] = isr_tim4,                 /* [30] TIM4 global Interrupt */
    [39] = isr_usart3,               /* [39] USART3 global Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare global interrupt */
    [48] = isr_fmc,                  /* [48] FMC global Interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_otg_hs_ep1_out,       /* [74] USB OTG HS End Point 1 Out global interrupt */
    [75] = isr_otg_hs_ep1_in,        /* [75] USB OTG HS End Point 1 In global interrupt */
    [76] = isr_otg_hs_wkup,          /* [76] USB OTG HS Wakeup through EXTI interrupt */
    [77] = isr_otg_hs,               /* [77] USB OTG HS global interrupt */
    [78] = isr_dcmi,                 /* [78] DCMI global interrupt */
    [84] = isr_spi4,                 /* [84] SPI4 global Interrupt */
    [87] = isr_sai1,                 /* [87] SAI1 global Interrupt */
    [91] = isr_sai2,                 /* [91] SAI2 global Interrupt */
    [92] = isr_quadspi,              /* [92] QuadSPI global Interrupt */
    [93] = isr_cec,                  /* [93] CEC global Interrupt */
    [94] = isr_spdif_rx,             /* [94] SPDIF-RX global Interrupt */
    [95] = isr_fmpi2c1_ev,           /* [95] FMPI2C1 Event Interrupt */
    [96] = isr_fmpi2c1_er,           /* [96] FMPI2C1 Error Interrupt */
#endif
};
