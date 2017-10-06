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

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_wwdg,                /* [0] windowed watchdog */
#if defined(CPU_MODEL_STM32F030R8)
    (0UL),                   /* [1] reserved */
    isr_rtc,                 /* [2] real time clock */
    isr_flash,               /* [3] flash memory controller */
    isr_rcc,                 /* [4] reset and clock control */
    isr_exti,                /* [5] external interrupt lines 0 and 1 */
    isr_exti,                /* [6] external interrupt lines 2 and 3 */
    isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (0UL),                   /* [8] reserved */
    isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    isr_adc1_comp,           /* [12] analog digital converter */
    isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    isr_tim1_cc,             /* [14] timer 1 capture compare */
    isr_tim2,                /* [15] timer 2 */
    isr_tim3,                /* [16] timer 3 */
    isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    (0UL),                   /* [18] reserved */
    isr_tim14,               /* [19] timer 14 */
    isr_tim15,               /* [20] timer 15 */
    isr_tim16,               /* [21] timer 16 */
    isr_tim17,               /* [22] timer 17 */
    isr_i2c1,                /* [23] I2C 1 */
    isr_i2c2,                /* [24] I2C 2 */
    isr_spi1,                /* [25] SPI 1 */
    isr_spi2,                /* [26] SPI 2 */
    isr_usart1,              /* [27] USART 1 */
    isr_usart2               /* [28] USART 2 */
#elif defined(CPU_MODEL_STM32F031K6)
    isr_pvd,                 /* [1] power control */
    isr_rtc,                 /* [2] real time clock */
    isr_flash,               /* [3] flash memory controller */
    isr_rcc,                 /* [4] reset and clock control */
    isr_exti,                /* [5] external interrupt lines 0 and 1 */
    isr_exti,                /* [6] external interrupt lines 2 and 3 */
    isr_exti,                /* [7] external interrupt lines 4 to 15 */
    (0UL),                   /* [8] reserved */
    isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    isr_adc1_comp,           /* [12] analog digital converter */
    isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    isr_tim1_cc,             /* [14] timer 1 capture compare */
    isr_tim2,                /* [15] timer 2 */
    isr_tim3,                /* [16] timer 3 */
    (0UL),                   /* [17] reserved */
    (0UL),                   /* [18] reserved */
    isr_tim14,               /* [19] timer 14 */
    (0UL),                   /* [20] reserved */
    isr_tim16,               /* [21] timer 16 */
    isr_tim17,               /* [22] timer 17 */
    isr_i2c1,                /* [23] I2C 1 */
    (0UL),                   /* [24] reserved */
    isr_spi1,                /* [25] SPI 1 */
    (0UL),                   /* [26] reserved */
    isr_usart1               /* [27] USART 1 */
#elif defined(CPU_MODEL_STM32F042K6)
    isr_pvd,                 /* [1] power control */
    isr_rtc,                 /* [2] real time clock */
    isr_flash,               /* [3] flash memory controller */
    isr_rcc,                 /* [4] reset and clock control */
    isr_exti,                /* [5] external interrupt lines 0 and 1 */
    isr_exti,                /* [6] external interrupt lines 2 and 3 */
    isr_exti,                /* [7] external interrupt lines 4 to 15 */
    isr_ts,                  /* [8] touch sensing input*/
    isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    isr_adc1_comp,           /* [12] analog digital converter */
    isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    isr_tim1_cc,             /* [14] timer 1 capture compare */
    isr_tim2,                /* [15] timer 2 */
    isr_tim3,                /* [16] timer 3 */
    (0UL),                   /* [17] reserved */
    (0UL),                   /* [18] reserved */
    isr_tim14,               /* [19] timer 14 */
    (0UL),                   /* [20] reserved */
    isr_tim16,               /* [21] timer 16 */
    isr_tim17,               /* [22] timer 17 */
    isr_i2c1,                /* [23] I2C 1 */
    (0UL),                   /* [24] reserved */
    isr_spi1,                /* [25] SPI 1 */
    isr_spi2,                /* [26] SPI 2 */
    isr_usart1,              /* [27] USART 1 */
    isr_usart2,              /* [28] USART 2 */
    (0UL),                   /* [29] reserved */
    isr_cec,                 /* [30] consumer electronics control */
    isr_usb                  /* [31] USB global Interrupts & EXTI Line18 Interrup */
#elif defined(CPU_MODEL_STM32F051R8)
    isr_pvd,                 /* [1] power control */
    isr_rtc,                 /* [2] real time clock */
    isr_flash,               /* [3] flash memory controller */
    isr_rcc,                 /* [4] reset and clock control */
    isr_exti,                /* [5] external interrupt lines 0 and 1 */
    isr_exti,                /* [6] external interrupt lines 2 and 3 */
    isr_exti,                /* [7] external interrupt lines 4 to 15 */
    isr_ts,                  /* [8] touch sensing input*/
    isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    isr_adc1_comp,           /* [12] analog digital converter */
    isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    isr_tim1_cc,             /* [14] timer 1 capture compare */
    isr_tim2,                /* [15] timer 2 */
    isr_tim3,                /* [16] timer 3 */
    isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    (0UL),                   /* [18] reserved */
    isr_tim14,               /* [19] timer 14 */
    isr_tim15,               /* [20] timer 15 */
    isr_tim16,               /* [21] timer 16 */
    isr_tim17,               /* [22] timer 17 */
    isr_i2c1,                /* [23] I2C 1 */
    isr_i2c2,                /* [24] I2C 2 */
    isr_spi1,                /* [25] SPI 1 */
    isr_spi2,                /* [26] SPI 2 */
    isr_usart1,              /* [27] USART 1 */
    isr_usart2,              /* [28] USART 2 */
    (0UL),                   /* [29] reserved */
    isr_cec,                 /* [30] consumer electronics control */
#elif defined(CPU_MODEL_STM32F070RB)
    (0UL),                   /* [1] reserved */
    isr_rtc,                 /* [2] real time clock */
    isr_flash,               /* [3] flash memory controller */
    isr_rcc,                 /* [4] reset and clock control */
    isr_exti,                /* [5] external interrupt lines 0 and 1 */
    isr_exti,                /* [6] external interrupt lines 2 and 3 */
    isr_exti,                /* [7] external interrupt lines 4 to 15 */
    isr_ts,                  /* [8] touch sensing input*/
    isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    isr_adc1_comp,           /* [12] analog digital converter */
    isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    isr_tim1_cc,             /* [14] timer 1 capture compare */
    (0UL),                   /* [15] reserved */
    isr_tim3,                /* [16] timer 3 */
    isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    isr_tim7,                /* [18] timer 7 */
    isr_tim14,               /* [19] timer 14 */
    isr_tim15,               /* [20] timer 15 */
    isr_tim16,               /* [21] timer 16 */
    isr_tim17,               /* [22] timer 17 */
    isr_i2c1,                /* [23] I2C 1 */
    isr_i2c2,                /* [24] I2C 2 */
    isr_spi1,                /* [25] SPI 1 */
    isr_spi2,                /* [26] SPI 2 */
    isr_usart1,              /* [27] USART 1 */
    isr_usart2,              /* [28] USART 2 */
    isr_usart3_8,            /* [29] USART 3 to 8 */
    (0UL),                   /* [30] reserved */
    isr_usb                  /* [31] USB global Interrupts & EXTI Line18 Interrup */
#else /* CPU_MODEL_STM32_F072RB, CPU_MODEL_STM32F091RC*/
    isr_pvd,                 /* [1] power control */
    isr_rtc,                 /* [2] real time clock */
    isr_flash,               /* [3] flash memory controller */
    isr_rcc,                 /* [4] reset and clock control */
    isr_exti,                /* [5] external interrupt lines 0 and 1 */
    isr_exti,                /* [6] external interrupt lines 2 and 3 */
    isr_exti,                /* [7] external interrupt lines 4 to 15 */
    isr_ts,                  /* [8] touch sensing input*/
    isr_dma1_ch1,            /* [9] direct memory access controller 1, channel 1*/
    isr_dma1_ch2_3,          /* [10] direct memory access controller 1, channel 2 and 3*/
    isr_dma1_ch4_5,          /* [11] direct memory access controller 1, channel 4 and 5*/
    isr_adc1_comp,           /* [12] analog digital converter */
    isr_tim1_brk_up_trg_com, /* [13] timer 1 break, update, trigger and communication */
    isr_tim1_cc,             /* [14] timer 1 capture compare */
    isr_tim2,                /* [15] timer 2 */
    isr_tim3,                /* [16] timer 3 */
    isr_tim6_dac,            /* [17] timer 6 and digital to analog converter */
    isr_tim7,                /* [18] timer 7 */
    isr_tim14,               /* [19] timer 14 */
    isr_tim15,               /* [20] timer 15 */
    isr_tim16,               /* [21] timer 16 */
    isr_tim17,               /* [22] timer 17 */
    isr_i2c1,                /* [23] I2C 1 */
    isr_i2c2,                /* [24] I2C 2 */
    isr_spi1,                /* [25] SPI 1 */
    isr_spi2,                /* [26] SPI 2 */
    isr_usart1,              /* [27] USART 1 */
    isr_usart2,              /* [28] USART 2 */
    isr_usart3_8,            /* [29] USART 3 to 8 */
    isr_cec,                 /* [30] consumer electronics control */
#if defined(CPU_MODEL_STM32F072RB)
    isr_usb                  /* [31] USB global Interrupts & EXTI Line18 Interrup */
#endif
#endif
};
