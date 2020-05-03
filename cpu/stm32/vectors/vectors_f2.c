/*
 * Copyright (C) 2015 Engineering-Spirit
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32F2 specific interrupt vectors */
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_can1_rx0(void);
WEAK_DEFAULT void isr_can1_rx1(void);
WEAK_DEFAULT void isr_can1_sce(void);
WEAK_DEFAULT void isr_can1_tx(void);
WEAK_DEFAULT void isr_can2_rx0(void);
WEAK_DEFAULT void isr_can2_rx1(void);
WEAK_DEFAULT void isr_can2_sce(void);
WEAK_DEFAULT void isr_can2_tx(void);
WEAK_DEFAULT void isr_cryp(void);
WEAK_DEFAULT void isr_dcmi(void);
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
WEAK_DEFAULT void isr_eth(void);
WEAK_DEFAULT void isr_eth_wkup(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_fsmc(void);
WEAK_DEFAULT void isr_hash_rng(void);
WEAK_DEFAULT void isr_i2c1_er(void);
WEAK_DEFAULT void isr_i2c1_ev(void);
WEAK_DEFAULT void isr_i2c2_er(void);
WEAK_DEFAULT void isr_i2c2_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_otg_fs(void);
WEAK_DEFAULT void isr_otg_fs_wkup(void);
WEAK_DEFAULT void isr_otg_hs(void);
WEAK_DEFAULT void isr_otg_hs_ep1_in(void);
WEAK_DEFAULT void isr_otg_hs_ep1_out(void);
WEAK_DEFAULT void isr_otg_hs_wkup(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_sdio(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_tamp_stamp(void);
WEAK_DEFAULT void isr_tim1_brk_tim9(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim1_trg_com_tim11(void);
WEAK_DEFAULT void isr_tim1_up_tim10(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim4(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim8_brk_tim12(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_tim8_trg_com_tim14(void);
WEAK_DEFAULT void isr_tim8_up_tim13(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
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
    [18] = isr_adc,                  /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    [19] = isr_can1_tx,              /* [19] CAN1 TX Interrupt */
    [20] = isr_can1_rx0,             /* [20] CAN1 RX0 Interrupt */
    [21] = isr_can1_rx1,             /* [21] CAN1 RX1 Interrupt */
    [22] = isr_can1_sce,             /* [22] CAN1 SCE Interrupt */
    [23] = isr_exti,                 /* [23] External Line[9:5] Interrupts */
    [24] = isr_tim1_brk_tim9,        /* [24] TIM1 Break interrupt and TIM9 global interrupt */
    [25] = isr_tim1_up_tim10,        /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    [26] = isr_tim1_trg_com_tim11,   /* [26] TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
    [27] = isr_tim1_cc,              /* [27] TIM1 Capture Compare Interrupt */
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
    [41] = isr_rtc_alarm,            /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    [42] = isr_otg_fs_wkup,          /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    [43] = isr_tim8_brk_tim12,       /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    [44] = isr_tim8_up_tim13,        /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    [45] = isr_tim8_trg_com_tim14,   /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    [46] = isr_tim8_cc,              /* [46] TIM8 Capture Compare Interrupt */
    [47] = isr_dma1_stream7,         /* [47] DMA1 Stream7 Interrupt */
    [48] = isr_fsmc,                 /* [48] FSMC global Interrupt */
    [49] = isr_sdio,                 /* [49] SDIO global Interrupt */
    [50] = isr_tim5,                 /* [50] TIM5 global Interrupt */
    [51] = isr_spi3,                 /* [51] SPI3 global Interrupt */
    [52] = isr_uart4,                /* [52] UART4 global Interrupt */
    [53] = isr_uart5,                /* [53] UART5 global Interrupt */
    [54] = isr_tim6_dac,             /* [54] TIM6 global and DAC1&2 underrun error  interrupts */
    [55] = isr_tim7,                 /* [55] TIM7 global interrupt */
    [56] = isr_dma2_stream0,         /* [56] DMA2 Stream 0 global Interrupt */
    [57] = isr_dma2_stream1,         /* [57] DMA2 Stream 1 global Interrupt */
    [58] = isr_dma2_stream2,         /* [58] DMA2 Stream 2 global Interrupt */
    [59] = isr_dma2_stream3,         /* [59] DMA2 Stream 3 global Interrupt */
    [60] = isr_dma2_stream4,         /* [60] DMA2 Stream 4 global Interrupt */
    [63] = isr_can2_tx,              /* [63] CAN2 TX Interrupt */
    [64] = isr_can2_rx0,             /* [64] CAN2 RX0 Interrupt */
    [65] = isr_can2_rx1,             /* [65] CAN2 RX1 Interrupt */
    [66] = isr_can2_sce,             /* [66] CAN2 SCE Interrupt */
    [67] = isr_otg_fs,               /* [67] USB OTG FS global Interrupt */
    [68] = isr_dma2_stream5,         /* [68] DMA2 Stream 5 global interrupt */
    [69] = isr_dma2_stream6,         /* [69] DMA2 Stream 6 global interrupt */
    [70] = isr_dma2_stream7,         /* [70] DMA2 Stream 7 global interrupt */
    [71] = isr_usart6,               /* [71] USART6 global interrupt */
    [72] = isr_i2c3_ev,              /* [72] I2C3 event interrupt */
    [73] = isr_i2c3_er,              /* [73] I2C3 error interrupt */
    [74] = isr_otg_hs_ep1_out,       /* [74] USB OTG HS End Point 1 Out global interrupt */
    [75] = isr_otg_hs_ep1_in,        /* [75] USB OTG HS End Point 1 In global interrupt */
    [76] = isr_otg_hs_wkup,          /* [76] USB OTG HS Wakeup through EXTI interrupt */
    [77] = isr_otg_hs,               /* [77] USB OTG HS global interrupt */

#if defined(CPU_LINE_STM32F207xx)
    [61] = isr_eth,                  /* [61] Ethernet global Interrupt */
    [62] = isr_eth_wkup,             /* [62] Ethernet Wakeup through EXTI line Interrupt */
    [78] = isr_dcmi,                 /* [78] DCMI global interrupt */
    [80] = isr_hash_rng,             /* [80] Hash and RNG global interrupt */
#elif defined(CPU_LINE_STM32F215xx)
    [79] = isr_cryp,                 /* [79] CRYP crypto global interrupt */
    [80] = isr_hash_rng,             /* [80] Hash and RNG global interrupt */
#elif defined(CPU_LINE_STM32F217xx)
    [61] = isr_eth,                  /* [61] Ethernet global Interrupt */
    [62] = isr_eth_wkup,             /* [62] Ethernet Wakeup through EXTI line Interrupt */
    [78] = isr_dcmi,                 /* [78] DCMI global interrupt */
    [79] = isr_cryp,                 /* [79] CRYP crypto global interrupt */
    [80] = isr_hash_rng,             /* [80] Hash and Rng global interrupt */
#endif
};
