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
#if defined(CPU_MODEL_STM32F413ZH)
/* STM32F413 specific interrupt vectors (CAN3)
 * See RM0430, part 10.2 */
WEAK_DEFAULT void isr_can3_tx(void);
WEAK_DEFAULT void isr_can3_rx0(void);
WEAK_DEFAULT void isr_can3_rx1(void);
WEAK_DEFAULT void isr_can3_sce(void);
#else
WEAK_DEFAULT void isr_otg_hs_ep1_out(void);
WEAK_DEFAULT void isr_otg_hs_ep1_in(void);
WEAK_DEFAULT void isr_otg_hs_wkup(void);
WEAK_DEFAULT void isr_otg_hs(void);
WEAK_DEFAULT void isr_dcmi(void);
#endif
WEAK_DEFAULT void isr_cryp(void);
WEAK_DEFAULT void isr_hash_rng(void);
WEAK_DEFAULT void isr_fpu(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) mem_manage_default,     /* memory manage exception */
    (void*) bus_fault_default,      /* bus fault exception */
    (void*) usage_fault_default,    /* usage fault exception */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) debug_mon_default,      /* debug monitor exception */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,               /* Window WatchDog */
    (void*) isr_pvd,                /* PVD through EXTI Line detection */
    (void*) isr_tamp_stamp,         /* Tamper and TimeStamps through the EXTI line */
    (void*) isr_rtc_wkup,           /* RTC Wakeup through the EXTI line */
    (void*) isr_flash,              /* FLASH */
    (void*) isr_rcc,                /* RCC */
    (void*) isr_exti,               /* EXTI Line0 */
    (void*) isr_exti,               /* EXTI Line1 */
    (void*) isr_exti,               /* EXTI Line2 */
    (void*) isr_exti,               /* EXTI Line3 */
    (void*) isr_exti,               /* EXTI Line4 */
    (void*) isr_dma1_stream0,       /* DMA1 Stream 0 */
    (void*) isr_dma1_stream1,       /* DMA1 Stream 1 */
    (void*) isr_dma1_stream2,       /* DMA1 Stream 2 */
    (void*) isr_dma1_stream3,       /* DMA1 Stream 3 */
    (void*) isr_dma1_stream4,       /* DMA1 Stream 4 */
    (void*) isr_dma1_stream5,       /* DMA1 Stream 5 */
    (void*) isr_dma1_stream6,       /* DMA1 Stream 6 */
    (void*) isr_adc,                /* ADC1, ADC2 and ADC3s */
    (void*) isr_can1_tx,            /* CAN1 TX */
    (void*) isr_can1_rx0,           /* CAN1 RX0 */
    (void*) isr_can1_rx1,           /* CAN1 RX1 */
    (void*) isr_can1_sce,           /* CAN1 SCE */
    (void*) isr_exti,               /* External Line[9:5]s */
    (void*) isr_tim1_brk_tim9,      /* TIM1 Break and TIM9 */
    (void*) isr_tim1_up_tim10,      /* TIM1 Update and TIM10 */
    (void*) isr_tim1_trg_com_tim11, /* TIM1 Trigger and Commutation and TIM11 */
    (void*) isr_tim1_cc,            /* TIM1 Capture Compare */
    (void*) isr_tim2,               /* TIM2 */
    (void*) isr_tim3,               /* TIM3 */
    (void*) isr_tim4,               /* TIM4 */
    (void*) isr_i2c1_ev,            /* I2C1 Event */
    (void*) isr_i2c1_er,            /* I2C1 Error */
    (void*) isr_i2c2_ev,            /* I2C2 Event */
    (void*) isr_i2c2_er,            /* I2C2 Error */
    (void*) isr_spi1,               /* SPI1 */
    (void*) isr_spi2,               /* SPI2 */
    (void*) isr_usart1,             /* USART1 */
    (void*) isr_usart2,             /* USART2 */
    (void*) isr_usart3,             /* USART3 */
    (void*) isr_exti,               /* External Line[15:10]s */
    (void*) isr_rtc_alarm,          /* RTC Alarm (A and B) through EXTI Line */
    (void*) isr_otg_fs_wkup,        /* USB OTG FS Wakeup through EXTI line */
    (void*) isr_tim8_brk_tim12,     /* TIM8 Break and TIM12 */
    (void*) isr_tim8_up_tim13,      /* TIM8 Update and TIM13 */
    (void*) isr_tim8_trg_com_tim14, /* TIM8 Trigger and Commutation and TIM14 */
    (void*) isr_tim8_cc,            /* TIM8 Capture Compare */
    (void*) isr_dma1_stream7,       /* DMA1 Stream7 */
    (void*) isr_fsmc,               /* FSMC */
    (void*) isr_sdio,               /* SDIO */
    (void*) isr_tim5,               /* TIM5 */
    (void*) isr_spi3,               /* SPI3 */
    (void*) isr_uart4,              /* UART4 */
    (void*) isr_uart5,              /* UART5 */
    (void*) isr_tim6_dac,           /* TIM6 and DAC1&2 underrun errors */
    (void*) isr_tim7,               /* TIM7 */
    (void*) isr_dma2_stream0,       /* DMA2 Stream 0 */
    (void*) isr_dma2_stream1,       /* DMA2 Stream 1 */
    (void*) isr_dma2_stream2,       /* DMA2 Stream 2 */
    (void*) isr_dma2_stream3,       /* DMA2 Stream 3 */
    (void*) isr_dma2_stream4,       /* DMA2 Stream 4 */
    (void*) isr_eth,                /* Ethernet */
    (void*) isr_eth_wkup,           /* Ethernet Wakeup through EXTI line */
    (void*) isr_can2_tx,            /* CAN2 TX */
    (void*) isr_can2_rx0,           /* CAN2 RX0 */
    (void*) isr_can2_rx1,           /* CAN2 RX1 */
    (void*) isr_can2_sce,           /* CAN2 SCE */
    (void*) isr_otg_fs,             /* USB OTG FS */
    (void*) isr_dma2_stream5,       /* DMA2 Stream 5 */
    (void*) isr_dma2_stream6,       /* DMA2 Stream 6 */
    (void*) isr_dma2_stream7,       /* DMA2 Stream 7 */
    (void*) isr_usart6,             /* USART6 */
    (void*) isr_i2c3_ev,            /* I2C3 event */
    (void*) isr_i2c3_er,            /* I2C3 error */
#if defined(CPU_MODEL_STM32F413ZH)
    (void*) isr_can3_tx,            /* CAN3 TX */
    (void*) isr_can3_rx0,           /* CAN3 RX0 */
    (void*) isr_can3_rx1,           /* CAN3 RX1 */
    (void*) isr_can3_sce,           /* CAN3 SCE */
    (void*) (0UL),                  /* Reserved */
#else
    (void*) isr_otg_hs_ep1_out,     /* USB OTG HS End Point 1 Out */
    (void*) isr_otg_hs_ep1_in,      /* USB OTG HS End Point 1 In */
    (void*) isr_otg_hs_wkup,        /* USB OTG HS Wakeup through EXTI */
    (void*) isr_otg_hs,             /* USB OTG HS */
    (void*) isr_dcmi,               /* DCMI */
#endif
    (void*) isr_cryp,               /* CRYP crypto */
    (void*) isr_hash_rng,           /* Hash and Rng */
    (void*) isr_fpu,                /* FPU */
};
