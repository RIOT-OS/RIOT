/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* STM32L1 specific interrupt vectors */
WEAK_DEFAULT void isr_wwdg(void);
WEAK_DEFAULT void isr_pvd(void);
WEAK_DEFAULT void isr_tamper_stamp(void);
WEAK_DEFAULT void isr_rtc_wkup(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_rcc(void);
WEAK_DEFAULT void isr_exti(void);
WEAK_DEFAULT void isr_dma1_ch1(void);
WEAK_DEFAULT void isr_dma1_ch2(void);
WEAK_DEFAULT void isr_dma1_ch3(void);
WEAK_DEFAULT void isr_dma1_ch4(void);
WEAK_DEFAULT void isr_dma1_ch5(void);
WEAK_DEFAULT void isr_dma1_ch6(void);
WEAK_DEFAULT void isr_dma1_ch7(void);
WEAK_DEFAULT void isr_adc1(void);
WEAK_DEFAULT void isr_usb_hp(void);
WEAK_DEFAULT void isr_usb_lp(void);
WEAK_DEFAULT void isr_dac(void);
WEAK_DEFAULT void isr_comp(void);
WEAK_DEFAULT void isr_lcd(void);
WEAK_DEFAULT void isr_tim9(void);
WEAK_DEFAULT void isr_tim10(void);
WEAK_DEFAULT void isr_tim11(void);
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
WEAK_DEFAULT void isr_usb_fs_wkup(void);
WEAK_DEFAULT void isr_tim6(void);
WEAK_DEFAULT void isr_tim7(void);
WEAK_DEFAULT void isr_sdio(void);
WEAK_DEFAULT void isr_tim5(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_uart4(void);
WEAK_DEFAULT void isr_uart5(void);
WEAK_DEFAULT void isr_dma2_ch1(void);
WEAK_DEFAULT void isr_dma2_ch2(void);
WEAK_DEFAULT void isr_dma2_ch3(void);
WEAK_DEFAULT void isr_dma2_ch4(void);
WEAK_DEFAULT void isr_dma2_ch5(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_comp_acq(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* STM specific peripheral handlers */
    (void*) isr_wwdg,
    (void*) isr_pvd,
    (void*) isr_tamper_stamp,
    (void*) isr_rtc_wkup,
    (void*) isr_flash,
    (void*) isr_rcc,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_exti,
    (void*) isr_dma1_ch1,
    (void*) isr_dma1_ch2,
    (void*) isr_dma1_ch3,
    (void*) isr_dma1_ch4,
    (void*) isr_dma1_ch5,
    (void*) isr_dma1_ch6,
    (void*) isr_dma1_ch7,
    (void*) isr_adc1,
    (void*) isr_usb_hp,
    (void*) isr_usb_lp,
    (void*) isr_dac,
    (void*) isr_comp,
    (void*) isr_exti,
    (void*) isr_lcd,
    (void*) isr_tim9,
    (void*) isr_tim10,
    (void*) isr_tim11,
    (void*) isr_tim2,
    (void*) isr_tim3,
    (void*) isr_tim4,
    (void*) isr_i2c1_ev,
    (void*) isr_i2c1_er,
    (void*) isr_i2c2_ev,
    (void*) isr_i2c2_er,
    (void*) isr_spi1,
    (void*) isr_spi2,
    (void*) isr_usart1,
    (void*) isr_usart2,
    (void*) isr_usart3,
    (void*) isr_exti,
    (void*) isr_rtc_alarm,
    (void*) isr_usb_fs_wkup,
    (void*) isr_tim6,
    (void*) isr_tim7,
    (void*) isr_sdio,
    (void*) isr_tim5,
    (void*) isr_spi3,
    (void*) isr_uart4,
    (void*) isr_uart5,
    (void*) isr_dma2_ch1,
    (void*) isr_dma2_ch2,
    (void*) isr_dma2_ch3,
    (void*) isr_dma2_ch4,
    (void*) isr_dma2_ch5,
    (void*) isr_aes,
    (void*) isr_comp_acq,
};
