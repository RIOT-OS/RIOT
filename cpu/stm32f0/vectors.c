/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
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
/* STM32F0 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_ts(void);
WEAK_DEFAULT void isr_dma1_ch1(void);
WEAK_DEFAULT void isr_dma1_ch2_3(void);
WEAK_DEFAULT void isr_dma1_ch4_5(void);
WEAK_DEFAULT void isr_adc1_comp(void);
WEAK_DEFAULT void isr_tim1_brk_up_trg_com(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim2(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim6_dac(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_tim14(void);
WEAK_DEFAULT void isr_tim15(void);
WEAK_DEFAULT void isr_tim16(void);
WEAK_DEFAULT void isr_tim17(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_spi1(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3_8(void);
WEAK_DEFAULT void isr_cec(void);
WEAK_DEFAULT void isr_usb(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),              /* pointer to the top of the stack */
    /* Cortex-M0 handlers */
    (void*) reset_handler_default,   /* entry point of the program */
    (void*) nmi_default,             /* [-14] non maskable interrupt handler */
    (void*) hard_fault_default,      /* [-13] hard fault exception */
    (void*) (0UL),                   /* [-12] reserved */
    (void*) (0UL),                   /* [-11] reserved */
    (void*) (0UL),                   /* [-10] reserved */
    (void*) (0UL),                   /* [-9] reserved */
    (void*) (0UL),                   /* [-8] reserved */
    (void*) (0UL),                   /* [-7] reserved */
    (void*) (0UL),                   /* [-6] reserved */
    (void*) isr_svc,                 /* [-5] system call interrupt, in RIOT used for
                                            switching into thread context on boot */
    (void*) (0UL),                   /* [-4] reserved */
    (void*) (0UL),                   /* [-3] reserved */
    (void*) isr_pendsv,              /* [-2] pendSV interrupt, in RIOT the actual
                                            context switching is happening here */
    (void*) isr_systick,             /* [-1] SysTick interrupt, not used in RIOT */
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,                /* [0] windowed watchdog */
#if defined(CPU_MODEL_STM32F030R8)
    (void*) (0UL),                   /* [1] reserved */
    (void*) isr_rtc,                 /* [2] real time clock */
    (void*) isr_flash,               /* [3] flash memory controller */
    (void*) isr_rcc,                 /* [4] reset and clock control */
    (void*) isr_exti,                /* [5] external interrupt lines 0 and 1 */
    (void*) isr_exti,                /* [6] external interrupt lines 2 and 3 */
    (void*) isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (void*) (0UL),                   /* [8] reserved */
    (void*) isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,           /* [12] analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,             /* [14] timer 1 capture compare */
    (void*) isr_tim2,                /* [15] timer 2 */
    (void*) isr_tim3,                /* [16] timer 3 */
    (void*) isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    (void*) (0UL),                   /* [18] reserved */
    (void*) isr_tim14,               /* [19] timer 14 */
    (void*) isr_tim15,               /* [20] timer 15 */
    (void*) isr_tim16,               /* [21] timer 16 */
    (void*) isr_tim17,               /* [22] timer 17 */
    (void*) isr_i2c1,                /* [23] I2C 1 */
    (void*) isr_i2c2,                /* [24] I2C 2 */
    (void*) isr_spi1,                /* [25] SPI 1 */
    (void*) isr_spi2,                /* [26] SPI 2 */
    (void*) isr_usart1,              /* [27] USART 1 */
    (void*) isr_usart2               /* [28] USART 2 */
#elif defined(CPU_MODEL_STM32F031K6)
    (void*) isr_pvd,                 /* [1] power control */
    (void*) isr_rtc,                 /* [2] real time clock */
    (void*) isr_flash,               /* [3] flash memory controller */
    (void*) isr_rcc,                 /* [4] reset and clock control */
    (void*) isr_exti,                /* [5] external interrupt lines 0 and 1 */
    (void*) isr_exti,                /* [6] external interrupt lines 2 and 3 */
    (void*) isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (void*) (0UL),                   /* [8] reserved */
    (void*) isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,           /* [12] analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,             /* [14] timer 1 capture compare */
    (void*) isr_tim2,                /* [15] timer 2 */
    (void*) isr_tim3,                /* [16] timer 3 */
    (void*) (0UL),                   /* [17] reserved */
    (void*) (0UL),                   /* [18] reserved */
    (void*) isr_tim14,               /* [19] timer 14 */
    (void*) (0UL),                   /* [20] reserved */
    (void*) isr_tim16,               /* [21] timer 16 */
    (void*) isr_tim17,               /* [22] timer 17 */
    (void*) isr_i2c1,                /* [23] I2C 1 */
    (void*) (0UL),                   /* [24] reserved */
    (void*) isr_spi1,                /* [25] SPI 1 */
    (void*) (0UL),                   /* [26] reserved */
    (void*) isr_usart1               /* [27] USART 1 */
#elif defined(CPU_MODEL_STM32F042K6)
    (void*) isr_pvd,                 /* [1] power control */
    (void*) isr_rtc,                 /* [2] real time clock */
    (void*) isr_flash,               /* [3] flash memory controller */
    (void*) isr_rcc,                 /* [4] reset and clock control */
    (void*) isr_exti,                /* [5] external interrupt lines 0 and 1 */
    (void*) isr_exti,                /* [6] external interrupt lines 2 and 3 */
    (void*) isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (void*) isr_ts,                  /* [8] touch sensing input*/
    (void*) isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,           /* [12] analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,             /* [14] timer 1 capture compare */
    (void*) isr_tim2,                /* [15] timer 2 */
    (void*) isr_tim3,                /* [16] timer 3 */
    (void*) (0UL),                   /* [17] reserved */
    (void*) (0UL),                   /* [18] reserved */
    (void*) isr_tim14,               /* [19] timer 14 */
    (void*) (0UL),                   /* [20] reserved */
    (void*) isr_tim16,               /* [21] timer 16 */
    (void*) isr_tim17,               /* [22] timer 17 */
    (void*) isr_i2c1,                /* [23] I2C 1 */
    (void*) (0UL),                   /* [24] reserved */
    (void*) isr_spi1,                /* [25] SPI 1 */
    (void*) isr_spi2,                /* [26] SPI 2 */
    (void*) isr_usart1,              /* [27] USART 1 */
    (void*) isr_usart2,              /* [28] USART 2 */
    (void*) (0UL),                   /* [29] reserved */
    (void*) isr_cec,                 /* [30] consumer electronics control */
    (void*) isr_usb                  /* [31] USB global Interrupts & EXTI Line18 Interrup */
#elif defined(CPU_MODEL_STM32F051R8)
    (void*) isr_pvd,                 /* [1] power control */
    (void*) isr_rtc,                 /* [2] real time clock */
    (void*) isr_flash,               /* [3] flash memory controller */
    (void*) isr_rcc,                 /* [4] reset and clock control */
    (void*) isr_exti,                /* [5] external interrupt lines 0 and 1 */
    (void*) isr_exti,                /* [6] external interrupt lines 2 and 3 */
    (void*) isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (void*) isr_ts,                  /* [8] touch sensing input*/
    (void*) isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,           /* [12] analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,             /* [14] timer 1 capture compare */
    (void*) isr_tim2,                /* [15] timer 2 */
    (void*) isr_tim3,                /* [16] timer 3 */
    (void*) isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    (void*) (0UL),                   /* [18] reserved */
    (void*) isr_tim14,               /* [19] timer 14 */
    (void*) isr_tim15,               /* [20] timer 15 */
    (void*) isr_tim16,               /* [21] timer 16 */
    (void*) isr_tim17,               /* [22] timer 17 */
    (void*) isr_i2c1,                /* [23] I2C 1 */
    (void*) isr_i2c2,                /* [24] I2C 2 */
    (void*) isr_spi1,                /* [25] SPI 1 */
    (void*) isr_spi2,                /* [26] SPI 2 */
    (void*) isr_usart1,              /* [27] USART 1 */
    (void*) isr_usart2,              /* [28] USART 2 */
    (void*) (0UL),                   /* [29] reserved */
    (void*) isr_cec,                 /* [30] consumer electronics control */
#elif defined(CPU_MODEL_STM32F070RB)
    (void*) (0UL),                   /* [1] reserved */
    (void*) isr_rtc,                 /* [2] real time clock */
    (void*) isr_flash,               /* [3] flash memory controller */
    (void*) isr_rcc,                 /* [4] reset and clock control */
    (void*) isr_exti,                /* [5] external interrupt lines 0 and 1 */
    (void*) isr_exti,                /* [6] external interrupt lines 2 and 3 */
    (void*) isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (void*) isr_ts,                  /* [8] touch sensing input*/
    (void*) isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,           /* [12] analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,             /* [14] timer 1 capture compare */
    (void*) (0UL),                   /* [15] reserved */
    (void*) isr_tim3,                /* [16] timer 3 */
    (void*) isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    (void*) isr_tim7,                /* [18] timer 7 */
    (void*) isr_tim14,               /* [19] timer 14 */
    (void*) isr_tim15,               /* [20] timer 15 */
    (void*) isr_tim16,               /* [21] timer 16 */
    (void*) isr_tim17,               /* [22] timer 17 */
    (void*) isr_i2c1,                /* [23] I2C 1 */
    (void*) isr_i2c2,                /* [24] I2C 2 */
    (void*) isr_spi1,                /* [25] SPI 1 */
    (void*) isr_spi2,                /* [26] SPI 2 */
    (void*) isr_usart1,              /* [27] USART 1 */
    (void*) isr_usart2,              /* [28] USART 2 */
    (void*) isr_usart3_8,            /* [29] USART 3 to 8 */
    (void*) (0UL),                   /* [30] reserved */
    (void*) isr_usb                  /* [31] USB global Interrupts & EXTI Line18 Interrup */
#else /* CPU_MODEL_STM32_F072RB, CPU_MODEL_STM32F091RC*/
    (void*) isr_pvd,                 /* [1] power control */
    (void*) isr_rtc,                 /* [2] real time clock */
    (void*) isr_flash,               /* [3] flash memory controller */
    (void*) isr_rcc,                 /* [4] reset and clock control */
    (void*) isr_exti,                /* [5] external interrupt lines 0 and 1 */
    (void*) isr_exti,                /* [6] external interrupt lines 2 and 3 */
    (void*) isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (void*) isr_ts,                  /* [8] touch sensing input*/
    (void*) isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,           /* [12] analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,             /* [14] timer 1 capture compare */
    (void*) isr_tim2,                /* [15] timer 2 */
    (void*) isr_tim3,                /* [16] timer 3 */
    (void*) isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    (void*) isr_tim7,                /* [18] timer 7 */
    (void*) isr_tim14,               /* [19] timer 14 */
    (void*) isr_tim15,               /* [20] timer 15 */
    (void*) isr_tim16,               /* [21] timer 16 */
    (void*) isr_tim17,               /* [22] timer 17 */
    (void*) isr_i2c1,                /* [23] I2C 1 */
    (void*) isr_i2c2,                /* [24] I2C 2 */
    (void*) isr_spi1,                /* [25] SPI 1 */
    (void*) isr_spi2,                /* [26] SPI 2 */
    (void*) isr_usart1,              /* [27] USART 1 */
    (void*) isr_usart2,              /* [28] USART 2 */
    (void*) isr_usart3_8,            /* [29] USART 3 to 8 */
    (void*) isr_cec,                 /* [30] consumer electronics control */
#if defined(CPU_MODEL_STM32F072RB)
    (void*) isr_usb                  /* [31] USB global Interrupts & EXTI Line18 Interrup */
#endif
#endif
};
