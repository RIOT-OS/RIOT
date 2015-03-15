/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

/**
 * memory markers as defined in the linker script
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. load data section from flash to ram
 * 2. overwrite uninitialized data section (BSS) with zeros
 * 3. initialize the newlib
 * 4. initialize the board (sync clock, setup std-IO)
 * 5. initialize and start RIOTs kernel
 */
void reset_handler(void)
{
    uint32_t *dst;
    uint32_t *src = &_etext;

    /* load data section from flash to ram */
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
void dummy_handler(void)
{
    while (1) {asm ("nop");}
}

void isr_nmi(void)
{
    while (1) {asm ("nop");}
}

void isr_mem_manage(void)
{
    while (1) {asm ("nop");}
}

void isr_debug_mon(void)
{
    while (1) {asm ("nop");}
}

void isr_hard_fault(void)
{
    while (1) {asm ("nop");}
}

void isr_bus_fault(void)
{
    while (1) {asm ("nop");}
}

void isr_usage_fault(void)
{
    while (1) {asm ("nop");}
}

/* Cortex-M specific interrupt vectors */
void isr_svc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)              __attribute__ ((weak, alias("dummy_handler")));

/* STM32F4 specific interrupt vector */
void isr_wwdg(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_pvd(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_tamp_stamp(void)           __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_wkup(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_flash(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_rcc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_exti0(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_exti1(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_exti2(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_exti3(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_exti4(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream0(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream1(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream2(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream3(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream4(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream5(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream6(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_adc(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_can1_tx(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_can1_rx0(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_can1_rx1(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_can1_sce(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_exti9_5(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_brk_tim9(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_up_tim10(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_trg_com_tim11(void)   __attribute__ ((weak, alias("dummy_handler")));
void isr_tim1_cc(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_tim2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tim3(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tim4(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1_ev(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c1_er(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2_ev(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c2_er(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_spi1(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi2(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_usart1(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_usart2(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_usart3(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_exti15_10(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_rtc_alarm(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_otg_fs_wkup(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_brk_tim12(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_up_tim13(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_trg_com_tim14(void)   __attribute__ ((weak, alias("dummy_handler")));
void isr_tim8_cc(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_dma1_stream7(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_fsmc(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_sdio(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_tim5(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_spi3(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_uart4(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_uart5(void)                __attribute__ ((weak, alias("dummy_handler")));
void isr_tim6_dac(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_tim7(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream0(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream1(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream2(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream3(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream4(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_eth(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_eth_wkup(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_can2_tx(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_can2_rx0(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_can2_rx1(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_can2_sce(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_otg_fs(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream5(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream6(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_dma2_stream7(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_usart6(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c3_ev(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_i2c3_er(void)              __attribute__ ((weak, alias("dummy_handler")));
void isr_otg_hs_ep1_out(void)       __attribute__ ((weak, alias("dummy_handler")));
void isr_otg_hs_ep1_in(void)        __attribute__ ((weak, alias("dummy_handler")));
void isr_otg_hs_wkup(void)          __attribute__ ((weak, alias("dummy_handler")));
void isr_otg_hs(void)               __attribute__ ((weak, alias("dummy_handler")));
void isr_dcmi(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_cryp(void)                 __attribute__ ((weak, alias("dummy_handler")));
void isr_hash_rng(void)             __attribute__ ((weak, alias("dummy_handler")));
void isr_fpu(void)                  __attribute__ ((weak, alias("dummy_handler")));
void isr_spi4(void)                 __attribute__ ((weak, alias("dummy_handler")));

/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Stack pointer */
    (void*) (&_estack),             /* pointer to the top of the empty stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler,          /* entry point of the program */
    (void*) isr_nmi,                /* non maskable interrupt handler */
    (void*) isr_hard_fault,         /* if you end up here its not good */
    (void*) isr_mem_manage,         /* memory controller interrupt */
    (void*) isr_bus_fault,          /* also not good to end up here */
    (void*) isr_usage_fault,        /* autsch */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt */
    (void*) isr_debug_mon,          /* debug interrupt */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, used for task switching in RIOT */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,               /* Window WatchDog */
    (void*) isr_pvd,                /* PVD through EXTI Line detection */
    (void*) isr_tamp_stamp,         /* Tamper and TimeStamps through the EXTI line */
    (void*) isr_rtc_wkup,           /* RTC Wakeup through the EXTI line */
    (void*) isr_flash,              /* FLASH */
    (void*) isr_rcc,                /* RCC */
    (void*) isr_exti0,              /* EXTI Line0 */
    (void*) isr_exti1,              /* EXTI Line1 */
    (void*) isr_exti2,              /* EXTI Line2 */
    (void*) isr_exti3,              /* EXTI Line3 */
    (void*) isr_exti4,              /* EXTI Line4 */
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
    (void*) isr_exti9_5,            /* External Line[9:5]s */
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
    (void*) isr_exti15_10,          /* External Line[15:10]s */
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
    (void*) isr_otg_hs_ep1_out,     /* USB OTG HS End Point 1 Out */
    (void*) isr_otg_hs_ep1_in,      /* USB OTG HS End Point 1 In */
    (void*) isr_otg_hs_wkup,        /* USB OTG HS Wakeup through EXTI */
    (void*) isr_otg_hs,             /* USB OTG HS */
    (void*) isr_dcmi,               /* DCMI */
    (void*) isr_cryp,               /* CRYP crypto */
    (void*) isr_hash_rng,           /* Hash and Rng */
    (void*) isr_fpu,                /* FPU */
#ifdef CPU_MODEL_STM32F401RE
    (void*) isr_spi4,
#endif
};
