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

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
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
    (void*) isr_wwdg,               /* [0] Window WatchDog Interrupt */
    (void*) isr_pvd_pvm,            /* [1] PVD/PVM1/PVM2/PVM3/PVM4 through EXTI Line detection Interrupts */
    (void*) isr_tamp_stamp,         /* [2] Tamper and TimeStamp interrupts through the EXTI line */
    (void*) isr_rtc_wkup,           /* [3] RTC Wakeup interrupt through the EXTI line */
    (void*) isr_flash,              /* [4] FLASH global Interrupt */
    (void*) isr_rcc,                /* [5] RCC global Interrupt */
    (void*) isr_exti,               /* [6] EXTI Line0 Interrupt */
    (void*) isr_exti,               /* [7] EXTI Line1 Interrupt */
    (void*) isr_exti,               /* [8] EXTI Line2 Interrupt */
    (void*) isr_exti,               /* [9] EXTI Line3 Interrupt */
    (void*) isr_exti,               /* [10] EXTI Line4 Interrupt */
    (void*) isr_dma1_channel1,      /* [11] DMA1 Channel 1 global Interrupt */
    (void*) isr_dma1_channel2,      /* [12] DMA1 Channel 2 global Interrupt */
    (void*) isr_dma1_channel3,      /* [13] DMA1 Channel 3 global Interrupt */
    (void*) isr_dma1_channel4,      /* [14] DMA1 Channel 4 global Interrupt */
    (void*) isr_dma1_channel5,      /* [15] DMA1 Channel 5 global Interrupt */
    (void*) isr_dma1_channel6,      /* [16] DMA1 Channel 6 global Interrupt */
    (void*) isr_dma1_channel7,      /* [17] DMA1 Channel 7 global Interrupt */
    (void*) isr_adc1_2,             /* [18] ADC1 global Interrupt */
    (void*) isr_can1_tx,            /* [19] CAN1 TX Interrupt */
    (void*) isr_can1_rx0,           /* [20] CAN1 RX0 Interrupt */
    (void*) isr_can1_rx1,           /* [21] CAN1 RX1 Interrupt */
    (void*) isr_can1_sce,           /* [22] CAN1 SCE Interrupt */
    (void*) isr_exti,               /* [23] External Line[9:5] Interrupts */
    (void*) isr_tim1_brk_tim15,     /* [24] TIM1 Break interrupt and TIM15 global interrupt */
    (void*) isr_tim1_up_tim16,      /* [25] TIM1 Update Interrupt and TIM16 global interrupt */
    (void*) isr_tim1_trg_com_tim17, /* [26] TIM1 Trigger and Commutation Interrupt */
    (void*) isr_tim1_cc,            /* [27] TIM1 Capture Compare Interrupt */
    (void*) isr_tim2,               /* [28] TIM2 global Interrupt */
#if defined(CPU_MODEL_STM32L432KC)
    (void*) (0UL),                  /* [29] Reserved */
    (void*) (0UL),                  /* [30] Reserved */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event Interrupt */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (void*) (0UL),                  /* [33] Reserved */
    (void*) (0UL),                  /* [34] Reserved */
    (void*) isr_spi1,               /* [35] SPI1 global Interrupt */
    (void*) (0UL),                  /* [36] Reserved */
    (void*) isr_usart1,             /* [37] USART1 global Interrupt */
    (void*) isr_usart2,             /* [38] USART2 global Interrupt */
    (void*) (0UL),                  /* [39] Reserved */
    (void*) isr_exti,               /* [40] External Line[15:10] Interrupts */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    (void*) (0UL),                  /* [42] Reserved */
    (void*) (0UL),                  /* [43] Reserved */
    (void*) (0UL),                  /* [44] Reserved */
    (void*) (0UL),                  /* [45] Reserved */
    (void*) (0UL),                  /* [46] Reserved */
    (void*) (0UL),                  /* [47] Reserved */
    (void*) (0UL),                  /* [48] Reserved */
    (void*) (0UL),                  /* [49] Reserved */
    (void*) (0UL),                  /* [50] Reserved */
    (void*) isr_spi3,               /* [51] SPI3 global Interrupt */
    (void*) (0UL),                  /* [52] Reserved */
    (void*) (0UL),                  /* [53] Reserved */
    (void*) isr_tim6_dac,           /* [54] TIM6 global and DAC1&2 underrun error interrupts */
    (void*) isr_tim7,               /* [55] TIM7 global interrupt */
    (void*) isr_dma2_channel1,      /* [56] DMA2 Channel 1 global Interrupt */
    (void*) isr_dma2_channel2,      /* [57] DMA2 Channel 2 global Interrupt */
    (void*) isr_dma2_channel3,      /* [58] DMA2 Channel 3 global Interrupt */
    (void*) isr_dma2_channel4,      /* [59] DMA2 Channel 4 global Interrupt */
    (void*) isr_dma2_channel5,      /* [60] DMA2 Channel 5 global Interrupt */
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) isr_comp,               /* [64] COMP1 and COMP2 Interrupts */
    (void*) isr_lptim1,             /* [65] LP TIM1 interrupt */
    (void*) isr_lptim2,             /* [66] LP TIM2 interrupt */
    (void*) isr_otg_fs,             /* [67] USB OTG FS global Interrupt */
    (void*) isr_dma2_channel6,      /* [68] DMA2 Channel 6 global interrupt */
    (void*) isr_dma2_channel7,      /* [69] DMA2 Channel 7 global interrupt */
    (void*) isr_lpuart1,            /* [70] LP UART1 interrupt */
    (void*) isr_quadspi,            /* [71] Quad SPI global interrupt */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    (void*) isr_i2c3_er,            /* [73] I2C3 error interrupt */
    (void*) isr_sai1,               /* [74] Serial Audio Interface 1 global interrupt */
    (void*) (0UL),                  /* [75] Reserved */
    (void*) isr_swpmi1,             /* [76] Serial Wire Interface 1 global interrupt */
    (void*) isr_tsc,                /* [77] Touch Sense Controller global interrupt */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved*/
    (void*) isr_rng,                /* [80] RNG global interrupt */
    (void*) isr_fpu,                /* [81] FPU global interrupt */
    (void*) isr_crs                 /* [82] CRS global interrupt */
#else /* CPU_MODEL_STM32L476RG */
    (void*) isr_tim3,               /* [29] TIM3 global Interrupt */
    (void*) isr_tim4,               /* [30] TIM4 global Interrupt */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event Interrupt */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (void*) isr_i2c2_ev,            /* [33] I2C2 Event Interrupt */
    (void*) isr_i2c2_er,            /* [34] I2C2 Error Interrupt */
    (void*) isr_spi1,               /* [35] SPI1 global Interrupt */
    (void*) isr_spi2,               /* [36] SPI2 global Interrupt */
    (void*) isr_usart1,             /* [37] USART1 global Interrupt */
    (void*) isr_usart2,             /* [38] USART2 global Interrupt */
    (void*) isr_usart3,             /* [39] USART3 global Interrupt */
    (void*) isr_exti,               /* [40] External Line[15:10] Interrupts */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line Interrupt */
    (void*) isr_dfsdm1_flt3,        /* [42] DFSDM1 Filter 3 global Interrupt */
    (void*) isr_tim8_brk,           /* [43] TIM8 Break Interrupt */
    (void*) isr_tim8_up,            /* [44] TIM8 Update Interrupt */
    (void*) isr_tim8_trg_com,       /* [45] TIM8 Trigger and Commutation Interrupt */
    (void*) isr_tim8_cc,            /* [46] TIM8 Capture Compare Interrupt */
    (void*) isr_adc3,               /* [47] ADC3 global  Interrupt */
    (void*) isr_fmc,                /* [48] FMC global Interrupt */
    (void*) isr_sdmmc1,             /* [49] SDMMC1 global Interrupt */
    (void*) isr_tim5,               /* [50] TIM5 global Interrupt */
    (void*) isr_spi3,               /* [51] SPI3 global Interrupt */
    (void*) isr_uart4,              /* [52] UART4 global Interrupt */
    (void*) isr_uart5,              /* [53] UART5 global Interrupt */
    (void*) isr_tim6_dac,           /* [54] TIM6 global and DAC1&2 underrun error interrupts */
    (void*) isr_tim7,               /* [55] TIM7 global interrupt */
    (void*) isr_dma2_channel1,      /* [56] DMA2 Channel 1 global Interrupt */
    (void*) isr_dma2_channel2,      /* [57] DMA2 Channel 2 global Interrupt */
    (void*) isr_dma2_channel3,      /* [58] DMA2 Channel 3 global Interrupt */
    (void*) isr_dma2_channel4,      /* [59] DMA2 Channel 4 global Interrupt */
    (void*) isr_dma2_channel5,      /* [60] DMA2 Channel 5 global Interrupt */
    (void*) isr_dfsdm1_flt0,        /* [61] DFSDM1 Filter 0 global Interrupt */
    (void*) isr_dfsdm1_flt1,        /* [62] DFSDM1 Filter 1 global Interrupt */
    (void*) isr_dfsdm1_flt2,        /* [63] DFSDM1 Filter 2 global Interrupt */
    (void*) isr_comp,               /* [64] COMP1 and COMP2 Interrupts */
    (void*) isr_lptim1,             /* [65] LP TIM1 interrupt */
    (void*) isr_lptim2,             /* [66] LP TIM2 interrupt */
    (void*) isr_otg_fs,             /* [67] USB OTG FS global Interrupt */
    (void*) isr_dma2_channel6,      /* [68] DMA2 Channel 6 global interrupt */
    (void*) isr_dma2_channel7,      /* [69] DMA2 Channel 7 global interrupt */
    (void*) isr_lpuart1,            /* [70] LP UART1 interrupt */
    (void*) isr_quadspi,            /* [71] Quad SPI global interrupt */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    (void*) isr_i2c3_er,            /* [73] I2C3 error interrupt */
    (void*) isr_sai1,               /* [74] Serial Audio Interface 1 global interrupt */
    (void*) isr_sai2,               /* [75] Serial Audio Interface 2 global interrupt */
    (void*) isr_swpmi1,             /* [76] Serial Wire Interface 1 global interrupt */
    (void*) isr_tsc,                /* [77] Touch Sense Controller global interrupt */
    (void*) isr_lcd,                /* [78] LCD global interrupt */
    (void*) (0UL),                  /* [79] Reserved*/
    (void*) isr_rng,                /* [80] RNG global interrupt */
    (void*) isr_fpu                 /* [81] FPU global interrupt */
#endif
};
