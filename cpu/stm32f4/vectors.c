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

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
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

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* [-14] non maskable interrupt handler */
    (void*) hard_fault_default,     /* [-13] hard fault exception */
    (void*) mem_manage_default,     /* [-12] memory manage exception */
    (void*) bus_fault_default,      /* [-11] bus fault exception */
    (void*) usage_fault_default,    /* [-10] usage fault exception */
    (void*) (0UL),                  /* [-9] Reserved */
    (void*) (0UL),                  /* [-8] Reserved */
    (void*) (0UL),                  /* [-7] Reserved */
    (void*) (0UL),                  /* [-6] Reserved */
    (void*) isr_svc,                /* [-5] system call interrupt, in RIOT used for
                                            switching into thread context on boot */
    (void*) debug_mon_default,      /* [-4] debug monitor exception */
    (void*) (0UL),                  /* [-3] Reserved */
    (void*) isr_pendsv,             /* [-2] pendSV interrupt, in RIOT the actual
                                            context switching is happening here */
    (void*) isr_systick,            /* [-1] SysTick interrupt, not used in RIOT */
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,               /* [0] Window WatchDog */
    (void*) isr_pvd,                /* [1] PVD through EXTI Line detection */
    (void*) isr_tamp_stamp,         /* [2] Tamper and TimeStamps through the EXTI line */
    (void*) isr_rtc_wkup,           /* [3] RTC Wakeup through the EXTI line */
    (void*) isr_flash,              /* [4] FLASH */
    (void*) isr_rcc,                /* [5] RCC */
    (void*) isr_exti,               /* [6] EXTI Line0 */
    (void*) isr_exti,               /* [7] EXTI Line1 */
    (void*) isr_exti,               /* [8] EXTI Line2 */
    (void*) isr_exti,               /* [9] EXTI Line3 */
    (void*) isr_exti,               /* [10] EXTI Line4 */
    (void*) isr_dma1_stream0,       /* [11] DMA1 Stream 0 */
    (void*) isr_dma1_stream1,       /* [12] DMA1 Stream 1 */
    (void*) isr_dma1_stream2,       /* [13] DMA1 Stream 2 */
    (void*) isr_dma1_stream3,       /* [14] DMA1 Stream 3 */
    (void*) isr_dma1_stream4,       /* [15] DMA1 Stream 4 */
    (void*) isr_dma1_stream5,       /* [16] DMA1 Stream 5 */
    (void*) isr_dma1_stream6,       /* [17] DMA1 Stream 6 */
    (void*) isr_adc,                /* [18] ADC1, ADC2 and ADC3s */
#if defined(CPU_MODEL_STM32F401RE)
    (void*) (0UL),                  /* [19] Reserved */
    (void*) (0UL),                  /* [20] Reserved */
    (void*) (0UL),                  /* [21] Reserved */
    (void*) (0UL),                  /* [22] Reserved */
    (void*) isr_exti,               /* [23] External Line[9:5]s */
    (void*) isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    (void*) isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    (void*) isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    (void*) isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    (void*) isr_tim2,               /* [28] TIM2 */
    (void*) isr_tim3,               /* [29] TIM3 */
    (void*) isr_tim4,               /* [30] TIM4 */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error */
    (void*) isr_i2c2_ev,            /* [33] I2C2 Event */
    (void*) isr_i2c2_er,            /* [34] I2C2 Error */
    (void*) isr_spi1,               /* [35] SPI1 */
    (void*) isr_spi2,               /* [36] SPI2 */
    (void*) isr_usart1,             /* [37] USART1 */
    (void*) isr_usart2,             /* [38] USART2 */
    (void*) (0UL),                  /* [39] Reserved */
    (void*) isr_exti,               /* [40] External Line[15:10]s */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    (void*) (0UL),                  /* [42] Reserved */
    (void*) (0UL),                  /* [43] Reserved */
    (void*) (0UL),                  /* [44] Reserved */
    (void*) (0UL),                  /* [45] Reserved */
    (void*) (0UL),                  /* [46] Reserved */
    (void*) isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    (void*) (0UL),                  /* [48] Reserved */
    (void*) isr_sdio,               /* [49] SDIO */
    (void*) isr_tim5,               /* [50] TIM5 */
    (void*) (0UL),                  /* [51] Reserved */
    (void*) (0UL),                  /* [52] Reserved */
    (void*) (0UL),                  /* [53] Reserved */
    (void*) (0UL),                  /* [54] Reserved */
    (void*) (0UL),                  /* [55] Reserved */
    (void*) isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    (void*) isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    (void*) isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    (void*) isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    (void*) isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) (0UL),                  /* [64] Reserved */
    (void*) (0UL),                  /* [65] Reserved */
    (void*) (0UL),                  /* [66] Reserved */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) (0UL),                  /* [74] Reserved */
    (void*) (0UL),                  /* [75] Reserved */
    (void*) (0UL),                  /* [76] Reserved */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu,                /* [81] FPU */
    (void*) (0UL),                  /* [82] Reserved */
    (void*) (0UL),                  /* [83] Reserved */
    (void*) isr_spi4                /* [84] SPI4 */
#elif defined(CPU_MODEL_STM32F410RB)
    (void*) (0UL),                  /* [19] Reserved */
    (void*) (0UL),                  /* [20] Reserved */
    (void*) (0UL),                  /* [21] Reserved */
    (void*) (0UL),                  /* [22] Reserved */
    (void*) isr_exti,               /* [23] External Line[9:5]s */
    (void*) isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    (void*) isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    (void*) isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    (void*) isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    (void*) (0UL),                  /* [28] Reserved */
    (void*) (0UL),                  /* [29] Reserved */
    (void*) (0UL),                  /* [30] Reserved */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error */
    (void*) isr_i2c2_ev,            /* [33] I2C2 Event */
    (void*) isr_i2c2_er,            /* [34] I2C2 Error */
    (void*) isr_spi1,               /* [35] SPI1 */
    (void*) isr_spi2,               /* [36] SPI2 */
    (void*) isr_usart1,             /* [37] USART1 */
    (void*) isr_usart2,             /* [38] USART2 */
    (void*) (0UL),                  /* [39] Reserved */
    (void*) isr_exti,               /* [40] External Line[15:10]s */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    (void*) (0UL),                  /* [42] Reserved */
    (void*) (0UL),                  /* [43] Reserved */
    (void*) (0UL),                  /* [44] Reserved */
    (void*) (0UL),                  /* [45] Reserved */
    (void*) (0UL),                  /* [46] Reserved */
    (void*) isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    (void*) (0UL),                  /* [48] Reserved */
    (void*) (0UL),                  /* [49] Reserved */
    (void*) isr_tim5,               /* [50] TIM5 */
    (void*) (0UL),                  /* [51] Reserved */
    (void*) (0UL),                  /* [52] Reserved */
    (void*) (0UL),                  /* [53] Reserved */
    (void*) isr_tim6_dac,           /* [54] TIM6 and DAC1&2 underrun errors */
    (void*) (0UL),                  /* [55] Reserved */
    (void*) isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    (void*) isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    (void*) isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    (void*) isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    (void*) isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) (0UL),                  /* [64] Reserved */
    (void*) (0UL),                  /* [65] Reserved */
    (void*) (0UL),                  /* [66] Reserved */
    (void*) (0UL),                  /* [67] Reserved */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) (0UL),                  /* [72] Reserved */
    (void*) (0UL),                  /* [73] Reserved */
    (void*) (0UL),                  /* [74] Reserved */
    (void*) (0UL),                  /* [75] Reserved */
    (void*) (0UL),                  /* [76] Reserved */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) isr_hash_rng,           /* [80] Hash and Rng */
    (void*) isr_fpu,                /* [81] FPU */
    (void*) (0UL),                  /* [82] Reserved */
    (void*) (0UL),                  /* [83] Reserved */
    (void*) (0UL),                  /* [84] Reserved */
    (void*) isr_spi5,               /* [85] SPI5 */
    (void*) (0UL),                  /* [86] Reserved */
    (void*) (0UL),                  /* [87] Reserved */
    (void*) (0UL),                  /* [88] Reserved */
    (void*) (0UL),                  /* [89] Reserved */
    (void*) (0UL),                  /* [90] Reserved */
    (void*) (0UL),                  /* [91] Reserved */
    (void*) (0UL),                  /* [92] Reserved */
    (void*) (0UL),                  /* [93] Reserved */
    (void*) (0UL),                  /* [94] Reserved */
    (void*) isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    (void*) isr_fmpi2c1_er,         /* [96] FMPI2C1 Error */
    (void*) isr_lptim1              /* [97] LP TIM1 */
#elif defined(CPU_MODEL_STM32F411RE)
    (void*) (0UL),                  /* [19] Reserved */
    (void*) (0UL),                  /* [20] Reserved */
    (void*) (0UL),                  /* [21] Reserved */
    (void*) (0UL),                  /* [22] Reserved */
    (void*) isr_exti,               /* [23] External Line[9:5]s */
    (void*) isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    (void*) isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    (void*) isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    (void*) isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    (void*) isr_tim2,               /* [28] TIM2 */
    (void*) isr_tim3,               /* [29] TIM3 */
    (void*) isr_tim4,               /* [30] TIM4 */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error */
    (void*) isr_i2c2_ev,            /* [33] I2C2 Event */
    (void*) isr_i2c2_er,            /* [34] I2C2 Error */
    (void*) isr_spi1,               /* [35] SPI1 */
    (void*) isr_spi2,               /* [36] SPI2 */
    (void*) isr_usart1,             /* [37] USART1 */
    (void*) isr_usart2,             /* [38] USART2 */
    (void*) isr_usart3,             /* [39] USART3 */
    (void*) isr_exti,               /* [40] External Line[15:10]s */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    (void*) isr_otg_fs_wkup,        /* [42] USB OTG FS Wakeup through EXTI line */
    (void*) (0UL),                  /* [43] Reserved */
    (void*) (0UL),                  /* [44] Reserved */
    (void*) (0UL),                  /* [45] Reserved */
    (void*) isr_tim8_cc,            /* [46] TIM8 Capture Compare */
    (void*) isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    (void*) (0UL),                  /* [22] Reserved */
    (void*) isr_sdio,               /* [49] SDIO */
    (void*) isr_tim5,               /* [50] TIM5 */
    (void*) isr_spi3,               /* [51] SPI3 */
    (void*) (0UL),                  /* [52] Reserved */
    (void*) (0UL),                  /* [53] Reserved */
    (void*) (0UL),                  /* [54] Reserved */
    (void*) (0UL),                  /* [55] Reserved */
    (void*) isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    (void*) isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    (void*) isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    (void*) isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    (void*) isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) (0UL),                  /* [64] Reserved */
    (void*) (0UL),                  /* [65] Reserved */
    (void*) (0UL),                  /* [66] Reserved */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) (0UL),                  /* [74] Reserved */
    (void*) (0UL),                  /* [75] Reserved */
    (void*) (0UL),                  /* [76] Reserved */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu,                /* [81] FPU */
    (void*) (0UL),                  /* [82] Reserved */
    (void*) (0UL),                  /* [83] Reserved */
    (void*) isr_spi4,               /* [84] SPI4 */
    (void*) isr_spi5                /* [85] SPI5 */
#else /* CPU_MODEL_STM32F413ZH, CPU_MODEL_STM32F413VG, CPU_MODEL_STM32F407VG
         CPU_MODEL_STM32F415RG, CPU_MODEL_STM32F412ZG, CPU_MODEL_STM32F446RE,
         CPU_MODEL_STM32F4446ZE */
    (void*) isr_can1_tx,            /* [19] CAN1 TX */
    (void*) isr_can1_rx0,           /* [20] CAN1 RX0 */
    (void*) isr_can1_rx1,           /* [21] CAN1 RX1 */
    (void*) isr_can1_sce,           /* [22] CAN1 SCE */
    (void*) isr_exti,               /* [23] External Line[9:5]s */
    (void*) isr_tim1_brk_tim9,      /* [24] TIM1 Break and TIM9 */
    (void*) isr_tim1_up_tim10,      /* [25] TIM1 Update and TIM10 */
    (void*) isr_tim1_trg_com_tim11, /* [26] TIM1 Trigger and Commutation and TIM11 */
    (void*) isr_tim1_cc,            /* [27] TIM1 Capture Compare */
    (void*) isr_tim2,               /* [28] TIM2 */
    (void*) isr_tim3,               /* [29] TIM3 */
    (void*) isr_tim4,               /* [30] TIM4 */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error */
    (void*) isr_i2c2_ev,            /* [33] I2C2 Event */
    (void*) isr_i2c2_er,            /* [34] I2C2 Error */
    (void*) isr_spi1,               /* [35] SPI1 */
    (void*) isr_spi2,               /* [36] SPI2 */
    (void*) isr_usart1,             /* [37] USART1 */
    (void*) isr_usart2,             /* [38] USART2 */
    (void*) isr_usart3,             /* [39] USART3 */
    (void*) isr_exti,               /* [40] External Line[15:10]s */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line */
    (void*) isr_otg_fs_wkup,        /* [42] USB OTG FS Wakeup through EXTI line */
    (void*) isr_tim8_brk_tim12,     /* [43] TIM8 Break and TIM12 */
    (void*) isr_tim8_up_tim13,      /* [44] TIM8 Update and TIM13 */
    (void*) isr_tim8_trg_com_tim14, /* [45] TIM8 Trigger and Commutation and TIM14 */
    (void*) isr_tim8_cc,            /* [46] TIM8 Capture Compare */
    (void*) isr_dma1_stream7,       /* [47] DMA1 Stream7 */
    (void*) isr_fsmc,               /* [48] FSMC */
    (void*) isr_sdio,               /* [49] SDIO */
    (void*) isr_tim5,               /* [50] TIM5 */
    (void*) isr_spi3,               /* [51] SPI3 */
    (void*) isr_uart4,              /* [52] UART4 */
    (void*) isr_uart5,              /* [53] UART5 */
    (void*) isr_tim6_dac,           /* [54] TIM6 and DAC1&2 underrun errors */
    (void*) isr_tim7,               /* [55] TIM7 */
    (void*) isr_dma2_stream0,       /* [56] DMA2 Stream 0 */
    (void*) isr_dma2_stream1,       /* [57] DMA2 Stream 1 */
    (void*) isr_dma2_stream2,       /* [58] DMA2 Stream 2 */
    (void*) isr_dma2_stream3,       /* [59] DMA2 Stream 3 */
    (void*) isr_dma2_stream4,       /* [60] DMA2 Stream 4 */
#if defined(CPU_MODEL_STM32F407VG)
    (void*) isr_eth,                /* [61] Ethernet */
    (void*) isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    (void*) isr_can2_tx,            /* [63] CAN2 TX */
    (void*) isr_can2_rx0,           /* [64] CAN2 RX0 */
    (void*) isr_can2_rx1,           /* [65] CAN2 RX1 */
    (void*) isr_can2_sce,           /* [66] CAN2 SCE */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    (void*) isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    (void*) isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    (void*) isr_otg_hs,             /* [77] USB OTG HS */
    (void*) isr_dcmi,               /* [78] DCMI */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu                 /* [81] FPU */
#elif defined(CPU_MODEL_STM32F415RG)
    (void*) isr_eth,                /* [61] Ethernet */
    (void*) isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    (void*) isr_can2_tx,            /* [63] CAN2 TX */
    (void*) isr_can2_rx0,           /* [64] CAN2 RX0 */
    (void*) isr_can2_rx1,           /* [65] CAN2 RX1 */
    (void*) isr_can2_sce,           /* [66] CAN2 SCE */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    (void*) isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    (void*) isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    (void*) isr_otg_hs,             /* [77] USB OTG HS */
    (void*) isr_dcmi,               /* [78] DCMI */
    (void*) isr_cryp,               /* [79] CRYP crypto */
    (void*) isr_hash_rng,           /* [80] Hash and Rng */
    (void*) isr_fpu                 /* [81] FPU */
#elif defined(CPU_MODEL_STM32F412ZG)
    (void*) isr_eth,                /* [61] Ethernet */
    (void*) isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    (void*) isr_can2_tx,            /* [63] CAN2 TX */
    (void*) isr_can2_rx0,           /* [64] CAN2 RX0 */
    (void*) isr_can2_rx1,           /* [65] CAN2 RX1 */
    (void*) isr_can2_sce,           /* [66] CAN2 SCE */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) (0UL),                  /* [74] Reserved */
    (void*) (0UL),                  /* [75] Reserved */
    (void*) (0UL),                  /* [76] Reserved */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) isr_rng,                /* [80] Rng */
    (void*) isr_fpu,                /* [81] FPU */
    (void*) (0UL),                  /* [82] Reserved */
    (void*) (0UL),                  /* [83] Reserved */
    (void*) isr_spi4,               /* [84] SPI4 */
    (void*) isr_spi5,               /* [85] SPI5 */
    (void*) (0UL),                  /* [86] Reserved */
    (void*) (0UL),                  /* [87] Reserved */
    (void*) (0UL),                  /* [88] Reserved */
    (void*) (0UL),                  /* [89] Reserved */
    (void*) (0UL),                  /* [90] Reserved */
    (void*) (0UL),                  /* [91] Reserved */
    (void*) isr_quadspi,            /* [92] QuadSPI */
    (void*) (0UL),                  /* [93] Reserved */
    (void*) (0UL),                  /* [94] Reserved */
    (void*) isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    (void*) isr_fmpi2c1_er          /* [96] FMPI2C1 Error */
#elif defined(CPU_MODEL_STM32F446RE) || defined(CPU_MODEL_STM32F446ZE)
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) isr_can2_tx,            /* [63] CAN2 TX */
    (void*) isr_can2_rx0,           /* [64] CAN2 RX0 */
    (void*) isr_can2_rx1,           /* [65] CAN2 RX1 */
    (void*) isr_can2_sce,           /* [66] CAN2 SCE */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    (void*) isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    (void*) isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    (void*) isr_otg_hs,             /* [77] USB OTG HS */
    (void*) isr_dcmi,               /* [78] DCMI */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) isr_fpu,                /* [81] FPU */
    (void*) (0UL),                  /* [82] Reserved */
    (void*) (0UL),                  /* [83] Reserved */
    (void*) isr_spi4,               /* [84] SPI4 */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [86] Reserved */
    (void*) isr_sai1,               /* [87] SAI1 */
    (void*) (0UL),                  /* [88] Reserved */
    (void*) (0UL),                  /* [89] Reserved */
    (void*) (0UL),                  /* [90] Reserved */
    (void*) isr_sai2,               /* [91] SAI2 */
    (void*) isr_quadspi,            /* [92] QuadSPI */
    (void*) isr_cec,                /* [93] CEC */
    (void*) isr_spdif_rx,           /* [94] SPDIF-RX */
    (void*) isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    (void*) isr_fmpi2c1_er          /* [96] FMPI2C1 Error */
#elif defined(CPU_MODEL_STM32F413ZH) || defined(CPU_MODEL_STM32F413VG)
    (void*) isr_eth,                /* [61] Ethernet */
    (void*) isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    (void*) isr_can2_tx,            /* [63] CAN2 TX */
    (void*) isr_can2_rx0,           /* [64] CAN2 RX0 */
    (void*) isr_can2_rx1,           /* [65] CAN2 RX1 */
    (void*) isr_can2_sce,           /* [66] CAN2 SCE */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) isr_can3_tx,            /* [74] CAN3 TX */
    (void*) isr_can3_rx0,           /* [75] CAN3 RX0 */
    (void*) isr_can3_rx1,           /* [76] CAN3 RX1 */
    (void*) isr_can3_sce,           /* [77] CAN3 SCE */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) isr_hash_rng,           /* [80] Hash and Rng */
    (void*) isr_fpu,                /* [81] FPU */
    (void*) isr_usart7,             /* [82] USART7 */
    (void*) isr_usart8,             /* [83] USART8 */
    (void*) isr_spi4,               /* [84] SPI4 */
    (void*) isr_spi5,               /* [85] SPI5 */
    (void*) (0UL),                  /* [86] Reserved */
    (void*) isr_sai1,               /* [87] SAI1 */
    (void*) isr_uart9,              /* [88] UART9 */
    (void*) isr_uart10,             /* [89] UART10 */
    (void*) (0UL),                  /* [90] Reserved */
    (void*) (0UL),                  /* [91] Reserved */
    (void*) isr_quadspi,            /* [92] QuadSPI */
    (void*) (0UL),                  /* [93] Reserved */
    (void*) (0UL),                  /* [94] Reserved */
    (void*) isr_fmpi2c1_ev,         /* [95] FMPI2C1 Event */
    (void*) isr_fmpi2c1_er,         /* [96] FMPI2C1 Error */
    (void*) isr_lptim1,             /* [97] LP TIM1 */
    (void*) isr_dfsdm2_flt0,        /* [98] DFSDM2 Filter 0 */
    (void*) isr_dfsdm2_flt1,        /* [99] DFSDM2 Filter 1 */
    (void*) isr_dfsdm2_flt2,        /* [100] DFSDM2 Filter 2 */
    (void*) isr_dfsdm2_flt3         /* [101] DFSDM2 Filter 3 */
#else /* CPU_MODEL_STM32F429ZI */
    (void*) isr_eth,                /* [61] Ethernet */
    (void*) isr_eth_wkup,           /* [62] Ethernet Wakeup through EXTI line */
    (void*) isr_can2_tx,            /* [63] CAN2 TX */
    (void*) isr_can2_rx0,           /* [64] CAN2 RX0 */
    (void*) isr_can2_rx1,           /* [65] CAN2 RX1 */
    (void*) isr_can2_sce,           /* [66] CAN2 SCE */
    (void*) isr_otg_fs,             /* [67] USB OTG FS */
    (void*) isr_dma2_stream5,       /* [68] DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* [69] DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* [70] DMA2 Stream 7 */
    (void*) isr_usart6,             /* [71] USART6 */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event */
    (void*) isr_i2c3_er,            /* [73] I2C3 error */
    (void*) isr_otg_hs_ep1_out,     /* [74] USB OTG HS End Point 1 Out */
    (void*) isr_otg_hs_ep1_in,      /* [75] USB OTG HS End Point 1 In */
    (void*) isr_otg_hs_wkup,        /* [76] USB OTG HS Wakeup through EXTI */
    (void*) isr_otg_hs,             /* [77] USB OTG HS */
    (void*) isr_dcmi,               /* [78] DCMI */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu,                /* [81] FPU */
    (void*) isr_usart7,             /* [82] USART7 */
    (void*) isr_usart8,             /* [83] USART8 */
    (void*) isr_spi4,               /* [84] SPI4 */
    (void*) isr_spi5,               /* [85] SPI5 */
    (void*) isr_spi6,               /* [86] SPI6 */
    (void*) isr_sai1,               /* [87] SAI1 */
    (void*) isr_ltdc,               /* [88] LDTC */
    (void*) isr_ltdc_er,            /* [89] LTDC Error */
    (void*) isr_dma2d               /* [90] Reserved */
#endif
#endif
};
