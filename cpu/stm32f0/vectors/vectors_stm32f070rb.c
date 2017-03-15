/*
 * Copyright (C) 2017 Inria
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
 * @brief       Interrupt vector definitions for stm32f070rb
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

#if defined(CPU_MODEL_STM32F070RB)
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_dma1_ch1(void);
WEAK_DEFAULT void isr_dma1_ch2_3(void);
WEAK_DEFAULT void isr_dma1_ch4_5(void);
WEAK_DEFAULT void isr_adc1_comp(void);
WEAK_DEFAULT void isr_tim1_brk_up_trg_com(void);
WEAK_DEFAULT void isr_tim1_cc(void);
WEAK_DEFAULT void isr_tim3(void);
WEAK_DEFAULT void isr_tim6_dac(void);
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
WEAK_DEFAULT void isr_usart3_4(void);
WEAK_DEFAULT void isr_usb(void);

ISR_VECTORS const void *mcu_interrupt_vector[] = {
    (void*) isr_wwdg,                /**< windowed watchdog */
    (void*) (0UL),                   /**< reserved */
    (void*) isr_rtc,                 /**< real time clock */
    (void*) isr_flash,               /**< flash memory controller */
    (void*) isr_rcc,                 /**< reset and clock control */
    (void*) isr_exti,                /**< external interrupt lines 0 and 1 */
    (void*) isr_exti,                /**< external interrupt lines 2 and 3 */
    (void*) isr_exti,                /**< external interrupt lines 4 to 15 */
    (void*) (0UL),                   /**< reserved */
    (void*) isr_dma1_ch1,            /**< direct memory access controller 1, channel 1*/
    (void*) isr_dma1_ch2_3,          /**< direct memory access controller 1, channel 2 and 3*/
    (void*) isr_dma1_ch4_5,          /**< direct memory access controller 1, channel 4 and 5*/
    (void*) isr_adc1_comp,           /**< analog digital converter */
    (void*) isr_tim1_brk_up_trg_com, /**< timer 1 break, update, trigger and communication */
    (void*) isr_tim1_cc,             /**< timer 1 capture compare */
    (void*) (0UL),                   /**< reserved */
    (void*) isr_tim3,                /**< timer 3 */
    (void*) isr_tim6_dac,            /**< timer 6 and digital to analog converter */
    (void*) (0UL),                   /**< reserved */
    (void*) isr_tim14,               /**< timer 14 */
    (void*) isr_tim15,               /**< timer 15 */
    (void*) isr_tim16,               /**< timer 16 */
    (void*) isr_tim17,               /**< timer 17 */
    (void*) isr_i2c1,                /**< I2C 1 */
    (void*) isr_i2c2,                /**< I2C 2 */
    (void*) isr_spi1,                /**< SPI 1 */
    (void*) isr_spi2,                /**< SPI 2 */
    (void*) isr_usart1,              /**< USART 1 */
    (void*) isr_usart2,              /**< USART 2 */
    (void*) isr_usart3_4,            /**< USART 3 and 4 */
    (void*) (0UL),                   /**< reserved */
    (void*) isr_usb                  /**< USB */
};
#endif
