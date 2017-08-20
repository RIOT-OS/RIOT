/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f3
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
/* STM32F3 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
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
WEAK_DEFAULT void isr_usb_hp_can_tx(void);
WEAK_DEFAULT void isr_usb_lp_can_rx0(void);
WEAK_DEFAULT void isr_can_rx1(void);
WEAK_DEFAULT void isr_can_sce(void);
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
WEAK_DEFAULT void isr_i2c3_ev(void);
WEAK_DEFAULT void isr_i2c3_er(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_rtc_alarm(void);
WEAK_DEFAULT void isr_usbwakeup(void);
WEAK_DEFAULT void isr_tim8_brk(void);
WEAK_DEFAULT void isr_tim8_up(void);
WEAK_DEFAULT void isr_tim8_trg_com(void);
WEAK_DEFAULT void isr_tim8_cc(void);
WEAK_DEFAULT void isr_adc3(void);
WEAK_DEFAULT void isr_fmc(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim7_dac(void);
WEAK_DEFAULT void isr_dma2_channel1(void);
WEAK_DEFAULT void isr_dma2_channel2(void);
WEAK_DEFAULT void isr_dma2_channel3(void);
WEAK_DEFAULT void isr_dma2_channel4(void);
WEAK_DEFAULT void isr_dma2_channel5(void);
WEAK_DEFAULT void isr_adc4(void);
WEAK_DEFAULT void isr_comp1_2_3(void);
WEAK_DEFAULT void isr_comp4_5_6(void);
WEAK_DEFAULT void isr_comp7(void);
WEAK_DEFAULT void isr_hrtim(void);
WEAK_DEFAULT void isr_hrtim_tim_a(void);
WEAK_DEFAULT void isr_hrtim_tim_b(void);
WEAK_DEFAULT void isr_hrtim_tim_c(void);
WEAK_DEFAULT void isr_hrtim_tim_d(void);
WEAK_DEFAULT void isr_hrtim_tim_e(void);
WEAK_DEFAULT void isr_hrtim_flt(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_usbwakeup_rmp(void);
WEAK_DEFAULT void isr_tim20_brk(void);
WEAK_DEFAULT void isr_tim20_up(void);
WEAK_DEFAULT void isr_tim20_trg_com(void);
WEAK_DEFAULT void isr_tim20_cc(void);
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_spi4(void);

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
    /* STMF3 specific peripheral handlers */
    (void*) isr_wwdg,               /* [0] Window WatchDog Interrupt */
    (void*) isr_pvd,                /* [1] PVD through EXTI Line detection Interrupt */
    (void*) isr_tamp_stamp,         /* [2] Tamper and TimeStamp interrupts through the EXTI line 19 */
    (void*) isr_rtc_wkup,           /* [3] RTC Wakeup interrupt through the EXTI line 20 */
    (void*) isr_flash,              /* [4] FLASH global Interrupt */
    (void*) isr_rcc,                /* [5] RCC global Interrupt */
    (void*) isr_exti,               /* [6] EXTI Line0 Interrupt */
    (void*) isr_exti,               /* [7] EXTI Line1 Interrupt */
    (void*) isr_exti,               /* [8] EXTI Line2 Interrupt and Touch Sense Controller Interrupt */
    (void*) isr_exti,               /* [9] EXTI Line3 Interrupt */
    (void*) isr_exti,               /* [10] EXTI Line4 Interrupt */
    (void*) isr_dma1_channel1,      /* [11] DMA1 Channel 1 Interrupt */
    (void*) isr_dma1_channel2,      /* [12] DMA1 Channel 2 Interrupt */
    (void*) isr_dma1_channel3,      /* [13] DMA1 Channel 3 Interrupt */
    (void*) isr_dma1_channel4,      /* [14] DMA1 Channel 4 Interrupt */
    (void*) isr_dma1_channel5,      /* [15] DMA1 Channel 5 Interrupt */
    (void*) isr_dma1_channel6,      /* [16] DMA1 Channel 6 Interrupt */
    (void*) isr_dma1_channel7,      /* [17] DMA1 Channel 7 Interrupt */
    (void*) isr_adc1_2,             /* [18] ADC1 & ADC2 Interrupts */
    (void*) isr_usb_hp_can_tx,      /* [19] USB Device High Priority or CAN TX Interrupts */
    (void*) isr_usb_lp_can_rx0,     /* [20] USB Device Low Priority or CAN RX0 Interrupts */
    (void*) isr_can_rx1,            /* [21] CAN RX1 Interrupt */
    (void*) isr_can_sce,            /* [22] CAN SCE Interrupt */
    (void*) isr_exti,               /* [23] External Line[9:5] Interrupts */
    (void*) isr_tim1_brk_tim15,     /* [24] TIM1 Break and TIM15 Interrupts */
    (void*) isr_tim1_up_tim16,      /* [25] TIM1 Update and TIM16 Interrupts */
    (void*) isr_tim1_trg_com_tim17, /* [26] TIM1 Trigger and Commutation and TIM17 Interrupt */
    (void*) isr_tim1_cc,            /* [27] TIM1 Capture Compare Interrupt */
    (void*) isr_tim2,               /* [28] TIM2 global Interrupt */
#if defined(CPU_MODEL_STM32F302R8)
    (void*) (0UL),                  /* [29] Reserved */
    (void*) (0UL),                  /* [30] Reserved */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (void*) isr_i2c2_ev,            /* [33] I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup) */
    (void*) isr_i2c2_er,            /* [34] I2C2 Error Interrupt */
    (void*) (0UL),                  /* [35] Reserved */
    (void*) isr_spi2,               /* [36] SPI2 global Interrupt */
    (void*) isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    (void*) isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    (void*) isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    (void*) isr_exti,               /* [40] External Line[15:10] Interrupts */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    (void*) isr_usbwakeup,          /* [42] USB Wakeup Interrupt */
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
    (void*) isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    (void*) (0UL),                  /* [55] Reserved */
    (void*) (0UL),                  /* [56] Reserved */
    (void*) (0UL),                  /* [57] Reserved */
    (void*) (0UL),                  /* [58] Reserved */
    (void*) (0UL),                  /* [59] Reserved */
    (void*) (0UL),                  /* [60] Reserved */
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    (void*) isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    (void*) (0UL),                  /* [66] Reserved */
    (void*) (0UL),                  /* [67] Reserved */
    (void*) (0UL),                  /* [68] Reserved */
    (void*) (0UL),                  /* [69] Reserved */
    (void*) (0UL),                  /* [70] Reserved */
    (void*) (0UL),                  /* [71] Reserved */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    (void*) isr_i2c3_er,            /* [73] I2C3 error interrupt */
    (void*) isr_usb_hp,             /* [74] USB High Priority global Interrupt remap */
    (void*) isr_usb_lp,             /* [75] USB Low Priority global Interrupt  remap */
    (void*) isr_usbwakeup_rmp,      /* [76] USB Wakeup Interrupt remap */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu                 /* [81] Floating point Interrupt */
#elif defined(CPU_MODEL_STM32F303K8)
    (void*) isr_tim3,               /* [29] TIM3 global Interrupt */
    (void*) (0UL),                  /* [30] Reserved */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (void*) (0UL),                  /* [33] Reserved */
    (void*) (0UL),                  /* [34] Reserved */
    (void*) isr_spi1,               /* [35] SPI1 global Interrupt */
    (void*) (0UL),                  /* [36] Reserved */
    (void*) isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    (void*) isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    (void*) isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    (void*) isr_exti,               /* [40] External Line[15:10] Interrupts */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    (void*) (0UL),                  /* [42] Reserved */
    (void*) (0UL),                  /* [43] Reserved */
    (void*) (0UL),                  /* [44] Reserved */
    (void*) (0UL),                  /* [45] Reserved */
    (void*) (0UL),                  /* [46] Reserved */
    (void*) (0UL),                  /* [47] Reserved */
    (void*) (0UL),                  /* [48] Reserved */
    (void*) (0UL),                  /* [49] Reserved */
    (void*) (0UL),                  /* [50] Reserved */
    (void*) (0UL),                  /* [51] Reserved */
    (void*) (0UL),                  /* [52] Reserved */
    (void*) (0UL),                  /* [53] Reserved */
    (void*) isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error interrupts */
    (void*) isr_tim7_dac,           /* [55] TIM7 global and DAC2 channel1 underrun error Interrupt */
    (void*) (0UL),                  /* [56] Reserved */
    (void*) (0UL),                  /* [57] Reserved */
    (void*) (0UL),                  /* [58] Reserved */
    (void*) (0UL),                  /* [59] Reserved */
    (void*) (0UL),                  /* [60] Reserved */
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    (void*) isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    (void*) (0UL),                  /* [66] Reserved */
    (void*) (0UL),                  /* [67] Reserved */
    (void*) (0UL),                  /* [68] Reserved */
    (void*) (0UL),                  /* [69] Reserved */
    (void*) (0UL),                  /* [70] Reserved */
    (void*) (0UL),                  /* [71] Reserved */
    (void*) (0UL),                  /* [72] Reserved */
    (void*) (0UL),                  /* [73] Reserved */
    (void*) (0UL),                  /* [74] Reserved */
    (void*) (0UL),                  /* [75] Reserved */
    (void*) (0UL),                  /* [76] Reserved */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu                 /* [81] Floating point Interrupt */
#elif (CPU_MODEL_STM32F334R8)
    (void*) isr_tim3,               /* [29] TIM3 global Interrupt */
    (void*) isr_tim4,               /* [30] TIM4 global Interrupt */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (void*) (0UL),                  /* [33] Reserved */
    (void*) (0UL),                  /* [34] Reserved */
    (void*) isr_spi1,               /* [35] SPI1 global Interrupt */
    (void*) (0UL),                  /* [36] Reserved */
    (void*) isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    (void*) isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    (void*) isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    (void*) isr_exti,               /* [40] External Line[15:10] Interrupts */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    (void*) (0UL),                  /* [42] Reserved */
    (void*) (0UL),                  /* [43] Reserved */
    (void*) (0UL),                  /* [44] Reserved */
    (void*) (0UL),                  /* [45] Reserved */
    (void*) (0UL),                  /* [46] Reserved */
    (void*) (0UL),                  /* [47] Reserved */
    (void*) (0UL),                  /* [48] Reserved */
    (void*) (0UL),                  /* [49] Reserved */
    (void*) (0UL),                  /* [50] Reserved */
    (void*) (0UL),                  /* [51] Reserved */
    (void*) (0UL),                  /* [52] Reserved */
    (void*) (0UL),                  /* [53] Reserved */
    (void*) isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    (void*) isr_tim7_dac,           /* [55] TIM7 global and DAC2 channel1 underrun error Interrupt */
    (void*) (0UL),                  /* [56] Reserved */
    (void*) (0UL),                  /* [57] Reserved */
    (void*) (0UL),                  /* [58] Reserved */
    (void*) (0UL),                  /* [59] Reserved */
    (void*) (0UL),                  /* [60] Reserved */
    (void*) (0UL),                  /* [61] Reserved */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    (void*) isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    (void*) (0UL),                  /* [66] Reserved */
    (void*) isr_hrtim,              /* [67] HRTIM Master Timer global Interrupts */
    (void*) isr_hrtim_tim_a,        /* [68] HRTIM Timer A global Interrupt */
    (void*) isr_hrtim_tim_b,        /* [69] HRTIM Timer B global Interrupt */
    (void*) isr_hrtim_tim_c,        /* [70] HRTIM Timer C global Interrupt */
    (void*) isr_hrtim_tim_d,        /* [71] HRTIM Timer D global Interrupt */
    (void*) isr_hrtim_tim_e,        /* [72] HRTIM Timer E global Interrupt */
    (void*) isr_hrtim_flt,          /* [73] HRTIM Fault global Interrupt */
    (void*) (0UL),                  /* [74] Reserved */
    (void*) (0UL),                  /* [75] Reserved */
    (void*) (0UL),                  /* [76] Reserved */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu                 /* [81] Floating point Interrupt */
#else /* CPU_MODEL_STM32F303VC, CPU_MODEL_STM32F303RE, CPU_MODEL_STM32F303ZE */
    (void*) isr_tim3,               /* [29] TIM3 global Interrupt */
    (void*) isr_tim4,               /* [30] TIM4 global Interrupt */
    (void*) isr_i2c1_ev,            /* [31] I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup) */
    (void*) isr_i2c1_er,            /* [32] I2C1 Error Interrupt */
    (void*) isr_i2c2_ev,            /* [33] I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup) */
    (void*) isr_i2c2_er,            /* [34] I2C2 Error Interrupt */
    (void*) isr_spi1,               /* [35] SPI1 global Interrupt */
    (void*) isr_spi2,               /* [36] SPI2 global Interrupt */
    (void*) isr_usart1,             /* [37] USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup) */
    (void*) isr_usart2,             /* [38] USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup) */
    (void*) isr_usart3,             /* [39] USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup) */
    (void*) isr_exti,               /* [40] External Line[15:10] Interrupts */
    (void*) isr_rtc_alarm,          /* [41] RTC Alarm (A and B) through EXTI Line 17 Interrupt */
    (void*) isr_usbwakeup,          /* [42] USB Wakeup Interrupt */
    (void*) isr_tim8_brk,           /* [43] TIM8 Break Interrupt */
    (void*) isr_tim8_up,            /* [44] TIM8 Update Interrupt */
    (void*) isr_tim8_trg_com,       /* [45] TIM8 Trigger and Commutation Interrupt */
    (void*) isr_tim8_cc,            /* [46] TIM8 Capture Compare Interrupt */
    (void*) isr_adc3,               /* [47] ADC3 global Interrupt */
#if defined(CPU_MODEL_STM32F303VC)
    (void*) (0UL),                  /* [48] Reserved */
#else
    (void*) isr_fmc,                /* [48] FMC global Interrupt */
#endif
    (void*) (0UL),                  /* [49] Reserved */
    (void*) (0UL),                  /* [50] Reserved */
    (void*) isr_spi3,               /* [51] SPI3 global Interrupt */
    (void*) isr_uart4,              /* [52] UART4 global Interrupt & EXTI Line34 Interrupt (UART4 wakeup) */
    (void*) isr_uart5,              /* [53] UART5 global Interrupt & EXTI Line35 Interrupt (UART5 wakeup) */
    (void*) isr_tim6_dac,           /* [54] TIM6 global and DAC channel 1&2 underrun error  interrupts */
    (void*) isr_tim7,               /* [55] TIM7 global Interrupt */
    (void*) isr_dma2_channel1,      /* [56] DMA2 Channel 1 global Interrupt */
    (void*) isr_dma2_channel2,      /* [57] DMA2 Channel 2 global Interrupt */
    (void*) isr_dma2_channel3,      /* [58] DMA2 Channel 3 global Interrupt */
    (void*) isr_dma2_channel4,      /* [59] DMA2 Channel 4 global Interrupt */
    (void*) isr_dma2_channel5,      /* [60] DMA2 Channel 5 global Interrupt */
    (void*) isr_adc4,               /* [61] ADC4  global Interrupt */
    (void*) (0UL),                  /* [62] Reserved */
    (void*) (0UL),                  /* [63] Reserved */
    (void*) isr_comp1_2_3,          /* [64] COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29 */
    (void*) isr_comp4_5_6,          /* [65] COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32 */
    (void*) isr_comp7,              /* [66] COMP7 global Interrupt via EXTI Line33 */
    (void*) (0UL),                  /* [67] Reserved */
    (void*) (0UL),                  /* [68] Reserved */
    (void*) (0UL),                  /* [69] Reserved */
    (void*) (0UL),                  /* [70] Reserved */
    (void*) (0UL),                  /* [71] Reserved */
#if defined(CPU_MODEL_STM32F303VC)
    (void*) (0UL),                  /* [72] Reserved */
    (void*) (0UL),                  /* [73] Reserved */
    (void*) isr_usb_hp,             /* [74] USB High Priority global Interrupt remap */
    (void*) isr_usb_lp,             /* [75] USB Low Priority global Interrupt  remap */
    (void*) isr_usbwakeup_rmp,      /* [76] USB Wakeup Interrupt remap */
    (void*) (0UL),                  /* [77] Reserved */
    (void*) (0UL),                  /* [78] Reserved */
    (void*) (0UL),                  /* [79] Reserved */
    (void*) (0UL),                  /* [80] Reserved */
    (void*) isr_fpu                 /* [81] Floating point Interrupt */
#else /* CPU_MODEL_STM32F303RE, CPU_MODEL_STM32F303ZE */
    (void*) isr_i2c3_ev,            /* [72] I2C3 event interrupt */
    (void*) isr_i2c3_er,            /* [73] I2C3 error interrupt */
    (void*) isr_usb_hp,             /* [74] USB High Priority global Interrupt remap */
    (void*) isr_usb_lp,             /* [75] USB Low Priority global Interrupt  remap */
    (void*) isr_usbwakeup_rmp,      /* [76] USB Wakeup Interrupt remap */
    (void*) isr_tim20_brk,          /* [77] TIM20 Break Interrupt */
    (void*) isr_tim20_up,           /* [78] TIM20 Update Interrupt */
    (void*) isr_tim20_trg_com,      /* [79] TIM20 Trigger and Commutation Interrupt */
    (void*) isr_tim8_cc,            /* [80] TIM20 Capture Compare Interrupt */
    (void*) isr_fpu,                /* [81] Floating point Interrupt */
    (void*) (0UL),                  /* [82] Reserved */
    (void*) (0UL),                  /* [83] Reserved */
    (void*) isr_spi4,               /* [84] SPI4 global Interrupt */
#endif
#endif
};
