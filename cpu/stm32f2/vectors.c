/*
 * Copyright (C) 2015 Engineering-Spirit
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
/* STM32F2 specific interrupt vectors */
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
WEAK_DEFAULT void isr_otg_hs_ep1_out(void);
WEAK_DEFAULT void isr_otg_hs_ep1_in(void);
WEAK_DEFAULT void isr_otg_hs_wkup(void);
WEAK_DEFAULT void isr_otg_hs(void);
WEAK_DEFAULT void isr_dcmi(void);
WEAK_DEFAULT void isr_cryp(void);
WEAK_DEFAULT void isr_hash_rng(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M3 handlers */
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
#if defined(CPU_MODEL_STM32F205RG)
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
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) isr_hash_rng,           /* [80] Hash and Rng */
#elif defined(CPU_MODEL_STM32F207ZG)
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
    (void*) isr_hash_rng,           /* [80] Hash and Rng */
#elif defined(CPU_MODEL_STM32F215RG) || defined (CPU_MODEL_STM32F215VG)
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
    (void*) (0UL),                  /* [78] Reserved */
    (void*) isr_cryp,               /* [79] CRYP crypto */
    (void*) isr_hash_rng,           /* [80] Hash and Rng */
#else /* CPU_MODEL_STM32F217ZG */
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
#endif
};
