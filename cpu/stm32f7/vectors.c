/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f7
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

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* STM32F7 specific interrupt vectors */
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
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_sdmmc1(void);
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
WEAK_DEFAULT void isr_otg_hs_ep1_out(void);
WEAK_DEFAULT void isr_otg_hs_ep1_in(void);
WEAK_DEFAULT void isr_otg_hs_wkup(void);
WEAK_DEFAULT void isr_otg_hs(void);
WEAK_DEFAULT void isr_dcmi(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_uart7(void);
WEAK_DEFAULT void isr_uart8(void);
WEAK_DEFAULT void isr_spi4(void);
WEAK_DEFAULT void isr_spi5(void);
WEAK_DEFAULT void isr_spi6(void);
WEAK_DEFAULT void isr_sai1(void);
WEAK_DEFAULT void isr_ltdc(void);
WEAK_DEFAULT void isr_ltdc_er(void);
WEAK_DEFAULT void isr_dma2d(void);
WEAK_DEFAULT void isr_sai2(void);
WEAK_DEFAULT void isr_quadspi(void);
WEAK_DEFAULT void isr_lptim1(void);
WEAK_DEFAULT void isr_cec(void);
WEAK_DEFAULT void isr_i2c4_ev(void);
WEAK_DEFAULT void isr_i2c4_er(void);
WEAK_DEFAULT void isr_spdif_rx(void);
#if defined(CPU_MODEL_STM32F769NI) || defined(CPU_MODEL_STM32F767ZI)
WEAK_DEFAULT void isr_dsi(void);
WEAK_DEFAULT void isr_dfsdm1_flt0(void);
WEAK_DEFAULT void isr_dfsdm1_flt1(void);
WEAK_DEFAULT void isr_dfsdm1_flt2(void);
WEAK_DEFAULT void isr_dfsdm1_flt3(void);
WEAK_DEFAULT void isr_can3_tx(void);
WEAK_DEFAULT void isr_can3_rx0(void);
WEAK_DEFAULT void isr_can3_rx1(void);
WEAK_DEFAULT void isr_can3_sce(void);
WEAK_DEFAULT void isr_jpeg(void);
WEAK_DEFAULT void isr_mdios(void);
#endif
#if defined(CPU_MODEL_STM32F769NI) || defined(CPU_MODEL_STM32F767ZI) \
    || defined(CPU_MODEL_STM32F722ZE)
WEAK_DEFAULT void isr_sdmmc2(void);
#endif

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),                /* pointer to the top of the stack */
    /* Cortex-M7 handlers */
    (void*) reset_handler_default,     /* entry point of the program */
    (void*) nmi_default,               /* [-14] non maskable interrupt handler */
    (void*) hard_fault_default,        /* [-13] hard fault exception */
    (void*) mem_manage_default,        /* [-12] memory manage exception */
    (void*) bus_fault_default,         /* [-11] bus fault exception */
    (void*) usage_fault_default,       /* [-10] usage fault exception */
    (void*) (0UL),                     /* [-9] Reserved */
    (void*) (0UL),                     /* [-8] Reserved */
    (void*) (0UL),                     /* [-7] Reserved */
    (void*) (0UL),                     /* [-6] Reserved */
    (void*) isr_svc,                   /* [-5] system call interrupt, in RIOT used for
                                               switching into thread context on boot */
    (void*) debug_mon_default,         /* [-4] debug monitor exception */
    (void*) (0UL),                     /* [-3] Reserved */
    (void*) isr_pendsv,                /* [-2] pendSV interrupt, in RIOT the actual
                                               context switching is happening here */
    (void*) isr_systick,               /* [-1] SysTick interrupt, not used in RIOT */
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,                  /* [0] Window WatchDog Interrupt */
    (void*) isr_pvd,                   /* [1] PVD through EXTI Line detection Interrupt */
    (void*) isr_tamp_stamp,            /* [2] Tamper and TimeStamp interrupts through the EXTI line */
    (void*) isr_rtc_wkup,              /* [3] RTC Wakeup interrupt through the EXTI line */
    (void*) isr_flash,                 /* [4] FLASH global Interrupt */
    (void*) isr_rcc,                   /* [5] RCC global Interrupt */
    (void*) isr_exti,                  /* [6] EXTI Line0 Interrupt */
    (void*) isr_exti,                  /* [7] EXTI Line1 Interrupt */
    (void*) isr_exti,                  /* [8] EXTI Line2 Interrupt */
    (void*) isr_exti,                  /* [9] EXTI Line3 Interrupt */
    (void*) isr_exti,                  /* [10] EXTI Line4 Interrupt */
    (void*) isr_dma1_stream0,          /* [11] DMA1 Stream 0 global Interrupt */
    (void*) isr_dma1_stream1,          /* [12] DMA1 Stream 1 global Interrupt */
    (void*) isr_dma1_stream2,          /* [13] DMA1 Stream 2 global Interrupt */
    (void*) isr_dma1_stream3,          /* [14] DMA1 Stream 3 global Interrupt */
    (void*) isr_dma1_stream4,          /* [15] DMA1 Stream 4 global Interrupt */
    (void*) isr_dma1_stream5,          /* [16] DMA1 Stream 5 global Interrupt */
    (void*) isr_dma1_stream6,          /* [17] DMA1 Stream 6 global Interrupt */
    (void*) isr_adc,                   /* [18] ADC1, ADC2 and ADC3 global Interrupts */
    (void*) isr_can1_tx,               /* [19] CAN1 TX Interrupt */
    (void*) isr_can1_rx0,              /* [20] CAN1 RX0 Interrupt */
    (void*) isr_can1_rx1,              /* [21] CAN1 RX1 Interrupt */
    (void*) isr_can1_sce,              /* [22] CAN1 SCE Interrupt */
    (void*) isr_exti,                  /* [23] External Line[9:5] Interrupts */
    (void*) isr_tim1_brk_tim9,         /* [24] TIM1 Break interrupt and TIM9 global interrupt */
    (void*) isr_tim1_up_tim10,         /* [25] TIM1 Update Interrupt and TIM10 global interrupt */
    (void*) isr_tim1_trg_com_tim11,    /* [26] TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
    (void*) isr_tim1_cc,               /* [27] TIM1 Capture Compare Interrupt */
    (void*) isr_tim2,                  /* [28] TIM2 global Interrupt */
    (void*) isr_tim3,                  /* [29] TIM3 global Interrupt */
    (void*) isr_tim4,                  /* [30] TIM4 global Interrupt */
    (void*) isr_i2c1_ev,               /* [31] I2C1 Event Interrupt */
    (void*) isr_i2c1_er,               /* [32] I2C1 Error Interrupt */
    (void*) isr_i2c2_ev,               /* [33] I2C2 Event Interrupt */
    (void*) isr_i2c2_er,               /* [34] I2C2 Error Interrupt */
    (void*) isr_spi1,                  /* [35] SPI1 global Interrupt */
    (void*) isr_spi2,                  /* [36] SPI2 global Interrupt */
    (void*) isr_usart1,                /* [37] USART1 global Interrupt */
    (void*) isr_usart2,                /* [38] USART2 global Interrupt */
    (void*) isr_usart3,                /* [39] USART3 global Interrupt */
    (void*) isr_exti,                  /* [40] External Line[15:10] Interrupts */
    (void*) isr_rtc_alarm,             /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    (void*) isr_otg_fs_wkup,           /* [42] USB OTG FS Wakeup through EXTI line interrupt */
    (void*) isr_tim8_brk_tim12,        /* [43] TIM8 Break Interrupt and TIM12 global interrupt */
    (void*) isr_tim8_up_tim13,         /* [44] TIM8 Update Interrupt and TIM13 global interrupt */
    (void*) isr_tim8_trg_com_tim14,    /* [45] TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    (void*) isr_tim8_cc,               /* [46] TIM8 Capture Compare Interrupt */
    (void*) isr_dma1_stream7,          /* [47] DMA1 Stream7 Interrupt */
    (void*) isr_fmc,                   /* [48] FMC global Interrupt */
    (void*) isr_sdmmc1,                /* [49] SDMMC1 global Interrupt */
    (void*) isr_tim5,                  /* [50] TIM5 global Interrupt */
    (void*) isr_spi3,                  /* [51] SPI3 global Interrupt */
    (void*) isr_uart4,                 /* [52] UART4 global Interrupt */
    (void*) isr_uart5,                 /* [53] UART5 global Interrupt */
    (void*) isr_tim6_dac,              /* [54] TIM6 global and DAC1&2 underrun error interrupts */
    (void*) isr_tim7,                  /* [55] TIM7 global interrupt */
    (void*) isr_dma2_stream0,          /* [56] DMA2 Stream 0 global Interrupt */
    (void*) isr_dma2_stream1,          /* [57] DMA2 Stream 1 global Interrupt */
    (void*) isr_dma2_stream2,          /* [58] DMA2 Stream 2 global Interrupt */
    (void*) isr_dma2_stream3,          /* [59] DMA2 Stream 3 global Interrupt */
    (void*) isr_dma2_stream4,          /* [60] DMA2 Stream 4 global Interrupt */
    (void*) isr_eth,                   /* [61] Ethernet global Interrupt */
    (void*) isr_eth_wkup,              /* [62] Ethernet Wakeup through EXTI line Interrupt */
    (void*) isr_can2_tx,               /* [63] CAN2 TX Interrupt */
    (void*) isr_can2_rx0,              /* [64] CAN2 RX0 Interrupt */
    (void*) isr_can2_rx1,              /* [65] CAN2 RX1 Interrupt */
    (void*) isr_can2_sce,              /* [66] CAN2 SCE Interrupt */
    (void*) isr_otg_fs,                /* [67] USB OTG FS global Interrupt */
    (void*) isr_dma2_stream5,          /* [68] DMA2 Stream 5 global interrupt */
    (void*) isr_dma2_stream6,          /* [69] DMA2 Stream 6 global interrupt */
    (void*) isr_dma2_stream7,          /* [70] DMA2 Stream 7 global interrupt */
    (void*) isr_usart6,                /* [71] USART6 global interrupt */
    (void*) isr_i2c3_ev,               /* [72] I2C3 event interrupt */
    (void*) isr_i2c3_er,               /* [73] I2C3 error interrupt */
    (void*) isr_otg_hs_ep1_out,        /* [74] USB OTG HS End Point 1 Out global interrupt */
    (void*) isr_otg_hs_ep1_in,         /* [75] USB OTG HS End Point 1 In global interrupt */
    (void*) isr_otg_hs_wkup,           /* [76] USB OTG HS Wakeup through EXTI interrupt */
    (void*) isr_otg_hs,                /* [77] USB OTG HS global interrupt */
#if defined(CPU_MODEL_STM32F722ZE)
    (void*) (0UL),                     /* [78] Reserved */
    (void*) (0UL),                     /* [79] Reserved */
    (void*) isr_rng,                   /* [80] RNG global interrupt */
    (void*) isr_fpu,                   /* [81] FPU global interrupt */
    (void*) isr_uart7,                 /* [82] UART7 global interrupt */
    (void*) isr_uart8,                 /* [83] UART8 global interrupt */
    (void*) isr_spi4,                  /* [84] SPI4 global Interrupt */
    (void*) isr_spi5,                  /* [85] SPI5 global Interrupt */
    (void*) (0UL),                     /* [86] Reserved */
    (void*) isr_sai1,                  /* [87] SAI1 global Interrupt */
    (void*) (0UL),                     /* [88] Reserved */
    (void*) (0UL),                     /* [89] Reserved */
    (void*) (0UL),                     /* [90] Reserved */
    (void*) isr_sai2,                  /* [91] SAI2 global Interrupt */
    (void*) isr_quadspi,               /* [92] Quad SPI global interrupt */
    (void*) isr_lptim1,                /* [93] LP TIM1 interrupt */
    (void*) (0UL),                     /* [94] Reserved */
    (void*) (0UL),                     /* [95] Reserved */
    (void*) (0UL),                     /* [96] Reserved */
    (void*) (0UL),                     /* [97] Reserved */
    (void*) (0UL),                     /* [98] Reserved */
    (void*) (0UL),                     /* [99] Reserved */
    (void*) (0UL),                     /* [100] Reserved */
    (void*) (0UL),                     /* [101] Reserved */
    (void*) (0UL),                     /* [102] Reserved */
    (void*) isr_sdmmc2,                /* [103] SDMMC2 global Interrup */
#else
    (void*) isr_dcmi,                  /* [78] DCMI global interrupt */
    (void*) (0UL),                     /* [79] Reserved */
    (void*) isr_rng,                   /* [80] RNG global interrupt */
    (void*) isr_fpu,                   /* [81] FPU global interrupt */
    (void*) isr_uart7,                 /* [82] UART7 global interrupt */
    (void*) isr_uart8,                 /* [83] UART8 global interrupt */
    (void*) isr_spi4,                  /* [84] SPI4 global Interrupt */
    (void*) isr_spi5,                  /* [85] SPI5 global Interrupt */
    (void*) isr_spi6,                  /* [86] SPI6 global Interrupt */
    (void*) isr_sai1,                  /* [87] SAI1 global Interrupt */
    (void*) isr_ltdc,                  /* [88] LTDC global Interrupt */
    (void*) isr_ltdc_er,               /* [89] LTDC Error global Interrupt */
    (void*) isr_dma2d,                 /* [90] DMA2D global Interrupt */
    (void*) isr_sai2,                  /* [91] SAI2 global Interrupt */
    (void*) isr_quadspi,               /* [92] Quad SPI global interrupt */
    (void*) isr_lptim1,                /* [93] LP TIM1 interrupt */
    (void*) isr_cec,                   /* [94] HDMI-CEC global Interrupt */
    (void*) isr_i2c4_ev,               /* [95] I2C4 Event Interrupt */
    (void*) isr_i2c4_er,               /* [96] I2C4 Error Interrupt */
    (void*) isr_spdif_rx,              /* [97] SPDIF-RX global Interrupt */
#if defined(CPU_MODEL_STM32F769NI)
    (void*) isr_dsi,                   /* [98] DSI global Interrupt */
#elif defined(CPU_MODEL_STM32F767ZI)
    (void*) (0UL),                     /* [98] Reserved */
#endif
#if defined(CPU_MODEL_STM32F769NI) || defined(CPU_MODEL_STM32F767ZI)
    (void*) isr_dfsdm1_flt0,           /* [99] DFSDM1 Filter 0 global Interrupt */
    (void*) isr_dfsdm1_flt1,           /* [100] DFSDM1 Filter 1 global Interrupt */
    (void*) isr_dfsdm1_flt2,           /* [101] DFSDM1 Filter 2 global Interrupt */
    (void*) isr_dfsdm1_flt3,           /* [102] DFSDM1 Filter 3 global Interrupt */
    (void*) isr_sdmmc2,                /* [103] SDMMC2 global Interrup */
    (void*) isr_can3_tx,               /* [104] CAN3 TX Interrupt */
    (void*) isr_can3_rx0,              /* [105] CAN3 RX0 Interrupt */
    (void*) isr_can3_rx1,              /* [106] CAN3 RX1 Interrupt */
    (void*) isr_can3_sce,              /* [107] CAN3 SCE Interrupt */
    (void*) isr_jpeg,                  /* [108] JPEG global Interrupt */
    (void*) isr_mdios,                 /* [109] MDIO Slave global Interrupt */
#endif
#endif
};
